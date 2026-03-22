<div align="center">

<h1>🛠️ Krynet Source Repository - Source Branch</h1>

<p><strong>Official Source Code for All Platforms</strong><br>
<em>Build any platform from source • Full transparency • AGPL-3.0 Licensed</em></p>

<p>
<img src="https://img.shields.io/badge/License-AGPL%203.0-red" alt="AGPL-3.0">
<img src="https://img.shields.io/badge/All%20Platforms-Buildable-brightgreen" alt="All Buildable">
<img src="https://img.shields.io/badge/Sciter.JS-Powered-orange" alt="Sciter">
<img src="https://img.shields.io/badge/Status-Active-brightgreen" alt="Active">
</p>

</div>

<hr>

<h2>📋 Platform Status</h2>

<table>
<tr><th>Platform</th><th>Build Status</th><th>Precompiled</th></tr>
<tr><td><strong>Windows</strong></td><td>🔨 <strong>Source Available</strong> (MinGW64)</td><td>❌ Unavailable</td></tr>
<tr><td><strong>macOS</strong></td><td>🔨 <strong>Source Available</strong> (Xcode/CMake)</td><td>❌ Unavailable</td></tr>
<tr><td><strong>iOS</strong></td><td>🔨 <strong>Source Available</strong> (Xcode)</td><td>❌ Unavailable</td></tr>
<tr><td><strong>Linux</strong></td><td>🔨 <strong>Source Available</strong> (+ Precompiled)</td><td>✅ Available</td></tr>
<tr><td><strong>Android</strong></td><td>🔨 <strong>Source Available</strong> (+ Precompiled)</td><td>✅ Available</td></tr>
</table>

<p><strong>Linux & Android:</strong> Precompiled binaries ready. Others: Build from source only.</p>

<hr>

<h2>🚨 Required External SDKs (Not Included)</h2>

<p><strong>⚠️ Sciter SDK NOT bundled</strong> (always latest version):</p>

<table>
<tr><th>SDK</th><th>Download</th><th>Usage</th></tr>
<tr><td><strong>Sciter.JS</strong></td><td><a href="https://sciter.com/">https://sciter.com/</a></td><td>All platforms</td></tr>
<tr><td><strong>Ultralight</strong></td><td><a href="https://ultralig.ht/">https://ultralig.ht/</a></td><td>Alternative renderer</td></tr>
</table>

<p><strong>Setup:</strong></p>
<pre>
cd Source/Windows    # or Linux, Android, etc.
mkdir -p thirdparty/sciter
# Download Sciter SDK → extract here
</pre>

<hr>

<h2>🚀 Quick Start - Build From Source Branch</h2>
<pre>
# Clone Source branch
git clone -b Source https://github.com/Krynet-LLC/Krynet.git
cd Krynet

# Platform builds
cd Source/Windows && make release      # Windows (source only)
cd Source/Linux  && ./build.sh         # Linux (precompiled available)
cd Source/Android && ./gradlew assembleRelease  # Android (precompiled available)
</pre>

<hr>

<h2>🏗️ Platform-Specific Builds</h2>

<h3>🪟 Windows</h3>
<pre>
cd Source/Windows
mkdir -p /sciter && # Add Sciter SDK
make clean && make release
# → Krynet.exe
</pre>

<h3>🐧 Linux</h3>
<pre>
cd Source/Linux
./build.sh --release
# → Krynet.AppImage (precompiled also available)
</pre>

<h3>🤖 Android</h3>
<pre>
cd Source/Android
./gradlew assembleRelease
# → krynet.apk (precompiled also available)
</pre>

<h3>📱 iOS</h3>
<pre>
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
</pre>

<h3>🍎 macOS</h3>
<pre>
cd Source/macOS
mkdir build && cd build
# 1. Add Sciter.JS SDK to /sciter/

# CMake build
cmake .. -DSCITER_PATH=/sciter -DCMAKE_BUILD_TYPE=Release
make -j8
open ../Krynet.dmg
# → Krynet.app inside DMG ✅
</pre>

<hr>

<h2>✅ Built Features</h2>
<pre>
🔹 wss://gateway.krynet.ai → Real-time
🔹 REST APIs → Platform services
🔹 WebTransfer → Files/media
🔹 WebTransport → Screenshare
🔹 TLS 1.3 → Full encryption
🔹 Sciter → Native performance
</pre>

<hr>

<h2>📜 AGPL-3.0 License</h2>
<p>
✅ Modify -  Distribute -  Commercial OK<br>
✅ Network use → Share source<br>
❌ No proprietary forks
</p>

<hr>

<div align="center">
<p><strong>🔒 Krynet Source Branch © 2030</strong><br>
Linux/Android precompiled. Windows/macOS/iOS: Build from source.</p>
<p><sub>AGPL-3.0 • <a href="https://sciter.com/">sciter.com</a> SDK required • <a href="https://github.com/Krynet-LLC/Krynet/tree/Source">Krynet</a></sub></p>
</div>
