
// This file is an inline implmenentation (II) file which is included in the
// NACocoa.m file. This is a bit special as it is marked as a .h file but
// actually contains non-inlinenable code. See NACocoa.m for more information.
// Do not include this file anywhere else!


#include "NAPreferences.h"


@implementation NACocoaWindow
- (id) initWithWindow:(NAWindow*)newWindow contentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag screen:(NSScreen *)screen{
  self = [super initWithContentRect:contentRect styleMask:aStyle backing:bufferingType defer:flag screen:screen];
  window = newWindow;
  trackingcount = 0;
  trackingarea = nil;
  [self setReleasedWhenClosed:NO];
  return self;
}
- (NAWindow*) window{
  return window;
}
- (NSTrackingArea*) trackingarea{
  return trackingarea;
}
- (NAUInt) trackingcount{
  return trackingcount;
}
- (BOOL)windowShouldClose:(id)sender{
  NABool shouldClose;
  NA_UNUSED(sender);
  naSetFlagi(&(window->flags), NA_CORE_WINDOW_FLAG_TRIES_TO_CLOSE, NA_TRUE);
  na_DispatchUIElementCommand((NA_UIElement*)window, NA_UI_COMMAND_CLOSES);
  shouldClose = !naGetFlagi(window->flags, NA_CORE_WINDOW_FLAG_PREVENT_FROM_CLOSING);
  naSetFlagi(&(window->flags), NA_CORE_WINDOW_FLAG_TRIES_TO_CLOSE | NA_CORE_WINDOW_FLAG_PREVENT_FROM_CLOSING, NA_FALSE);
  return (BOOL)shouldClose;
}
- (void)setContentRect:(NARect)rect{
  NSRect frame = [NSWindow frameRectForContentRect:naMakeNSRectWithRect(rect) styleMask:[self styleMask]];
  [super setFrame:frame display:YES];
  na_DispatchUIElementCommand((NA_UIElement*)window, NA_UI_COMMAND_RESHAPE);
}
- (void)setWindowTitle:(const NAUTF8Char*) title{
  [self setTitle:[NSString stringWithUTF8String:title]];
}
- (void)setKeepOnTop:(NABool) keepOnTop{
  if(keepOnTop){
    [self setLevel:NSFloatingWindowLevel];
  }else{
    [self setLevel:NSNormalWindowLevel];
  }
}
- (void)renewMouseTracking{
  trackingarea = [[NSTrackingArea alloc] initWithRect:[[self contentView] bounds]
      options:NSTrackingMouseMoved | NSTrackingMouseEnteredAndExited | NSTrackingActiveWhenFirstResponder
      owner:self userInfo:nil];
  [[self contentView] addTrackingArea:trackingarea];
}
- (void)clearMouseTracking{
  [[self contentView] removeTrackingArea:trackingarea];
  NA_COCOA_RELEASE(trackingarea);
  trackingarea = nil;
}
- (void)retainMouseTracking{
  trackingcount++;
  if(trackingcount == 1){
    [self setAcceptsMouseMovedEvents:YES];
    na_RenewWindowMouseTracking(window);
  }
}
- (void)releaseMouseTracking{
  trackingcount--;
  if(trackingcount == 0){
    [self setAcceptsMouseMovedEvents:NO];
    na_ClearWindowMouseTracking(window);
  }
}
- (void)mouseMoved:(NSEvent*)event{
  na_SetMouseMovedByDiff([event deltaX], -[event deltaY]);
  na_DispatchUIElementCommand((NA_UIElement*)window, NA_UI_COMMAND_MOUSE_MOVED);
//  [NSEvent setMouseCoalescingEnabled:NO];
}
- (void)mouseEntered:(NSEvent*)event{
  NA_UNUSED(event);
  na_SetMouseEnteredAtPos(naMakePosWithNSPoint([NSEvent mouseLocation]));
  na_DispatchUIElementCommand((NA_UIElement*)window, NA_UI_COMMAND_MOUSE_ENTERED);
}
- (void)mouseExited:(NSEvent*)event{
  NA_UNUSED(event);
  na_SetMouseExitedAtPos(naMakePosWithNSPoint([NSEvent mouseLocation]));
  na_DispatchUIElementCommand((NA_UIElement*)window, NA_UI_COMMAND_MOUSE_EXITED);
}
- (void)keyUp:(NSEvent*)event{
  NA_UNUSED(event);
  na_DispatchUIElementCommand((NA_UIElement*)window, NA_UI_COMMAND_KEYUP);
}
- (void)windowDidResize:(NSNotification *)notification{
  NA_UNUSED(notification);
  na_RememberWindowPosition(window);
}
- (void)windowDidMove:(NSNotification *)notification{
  NA_UNUSED(notification);
  na_RememberWindowPosition(window);
}
@end



NA_DEF NAWindow* naNewWindow(const NAUTF8Char* title, NARect rect, NABool resizeable, NAInt storageTag){
  NSRect contentRect;
  NSUInteger styleMask;
  NACocoaWindow* cocoawindow;
  NASpace* space;
  NAWindow* window = naAlloc(NAWindow);

  rect = naSetWindowStorageTag(window, storageTag, rect, resizeable);

  contentRect = naMakeNSRectWithRect(rect);
  styleMask = NAWindowStyleMaskTitled | NAWindowStyleMaskClosable | NAWindowStyleMaskMiniaturizable;
  if(resizeable){styleMask |= NAWindowStyleMaskResizable;}
  cocoawindow = [[NACocoaWindow alloc] initWithWindow:window contentRect:contentRect styleMask:styleMask backing:NSBackingStoreBuffered defer:NO screen:nil];
  
  [cocoawindow setDelegate:cocoawindow];
  [cocoawindow setTitle:[NSString stringWithUTF8String:title]];
  [cocoawindow setInitialFirstResponder:[cocoawindow contentView]];
  na_InitWindow(window, NA_COCOA_PTR_OBJC_TO_C(cocoawindow), NA_NULL, NA_FALSE, resizeable, rect);

  space = naNewSpace(rect.size);
  naSetWindowContentSpace(window, space);

  na_SetUIElementParent(window, naGetApplication());

  return window;
}



NA_DEF void na_DestructWindow(NAWindow* window){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  [cocoawindow close];
  naReleaseUIElement(window->contentSpace);
  na_ClearWindow(window);
}



NA_DEF void naSetWindowTitle(NAWindow* window, const NAUTF8Char* title){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  [cocoawindow setWindowTitle:title];
}



NA_DEF void naKeepWindowOnTop(NAWindow* window, NABool keepOnTop){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  [cocoawindow setKeepOnTop:keepOnTop];
}



NA_DEF void naSetWindowRect(NAWindow* window, NARect rect){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  NARect currect = naGetUIElementRect(window, NA_NULL, NA_FALSE);
  if(!naEqualRect(currect, rect)){
    [cocoawindow setContentRect:rect];
  }
}



NA_DEF NAUIImageResolution naGetWindowUIResolution(NAWindow* window){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  CGFloat res = naGetWindowBackingScaleFactor(cocoawindow);

  return (res == 1.) ? NA_UIIMAGE_RESOLUTION_1x : NA_UIIMAGE_RESOLUTION_2x;
}



NA_DEF void naSetWindowFirstTabElement(NAWindow* window, void* firstTabElem){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  naDefineCocoaObject(NSView, cocoaFirstTab, firstTabElem);
  [cocoawindow setInitialFirstResponder:cocoaFirstTab];
}



NA_HDEF NARect na_GetWindowAbsoluteInnerRect(NA_UIElement* window){
  NARect rect;
  NSRect contentrect;
  NSRect windowframe;
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  contentrect = [[cocoawindow contentView] frame];
  windowframe = [cocoawindow frame];
  rect.pos.x = windowframe.origin.x + contentrect.origin.x;
  rect.pos.y = windowframe.origin.y + contentrect.origin.y;
  rect.size.width = contentrect.size.width;
  rect.size.height = contentrect.size.height;
  return rect;
}



NA_HDEF NARect na_GetWindowAbsoluteOuterRect(NA_UIElement* window){
  NARect rect;
  NSRect windowframe;
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  windowframe = [cocoawindow frame];
  rect.pos.x = windowframe.origin.x;
  rect.pos.y = windowframe.origin.y;
  rect.size.width = windowframe.size.width;
  rect.size.height = windowframe.size.height;
  return rect;
}



NA_HDEF void na_RenewWindowMouseTracking(NAWindow* window){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  [cocoawindow renewMouseTracking];
}



NA_HDEF void na_ClearWindowMouseTracking(NAWindow* window){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  [cocoawindow clearMouseTracking];
}



//NA_HDEF void* na_AllocMouseTracking(NAWindow* window){
//  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
//  [cocoawindow retainMouseTracking];
//}
//
//
//
//NA_HDEF void na_DeallocMouseTracking(NAWindow* window){
//  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
//  [cocoawindow releaseMouseTracking];
//}



NA_DEF void naClearWindow(NAWindow* window){
  NA_UNUSED(window);
}



NA_DEF void naShowWindow(NAWindow* window){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  [cocoawindow makeKeyAndOrderFront:NA_NULL];
}



NA_DEF void naCloseWindow(NAWindow* window){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  [cocoawindow performClose:NA_NULL];
}



NA_DEF void naSetWindowContentSpace(NAWindow* window, void* uiElement){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  naDefineCocoaObject(NSView, cocoaelem, uiElement);
  if([cocoawindow trackingarea]){na_ClearWindowMouseTracking(window);}
  [cocoawindow setContentView:cocoaelem];
  [cocoawindow setInitialFirstResponder:[cocoawindow contentView]];
  
  window->contentSpace = (NASpace*)uiElement;
  na_SetUIElementParent(uiElement, window);
  
  if([cocoawindow trackingcount]){na_RenewWindowMouseTracking(window);}
}



NA_DEF void naSetWindowFullscreen(NAWindow* window, NABool fullScreen){
  naDefineCocoaObject(NACocoaWindow, cocoawindow, window);
  if(fullScreen != naIsWindowFullscreen(window)){
    if(fullScreen){
      window->windowedFrame = naMakeRectWithNSRect([cocoawindow frame]);
      [cocoawindow setStyleMask:NAWindowStyleMaskBorderless];
      [cocoawindow setFrame:[[NSScreen mainScreen] frame] display:YES];
      [cocoawindow setLevel:kCGScreenSaverWindowLevel];
    }else{
      [cocoawindow setStyleMask:NAWindowStyleMaskTitled | NAWindowStyleMaskClosable | NAWindowStyleMaskMiniaturizable | NAWindowStyleMaskResizable];
      [cocoawindow setFrame:naMakeNSRectWithRect(window->windowedFrame) display:YES];
      [cocoawindow setLevel:NSNormalWindowLevel];
    }
    naSetFlagi(&(window->flags), NA_CORE_WINDOW_FLAG_FULLSCREEN, fullScreen);
    // Setting the first responder again is necessary as otherwise the first
    // responder is lost.
    [cocoawindow makeFirstResponder:[cocoawindow contentView]];
  }
}



// This is free and unencumbered software released into the public domain.

// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// For more information, please refer to <http://unlicense.org/>
