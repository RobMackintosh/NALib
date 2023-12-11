
// This file is an inline implmenentation (II) file which is included in the
// NAWINAPI.c file. This is a bit special as it is marked as a .h file but
// actually contains non-inlinenable code. See NAWINAPI.c for more information.
// Do not include this file anywhere else!

#include "../NAPreferences.h"
#include "../../NAApp/NAApp.h"

#if NA_COMPILE_GUI == 1


NA_PreferencesPtr na_nativePreferences = NA_NULL;



NA_HDEF NA_PreferencesPtr na_GetNativePreferences(){
  #if NA_DEBUG
    if(!naGetApplication())
      naError("No application running. Use naStartApplication.");
  #endif
  if(!na_nativePreferences){
    NAString* appname = naNewApplicationName();
    NAString* companyname = naNewApplicationCompanyName();

    NAString* fullKeyName;
    if(companyname){
      fullKeyName = naNewStringWithFormat("Software\\%s\\%s", naGetStringUTF8Pointer(companyname), naGetStringUTF8Pointer(appname));
    }else{
      fullKeyName = naNewStringWithFormat("Software\\%s", naGetStringUTF8Pointer(appname));
    }

    WCHAR* systemFullKeyName = naAllocWideCharStringWithUTF8String(naGetStringUTF8Pointer(fullKeyName));
    LSTATUS errorCode = RegOpenKeyW(HKEY_CURRENT_USER, systemFullKeyName, &na_nativePreferences);
    if(errorCode != ERROR_SUCCESS){
      errorCode = RegCreateKeyW(HKEY_CURRENT_USER, systemFullKeyName, &na_nativePreferences);
    }

    naFree(systemFullKeyName);
    naDelete(appname);
    if(companyname){naDelete(companyname);}
    naDelete(fullKeyName);
  }
  return na_nativePreferences;
}



#endif // NA_COMPILE_GUI == 1

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
