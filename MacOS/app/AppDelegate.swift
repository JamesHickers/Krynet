import Cocoa
import SciterLit

@main
class AppDelegate: NSObject, NSApplicationDelegate {

    var window: NSWindow!
    var sciterView: SciterLiteView!

    func applicationDidFinishLaunching(_ notification: Notification) {
        let screenRect = NSScreen.main?.frame ?? NSRect(x: 0, y: 0, width: 800, height: 600)
        window = NSWindow(contentRect: screenRect,
                          styleMask: [.titled, .closable, .resizable, .miniaturizable],
                          backing: .buffered,
                          defer: false)
        window.title = "Krynet MacOS Client"

        sciterView = SciterLiteView(frame: window.contentView!.bounds)
        sciterView.autoresizingMask = [.width, .height]
        sciterView.backgroundColor = .black

        window.contentView?.addSubview(sciterView)
        window.makeKeyAndOrderFront(nil)

        loadLocalVersion()
        loadKrynet()
        checkForUpdates()
    }

    // MARK: - Version Info
    struct VersionInfo: Codable {
        let version: String
        let build: String
        let platform: String
        let hash: String
    }

    private var versionInfo: VersionInfo?

    private func loadLocalVersion() {
        if let url = Bundle.main.url(forResource: "version", withExtension: "json"),
           let data = try? Data(contentsOf: url),
           let info = try? JSONDecoder().decode(VersionInfo.self, from: data) {
            versionInfo = info
        }
    }

    // MARK: - Load Krynet Web Client (with offline fallback)
    private func loadKrynet() {
        if let htmlPath = Bundle.main.path(forResource: "krynet", ofType: "htm") {
            let htmlURL = URL(fileURLWithPath: htmlPath)
            sciterView.loadUrl(URLRequest(url: htmlURL))
        } else if let url = URL(string: "https://krynet.ai/web") {
            let request = URLRequest(url: url)
            let task = URLSession.shared.dataTask(with: request) { data, _, error in
                DispatchQueue.main.async {
                    if let _ = data, error == nil {
                        self.sciterView.loadUrl(request)
                    } else {
                        let offlineHTML = """
                        <html>
                        <body style='text-align:center;margin-top:20%;font-family:sans-serif;'>
                            <h1>Sorry, Krynet.ai may be down, check back later</h1>
                        </body>
                        </html>
                        """
                        self.sciterView.loadHtml(offlineHTML, baseUrl: URL(string: "offline://fallback"))
                    }
                }
            }
            task.resume()
        }
    }

    // MARK: - GitHub Update Check (MacOS only)
    private func checkForUpdates() {
        guard let local = versionInfo,
              let url = URL(string: "https://api.github.com/repos/Krynet-LLC/Krynet/releases/latest") else { return }

        let task = URLSession.shared.dataTask(with: url) { data, _, error in
            guard let data = data, error == nil else { return }
            do {
                if let release = try JSONSerialization.jsonObject(with: data) as? [String: Any],
                   let latestTag = release["tag_name"] as? String,
                   let assets = release["assets"] as? [[String: Any]] {

                    var macAssetUrl: String?
                    var githubHash: String?

                    for asset in assets {
                        if let name = asset["name"] as? String,
                           name.contains("MacOS") || name.contains(".dmg") || name.contains(".app") {
                            macAssetUrl = asset["browser_download_url"] as? String
                            githubHash = asset["sha256"] as? String
                            break
                        }
                    }

                    if let macUrl = macAssetUrl, latestTag != local.version {
                        var hashWarning = ""
                        if let githubHash = githubHash, githubHash != local.hash {
                            hashWarning = "⚠ Warning: This client hash differs from the official release!"
                        }

                        DispatchQueue.main.async {
                            self.showUpdatePopup(version: latestTag, url: macUrl, hashWarning: hashWarning)
                        }
                    }
                }
            } catch {
                print("❌ Failed to parse GitHub release JSON")
            }
        }
        task.resume()
    }

    // MARK: - Update Popup (Embedded HTML)
    private func showUpdatePopup(version: String, url: String, hashWarning: String) {
        let html = """
        <html>
        <head>
        <style>
        body { font-family: sans-serif; text-align:center; padding:20px; background:#f9f9f9; }
        h2 { color:#2a7fff; }
        p.warning { color:red; font-weight:bold; }
        button { margin:10px; padding:10px 20px; font-size:16px; border:none; border-radius:6px; background:#2a7fff; color:white; cursor:pointer; }
        button:hover { background:#1f5fcc; }
        </style>
        </head>
        <body>
        <h2>New Krynet MacOS Version Available!</h2>
        <p id="version-info">Latest version: \(version)</p>
        <p class="warning">\(hashWarning)</p>
        <button onclick="window.location.href='\(url)'">Download</button>
        <button onclick="window.close()">Close</button>
        </body>
        </html>
        """
        let popup = SciterLiteView(frame: NSRect(x: 50, y: 100, width: 400, height: 300))
        popup.loadHtml(html, baseUrl: URL(string: "update://popup"))
        popup.wantsLayer = true
        popup.layer?.cornerRadius = 12
        popup.layer?.masksToBounds = true
        window.contentView?.addSubview(popup)
    }
}
