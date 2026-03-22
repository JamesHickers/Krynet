// File: krynet_linux_client.cpp
#include <sciter-x.h>
#include <sciter-x-window.hpp>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

using json = nlohmann::json;

// Embedded HTML popup
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
<h2>New Krynet Linux Version Available!</h2>
<p id="version-info"></p>
<p id="hash-warning" class="warning"></p>
<button id="download-btn">Download Linux Binary</button>
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

// Update check
void checkForUpdates(sciter::window& mainWindow) {
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
        std::string linux_url;
        std::string github_hash;
        bool found_linux = false;

        for(auto& asset : release["assets"]) {
            std::string name = asset.value("name", "");
            if(name.find("linux") != std::string::npos || name.find(".AppImage") != std::string::npos) {
                linux_url = asset.value("browser_download_url", "");
                github_hash = asset.value("sha256", "");
                found_linux = true;
                break;
            }
        }

        if(found_linux && !linux_url.empty() && latest_tag != local_ver) {
            std::string hashWarning;
            if(!github_hash.empty() && github_hash != local_hash) {
                hashWarning = "⚠ Warning: Your client hash differs from the official release!";
            }

            // Show popup
            sciter::window popup(
                SW_POPUP | SW_RESIZEABLE | SW_TITLEBAR
            );
            popup.load_html(update_html, L"update://popup");

            std::wstring ws_ver(latest_tag.begin(), latest_tag.end());
            std::wstring ws_url(linux_url.begin(), linux_url.end());
            std::wstring ws_warn(hashWarning.begin(), hashWarning.end());
            std::wstring script = L"showUpdate('" + ws_ver + L"','" + ws_url + L"','" + ws_warn + L"');";
            popup.call_script(script.c_str(), script.size());
        }

    } catch(...) {
        // Failed JSON parse
    }
}

// Main
int main() {
    sciter::window mainWindow(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN);

    BOOL ok = mainWindow.load_url(L"https://krynet.ai");
    if(!ok) {
        const wchar_t* fallback_html = L"data:text/html,<html><body style='text-align:center;margin-top:20%;font-family:sans-serif;'><h1>Sorry, Krynet.ai may be down, check back later</h1></body></html>";
        mainWindow.load_html(fallback_html, wcslen(fallback_html));
    }

    checkForUpdates(mainWindow);

    mainWindow.run_app();
    return 0;
}
