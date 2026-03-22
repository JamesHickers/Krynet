#include <sciter-x.h>
#include <sciter-x-window.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <curl/curl.h>

namespace fs = std::filesystem;

// Default installation path
const std::string DEFAULT_INSTALL_PATH = "/opt/Krynet";

// Official Linux release
const std::string LINUX_RELEASE_URL = "https://github.com/Krynet-LLC/Krynet/releases/latest/download/Krynet.AppImage";

// ---------------- Helper Functions ----------------
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
    if (!curl) return false;
    FILE* fp = fopen(outPath.c_str(), "wb");
    if (!fp) return false;

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

bool check_online() {
    int ret = system("ping -c1 krynet.ai > /dev/null 2>&1");
    return (ret == 0);
}

void create_desktop_shortcut(const fs::path& installPath) {
    std::string home = std::getenv("HOME");
    std::string desktopFile = home + "/Desktop/Krynet.desktop";
    std::ofstream out(desktopFile);
    out << "[Desktop Entry]\n"
           "Type=Application\n"
           "Name=Krynet\n"
           "Exec=" << installPath.string() << "/Krynet.AppImage\n"
           "Icon=" << installPath.string() << "/assets/krynet.png\n"
           "Terminal=false\n"
           "Categories=Network;\n";
    out.close();
    fs::permissions(desktopFile, fs::perms::owner_all | fs::perms::group_read | fs::perms::others_read);
}

void create_menu_entry(const fs::path& installPath) {
    std::string home = std::getenv("HOME");
    fs::create_directories(home + "/.local/share/applications");
    std::string menuFile = home + "/.local/share/applications/krynet.desktop";
    std::ofstream out(menuFile);
    out << "[Desktop Entry]\n"
           "Type=Application\n"
           "Name=Krynet\n"
           "Exec=" << installPath.string() << "/Krynet.AppImage\n"
           "Icon=" << installPath.string() << "/assets/krynet.png\n"
           "Terminal=false\n"
           "Categories=Network;\n";
    out.close();
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
body { font-family: sans-serif; background: #1e1e1e; color: #fff; text-align: center; }
input[type=text] { width: 350px; padding: 5px; }
button { margin: 10px; padding: 10px 20px; font-size: 16px; }
#progressBar { width: 80%; height: 20px; background: #444; margin: 20px auto; border-radius: 5px; }
#progressFill { height: 100%; width: 0%; background: #00ff00; border-radius: 5px; }
</style>
</head>
<body>
<h1>Krynet Setup Wizard</h1>
<p>Install Krynet Linux Client</p>
<p>Install Path: <input type="text" id="installPath" value="/opt/Krynet"></p>
<p><label><input type="checkbox" id="desktopShortcut" checked> Create Desktop Shortcut</label></p>
<button onclick="install()">Install</button>
<div id="status"></div>
<div id="progressBar"><div id="progressFill"></div></div>
<script type="text/tiscript">
function install() {
    var path = document.getElementById('installPath').value;
    var shortcut = document.getElementById('desktopShortcut').checked;
    Host.call('performInstall', path, shortcut);
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
            char path[512]; bool shortcut = true;
            if(argc>0) SciterValueToString(args[0], path, 512);
            if(argc>1) SciterValueToBool(args[1], &shortcut);

            fs::path installPath = fs::path(path);
            fs::create_directories(installPath);

            if(!check_online()){
                MessageBoxA(NULL, "⚠️ Krynet.ai may be down. Check back later.", "Krynet Setup Warning", MB_OK | MB_ICONWARNING);
            }

            // Download file with progress
            VALUE sciterValue;
            SciterInitValue(&sciterValue); // dummy placeholder for progress callback
            std::string exeDest = installPath.string() + "/Krynet.AppImage";
            if(download_file(LINUX_RELEASE_URL, exeDest, &sciterValue)){
                system(("chmod +x " + exeDest).c_str());
            } else {
                MessageBoxA(NULL, "❌ Failed to download Krynet!", "Error", MB_OK | MB_ICONERROR);
                return SCDOM_RESULT_OK;
            }

            // Desktop shortcut
            if(shortcut) create_desktop_shortcut(installPath);
            // Always menu entry
            create_menu_entry(installPath);
        }
        return SCDOM_RESULT_OK;
    }, nullptr);

    mainWindow.expand();
    mainWindow.run_app();
    return 0;
}
