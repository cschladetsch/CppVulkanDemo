//
//  ViewController.m
//  macos-app
//

#import <QuartzCore/CAMetalLayer.h>

#import "ViewController.h"
#include "App.hpp"

#import <Metal/Metal.h>

@implementation ViewController {
    CVDisplayLinkRef _displayLink;
    
    VulkanApp app_;
}

/** Since this is a single-view app, init Vulkan when the view is loaded. */
-(void) viewDidLoad {
    [super viewDidLoad];
    
    self.view.wantsLayer = YES;        // Back the view with a layer created by the makeBackingLayer method.
    
    CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
    CVDisplayLinkSetOutputCallback(_displayLink, &DisplayLinkCallback, &app_);
    
    NSString *textureName = [[NSBundle mainBundle] pathForResource:@"chalet" ofType:@"jpg"];
    NSString *modelName = [[NSBundle mainBundle] pathForResource:@"chalet" ofType:@"obj"];
    NSString *fragName = [[NSBundle mainBundle] pathForResource:@"frag" ofType:@"spv"];
    NSString *vertName = [[NSBundle mainBundle] pathForResource:@"vert" ofType:@"spv"];
    
    NSData *frag = [NSData dataWithContentsOfFile: fragName];
    NSData *vert = [NSData dataWithContentsOfFile: vertName];
    
    std::vector<char> fragData([frag length]);
    std::vector<char> vertData([vert length]);
    
    [frag getBytes:fragData.data() length:fragData.size()];
    [vert getBytes:vertData.data() length:vertData.size()];
    
    const char *texName = [textureName UTF8String];
    const char *modName = [modelName UTF8String];
    app_.Create((__bridge void *)self.view, vertData, fragData, texName, modName);

    CVDisplayLinkStart(_displayLink);
}

/** Rendering loop callback function for use with a CVDisplayLink. */
static CVReturn DisplayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp* now,
                                    const CVTimeStamp* outputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags* flagsOut,
                                    void* target) {
    VulkanApp *app = (VulkanApp *)target;
    app->Render();
    return kCVReturnSuccess;
}

@end

@implementation VulkanView

/** Indicates that the view wants to draw using the backing layer instead of using drawRect:.  */
-(BOOL) wantsUpdateLayer { return YES; }

/** Returns a Metal-compatible layer. */
+(Class) layerClass { return [CAMetalLayer class]; }

/** If the wantsLayer property is set to YES, this method will be invoked to return a layer instance. */
-(CALayer*) makeBackingLayer {
    CALayer* layer = [self.class.layerClass layer];
    CGSize viewScale = [self convertSizeToBacking: CGSizeMake(1.0, 1.0)];
    layer.contentsScale = MIN(viewScale.width, viewScale.height);
    return layer;
}

@end
