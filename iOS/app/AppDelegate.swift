import UIKit
import SciterLit

@main
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?
    
    func application(
        _ application: UIApplication,
        didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?
    ) -> Bool {

        // Create main window
        window = UIWindow(frame: UIScreen.main.bounds)
        let sciterVC = SciterLiteViewController()
        window?.rootViewController = sciterVC
        window?.makeKeyAndVisible()
        
        return true
    }
}

// MARK: - SciterLite View Controller
class SciterLiteViewController: UIViewController {
    
    private var sciterView: SciterLiteView!
    private var versionInfo: VersionInfo?
    
    override func loadView() {
        sciterView = SciterLiteView(frame: .zero)
        sciterView.isOpaque = true
        sciterView.backgroundColor = .black
        sciterView.contentMode = .scaleAspectFit
        sciterView.scrollBounce = false
        sciterView.touchPassthrough = false
        view = sciterView
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        loadLocalVersion()
        loadKrynet()
        checkForUpdates()
        print("🚀 Krynet iOS Sciter.Lite Client Loaded")
    }
    
    // MARK: - Version Info
    struct VersionInfo: Codable {
        let version: String
        let build: String
        let platform: String
        let hash: String
    }
    
    private func loadLocalVersion() {
        if let url = Bundle.main.url(forResource: "version", withExtension: "json"),
           let data = try? Data(contentsOf: url),
           let info = try? JSONDecoder().decode(VersionInfo.self, from: data) {
            versionInfo = info
        }
    }
    
    // MARK: - Load Krynet Web Client
    private func loadKrynet() {
        if let htmlPath = Bundle.main.path(forResource: "krynet", ofType: "htm") {
            let htmlURL = URL(fileURLWithPath: htmlPath)
            sciterView.loadUrl(URLRequest(url: htmlURL))
        } else if let url = URL(string: "https://krynet.ai/web") {
            sciterView.loadUrl(URLRequest(url: url))
        }
    }
    
    // MARK: - Update Check
    private func checkForUpdates() {
        guard let local = versionInfo,
              let url = URL(string: "https://api.github.com/repos/Krynet-LLC/Krynet/releases/latest") else { return }
        
        let task = URLSession.shared.dataTask(with: url) { data, _, error in
            guard let data = data, error == nil else { return }
            do {
                if let release = try JSONSerialization.jsonObject(with: data) as? [String: Any],
                   let latestTag = release["tag_name"] as? String,
                   let assets = release["assets"] as? [[String: Any]] {
                    
                    var iosAssetUrl: String?
                    var githubHash: String?
                    
                    for asset in assets {
                        if let name = asset["name"] as? String, name.contains("iOS") || name.contains(".ipa") {
                            iosAssetUrl = asset["browser_download_url"] as? String
                            githubHash = asset["sha256"] as? String
                            break
                        }
                    }
                    
                    if let latestTag = latestTag as String?,
                       let iosUrl = iosAssetUrl,
                       latestTag != local.version {
                        
                        var hashWarning = ""
                        if let githubHash = githubHash, githubHash != local.hash {
                            hashWarning = "⚠ Warning: This client hash differs from the official release!"
                        }
                        
                        DispatchQueue.main.async {
                            self.showUpdatePopup(version: latestTag, url: iosUrl, hashWarning: hashWarning)
                        }
                    }
                }
            } catch {
                print("❌ Failed to parse GitHub release JSON")
            }
        }
        task.resume()
    }
    
    // MARK: - Embedded Update Popup
    private func showUpdatePopup(version: String, url: String, hashWarning: String) {
        let html = """
        <html>
        <head>
        <title>Krynet Update</title>
        <style>
            body { font-family: sans-serif; text-align:center; padding:20px; background:#f9f9f9; }
            h2 { color:#2a7fff; }
            p.warning { color:red; font-weight:bold; }
            button { margin:10px; padding:10px 20px; font-size:16px; border:none; border-radius:6px; background:#2a7fff; color:white; cursor:pointer; }
            button:hover { background:#1f5fcc; }
        </style>
        </head>
        <body>
        <h2>New Krynet iOS Version Available!</h2>
        <p id="version-info">Latest version: \(version)</p>
        <p class="warning">\(hashWarning)</p>
        <button onclick="window.location.href='\(url)'">Download IPA</button>
        <button onclick="window.close()">Close</button>
        </body>
        </html>
        """
        let popup = SciterLiteView(frame: CGRect(x: 50, y: 100, width: 300, height: 300))
        popup.loadHtml(html, baseUrl: URL(string: "update://popup"))
        popup.backgroundColor = .white
        popup.layer.cornerRadius = 12
        popup.layer.masksToBounds = true
        self.view.addSubview(popup)
    }
    
    // MARK: - External Link Helper
    @objc func openExternalLink(_ urlString: String) {
        if let url = URL(string: urlString) {
            UIApplication.shared.open(url)
        }
    }
    
    // MARK: - Orientation
    override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        return .allButUpsideDown
    }
    
    override func viewWillTransition(to size: CGSize, with coordinator: UIViewControllerTransitionCoordinator) {
        super.viewWillTransition(to: size, with: coordinator)
        sciterView.frame = CGRect(origin: .zero, size: size)
    }
}
