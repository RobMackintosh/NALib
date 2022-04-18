
// This file is an inline implmenentation (II) file which is included in the
// NAWINAPI.c file. This is a bit special as it is marked as a .h file but
// actually contains non-inlinenable code. See NAWINAPI.c for more information.
// Do not include this file anywhere else!



NAWINAPICallbackInfo naSliderWINAPIProc(void* uiElement, UINT message, WPARAM wParam, LPARAM lParam){
  NAWINAPICallbackInfo info = {NA_FALSE, 0};
  NSSlider* slider = (NSSlider*)uiElement;

  switch(message){
  case WM_WINDOWPOSCHANGING:
  case WM_CHILDACTIVATE:
  case WM_WINDOWPOSCHANGED:
  case WM_MOVE:
  case WM_SHOWWINDOW:
  case WM_PAINT:
  case WM_NCPAINT:
  case WM_ERASEBKGND:
  case WM_NCHITTEST:
  case WM_SETCURSOR:
  case WM_MOUSEMOVE:
  case WM_MOUSELEAVE:
  case WM_MOUSEACTIVATE:
  case WM_IME_SETCONTEXT:
  case WM_DESTROY:
  case WM_NCDESTROY:
  case WM_CAPTURECHANGED:
  case WM_IME_NOTIFY:
  case WM_TIMER:
  case TBM_SETPOS:  // (WM_USER + 5)
  case TBM_GETPOS: // (WM_USER + 0)
    break;

  case WM_SETFOCUS:
  case WM_KILLFOCUS:
    // We do not display any caret.
    info.hasBeenHandeled = NA_TRUE;
    info.result = 0;
    break;

  case WM_LBUTTONDOWN:
    slider->sliderInMovement = true;
    info.hasBeenHandeled = NA_TRUE;
    info.result = 0;
    break;

  case WM_LBUTTONUP:
    slider->sliderInMovement = false;
    info.hasBeenHandeled = NA_TRUE;
    info.result = 0;
    break;

  default:
    printf("Uncaught Slider message" NA_NL);
    break;
  }
  
  return info;
}



NAWINAPICallbackInfo naSliderWINAPIScroll(void* uiElement, WPARAM wParam){
  NAWINAPICallbackInfo info = {NA_TRUE, 0};

  //int16 lo = LOWORD(wParam);
  //int16 hi = HIWORD(wParam);

  NAPos pos = naGetMousePos(naGetMouseStatus());
  NARect rect = naGetUIElementRect(uiElement, naGetApplication(), NA_FALSE);

  naSetSliderValue(uiElement, (double)pos.x / (double)rect.size.width);
  na_DispatchUIElementCommand(uiElement, NA_UI_COMMAND_EDITED);

  return info;
}



NA_DEF NASlider* naNewSlider(double width){
  NAWINAPISlider* winapiSlider = naNew(NAWINAPISlider);

  TCHAR* systemText = naAllocSystemStringWithUTF8String("Slider");

	HWND nativePtr = CreateWindow(
		TRACKBAR_CLASS,
    systemText,
    WS_CHILD | WS_VISIBLE | TBS_NOTICKS,
		0,
    0,
    (int)width,
    24,
		naGetApplicationOffscreenWindow(),
    NULL,
    (HINSTANCE)naGetUIElementNativePtr(naGetApplication()),
    NULL);
  
  naFree(systemText);

  SendMessage(nativePtr, TBM_SETRANGEMIN , 
    (WPARAM) TRUE,
    (LPARAM) NA_ZERO_u32);
  SendMessage(nativePtr, TBM_SETRANGEMAX, 
    (WPARAM) TRUE,
    (LPARAM) NA_MAX_i32);

  //SendMessage(hWnd, TBM_SETPAGESIZE, 
  //    0, (LPARAM) 4);                  // new page size 

  //SendMessage(hWnd, TBM_SETSEL, 
  //    (WPARAM) FALSE,                  // redraw flag 
  //    (LPARAM) MAKELONG(0x3000, 0x7000)); 
  //      

  //SetFocus(hWnd); 

  NAWINAPIApplication* app = (NAWINAPIApplication*)naGetApplication();
  WNDPROC oldproc = (WNDPROC)SetWindowLongPtr(nativePtr, GWLP_WNDPROC, (LONG_PTR)naWINAPIWindowCallback);
  if(!app->oldSliderWindowProc){app->oldSliderWindowProc = oldproc;}

  na_InitSlider(&(winapiSlider->slider), nativePtr);

  return (NASlider*)winapiSlider;
}



NA_DEF void na_DestructWINAPISlider(NAWINAPISlider* winapiSlider){
  na_ClearSlider((NASlider*)winapiSlider);
}



NA_DEF void naSetSliderVisible(NASlider* slider, NABool visible){
  // todo
}
NA_DEF void naSetSliderEnabled(NASlider* slider, NABool enabled){
  // todo
}



NA_API double naGetSliderValue(const NASlider* slider){
  int32 sliderValue = (int32)SendMessage(naGetUIElementNativePtrConst(slider), TBM_GETPOS, 0, 0); 
  double plainValue = (double)sliderValue / (double)NA_MAX_i32;
  return plainValue * (slider->max - slider->min) + slider->min;
}



NA_DEF double naGetSliderStaticValue(const NASlider* slider){
  return slider->staticValue;
}



NA_API void naSetSliderValue(NASlider* slider, double value){
  double plainValue = (value - slider->min) / (slider->max - slider->min);
  int32 sliderValue = (int32)(plainValue * (double)NA_MAX_i32);
  SendMessage(naGetUIElementNativePtr(slider), TBM_SETPOS, 
    (WPARAM) TRUE, // redraw flag 
    (LPARAM) sliderValue);
  if(!slider->sliderInMovement){
    slider->staticValue = value;
  }
}



NA_API void naSetSliderRange(NASlider* slider, double min, double max, NAInt tickCount){
  // todo
  slider->min = min;
  slider->max = max;
}



NA_HDEF NARect na_GetSliderAbsoluteInnerRect(const NA_UIElement* slider){
  RECT contentRect;
  NARect screenRect;
  NARect rect;
  GetClientRect(naGetUIElementNativePtrConst(slider), &contentRect);

  POINT testPoint = {0, 0};

  ClientToScreen(naGetUIElementNativePtrConst(slider), &testPoint);
  screenRect = naGetMainScreenRect();

  rect.pos.x = testPoint.x;
  rect.pos.y = (double)screenRect.size.height - ((double)testPoint.y + ((double)contentRect.bottom - (double)contentRect.top));
  rect.size.width = (double)contentRect.right - (double)contentRect.left;
  rect.size.height = (double)contentRect.bottom - (double)contentRect.top;
  return rect;
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
