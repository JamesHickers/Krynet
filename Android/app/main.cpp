// File: krynet_android_client.cpp
#include <jni.h>
#include <android/log.h>
#include "sciter-x.h"
#include "sciter-x-android.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "KRYNET", __VA_ARGS__)
using json = nlohmann::json;

// Embedded HTML for update popup
const wchar_t* update_popup_html = LR"(
<html>
<head>
  <title>Krynet Update</title>
  <style>
    body { font-family: sans-serif; text-align: center; padding: 20px; background: #f9f9f9; }
    h2 { color: #2a7fff; }
    button { margin: 10px; padding: 10px 20px; font-size: 16px;
             border: none; border-radius: 6px; background-color: #2a7fff; color: white; cursor: pointer; }
    button:hover { background-color: #1f5fcc; }
  </style>
</head>
<body>
  <h2>New Krynet Android Version Available!</h2>
  <p id="version-info"></p>
  <button id="download-btn">Download APK</button>
  <button id="close-btn">Close</button>

  <script type="text/javascript">
    function showUpdate(version, url) {
      document.getElementById('version-info').innerText = "Latest version: " + version;
      document.getElementById('download-btn').onclick = function() { window.open(url, "_system"); };
      document.getElementById('close-btn').onclick = function() { window.close(); };
    }
  </script>
</body>
</html>
)";

// Helper for CURL
static size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* str = static_cast<std::string*>(userdata);
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

// Function to check for updates and show popup if needed
void checkForUpdates(HWINDOW parent) {
    // Load local version.json
    json local_version;
    std::ifstream file("version.json");
    if(file.is_open()) {
        file >> local_version;
        file.close();
    }
    std::string local_ver = local_version.value("version", "0.0.0");
    LOGI("Local Android client version: %s", local_ver.c_str());

    // Fetch GitHub release info
    CURL* curl = curl_easy_init();
    if(!curl) return;

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/Krynet-LLC/Krynet/releases/latest");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "KrynetClient/1.0");
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if(res != CURLE_OK) {
        LOGI("GitHub update check failed: %d", res);
        return;
    }

    try {
        auto release = json::parse(response);
        std::string latest_tag = release.value("tag_name", "");
        std::string apk_url;
        bool found_android = false;

        for(auto& asset : release["assets"]) {
            std::string name = asset.value("name","");
            if(name.find(".apk") != std::string::npos || name.find("Android") != std::string::npos) {
                apk_url = asset.value("browser_download_url","");
                found_android = true;
                LOGI("Found Android asset: %s", name.c_str());
                break;
            }
        }

        if(found_android && latest_tag != local_ver && !apk_url.empty()) {
            LOGI("New Android release available! Local: %s, GitHub: %s", local_ver.c_str(), latest_tag.c_str());

            // Create Sciter popup window
            HWINDOW popup = SciterCreateWindow(SW_POPUP | SW_RESIZEABLE | SW_TITLEBAR, nullptr, nullptr, parent);
            SciterLoadHtml(popup, update_popup_html, wcslen(update_popup_html), L"update://popup");

            // Inject version and URL into JS
            std::wstring ws_ver(latest_tag.begin(), latest_tag.end());
            std::wstring ws_url(apk_url.begin(), apk_url.end());
            std::wstring script = L"showUpdate('" + ws_ver + L"','" + ws_url + L"');";
            SciterCallScript(popup, script.c_str(), script.size());
        } else {
            LOGI("Android client is up-to-date or no Android asset found.");
        }

    } catch(...) {
        LOGI("Failed to parse GitHub release JSON");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_krynet_MainActivity_startSciter(JNIEnv* env, jobject thiz)
{
    // Disable debug in production
    SciterSetOption(nullptr, SCITER_SET_DEBUG_MODE, FALSE);
    SciterSetOption(nullptr, SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_SOCKET_IO);

    // Create main window
    HWINDOW hwnd = SciterCreateWindow(SW_MAIN, nullptr, nullptr, nullptr);
    if(!hwnd) {
        LOGI("Failed to create main Sciter window");
        return;
    }

    // Load Krynet web client
    BOOL ok = SciterLoadUrl(hwnd, L"https://krynet.ai");
    if(!ok) {
        const wchar_t* offline_html =
            L"data:text/html,<html><body style='font-family:sans-serif;"
            L"text-align:center;margin-top:20%;'>"
            L"<h1>Sorry, Krynet.ai may be down, check back later</h1>"
            L"</body></html>";
        SciterLoadFile(hwnd, offline_html);
        LOGI("Site unreachable, displaying offline fallback");
    }

    // Check for GitHub updates
    checkForUpdates(hwnd);

    // Run event loop
    MSG msg;
    while(GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
