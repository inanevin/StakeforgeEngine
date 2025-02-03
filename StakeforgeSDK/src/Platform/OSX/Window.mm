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

#include "SFG/Platform/Window.hpp"
#include "SFG/Platform/InputAction.hpp"
#include "SFG/Platform/InputMappings.hpp"
#include <Cocoa/Cocoa.h>
#include <functional>


namespace SFG
{
    class Window;

namespace
{
    float SFGConvertY(float targetY, float sizeY, NSWindow* wnd)
    {
        NSScreen *currentScreen = nullptr;
        
        if(wnd == nullptr)
            currentScreen = [NSScreen mainScreen];
        else
            currentScreen = [wnd screen];
        
        NSRect screenRect = [currentScreen frame];
        
        // Convert the y-coordinate
        // Substract the window's height so that we're setting the  y-coordinate for the top-left corner
        return screenRect.size.height - targetY - sizeY;
    }

    uint32 SFGGetStyle(WindowStyle style)
    {
        NSUInteger nsStyle = 0;
        
        if(style == WindowStyle::ApplicationWindow)
        {
            nsStyle = NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable |  NSWindowStyleMaskTitled;
        }
        else if(style == WindowStyle::Borderless)
        {
            nsStyle = NSWindowStyleMaskBorderless  | NSWindowStyleMaskResizable;
        }
        
        return static_cast<uint32>(nsStyle);
    }
    
    MonitorInfo SFGGetMonitorInfo(NSScreen* screen)
    {
        MonitorInfo info;
        NSRect screenFrame = [screen frame];
        
        info.fullSize.x = screenFrame.size.width;
        info.fullSize.y = screenFrame.size.height;
        
        NSRect visibleFrame = [screen visibleFrame];
        CGFloat bottomBarHeight = visibleFrame.origin.y;
        CGFloat topBarHeight = screenFrame.size.height - bottomBarHeight -  visibleFrame.size.height;
        info.workSize.x = visibleFrame.size.width;
        info.workSize.y = visibleFrame.size.height;
        info.position.x = visibleFrame.origin.x;
        info.position.y = topBarHeight;
        
        NSDictionary *description = [screen deviceDescription];
        NSNumber *screenID = [description objectForKey:@"NSScreenNumber"];
        
        CGDirectDisplayID displayID = (CGDirectDisplayID)[screenID longValue];
        CGSize displayPhysicalSize = CGDisplayScreenSize(displayID); // in  millimeters
        
        CGFloat scaleFactor = [screen backingScaleFactor];
        NSSize displayPixelSize = [[description objectForKey:NSDeviceSize]  sizeValue];
        
        CGFloat dpi = (displayPixelSize.width / displayPhysicalSize.width) *    25.4f;
        
        info.dpi = dpi;
        info.dpiScale = scaleFactor;
        info.isPrimary = (screen == [[NSScreen screens] objectAtIndex:0]);
        
        return info;
    }
}
}

@interface SFGNSWindow : NSWindow
{
    SFG::Window* sfgWindow;
}

-(void)setSFGWindow:(SFG::Window*)sfgWnd;
@end

@implementation SFGNSWindow

- (void)setSFGWindow:(SFG::Window*)sfgWnd{
    sfgWindow = sfgWnd;
}

- (void)keyDown:(NSEvent *)event
{
    const SFG::WindowEvent ev = {
        .window = sfgWindow,
        .type = SFG::WindowEventType::Key,
        .button = SFG::InputCode(event.keyCode),
        .action = event.isARepeat ? SFG::InputAction::Repeated : SFG::InputAction::Pressed,
        .isHighFrequency = false,
    };
    
    sfgWindow->AddEvent(ev);
}

-(void)keyUp:(NSEvent *)event
{
    const SFG::WindowEvent ev = {
        .window = sfgWindow,
        .type = SFG::WindowEventType::Key,
        .button = SFG::InputCode(event.keyCode),
        .action = SFG::InputAction::Released,
        .isHighFrequency = false,
    };
    
    sfgWindow->AddEvent(ev);
}

-(void)mouseDown:(NSEvent *)event
{
    NSPoint loc = [event locationInWindow];
    const SFG::Vector2i pos = SFG::Vector2i(static_cast<int32>(loc.x), static_cast<int32>(sfgWindow->GetSize().y - loc.y));

    const SFG::WindowEvent ev = {
        .window = sfgWindow,
        .type = SFG::WindowEventType::MouseButton,
        .button = SFG::InputCode::Mouse0,
        .action = event.clickCount == 2 ? SFG::InputAction::Repeated : SFG::InputAction::Pressed,
        .value = pos,
        .isHighFrequency = false,
    };
    
    sfgWindow->AddEvent(ev);
}

-(void)rightMouseDown:(NSEvent *)event
{
    NSPoint loc = [event locationInWindow];
    const SFG::Vector2i pos = SFG::Vector2i(static_cast<int32>(loc.x), static_cast<int32>(sfgWindow->GetSize().y - loc.y));

    const SFG::WindowEvent ev = {
        .window = sfgWindow,
        .type = SFG::WindowEventType::MouseButton,
        .button = SFG::InputCode::Mouse1,
        .action = SFG::InputAction::Released,
        .value = pos,
        .isHighFrequency = false,
    };
    
    sfgWindow->AddEvent(ev);
}

-(void)otherMouseDown:(NSEvent *)event
{
    NSPoint loc = [event locationInWindow];
    const SFG::Vector2i pos = SFG::Vector2i(static_cast<int32>(loc.x), static_cast<int32>(sfgWindow->GetSize().y - loc.y));

    const SFG::WindowEvent ev = {
        .window = sfgWindow,
        .type = SFG::WindowEventType::MouseButton,
        .button = SFG::InputCode(event.buttonNumber),
        .action = event.clickCount == 2 ? SFG::InputAction::Repeated : SFG::InputAction::Pressed,
        .value = pos,
        .isHighFrequency = false,
    };
    
    sfgWindow->AddEvent(ev);
}

-(void)mouseUp:(NSEvent *)event
{
    NSPoint loc = [event locationInWindow];
    const SFG::Vector2i pos = SFG::Vector2i(static_cast<int32>(loc.x), static_cast<int32>(sfgWindow->GetSize().y - loc.y));

    const SFG::WindowEvent ev = {
        .window = sfgWindow,
        .type = SFG::WindowEventType::MouseButton,
        .button = SFG::InputCode::Mouse0,
        .action = SFG::InputAction::Released,
        .value = pos,
        .isHighFrequency = false,
    };
    
    sfgWindow->AddEvent(ev);
}

-(void)rightMouseUp:(NSEvent *)event
{
    NSPoint loc = [event locationInWindow];
    const SFG::Vector2i pos = SFG::Vector2i(static_cast<int32>(loc.x), static_cast<int32>(sfgWindow->GetSize().y - loc.y));

    const SFG::WindowEvent ev = {
        .window = sfgWindow,
        .type = SFG::WindowEventType::MouseButton,
        .button = SFG::InputCode::Mouse1,
        .action = SFG::InputAction::Released,
        .value = pos,
        .isHighFrequency = false,
    };
    
    sfgWindow->AddEvent(ev);
}

-(void)otherMouseUp:(NSEvent *)event
{
    NSPoint loc = [event locationInWindow];
    const SFG::Vector2i pos = SFG::Vector2i(static_cast<int32>(loc.x), static_cast<int32>(sfgWindow->GetSize().y - loc.y));

    const SFG::WindowEvent ev = {
        .window = sfgWindow,
        .type = SFG::WindowEventType::MouseButton,
        .button = SFG::InputCode(event.buttonNumber),
        .action = SFG::InputAction::Released,
        .value = pos,
        .isHighFrequency = false,
    };
    
    sfgWindow->AddEvent(ev);
}

-(void)scrollWheel:(NSEvent *)event
{
    SFG::Vector2i scrollAmount;

        if (event.hasPreciseScrollingDeltas) {
            // Trackpad scrolling (smooth values)
            scrollAmount.x = (int)(event.scrollingDeltaX / 10.0f);
            scrollAmount.y = (int)(event.scrollingDeltaY / 10.0f);
        } else {
            // Mouse scrolling (step-based)
            scrollAmount.x = (event.scrollingDeltaX > 0) ? 1 : (event.scrollingDeltaX < 0 ? -1 : 0);
            scrollAmount.y = (event.scrollingDeltaY > 0) ? 1 : (event.scrollingDeltaY < 0 ? -1 : 0);
        }
    
    const SFG::WindowEvent ev = {
        .window = sfgWindow,
        .type = SFG::WindowEventType::MouseWheel,
        .value = scrollAmount,
        .isHighFrequency = false,
    };
    
    sfgWindow->AddEvent(ev);
}

-(void)mouseMoved:(NSEvent *)event
{
    NSPoint loc = [event locationInWindow];
    
    static SFG::Vector2i prev = SFG::Vector2i::Zero;
    const SFG::Vector2i pos = SFG::Vector2i(static_cast<int32>(loc.x), static_cast<int32>(sfgWindow->GetSize().y - loc.y));
    
    const SFG::Vector2i delta = pos - prev;
    prev = pos;
 
    // We only send mouse delta event if the mouse is moved inside the window, or if we are running in high frequency input mode.
    bool outside = false;
    if(pos.x < 0 || pos.x > static_cast<int32>(sfgWindow->GetSize().x) || pos.y < 0 || pos.y > static_cast<int32>(sfgWindow->GetSize().y))
        outside = true;
    if(!outside || (outside && sfgWindow->GetIsHighFrequencyMode()))
    {
        const SFG::WindowEvent ev = {
            .window = sfgWindow,
            .type = SFG::WindowEventType::MouseDelta,
            .value = delta,
            .isHighFrequency = false,
        };
        sfgWindow->AddEvent(ev);
    }
    
    const SFG::Vector2i clamped = SFG::Vector2i::Clamp(pos, SFG::Vector2i::Zero, sfgWindow->GetSize());
    sfgWindow->SetMousePosition(clamped);
    sfgWindow->SetMousePositionAbs(sfgWindow->GetPosition() + pos);
}

-(BOOL)acceptsMouseMovedEvents
{
    return YES;
}
-(BOOL)acceptsFirstResponder
{
    return YES;
}
- (BOOL)canBecomeMainWindow {
    return YES;
}

- (BOOL)canBecomeKeyWindow {
    return YES;
}
@end


@interface SFGView : NSView
{
    
}

@end

@implementation SFGView

@end

@interface SFGWindowDelegate : NSObject<NSWindowDelegate>{
    std::function<void(const SFG::Vector2i& pos)> windowMoved;
    std::function<void(const SFG::Vector2ui& size)> windowResized;
    std::function<void()> windowClosed;
    std::function<void()> windowScreenChanged;
    std::function<void()> windowBackingPropsChanged;
    std::function<void()> windowResignedKey;
    std::function<void()> windowBecameKey;
}

- (void) setWindowMoved:(std::function<void(const SFG::Vector2i& pos)>)callback;
- (void) setWindowResized:(std::function<void(const SFG::Vector2ui& size)>)callback;
- (void) setWindowClosed:(std::function<void()>)callback;
- (void) setWindowScreenChange:(std::function<void()>)callback;
- (void) setWindowBackingPropsChanged:(std::function<void()>)callback;
- (void) setWindowResignedKey:(std::function<void()>)callback;
- (void) setWindowBecameKey:(std::function<void()>)callback;
@end

@implementation SFGWindowDelegate

- (void) setWindowMoved:(std::function<void (const SFG::Vector2i &)>)callback
{
    windowMoved = callback;
}

- (void) setWindowResized:(std::function<void (const SFG::Vector2ui&)>)callback
{
    windowResized = callback;
}

- (void) setWindowClosed:(std::function<void ()>)callback
{
    windowClosed = callback;
}

- (void) setWindowScreenChange:(std::function<void ()>)callback
{
    windowScreenChanged = callback;
}

- (void) setWindowBackingPropsChanged:(std::function<void ()>)callback
{
    windowBackingPropsChanged = callback;
}

- (void) setWindowResignedKey:(std::function<void ()>)callback
{
    windowResignedKey = callback;
}

- (void) setWindowBecameKey:(std::function<void ()>)callback
{
    windowBecameKey = callback;
}

- (void)windowDidMove:(NSNotification *)notification
{
    NSWindow* window = (NSWindow*)[notification object];
    NSRect frame = [window frame];
    windowMoved(SFG::Vector2i(static_cast<int32>(frame.origin.x), static_cast<int32>(frame.origin.y)));
}

- (void)windowDidResize:(NSNotification *)notification
{
    NSWindow* window = (NSWindow*)[notification object];
    NSRect frame = [window frame];
    windowResized(SFG::Vector2ui(static_cast<uint32>(frame.size.width), static_cast<uint32>(frame.size.height)));
}

-(void)windowWillClose:(NSNotification *)notification
{
    windowClosed();
}

-(void)windowDidChangeScreen:(NSNotification *)notification
{
    windowScreenChanged();
}
-(void)windowDidChangeBackingProperties:(NSNotification *)notification
{
    windowBackingPropsChanged();
}

-(void)windowDidBecomeKey:(NSNotification *)notification
{
    windowBecameKey();
}

-(void)windowDidResignKey:(NSNotification *)notification
{
    windowResignedKey();
}

@end

namespace SFG
{

    Window* Window::Create(uint32 windowID, const Vector2i& pos, const Vector2ui& size, const char* title, WindowStyle style)
    {

        NSRect frame = NSMakeRect(pos.x, SFGConvertY(pos.y, size.y, nullptr), size.x, size.y);
        
        SFGNSWindow* wnd = [[SFGNSWindow alloc] initWithContentRect:frame styleMask:SFGGetStyle(style) backing:NSBackingStoreBuffered defer:NO];
        
        if(!wnd)
            return nullptr;

        [wnd retain];
        NSScreen* screen = [wnd screen];
        
        Window* window = new Window();
        window->m_size = size;
        window->m_trueSize = size;
        window->m_id = windowID;
        window->m_style = style;
        window->m_position = pos;
        window->m_windowHandle = static_cast<void*>(wnd);
        window->m_monitorInfo = SFGGetMonitorInfo(screen);
        
        [wnd setSFGWindow:window];
        
        SFGView* view = [[SFGView alloc] init];
        [view retain];
        [wnd setContentView:view];
        
        /*
            Window delegate and it's callbacks.
         */
        SFGWindowDelegate* delegate = [[SFGWindowDelegate alloc] init];
        [delegate retain];
        [wnd setDelegate:delegate];
        
        [delegate setWindowMoved:[window](const Vector2i& pos) {
            window->m_position = pos;
        }];
        
        [delegate setWindowResized:[window](const Vector2ui& size) {
            window->m_size = size;
            window->m_trueSize = size;
            window->m_sizeDirty = true;
        }];
        
        [delegate setWindowClosed:[window](){
            window->Close();
        }];
        
        [delegate setWindowScreenChange:[window, wnd](){
            NSScreen* screen = [wnd screen];
            window->m_monitorInfo = SFGGetMonitorInfo(screen);
        }];
        
        [delegate setWindowBackingPropsChanged:[window, wnd](){
            NSScreen* screen = [wnd screen];
            window->m_monitorInfo = SFGGetMonitorInfo(screen);
        }];
        
        [delegate setWindowResignedKey:[window](){
            window->m_hasFocus = false;
        }];
        
        [delegate setWindowBecameKey:[window](){
            window->m_hasFocus = true;
        }];
        
        window->BringToFront();
        return window;
    }

    void Window::Destroy(Window* window)
    {
        SFGNSWindow* sfgNSWindow = static_cast<SFGNSWindow*>(window->m_windowHandle);
        
        // Free delegate if exists.
        SFGWindowDelegate* delegate = static_cast<SFGWindowDelegate*>([sfgNSWindow delegate]);
         [delegate release];
        
        // Free NSView
        NSView* nsView = [sfgNSWindow contentView];
        SFGView* view = static_cast<SFGView*>(nsView);
        [view release];
        
        // Free NSWindow
        [sfgNSWindow close];
        [sfgNSWindow release];
        window->m_windowHandle = nullptr;
    }

    void Window::SetPosition(const Vector2i &pos)
    {
        SFGNSWindow* wnd = static_cast<SFGNSWindow*>(m_windowHandle);
        NSPoint p;
        p.x = pos.x;
        p.y = SFGConvertY(pos.y, m_size.y, wnd);
        [wnd setFrameOrigin:p];
    }

    void Window::SetSize(const Vector2ui &size)
    {
        SFGNSWindow* wnd = static_cast<SFGNSWindow*>(m_windowHandle);
        NSSize sz;
        sz.width = static_cast<CGFloat>(size.x);
        sz.height = static_cast<CGFloat>(size.y);
        [wnd setContentSize:sz];
        m_size = size;
        m_trueSize = size;
    }

    void Window::SetStyle(WindowStyle style)
    {
        SFGNSWindow* wnd = static_cast<SFGNSWindow*>(m_windowHandle);
        m_style = style;
        [wnd setStyleMask:SFGGetStyle(style)];
    }

    void Window::BringToFront()
    {
        SFGNSWindow* wnd = static_cast<SFGNSWindow*>(m_windowHandle);
        [wnd makeKeyAndOrderFront:nil];
    }

} // namespace SFG
