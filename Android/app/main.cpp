#include <jni.h>
#include <android/log.h>
#include "sciter-x.h"
#include "sciter-x-android.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "KRYNET", __VA_ARGS__)

// Stub functions for future TPM / security hooks
bool check_tpm_available() {
    return false; // Android usually uses hardware-backed keystore
}

extern "C"
JNIEXPORT void JNICALL
Java_com_krynet_MainActivity_startSciter(JNIEnv* env, jobject thiz)
{
    // Disable debug mode in production
    SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, FALSE);

    // Enable network/socket I/O for always-online chat
    SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_SOCKET_IO);

    // Create the Sciter window
    HWINDOW hwnd = SciterCreateWindow(SW_MAIN, nullptr, nullptr, nullptr);
    if(!hwnd) {
        LOGI("Failed to create Sciter window");
        return;
    }

    // Attempt to load Krynet online client
    BOOL ok = SciterLoadUrl(hwnd, L"https://krynet.ai");
    if(!ok) {
        // Friendly fallback HTML if site cannot be reached
        const wchar_t* offline_html =
            L"data:text/html,<html><body style='font-family:sans-serif;"
            L"text-align:center;margin-top:20%;'>"
            L"<h1>Sorry, Krynet.ai may be down, check back later</h1>"
            L"</body></html>";
        SciterLoadFile(hwnd, offline_html);
        LOGI("Site unreachable, displaying offline fallback");
    }

    LOGI("Krynet client loaded successfully");
}
