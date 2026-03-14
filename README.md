<div align="center">

# 🛠️ Krynet Source Repository

**Official Source Code for All Platforms**  
*Build any platform from source • Full transparency • AGPL-3.0 Licensed*

<p>
<img src="https://img.shields.io/badge/License-AGPL%203.0-red" alt="AGPL-3.0">
<img src="https://img.shields.io/badge/All%20Platforms-Buildable-brightgreen" alt="All Buildable">
<img src="https://img.shields.io/badge/Sciter.JS-Powered-orange" alt="Sciter">
<img src="https://img.shields.io/badge/Status-Active-brightgreen" alt="Active">
</p>

</div>

---

## 📋 Platform Status

| Platform | Build Status | Precompiled |
|----------|--------------|-------------|
| **Windows** | 🔨 **Source Available** (MinGW64) | ✅ Available |
| **macOS** | 🔨 **Source Available** (Xcode/CMake) | ✅ Available |
| **iOS** | 🔨 **Source Available** (Xcode) | ✅ Available |
| **Linux** | 🔨 **Source Available** (+ Precompiled) | ✅ Available |
| **Android** | 🔨 **Source Available** (+ Precompiled) | ✅ Available |

**All platforms fully buildable from source!**

---

## 🚀 Quick Start - Build Any Platform

```bash
# Clone full repo with submodules
git clone --recursive https://github.com/Krynet-LLC/Krynet.git
cd Krynet

# Unified build system
./build.sh --platform=windows    # or linux, ios, android
# Output: platform-specific binaries
```

---

## 🏗️ Platform-Specific Builds

### 🪟 **Windows** (MinGW64)
```bash
cd platforms/windows
make clean && make release
# Output: Krynet.exe
```
**Requires:** MinGW64 (gcc/g++ 12+), Sciter.JS SDK

### 🍎 **macOS** (CMake/Xcode)
```bash
cd platforms/macos
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8
# Output: Krynet.app
```

### 📱 **iOS** (Xcode)
```bash
cd platforms/ios
open Krynet.xcodeproj
# Sign → Archive → Export IPA
```

### 🐧 **Linux** (Any distro)
```bash
cd platforms/linux
./build.sh --release
# Output: Krynet.AppImage + deb/rpm
```
**glibc 2.31+ • AppImage universal**

### 🤖 **Android** (Gradle)
```bash
cd platforms/android
./gradlew assembleRelease
# Output: app/build/outputs/apk/release/krynet.apk
```

---

## 📁 Repository Structure

```
krynet/
├── src/
│   ├── core/          # WebSocket/REST/WebTransport
│   ├── crypto/        # TLS + protocol layers
│   ├── ui/            # Sciter HTML/CSS/JS
│   └── platforms/     # Platform-specific code
├── thirdparty/        # Sciter.JS, LuaJIT, libs
├── CMakeLists.txt     # Cross-platform build
└── build.sh           # Unified build script
```

---

## 🛠️ Compiler Requirements

| Platform | Primary Build | Minimum Version |
|----------|---------------|-----------------|
| **Windows** | MinGW64 | gcc/g++ 12.0 |
| **macOS** | Xcode/CMake | Xcode 15 / clang 15 |
| **iOS** | Xcode | iOS 15+ target |
| **Linux** | gcc/clang | gcc 11+ / glibc 2.31 |
| **Android** | Gradle/NDK | NDK r25+ |

**All platforms:** Sciter.JS SDK required

---

## ✅ What You Get When Built

```
🔹 WebSocket gateway → wss://gateway.krynet.ai
🔹 REST APIs → /api/v1/*
🔹 WebTransfer → Files/media
🔹 WebTransport → Screenshare (HTTP/3)
🔹 TLS 1.3 → All traffic encrypted
🔹 Sciter rendering → Native performance
🔹 Cross-platform audio/notifications
```

---

## 🔗 Feature Complete

| Feature | Status |
|---------|--------|
| **Real-time messaging** | ✅ |
| **Voice/Video calls** | ✅ |
| **Screensharing** | ✅ (WebTransport) |
| **File transfers** | ✅ (WebTransfer) |
| **E2EE** | ✅ (Double layer) |
| **Plugins/Mods** | ✅ |
| **Privacy tools** | ✅ |

---

## 📜 AGPL-3.0 License

```
✅ Full source modification allowed
✅ Distribution allowed (with source)
✅ Network use requires source publication
✅ Commercial use OK (AGPL terms)
❌ Cannot make proprietary derivative
```

**Krynet servers remain proprietary.**

---

## 🎯 Development Workflow

```bash
# Development build (hot reload)
./build.sh --dev --platform=linux

# Test
./Krynet --test

# Package
./package.sh --platform=windows
```

**Hot reload:** Sciter UI changes instantly during dev.

---

## 🔗 Official Resources

- [Krynet.ai](https://krynet.ai)
- [Precompiled Downloads](https://github.com/Krynet-LLC/Krynet/releases)
- [Documentation](https://krynet.ai/docs)
- [Community Ports](https://github.com/JamesHickers/Krynet-ports)

---

<div align="center">

**🔒 Krynet Source © 2030**  
*Build it yourself. Own your client. Stay private.*

<sub>AGPL-3.0 • Sciter-powered • Fully buildable across all platforms.</sub>

</div>
