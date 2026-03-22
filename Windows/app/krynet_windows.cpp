#include <windows.h>
#include "sciter-x.h"
#include "sciter-x-window.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

using json = nlohmann::json;

// --------------------------
// Embedded HTML: update popup
// --------------------------
const wchar_t* update_html = LR"(
<html>
<head>
<style>
body { font-family:sans-serif; text-align:center; padding:20px; background:#f9f9f9; }
h2 { color:#2a7fff; }
p.warning { color:red; font-weight:bold; }
button { margin:10px; padding:10px 20px; font-size:16px; border:none; border-radius:6px; background:#2a7fff; color:white; cursor:pointer; }
button:hover { background:#1f5fcc; }
</style>
</head>
<body>
<h2>New Krynet Windows Version Available!</h2>
<p id="version-info"></p>
<p id="hash-warning" class="warning"></p>
<button id="download-btn">Download EXE</button>
<button onclick="window.close()">Close</button>
<script>
function showUpdate(version, url, warn) {
    document.getElementById('version-info').innerText = "Latest version: " + version;
    document.getElementById('hash-warning').innerText = warn;
    document.getElementById('download-btn').onclick = function() {
        window.open(url, "_system");
    };
}
</script>
</body>
</html>
)";

// --------------------------
// Embedded HTML: site down
// --------------------------
const wchar_t* site_down_html = LR"(
<html>
<head>
<style>
body { font-family:sans-serif; text-align:center; padding:40px; background:#f9f9f9; }
h2 { color:#ff4d4d; }
</style>
</head>
<body>
<h2>⚠ Sorry, Krynet.ai may be down</h2>
<p>Check back later</p>
</body>
</html>
)";

// --------------------------
// CURL callback
// --------------------------
static size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* str = static_cast<std::string*>(userdata);
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

// --------------------------
// Check if Krynet site is reachable
// --------------------------
bool isSiteOnline() {
    CURL* curl = curl_easy_init();
    if(!curl) return false;

    curl_easy_setopt(curl, CURLOPT_URL, "https://krynet.ai/web");
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
}

// --------------------------
// GitHub update checker
// --------------------------
void checkForUpdates(HWND hwnd) {
    std::ifstream file("version.json");
    if(!file.is_open()) return;

    json local;
    file >> local;
    file.close();

    std::string local_ver = local.value("version", "0.0.0");
    std::string local_hash = local.value("hash", "");

    CURL* curl = curl_easy_init();
    if(!curl) return;

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/Krynet-LLC/Krynet/releases/latest");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "KrynetClient");
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    if(curl_easy_perform(curl) != CURLE_OK) {
        curl_easy_cleanup(curl);
        return;
    }
    curl_easy_cleanup(curl);

    try {
        auto release = json::parse(response);

        std::string latest_tag = release.value("tag_name", "");
        std::string exe_url;
        std::string github_hash;

        for(auto& asset : release["assets"]) {
            std::string name = asset.value("name", "");

            // ONLY pick Windows builds
            if(name.find(".exe") != std::string::npos) {
                exe_url = asset.value("browser_download_url", "");
                github_hash = asset.value("sha256", "");
                break;
            }
        }

        if(!exe_url.empty() && latest_tag != local_ver) {
            std::string warn;

            if(!github_hash.empty() && github_hash != local_hash) {
                warn = "⚠ Warning: Client hash mismatch!";
            }

            HWINDOW popup = SciterCreateWindow(
                SW_POPUP | SW_TITLEBAR | SW_RESIZEABLE,
                nullptr, nullptr, hwnd
            );

            SciterLoadHtml(popup, update_html, wcslen(update_html), L"update://popup");

            std::wstring wv(latest_tag.begin(), latest_tag.end());
            std::wstring wu(exe_url.begin(), exe_url.end());
            std::wstring ww(warn.begin(), warn.end());

            std::wstring script =
                L"showUpdate('" + wv + L"','" + wu + L"','" + ww + L"');";

            SciterCallScript(popup, script.c_str(), script.size());
        }

    } catch(...) {}
}

// --------------------------
// WinMain
// --------------------------
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    RECT rc = { CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT };

    HWND hwnd = SciterCreateWindow(
        SW_MAIN | SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS,
        &rc, nullptr, nullptr, nullptr
    );

    // ALWAYS attempt live site
    if(isSiteOnline()) {
        SciterLoadFile(hwnd, L"https://krynet.ai/web");
    } else {
        SciterLoadHtml(hwnd, site_down_html, wcslen(site_down_html), L"down://");
    }

    // Update check
    checkForUpdates(hwnd);

    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    while(GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
