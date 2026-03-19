#include <jni.h>
#include <android/log.h>
#include "sciter-x.h"
#include "sciter-x-android.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "KRYNET", __VA_ARGS__)

extern "C"
JNIEXPORT void JNICALL
Java_com_krynet_MainActivity_startSciter(JNIEnv* env, jobject thiz)
{
    SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);

    HWINDOW hwnd = SciterCreateWindow(
        SCITER_CREATE_WINDOW_FLAGS::SW_MAIN,
        nullptr,
        nullptr,
        nullptr
    );

    if(!hwnd) {
        LOGI("Failed to create Sciter window");
        return;
    }

    BOOL ok = SciterLoadUrl(hwnd, L"https://krynet.ai");

    LOGI("Load result: %d", ok);
}
