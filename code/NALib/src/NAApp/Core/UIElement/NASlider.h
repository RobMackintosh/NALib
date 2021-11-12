
#if defined NA_UI_SLIDER_INCLUDED || !defined NA_APP_INCLUDED
  #warning "Do not include this file directly. Use NAApp.h"
#endif
#ifndef NA_UI_SLIDER_INCLUDED
#define NA_UI_SLIDER_INCLUDED



// Creates a new Slider. Default height is 24.
NA_API NASlider* naNewSlider(double width);

// Changes the visibility or enabled state of the button.
NA_API void naSetSliderVisible(NASlider* slider, NABool visible);
NA_API void naSetSliderEnabled(NASlider* slider, NABool enabled);

// Gets or sets the current double value of the slider. Must be in [0, 1]
NA_API double naGetSliderValue(NASlider* slider);
NA_API void naSetSliderValue(NASlider* slider, double value);

// Sets how many ticks are shown.
NA_API void naSetSliderTickCount(NASlider* slider, NAInt tickCount);



#endif // NA_UI_SLIDER_INCLUDED



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
