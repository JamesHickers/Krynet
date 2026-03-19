import UIKit
import SciterLit

class SciterLiteViewController: UIViewController {

    private var sciterView: SciterLiteView!

    override func loadView() {
        // Sciter.Lite configuration - mobile optimized
        sciterView = SciterLiteView(frame: .zero)
        
        // Native Krynet app behavior (passed to Sciter JS)
        sciterView.setAttribute("isKrynetApp", value: "true")
        
        // Mobile optimizations
        sciterView.isOpaque = true
        sciterView.backgroundColor = .black
        sciterView.contentMode = .scaleAspectFit
        
        // Native feel - disable bounces/navigation gestures
        sciterView.scrollBounce = false
        sciterView.touchPassthrough = false
        
        view = sciterView
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        loadKrynet()
        print("🚀 Krynet iOS Sciter.Lite Client Loaded")
    }

    override var prefersStatusBarHidden: Bool { true }

    private func loadKrynet() {
        // Load local krynet.htm (bundled UI) - offline capable
        if let htmlPath = Bundle.main.path(forResource: "krynet", ofType: "htm") {
            let htmlURL = URL(fileURLWithPath: htmlPath)
            let request = URLRequest(url: htmlURL)
            sciterView.loadUrl(request)
        } else {
            // Fallback: remote URL if local missing
            if let url = URL(string: "https://krynet.ai/web") {
                let request = URLRequest(url: url)
                sciterView.loadUrl(request)
            }
        }
    }

    // MARK: - Sciter Native Callbacks (Replaces WKNavigationDelegate)
    
    // External links → Safari (called from Sciter JS)
    @objc func openExternalLink(_ urlString: String) {
        if let url = URL(string: urlString) {
            UIApplication.shared.open(url)
        }
    }
}
