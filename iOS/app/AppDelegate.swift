import UIKit
import SciterLit

@main
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?
    var sciterView: SciterLiteView?

    func application(
        _ application: UIApplication,
        didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?
    ) -> Bool {

        // Create main window
        window = UIWindow(frame: UIScreen.main.bounds)
        
        // Create Sciter.Lite view controller
        let sciterVC = SciterLiteViewController()
        window?.rootViewController = sciterVC
        window?.makeKeyAndVisible()
        
        return true
    }
}

// MARK: - Sciter.Lite View Controller
class SciterLiteViewController: UIViewController {
    
    var sciterView: SciterLiteView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupSciterLiteView()
    }
    
    private func setupSciterLiteView() {
        // Initialize Sciter.Lite view (mobile optimized)
        sciterView = SciterLiteView(frame: view.bounds)
        sciterView.autoresizingMask = [.flexibleWidth, .flexibleHeight]
        sciterView.backgroundColor = .black
        
        // Load Krynet UI (local HTML/CSS/JS)
        if let htmlPath = Bundle.main.path(forResource: "krynet", ofType: "htm") {
            let htmlURL = URL(fileURLWithPath: htmlPath)
            sciterView.loadUrl(URLRequest(url: htmlURL))
        } else {
            print("❌ krynet.htm not found - add to bundle!")
        }
        
        // Mobile optimizations
        sciterView.isOpaque = true
        sciterView.contentMode = .scaleAspectFit
        
        view.addSubview(sciterView)
    }
    
    override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        return .allButUpsideDown  // iOS standard
    }
    
    override func viewWillTransition(to size: CGSize, with coordinator: UIViewControllerTransitionCoordinator) {
        super.viewWillTransition(to: size, with: coordinator)
        // Handle rotation - Sciter.Lite auto-resizes
        sciterView.frame = CGRect(origin: .zero, size: size)
    }
}
