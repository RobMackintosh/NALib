
// This file is an inline implmenentation (II) file which is included in the
// NAWINAPI.c file. This is a bit special as it is marked as a .h file but
// actually contains non-inlinenable code. See NAWINAPI.c for more information.
// Do not include this file anywhere else!

#define NA_WINAPI_BUTTON_BORDERLESS 0x01
#define NA_WINAPI_BUTTON_STATEFUL   0x02
#define NA_WINAPI_BUTTON_IMAGE      0x04
#define NA_WINAPI_BUTTON_STATE      0x08



NAWINAPICallbackInfo naButtonWINAPIProc(void* uiElement, UINT message, WPARAM wParam, LPARAM lParam){
  NAWINAPICallbackInfo info = {NA_FALSE, 0};

  switch(message){
  case WM_SETFONT:
  case WM_WINDOWPOSCHANGING:
  case WM_CHILDACTIVATE:
  case WM_WINDOWPOSCHANGED:
  case WM_MOVE:
  case WM_SHOWWINDOW:
  case WM_PAINT:
  case WM_NCPAINT:
  case WM_ERASEBKGND:
  case WM_GETTEXTLENGTH:
  case WM_GETTEXT:
  case WM_NCHITTEST:
  case WM_SETCURSOR:
  case WM_MOUSEMOVE: // captured in naUIElementWINAPIProc
  case WM_MOUSELEAVE: // captured in naUIElementWINAPIProc
  case WM_MOUSEACTIVATE:
  case WM_LBUTTONDOWN:
  case WM_IME_SETCONTEXT:
  case WM_CANCELMODE:
  case WM_CAPTURECHANGED:
  case WM_STYLECHANGING:
  case WM_STYLECHANGED:
  case WM_IME_NOTIFY:
  case WM_LBUTTONUP:
  case WM_LBUTTONDBLCLK:
  case WM_DESTROY:
  case WM_NCDESTROY:
  case BM_SETSTATE:
  case BM_SETCHECK:
  case BM_GETSTATE:
    break;

  // Menu messages
  case WM_ENTERMENULOOP:
  case WM_INITMENU:
  case WM_INITMENUPOPUP:
  case 0x93:  // undocumented
  case 0x94:  // undocumented
  case WM_ENTERIDLE:
  case WM_MENUSELECT:
  case WM_UNINITMENUPOPUP:
  case WM_EXITMENULOOP:
    break;

  case WM_SETFOCUS:
  case WM_KILLFOCUS:
    // We do not display any caret.
    info.hasBeenHandeled = NA_TRUE;
    info.result = 0;
    break;

  default:
    printf("Uncaught Button message" NA_NL);
    break;
  }
  
  return info;
}



NA_HDEF NABool na_GetButtonState(const NAWINAPIButton* winapiButton){
  return naGetFlagu32(winapiButton->state, NA_WINAPI_BUTTON_STATE);
}

NA_HDEF void na_SetButtonState(NAWINAPIButton* winapiButton, NABool state){
  naSetFlagu32(&(winapiButton->state), NA_WINAPI_BUTTON_STATE, state);
  SendMessage(naGetUIElementNativePtr(winapiButton), BM_SETSTATE, (WPARAM)state, (LPARAM)NA_NULL);
}



NABool naButtonWINAPINotify(void* uiElement, WORD notificationCode){
  NABool hasBeenHandeled = NA_FALSE;
  NAWINAPIButton* winapiButton = (NAWINAPIButton*)uiElement;

  switch(notificationCode){
  case BN_CLICKED:
  case BN_DOUBLECLICKED:
    if(naGetFlagu32(winapiButton->state, NA_WINAPI_BUTTON_STATEFUL)){
      na_SetButtonState(winapiButton, !na_GetButtonState(winapiButton));
    }
    na_DispatchUIElementCommand(uiElement, NA_UI_COMMAND_PRESSED);
    hasBeenHandeled = NA_TRUE;
    break;
  default:
    printf("Uncaught Button notification" NA_NL);
    break;
  }
  return hasBeenHandeled;
}



NAWINAPICallbackInfo naButtonWINAPIDrawItem (void* uiElement, DRAWITEMSTRUCT* drawitemstruct){
  HBITMAP hOldBitmap;
  NASizei size1x;
  NASizei buttonsize;
  NAPosi offset;
  const NABabyImage* foreImage;
  NAByte* backBuffer;
  HBITMAP hBackBitmap;
  NABabyImage* backImage;
  NABabyImage* blendedImage;
  NAByte* blendedBuffer;
  HBITMAP hBlendedBitmap;

  HDC hMemDC = CreateCompatibleDC(drawitemstruct->hDC);

  NAWINAPIButton* winapiButton = (NAWINAPIButton*)uiElement;
  NAWINAPICallbackInfo info = {NA_TRUE, TRUE};

  CallWindowProc(na_GetApplicationOldButtonWindowProc(), naGetUIElementNativePtr(uiElement), WM_ERASEBKGND, (WPARAM)drawitemstruct->hDC, (LPARAM)NA_NULL);

  if(!naGetFlagu32(winapiButton->state, NA_WINAPI_BUTTON_BORDERLESS)){
    long oldstyle = (long)GetWindowLongPtr(naGetUIElementNativePtr(uiElement), GWL_STYLE);
    long newstyle = (oldstyle & ~BS_OWNERDRAW) | BS_TEXT | BS_CENTER | BS_VCENTER;
    SetWindowLongPtr(naGetUIElementNativePtr(uiElement), GWL_STYLE, (LONG_PTR)newstyle);
    // Oh boi. That is one hell of a hidden feature. Usually, the WM_PAINT message does not
    // use wParam and lParam at all. But there are some common controls (and buttons seems to
    // be one of them) which in fact only work if you send the device context in wParam.
    CallWindowProc(na_GetApplicationOldButtonWindowProc(), naGetUIElementNativePtr(uiElement), WM_PAINT, (WPARAM)drawitemstruct->hDC, (LPARAM)NA_NULL);
    SetWindowLongPtr(naGetUIElementNativePtr(uiElement), GWL_STYLE, (LONG_PTR)oldstyle);
  }

  if(winapiButton->button.uiImage)
  {
    size1x = naGetUIImage1xSize(winapiButton->button.uiImage);

    buttonsize = naMakeSizei(
      drawitemstruct->rcItem.right - drawitemstruct->rcItem.left,
      drawitemstruct->rcItem.bottom - drawitemstruct->rcItem.top);
    offset = naMakePosi(
      (buttonsize.width - size1x.width) / 2,
      (buttonsize.height - size1x.height) / 2);

    NABool alt = NA_FALSE;
    LRESULT result = SendMessage(naGetUIElementNativePtr(winapiButton), BM_GETSTATE, (WPARAM)NA_NULL, (LPARAM)NA_NULL);
    NABool pushed = (result & BST_PUSHED) == BST_PUSHED;
    if(naGetFlagu32(winapiButton->state, NA_WINAPI_BUTTON_STATEFUL))
    {
      if(na_GetButtonState(winapiButton)){
        alt = NA_TRUE;
      }
      // unfortuantely, the following does not work as the button is not drawn
      // with the correct state when releasing the mouse.
      //if(pushed){alt = !alt;}
    }else{
      alt = pushed;
    }

    if(alt){
      foreImage = na_GetUIImageBabyImage(winapiButton->button.uiImage, NA_UIIMAGE_RESOLUTION_1x, NA_UIIMAGE_SKIN_LIGHT, NA_UIIMAGE_STATUS_IDLE);
    }else{
      foreImage = na_GetUIImageBabyImage(winapiButton->button.uiImage, NA_UIIMAGE_RESOLUTION_1x, NA_UIIMAGE_SKIN_LIGHT,  NA_UIIMAGE_STATUS_IDLE);
    }

    // We store the background where the image will be placed.
    backBuffer = naMalloc(size1x.width * size1x.height * 4);
    hBackBitmap = CreateBitmap((int)size1x.width, (int)size1x.height, 1, 32, backBuffer);
    hOldBitmap = SelectObject(hMemDC, hBackBitmap);
    BitBlt(hMemDC, 0, 0, (int)size1x.width, (int)size1x.height, drawitemstruct->hDC, (int)offset.x, (int)offset.y, SRCCOPY);
    backImage = naCreateBabyImageFromNativeImage(hBackBitmap);

    // Now we blend manually the foreground to the background.
    blendedImage = naCreateBabyImageWithBlend(backImage, foreImage, NA_BLEND_OVERLAY, 1.f);
    blendedBuffer = naMalloc(size1x.width * size1x.height * 4);
    naConvertBabyImageTou8(blendedImage, blendedBuffer, NA_TRUE, NA_COLOR_BUFFER_BGR0);
    hBlendedBitmap = CreateBitmap((int)size1x.width, (int)size1x.height, 1, 32, blendedBuffer);

    // Finally, we put the blended image onscreen.
    SelectObject(hMemDC, hBlendedBitmap);
    BitBlt(drawitemstruct->hDC, (int)offset.x, (int)offset.y, (int)size1x.width, (int)size1x.height, hMemDC, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBitmap);

    // Deleting the blended objects and buffers
    DeleteObject(hBlendedBitmap);
    naFree(blendedBuffer);
    naReleaseBabyImage(blendedImage);

    // Deleting background objects and buffers
    DeleteObject(hBackBitmap);
    naFree(backBuffer);
    naReleaseBabyImage(backImage);
  }

  // Deleting device contexts
  DeleteDC(hMemDC);
  ReleaseDC(drawitemstruct->hwndItem, drawitemstruct->hDC);

  return info;
}



NA_DEF NAButton* naNewTextButton(const NAUTF8Char* text, double width, uint32 flags){
  NAWINAPIButton* winapiButton = naNew(NAWINAPIButton);

  #if NA_DEBUG
    if(naGetFlagu32(flags, NA_BUTTON_BORDERLESS))
      naError("Borderless Text buttons should not be used as they can not be distinguished.");
  #endif

  TCHAR* systemText = naAllocSystemStringWithUTF8String(text);

  winapiButton->rect = naMakeRectS(0., 0., width, 24.);
  double uiScale = naGetUIElementResolutionFactor(NA_NULL);

	HWND nativePtr = CreateWindow(
		TEXT("BUTTON"),
    systemText,
    WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER | BS_TEXT | BS_PUSHBUTTON,
		0,
    0,
    (int)(winapiButton->rect.size.width * uiScale),
    (int)(winapiButton->rect.size.height * uiScale),
    naGetApplicationOffscreenWindow(),
    NULL,
    (HINSTANCE)naGetUIElementNativePtr(naGetApplication()),
    NULL);
  
  naFree(systemText);

  SendMessage(nativePtr, WM_SETFONT, (WPARAM)naGetFontNativePointer(naGetSystemFont()), MAKELPARAM(TRUE, 0));

  NAWINAPIApplication* app = (NAWINAPIApplication*)naGetApplication();
  WNDPROC oldproc = (WNDPROC)SetWindowLongPtr(nativePtr, GWLP_WNDPROC, (LONG_PTR)naWINAPIWindowCallback);
  if(!app->oldButtonWindowProc){app->oldButtonWindowProc = oldproc;}

  na_InitButton((NAButton*)winapiButton, nativePtr, NA_NULL, flags);
  winapiButton->state = 0;
  #if NA_DEBUG
    if(naGetFlagu32(flags, NA_BUTTON_BORDERLESS))
      naError("Borderless text buttons have undefined behaviour. Use Labels with reactions instead.");
  #endif
  naSetFlagu32(&(winapiButton->state), NA_WINAPI_BUTTON_BORDERLESS, naGetFlagu32(flags, NA_BUTTON_BORDERLESS)); 
  naSetFlagu32(&(winapiButton->state), NA_WINAPI_BUTTON_STATEFUL, naGetFlagu32(flags, NA_BUTTON_STATEFUL)); 
  naSetFlagu32(&(winapiButton->state), NA_WINAPI_BUTTON_IMAGE, NA_FALSE); 

  return (NAButton*)winapiButton;
}



NA_DEF NAButton* naNewImageButton(const NAUIImage* uiImage, NASize size, uint32 flags){
  NAWINAPIButton* winapiButton = naNew(NAWINAPIButton);
  
  #if NA_DEBUG
    if(!uiImage)
      naError("uiImage is null");
  #endif

  winapiButton->rect = naMakeRect(naMakePos(0., 0.), size);
  double uiScale = naGetUIElementResolutionFactor(NA_NULL);

  HWND nativePtr = CreateWindow(
    TEXT("BUTTON"),
    TEXT(""),
    WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
    0,
    0,
    (int)(winapiButton->rect.size.width * uiScale),
    (int)(winapiButton->rect.size.height * uiScale),
    naGetApplicationOffscreenWindow(),
    NULL,
    (HINSTANCE)naGetUIElementNativePtr(naGetApplication()),
    NULL);
    
  NAWINAPIApplication* app = (NAWINAPIApplication*)naGetApplication();
  WNDPROC oldproc = (WNDPROC)SetWindowLongPtr(nativePtr, GWLP_WNDPROC, (LONG_PTR)naWINAPIWindowCallback);
  if(!app->oldButtonWindowProc){app->oldButtonWindowProc = oldproc;}

  na_InitButton((NAButton*)winapiButton, nativePtr, uiImage, flags);
  winapiButton->state = 0;
  naSetFlagu32(&(winapiButton->state), NA_WINAPI_BUTTON_BORDERLESS, naGetFlagu32(flags, NA_BUTTON_BORDERLESS)); 
  naSetFlagu32(&(winapiButton->state), NA_WINAPI_BUTTON_STATEFUL, naGetFlagu32(flags, NA_BUTTON_STATEFUL)); 
  naSetFlagu32(&(winapiButton->state), NA_WINAPI_BUTTON_IMAGE, NA_TRUE); 

  return (NAButton*)winapiButton;
}



NA_DEF void na_DestructWINAPIButton(NAWINAPIButton* winapiButton){
  na_ClearButton((NAButton*)winapiButton);
}



NA_DEF void naSetButtonVisible(NAButton* button, NABool visible){
  //todo
}



NA_DEF void naSetButtonEnabled(NAButton* button, NABool enabled){
  // todo
}



NA_DEF NABool naGetButtonState(const NAButton* button){
  const NAWINAPIButton* winapiButton = (const NAWINAPIButton*)button;
  #if NA_DEBUG
    if(!naGetFlagu32(winapiButton->state, NA_WINAPI_BUTTON_STATEFUL))
      naError("This is not a stateful button");
  #endif
  return na_GetButtonState(winapiButton);
}



NA_DEF void naSetButtonState(NAButton* button, NABool state){
  NAWINAPIButton* winapiButton = (NAWINAPIButton*)button;
  // Note that BM_SETSTATE only changes the visual highlight, not the state of the
  // WINAPI button. Therefore, we need a separate state boolean.
  if(naGetFlagu32(winapiButton->state, NA_WINAPI_BUTTON_STATEFUL))
  {
    na_SetButtonState(winapiButton, state);
  }else{
    #if NA_DEBUG
      naError("This is not a stateful button");
    #endif
  }
}



NA_DEF void naSetButtonText(NAButton* button, const NAUTF8Char* text){
  NAWINAPIButton* winapiButton = (NAWINAPIButton*)button;
  #if NA_DEBUG
    if(naGetFlagu32(winapiButton->state, NA_WINAPI_BUTTON_IMAGE))
      naError("This is not a text button");
  #endif
  // todo
}



NA_DEF void naSetButtonImage(NAButton* button, const NAUIImage* uiImage){
  NAWINAPIButton* winapiButton = (NAWINAPIButton*)button;
  #if NA_DEBUG
    if(!naGetFlagu32(winapiButton->state, NA_WINAPI_BUTTON_IMAGE))
      naError("This is not a image button");
  #endif
  // todo
}



NA_DEF NABool naIsButtonStateful(NAButton* button){
  // todo
  return NA_FALSE;
}



NA_DEF NABool naIsButtonBorderless(NAButton* button){
  // todo
  return NA_FALSE;
}



NA_DEF NABool naIsButtonTextual(NAButton* button){
  // todo
  return NA_TRUE;
}



NA_DEF void naSetButtonSubmit(NAButton* button, NAReactionHandler handler, void* controller){
  NAWINAPIButton* winapiButton = (NAWINAPIButton*)button;
  long style = (long)GetWindowLongPtr(naGetUIElementNativePtr(winapiButton), GWL_STYLE);
  style = (style & ~SS_TYPEMASK) | BS_DEFPUSHBUTTON;
  SetWindowLongPtr(naGetUIElementNativePtr(winapiButton), GWL_STYLE, (LONG_PTR)style);
  naAddUIKeyboardShortcut(
    naGetUIElementWindow(button),
    naMakeKeyStroke(NA_MODIFIER_FLAG_NONE, NA_KEYCODE_ENTER),
    handler,
    controller);
}



NA_DEF void naSetButtonAbort(NAButton* button, NAReactionHandler handler, void* controller){
  naAddUIKeyboardShortcut(
    naGetUIElementWindow(button),
    naMakeKeyStroke(NA_MODIFIER_FLAG_NONE, NA_KEYCODE_ESC),
    handler,
    controller);
}



NA_HDEF NARect na_GetButtonRect(const NA_UIElement* button)
{
  const NAWINAPIButton* winapiButton = (const NAWINAPIButton*)button;
  return winapiButton->rect;
}

NA_HDEF void na_SetButtonRect(NA_UIElement* button, NARect rect){
  NAWINAPIButton* winapiButton = (NAWINAPIButton*)button;

  winapiButton->rect = rect;
  double uiScale = naGetUIElementResolutionFactor(NA_NULL);
  NARect parentRect = naGetUIElementRect(naGetUIElementParent(button));

  SetWindowPos(
    naGetUIElementNativePtr(button),
    HWND_TOP,
    (int)(winapiButton->rect.pos.x * uiScale),
    (int)((parentRect.size.height - winapiButton->rect.pos.y - winapiButton->rect.size.height) * uiScale),
    (int)(winapiButton->rect.size.width * uiScale),
    (int)(winapiButton->rect.size.height * uiScale),
    0);
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
