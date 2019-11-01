/*
 * DemoViewController.m
 *
 * Copyright (c) 2014-2018 The Brenwill Workshop Ltd. (http://www.brenwill.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import "ViewController.h"
#include "vtb/App.hpp"

#import <Metal/Metal.h>

@implementation ViewController {
	CADisplayLink* _displayLink;
    
    VulkanApp app_;
}

/** Since this is a single-view app, init Vulkan when the view is loaded. */
-(void) viewDidLoad {
	[super viewDidLoad];

	self.view.contentScaleFactor = UIScreen.mainScreen.nativeScale;

	uint32_t fps = 60;
	_displayLink = [CADisplayLink displayLinkWithTarget: self selector: @selector(renderLoop)];
	[_displayLink setFrameInterval: 60 / fps];
	[_displayLink addToRunLoop: NSRunLoop.currentRunLoop forMode: NSDefaultRunLoopMode];
    
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
}

-(void) renderLoop {
    app_.Render();
}

@end

@implementation View

/** Returns a Metal-compatible layer. */
+(Class) layerClass { return [CAMetalLayer class]; }

@end
