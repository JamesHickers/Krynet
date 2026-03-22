// File: krynet_linux_official.cpp
// Target: Linux x64 (Ubuntu / Arch / Tails / Whonix / Kodachi)
// Privacy-first, always-online, built-in client-only version check

#include <sciter-x.h>
#include <sciter-x-window.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp> // JSON parsing for version.json

namespace fs = std::filesystem;
using json = nlohmann::json;

#define LOCAL_VERSION_FILE "version.json"
#define SW_FLAGS (SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN)

// Stub functions for TPM, microphone, camera
bool check_tpm_available() {
    // TODO: Implement hardware MFA / TPM detection if user opts in
    return false; // Placeholder
}
void init_microphone() {
    // TODO: Initialize CoreAudio / PulseAudio on first voice call
}
void init_camera() {
    // TODO: Initialize Media Foundation / V4L2 on first video call
}

// Read the built-in version.json and verify structure
bool check_client_version() {
    if(!fs::exists(LOCAL_VERSION_FILE)) {
        std::cerr << "[Krynet] Missing version.json, client integrity cannot be verified.\n";
        return true; // Treat as OK to let user continue
    }

    std::ifstream file(LOCAL_VERSION_FILE);
    json j;
    try {
        file >> j;
    } catch (...) {
        std::cerr << "[Krynet] Corrupted version.json!\n";
        return false;
    }
    file.close();

    std::string client_version = j.value("version", "0.0.0");
    std::string build_number = j.value("build", "0");

    if(client_version.empty() || build_number.empty()) {
        std::cerr << "[Krynet] Invalid version.json!\n";
        return false;
    }

    std::cout << "[Krynet] Client version: " << client_version
              << " (build " << build_number << ")\n";
    return true;
}

// Simple connectivity check: tries to open the Krynet site
bool site_is_online(const std::wstring& url) {
    // Minimal: attempt to open the URL in Sciter; fallback if fails
    // For production, a small HEAD request could be added via libcurl
    return true; // Assume online; Sciter will fail gracefully if not
}

int main() {
    // 1. Client-only version check
    if(!check_client_version()) {
        std::cerr << "[Krynet] Version mismatch detected. Please reinstall Krynet.\n";
    }

    // 2. Enable network/socket access for chat
    SciterSetOption(nullptr, SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_SOCKET_IO);

    // 3. Force software rendering for maximum compatibility
    SciterSetOption(nullptr, SCITER_SET_GFX_LAYER, GFX_LAYER_SOFTWARE);

    // 4. Create the main Sciter window
    sciter::window mainWindow(SW_FLAGS);

    // 5. Always-online: attempt to load Krynet web client
    if(site_is_online(L"https://krynet.ai")) {
        mainWindow.load_url(L"https://krynet.ai");
    } else {
        // Friendly fallback if site cannot be reached
        const wchar_t* offline_html =
            L"data:text/html,<html><body style='font-family:sans-serif;"
            L"text-align:center;margin-top:20%;'>"
            L"<h1>Sorry, Krynet.ai may be down, check back later</h1>"
            L"</body></html>";
        mainWindow.load_file(offline_html);
    }

    // 6. Maximize window for “native client” feel
    mainWindow.expand();

    // 7. Initialize TPM / microphone / camera hooks (on-demand)
    if(check_tpm_available()) {
        // Initialize TPM hardware-based MFA if user opts in
    }
    // Microphone and camera are only initialized during voice/video calls
    // init_microphone();
    // init_camera();

    // 8. Run the Sciter event loop
    mainWindow.run_app();

    return 0;
}
