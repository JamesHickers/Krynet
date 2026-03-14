dd<div align="center">

# 🛠️ Krynet Source Repository - Source Branch

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
| **Windows** | 🔨 **Source Available** (MinGW64) | ❌ **Unavailable** |
| **macOS** | 🔨 **Source Available** (Xcode/CMake) | ❌ **Unavailable** |
| **iOS** | 🔨 **Source Available** (Xcode) | ❌ **Unavailable** |
| **Linux** | 🔨 **Source Available** (+ Precompiled) | ✅ **Available** |
| **Android** | 🔨 **Source Available** (+ Precompiled) | ✅ **Available** |

**Linux & Android: Precompiled binaries ready. Others: Build from source only.**

---

## 📂 Platform Source Folders

| Platform | Source Path |
|----------|-------------|
| **Windows** | [Source/Windows](https://github.com/JamesHickers/Krynet/tree/Source/Windows) |
| **Linux** | [Source/Linux](https://github.com/JamesHickers/Krynet/tree/Source/Linux) |
| **Android** | [Source/Android](https://github.com/JamesHickers/Krynet/tree/Source/Android) |
| **iOS** | `Source/iOS` |
| **macOS** | `Source/macOS` |

---

## 🚨 **Required External SDKs** *(Not Included)*

**⚠️ Sciter SDK NOT bundled** (always latest version):

| SDK | Download | Usage |
|-----|----------|-------|
| **Sciter.JS** | [![Sciter](https://sciter.com/)](https://sciter.com/) | **All platforms** |
| **Ultralight** | [![Ultralight](https://ultralig.ht/)](https://ultralig.ht/) | **Alternative renderer** |

**Setup:**
```bash
cd Source/Windows    # or Linux, Android, etc.
mkdir -p thirdparty/sciter
# Download Sciter SDK → extract here
```

---

## 🚀 Quick Start - Build From Source Branch

```bash
# Clone Source branch
git clone -b Source https://github.com/JamesHickers/Krynet.git
cd Krynet

# Platform builds
cd Source/Windows && make release      # Windows (source only)
cd Source/Linux  && ./build.sh         # Linux (precompiled available)
cd Source/Android && ./gradlew assembleRelease  # Android (precompiled available)
```

---

## 🏗️ Platform-Specific Builds

### 🪟 **Windows**
```bash
cd Source/Windows
mkdir -p /sciter && # Add Sciter SDK
make clean && make release
# → Krynet.exe
```

### 🐧 **Linux**
```bash
cd Source/Linux
./build.sh --release
# → Krynet.AppImage (precompiled also available)
```

### 🤖 **Android**
```bash
cd Source/Android
./gradlew assembleRelease
# → krynet.apk (precompiled also available)
```

### 📱 **iOS**
```bash
cd Source/iOS
# 1. Add Sciter.Lite SDK to /sciter/
# 2. Drag libSciterLit.a → Xcode project
open Krynet.xcodeproj

# In Xcode:
# → Select "Krynet" target
# → Signing & Capabilities → Add your Apple ID
# → Product → Archive
# → Distribute App → Ad Hoc / Development → Export IPA
# → krynet.ipa ✅
---
### 🍎 **macOS**
```bash
cd Source/macOS
mkdir build && cd build
# 1. Add Sciter.JS SDK to /sciter/

# CMake build
cmake .. -DSCITER_PATH=/sciter -DCMAKE_BUILD_TYPE=Release
make -j8
open ../Krynet.dmg
# → Krynet.app inside DMG ✅


## ✅ Built Features

```
🔹 wss://gateway.krynet.ai → Real-time
🔹 REST APIs → Platform services
🔹 WebTransfer → Files/media
🔹 WebTransport → Screenshare
🔹 TLS 1.3 → Full encryption
🔹 Sciter → Native performance
```

---

## 📜 AGPL-3.0 License
```
✅ Modify -  Distribute -  Commercial OK
✅ Network use → Share source
❌ No proprietary forks
```

---

<div align="center">

**🔒 Krynet Source Branch © 2030**  
*Linux/Android precompiled. Windows/macOS/iOS: Build from source.*

<sub>AGPL-3.0 • [sciter.com](https://sciter.com/) SDK required • [JamesHickers/Krynet](https://github.com/Krynet-LLC/Krynet/tree/Source)</sub>

</div>
