#include <cstdint>
#include <windows.h>
#include "sciter-x.h"

// Define window flags for MinGW
#define SW_TITLEBAR   1
#define SW_RESIZEABLE 2
#define SW_CONTROLS   4
#define SW_MAIN       8

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // Enable network/socket I/O
    SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_SOCKET_IO);

    // Create top-level Sciter window
    RECT rc = { CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT };
    HWND hwnd = SciterCreateWindow(
        SW_MAIN | SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS,
        &rc,
        nullptr,  // callback
        nullptr,  // callback param
        nullptr   // parent
    );

    // Load Krynet website
    SciterLoadFile(hwnd, L"https://krynet.ai");

    // Show the window
    ShowWindow(hwnd, SW_SHOW);

    // Standard Windows message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
