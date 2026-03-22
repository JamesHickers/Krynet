#include <windows.h>
#include "sciter-x.h"
#include "sciter-x-window.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

using json = nlohmann::json;

// Embedded HTML for update popup
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
<button id="close-btn" onclick="window.close()">Close</button>
<script type="text/javascript">
function showUpdate(version, url, hashWarning) {
    document.getElementById('version-info').innerText = "Latest version: " + version;
    document.getElementById('hash-warning').innerText = hashWarning;
    document.getElementById('download-btn').onclick = function() { window.open(url, "_system"); };
}
</script>
</body>
</html>
)";

// CURL callback
static size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* str = static_cast<std::string*>(userdata);
    str->append(ptr, size * nmemb);
    return size * nmemb;
}

// Check GitHub for updates
void checkForUpdates(HWND hwnd) {
    std::ifstream file("version.json");
    if(!file.is_open()) return;

    json local_version;
    file >> local_version;
    file.close();

    std::string local_ver = local_version.value("version", "0.0.0");
    std::string local_hash = local_version.value("hash", "");

    CURL* curl = curl_easy_init();
    if(!curl) return;

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/Krynet-LLC/Krynet/releases/latest");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "KrynetClient/1.0");
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
        bool found_windows = false;

        for(auto& asset : release["assets"]) {
            std::string name = asset.value("name", "");
            if(name.find(".exe") != std::string::npos) {
                exe_url = asset.value("browser_download_url", "");
                github_hash = asset.value("sha256", "");
                found_windows = true;
                break;
            }
        }

        if(found_windows && !exe_url.empty() && latest_tag != local_ver) {
            std::string hashWarning;
            if(!github_hash.empty() && github_hash != local_hash) {
                hashWarning = "⚠ Warning: Your client hash differs from the official release!";
            }

            // Create popup
            HWINDOW popup = SciterCreateWindow(SW_POPUP | SW_RESIZEABLE | SW_TITLEBAR, nullptr, nullptr, hwnd);
            SciterLoadHtml(popup, update_html, wcslen(update_html), L"update://popup");

            std::wstring ws_ver(latest_tag.begin(), latest_tag.end());
            std::wstring ws_url(exe_url.begin(), exe_url.end());
            std::wstring ws_warn(hashWarning.begin(), hashWarning.end());
            std::wstring script = L"showUpdate('" + ws_ver + L"','" + ws_url + L"','" + ws_warn + L"');";

            SciterCallScript(popup, script.c_str(), script.size());
        }

    } catch(...) {
        // Failed JSON parse
    }
}
