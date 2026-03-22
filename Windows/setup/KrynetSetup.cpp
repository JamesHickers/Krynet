#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <filesystem>
#include <urlmon.h>
#include <string>
#include <thread>
#include "sciter-x.h"
#include "sciter-x-window.hpp"
#include "sciter-x-stdlib.h"

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Shlwapi.lib")

namespace fs = std::filesystem;

// ---------------- Configuration ----------------
const wchar_t* DEFAULT_INSTALL_PATH = L"C:\\Program Files\\Krynet";
const char* GITHUB_RELEASES_API = "https://github.com/Krynet-LLC/Krynet/releases/latest";

// ---------------- Helper Functions ----------------
bool download_file(const std::wstring& url, const std::wstring& outPath) {
    HRESULT hr = URLDownloadToFileW(NULL, url.c_str(), outPath.c_str(), 0, NULL);
    return SUCCEEDED(hr);
}

void create_desktop_shortcut(const fs::path& installPath) {
    wchar_t desktopPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktopPath))) {
        CoInitialize(NULL);
        IShellLinkW* pShellLink = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&pShellLink))) {
            std::wstring exePath = installPath.wstring() + L"\\Krynet.exe";
            pShellLink->SetPath(exePath.c_str());
            pShellLink->SetDescription(L"Krynet Messenger");
            IPersistFile* ppf = nullptr;
            if (SUCCEEDED(pShellLink->QueryInterface(IID_IPersistFile, (void**)&ppf))) {
                std::wstring shortcutPath = std::wstring(desktopPath) + L"\\Krynet.lnk";
                ppf->Save(shortcutPath.c_str(), TRUE);
                ppf->Release();
            }
            pShellLink->Release();
        }
        CoUninitialize();
    }
}

bool check_online() {
    return InternetCheckConnection(L"https://krynet.ai", FLAG_ICC_FORCE_CONNECTION, 0);
}

void launch_krynet(const fs::path& installPath) {
    std::wstring exePath = installPath.wstring() + L"\\Krynet.exe";
    ShellExecuteW(NULL, L"open", exePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

// ---------------- Main Setup Wizard ----------------
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_SOCKET_IO);
    sciter::window mainWindow(SW_MAIN | SW_TITLEBAR | SW_RESIZEABLE);

    const char* htmlUI = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Krynet Setup</title>
<style>
body { font-family: Segoe UI, sans-serif; background: #1E1E1E; color: #FFF; }
.container { width: 500px; margin: 40px auto; text-align: center; }
button { padding: 10px 20px; margin: 10px; font-size: 16px; }
input[type=text] { width: 300px; padding: 5px; font-size: 14px; }
</style>
</head>
<body>
<div class="container">
<h1>Krynet Setup Wizard</h1>
<p>Select installation options:</p>
<p>
<label><input type="checkbox" id="desktopShortcut" checked> Create Desktop Shortcut</label>
</p>
<p>
<label>Install Path: <input type="text" id="installPath" value="C:\Program Files\Krynet"></label>
</p>
<button onclick="install()">Install</button>
<div id="status"></div>
</div>
<script type="text/tiscript">
function install() {
    var desktop = document.getElementById('desktopShortcut').checked;
    var path = document.getElementById('installPath').value;
    Host.call('performInstall', desktop, path);
    document.getElementById('status').innerHTML = '✅ Installation complete! You can launch Krynet now.';
}
function launch() {
    var path = document.getElementById('installPath').value;
    Host.call('launchKrynet', path);
}
</script>
</body>
</html>
)HTML";

    mainWindow.load_html(htmlUI, L"about:blank");

    SciterSetHostCallback(mainWindow.get_hwnd(), [](const char* method, const VALUE* args, UINT argc, VALUE* result, LPVOID param){
        if(strcmp(method,"launchKrynet")==0){
            if(argc>0){ char path[MAX_PATH]; SciterValueToString(args[0], path, MAX_PATH); launch_krynet(fs::path(path)); }
        } else if(strcmp(method,"performInstall")==0){
            bool createDesktop = false;
            char installPathStr[MAX_PATH];
            if(argc>0) SciterValueToBool(args[0], &createDesktop);
            if(argc>1) SciterValueToString(args[1], installPathStr, MAX_PATH);

            fs::path installPath = fs::path(installPathStr);
            fs::create_directories(installPath);

            if(!check_online()){
                MessageBoxW(NULL,L"⚠️ Krynet.ai may be down. Please check back later.",L"Krynet Setup Warning",MB_OK|MB_ICONWARNING);
            }

            // Download latest Windows x64 Krynet.exe
            std::wstring exeUrl = L"https://github.com/Krynet-LLC/Krynet/releases/latest/download/Krynet.exe";
            std::wstring exeDest = installPath.wstring() + L"\\Krynet.exe";
            if(download_file(exeUrl, exeDest)){
                MessageBoxW(NULL,L"Krynet.exe downloaded successfully!",L"Download Complete",MB_OK);
            } else {
                MessageBoxW(NULL,L"❌ Failed to download Krynet.exe!",L"Error",MB_OK|MB_ICONERROR);
                return SCDOM_RESULT_OK;
            }

            // TODO: Download dependencies (sciter.dll, assets/plugins) if needed

            if(createDesktop) create_desktop_shortcut(installPath);
        }
        return SCDOM_RESULT_OK;
    }, nullptr);

    mainWindow.expand();
    mainWindow.run_app();
    return 0;
}
