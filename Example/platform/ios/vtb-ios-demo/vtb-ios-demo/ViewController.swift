//
//  ViewController.swift
//  vtb-ios-demo
//
//  Created by Christian on 19/4/18.
//  Copyright © 2018 Christian. All rights reserved.
//

import UIKit
import Metal

class View : UIView {
    override class var layerClass: AnyClass {
        get {
            return CAEAGLLayer.self
        }
    }
}

class ViewController: UIViewController {
    override func viewDidLoad() {
        super.viewDidLoad()

        let displayLink = CADisplayLink(target: self, selector: #selector(render))
        displayLink.add(to: RunLoop.current, forMode: RunLoopMode.defaultRunLoopMode)

        let device : UnsafeMutablePointer<VtbDevice>
//        let window : UnsafeRawPointer<UIView> = &self.view
//        vtbDevice_Create(window, &device)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        
    }
    
    @objc func render() {
        
    }
}

