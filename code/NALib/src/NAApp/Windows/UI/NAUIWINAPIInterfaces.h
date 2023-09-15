
// This file is an inline implmenentation (II) file which is included in the
// NAWINAPI.c file. This is a bit special as it is marked as a .h file but
// actually contains non-inlinenable code. See NAWINAPI.c for more information.
// Do not include this file anywhere else!



// The following struct stores all relevant data which will then be stored in
// a list of the running NAWINAPIApplication.
typedef struct NAWINAPITimerStruct NAWINAPITimerStruct;
struct NAWINAPITimerStruct {
  UINT      key;
  NAMutator func;
  void*     arg;
};

typedef struct NAWINAPIColor NAWINAPIColor;
struct NAWINAPIColor {
  COLORREF color;
  HBRUSH   brush;
};



// Typedefs of all ui elements
typedef struct NAWINAPIApplication NAWINAPIApplication;
typedef struct NAWINAPIButton NAWINAPIButton;
typedef struct NAWINAPICheckBox NAWINAPICheckBox;
typedef struct NAWINAPIImageSpace NAWINAPIImageSpace;
typedef struct NAWINAPILabel NAWINAPILabel;
typedef struct NAWINAPIMenu NAWINAPIMenu;
typedef struct NA_WINAPIMenuItem NA_WINAPIMenuItem;
#if NA_COMPILE_OPENGL == 1
  typedef struct NAWINAPIOpenGLSpace NAWINAPIOpenGLSpace;
#endif
typedef struct NAWINAPIPopupButton NAWINAPIPopupButton;
typedef struct NAWINAPIRadio NAWINAPIRadio;
typedef struct NAWINAPISlider NAWINAPISlider;
typedef struct NAWINAPISpace NAWINAPISpace;
typedef struct NAWINAPITextBox NAWINAPITextBox;
typedef struct NAWINAPITextField NAWINAPITextField;
typedef struct NAWINAPIWindow NAWINAPIWindow;



// Prototypes of all ui element desctructors
NA_HAPI void na_DestructWINAPIApplication(NAWINAPIApplication* winapiApplication);
NA_HAPI void na_DestructWINAPIButton(NAWINAPIButton* winapiButton);
NA_HAPI void na_DestructWINAPICheckBox(NAWINAPICheckBox* winapiCheckBox);
NA_HAPI void na_DestructWINAPIImageSpace(NAWINAPIImageSpace* winapiImageSpace);
NA_HAPI void na_DestructWINAPILabel(NAWINAPILabel* winapiLabel);
NA_HAPI void na_DestructWINAPIMenu(NAWINAPIMenu* winapiMenu);
NA_HAPI void na_DestructWINAPIMenuItem(NA_WINAPIMenuItem* winapiMenuItem);
#if NA_COMPILE_OPENGL == 1
  NA_HAPI void na_DestructWINAPIOpenGLSpace(NAWINAPIOpenGLSpace* winapiOpenGLSpace);
#endif
NA_HAPI void na_DestructWINAPIPopupButton(NAWINAPIPopupButton* winapiPopupButton);
NA_HAPI void na_DestructWINAPIRadio(NAWINAPIRadio* winapiRadio);
NA_HAPI void na_DestructWINAPISlider(NAWINAPISlider* winapiSlider);
NA_HAPI void na_DestructWINAPISpace(NAWINAPISpace* winapiSpace);
NA_HAPI void na_DestructWINAPITextBox(NAWINAPITextBox* winapiTextBox);
NA_HAPI void na_DestructWINAPITextField(NAWINAPITextField* winapiTextField);
NA_HAPI void na_DestructWINAPIWindow(NAWINAPIWindow* winapiWindow);



struct NAWINAPIApplication {
  NAApplication    application;
  NAList           timers;
  HWND             offscreenWindow;
  NONCLIENTMETRICS nonClientMetrics;
  HICON            appIcon;

  NA_UIElement*    mouseHoverElement;
  const NAMenu*    lastOpenedMenu;
  UINT             nextMenuItemId;

  WNDPROC          oldButtonWindowProc;
  WNDPROC          oldCheckBoxWindowProc;
  WNDPROC          oldLabelWindowProc;
  WNDPROC          oldPopupButtonWindowProc;
  WNDPROC          oldRadioWindowProc;
  WNDPROC          oldSliderWindowProc;
  WNDPROC          oldTextFieldWindowProc;

  NAWINAPIColor    fgColor;
  NAWINAPIColor    fgColorDisabled;
  NAWINAPIColor    bgColor;
  NAWINAPIColor    bgColorAlternate;
  NAWINAPIColor    bgColorAlternate2;
};

struct NAWINAPIButton{
  NAButton button;
  NARect   rect;
  uint32   state;
};

struct NAWINAPICheckBox {
  NACheckBox checkBox;
  NARect     rect;
};

struct NAWINAPIImageSpace {
  NAImageSpace imageSpace;
  NARect       rect;
};

struct NAWINAPILabel {
  NALabel   label;
  NARect    rect;
  NABool    enabled;
  NAString* href;
};

struct NAWINAPIMenu {
  NAMenu   menu;
  HMENU    hMenu;
};

struct NA_WINAPIMenuItem {
  NAMenuItem   menuItem;
  NABool       isSeparator;
  NAString*    text;
};

#if NA_COMPILE_OPENGL == 1
  struct NAWINAPIOpenGLSpace {
    NAOpenGLSpace openGLSpace;
    NARect        rect;
    HGLRC         hRC;    // The rendering context for OpenGL
  };
#endif

struct NAWINAPIPopupButton {
  NAPopupButton popupButton;
  NARect        rect;
};

struct NAWINAPIRadio {
  NARadio radio;
  NARect  rect;
};

struct NAWINAPISlider {
  NASlider slider;
  NARect   rect;
};

struct NAWINAPISpace {
  NASpace        space;
  NARect         rect;
  NAWINAPIColor* lastBgColor;
};

struct NAWINAPITextBox {
  NATextBox textBox;
  NARect    rect;
  void*     nextTabStop;
  void*     prevTabStop;
};

struct NAWINAPITextField {
  NATextField textField;
  NARect      rect;
  void*       nextTabStop;
  void*       prevTabStop;
};

struct NAWINAPIWindow {
  NAWindow      window;
  NARect        rect;
  NA_UIElement* firstResponder;
};



NA_RUNTIME_TYPE(NAWINAPIApplication, na_DestructWINAPIApplication, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPIButton, na_DestructWINAPIButton, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPICheckBox, na_DestructWINAPICheckBox, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPIImageSpace, na_DestructWINAPIImageSpace, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPILabel, na_DestructWINAPILabel, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPIMenu, na_DestructWINAPIMenu, NA_FALSE);
NA_RUNTIME_TYPE(NA_WINAPIMenuItem, na_DestructWINAPIMenuItem, NA_FALSE);
#if NA_COMPILE_OPENGL == 1
  NA_RUNTIME_TYPE(NAWINAPIOpenGLSpace, na_DestructWINAPIOpenGLSpace, NA_FALSE);
#endif
NA_RUNTIME_TYPE(NAWINAPIPopupButton, na_DestructWINAPIPopupButton, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPIRadio, na_DestructWINAPIRadio, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPISlider, na_DestructWINAPISlider, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPISpace, na_DestructWINAPISpace, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPITextBox, na_DestructWINAPITextBox, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPITextField, na_DestructWINAPITextField, NA_FALSE);
NA_RUNTIME_TYPE(NAWINAPIWindow, na_DestructWINAPIWindow, NA_FALSE);



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
