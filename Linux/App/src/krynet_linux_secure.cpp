// File: krynet_linux_secure.cpp
// Target: Linux (Ubuntu / Arch / Tails / Whonix / Kodachi)
// Uses Sciter.JS runtime (libsciter.so)
#include <sciter-x.h>
#include <sciter-x-window.hpp>

int main() {
    // Only allow network/socket access (privacy-first)
    SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_SOCKET_IO);

    // Force software rendering for maximum compatibility
    SciterSetOption(NULL, SCITER_SET_GFX_LAYER, GFX_LAYER_SOFTWARE);

    // Create a top-level Sciter window
    sciter::window mainWindow(
        SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN
    );

    // Load Krynet web client
    mainWindow.load_url(L"https://krynet.ai");

    // Maximize window to give a “native client” feel
    mainWindow.expand();

    // Run event loop
    mainWindow.run_app();

    return 0;
}
