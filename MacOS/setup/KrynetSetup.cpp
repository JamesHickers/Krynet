#include <sciter-x.h>
#include <sciter-x-window.hpp>
#include <filesystem>
#include <cstdlib>
#include <curl/curl.h>

namespace fs = std::filesystem;

// Default install path
const std::string INSTALL_PATH = "/Applications/Krynet.app";

// Official macOS release
const std::string MACOS_RELEASE_URL = "https://github.com/Krynet-LLC/Krynet/releases/latest/download/Krynet.dmg";

// ---------------- CURL helpers ----------------
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if(dltotal > 0) {
        double percent = (double)dlnow / (double)dltotal * 100.0;
        VALUE* sciterCallback = reinterpret_cast<VALUE*>(clientp);
        sciter::value args[1];
        args[0] = sciter::value(percent);
        sciter::value result;
        SciterCall(*sciterCallback, "updateProgress", args, 1, &result);
    }
    return 0;
}

bool download_file(const std::string& url, const std::string& outPath, VALUE* sciterCallback) {
    CURL* curl = curl_easy_init();
    if(!curl) return false;
    FILE* fp = fopen(outPath.c_str(), "wb");
    if(!fp) return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, sciterCallback);

    CURLcode res = curl_easy_perform(curl);
    fclose(fp);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
}

// ---------------- Desktop shortcut ----------------
void create_desktop_shortcut(const fs::path& appPath) {
    std::string home = std::getenv("HOME");
    std::string shortcut = home + "/Desktop/Krynet.app";
    fs::remove_all(shortcut);
    fs::create_directory_symlink(appPath, shortcut);
}

// ---------------- Main ----------------
int main() {
    SciterSetOption(nullptr, SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_SOCKET_IO);
    sciter::window mainWindow(SW_MAIN | SW_TITLEBAR | SW_RESIZEABLE);

    const char* htmlUI = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Krynet Setup</title>
<style>
body { font-family:sans-serif; background:#1e1e1e; color:#fff; text-align:center; }
button { margin:10px; padding:10px 20px; font-size:16px; }
#progressBar { width:80%; height:20px; background:#444; margin:20px auto; border-radius:5px; }
#progressFill { height:100%; width:0%; background:#00ff00; border-radius:5px; }
</style>
</head>
<body>
<h1>Krynet MacOS Setup Wizard</h1>
<p>Install Path: /Applications/Krynet.app</p>
<p><label><input type="checkbox" id="desktopShortcut" checked> Create Desktop Shortcut</label></p>
<button onclick="install()">Install</button>
<div id="status"></div>
<div id="progressBar"><div id="progressFill"></div></div>
<script type="text/tiscript">
function install() {
    var shortcut = document.getElementById('desktopShortcut').checked;
    Host.call('performInstall', shortcut);
    document.getElementById('status').innerHTML = '⬇️ Downloading...';
}
function updateProgress(percent) {
    document.getElementById('progressFill').style.width = percent + '%';
    if(percent >= 100) document.getElementById('status').innerHTML = '✅ Installation complete!';
}
</script>
</body>
</html>
)HTML";

    mainWindow.load_html(htmlUI, L"about:blank");

    SciterSetHostCallback(mainWindow.get_hwnd(), [](const char* method, const VALUE* args, UINT argc, VALUE* result, LPVOID param){
        if(strcmp(method,"performInstall")==0){
            bool shortcut = true;
            if(argc>0) SciterValueToBool(args[0], &shortcut);

            // Ensure Applications exists
            fs::create_directories(fs::path(INSTALL_PATH).parent_path());

            // Online check
            int online = system("ping -c1 krynet.ai > /dev/null 2>&1");
            if(online != 0) {
                printf("⚠️ Krynet.ai may be down. Check back later.\n");
            }

            // Download DMG to /tmp first
            std::string dmgPath = "/tmp/Krynet.dmg";
            VALUE sciterValue;
            SciterInitValue(&sciterValue);
            if(download_file(MACOS_RELEASE_URL, dmgPath, &sciterValue)){
                // Mount DMG
                system(("hdiutil attach " + dmgPath + " -nobrowse -quiet").c_str());
                // Copy app to /Applications
                system(("cp -R /Volumes/Krynet/Krynet.app \"" + INSTALL_PATH + "\"").c_str());
                // Unmount DMG
                system("hdiutil detach /Volumes/Krynet -quiet");
            } else {
                printf("❌ Failed to download Krynet!\n");
                return SCDOM_RESULT_OK;
            }

            // Desktop shortcut
            if(shortcut) create_desktop_shortcut(INSTALL_PATH);
        }
        return SCDOM_RESULT_OK;
    }, nullptr);

    mainWindow.expand();
    mainWindow.run_app();
    return 0;
}
