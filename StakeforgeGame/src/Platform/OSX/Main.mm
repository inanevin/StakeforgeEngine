/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Game.hpp"
#include "SFG/Core/App.hpp"
#include <Cocoa/Cocoa.h>


@interface                                         OSXAppDelegate : NSObject <NSApplicationDelegate>
@property(assign) void*                             app;
@end

CVDisplayLinkRef displayLink;

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
                               const CVTimeStamp *inNow,
                               const CVTimeStamp *inOutputTime,
                               CVOptionFlags flagsIn,
                               CVOptionFlags *flagsOut,
                               void *displayLinkContext)
{
    //((SFG::App*)displayLinkContext)->Render();
    return kCVReturnSuccess;
}

@implementation OSXAppDelegate

- (instancetype)init {
    self = [super init];
    if (self) {
        //_shouldQuit = NO;
        // Register an Apple event handler for the Quit command.
        [[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
                                                           andSelector:@selector(handleQuitEvent:withReplyEvent:)
                                                         forEventClass:kCoreEventClass
                                                            andEventID:kAEQuitApplication];
    }
    return self;
}

- (void)handleQuitEvent:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)reply {
    ((SFG::App*)self.app)->RequestClose();
}

// Optionally override terminate: if needed.
- (void)terminate:(id)sender {
   // self.shouldQuit = YES;
}

- (BOOL)canAccessFolder:(NSString*)folderPath
{
    NSFileManager* fileManager = [NSFileManager defaultManager];
    return [fileManager isReadableFileAtPath:folderPath];
}

- (void)checkAndRequestFolderAccess:(NSString*)folderPath
{
    if (![self canAccessFolder:folderPath])
    {
        NSLog(@"Access denied. You may need to prompt the user or handle permission settings.");
        // Optionally, guide the user to enable permissions
    }
    else
    {
        NSLog(@"Access granted.");
    }
}

@end


SFG_API int main(int argc, char* argv[])
{
    @autoreleasepool
    {
        NSApplication* app = [NSApplication sharedApplication];
        
        /*
            Initialize platform.
         */
        
        NSString*    bundlePath    = [[NSBundle mainBundle] bundlePath];
        NSString*    parentDir    = [bundlePath stringByDeletingLastPathComponent];
        const char* cStringPath = [parentDir fileSystemRepresentation];

        if (chdir(cStringPath) != 0)
        {
            // Handle error
            NSLog(@"Failed to change working directory to: %@", parentDir);
        }

        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        [app setPresentationOptions:NSApplicationPresentationDefault];
        [app activateIgnoringOtherApps:YES];
        
        
        OSXAppDelegate* osxAppDelegate = [[OSXAppDelegate alloc] init];
        [app setDelegate:osxAppDelegate];

        [osxAppDelegate checkAndRequestFolderAccess:@"Users"];

        SFG::String errString = "";
        SFG::App sfgApp = SFG::App();
        SFG::Game game = SFG::Game(sfgApp);
        
        [osxAppDelegate setApp:&sfgApp];
       
        sfgApp.Initialize(errString);
        
        

        if (!errString.empty())
        {
            NSAlert* alert = [[NSAlert alloc] init];
            [alert setMessageText:@"Error"];
            NSString *errorMessage = [NSString stringWithCString:errString.c_str()
                                   encoding:[NSString defaultCStringEncoding]];
            [alert setInformativeText:errorMessage];
            [alert setAlertStyle:NSAlertStyleCritical];
            [alert addButtonWithTitle:@"OK"];
            [alert runModal];

            sfgApp.Shutdown();
            [app terminate:nil];
            return 0;
        }


        @autoreleasepool {
            sfgApp.Tick();
        }

        sfgApp.Shutdown();

        [app terminate:nil];
    }

    return 0;
}
