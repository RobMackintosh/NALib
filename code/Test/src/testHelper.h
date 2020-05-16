
#define NA_TEST_STRINGIFY(A) #A
#define NA_TEST_NALIB_PATH(path, file) NA_TEST_STRINGIFY(path ## ../../NALib/src/file)

#include <string.h>



typedef struct NATestData NATestData;

void naStartTesting(const char* rootName);
void naStopTesting(void);
void naAddTest(const char* expr, int success, int lineNum);
void naStartTestGroup(const char* name, int lineNum);
void naStopTestGroup();
#define naT(expr)\
  {\
    NABool success = expr;\
    naAddTest(#expr, success, __LINE__);\
  }
#define naG(name)\
  naStartTestGroup(name, __LINE__);\
  for(int g = 0; g < 1 ; g++, naStopTestGroup())
#define naF(name)\
  {\
  naStartTestGroup(#name, __LINE__);\
  test ## name();\
  naStopTestGroup();\
  }



#define NA_TEST_MAX_MACRO_LENGTH 40

#define printMacroHead(macro)\
{\
  const char* macroStr = macro;\
  printf(macroStr);\
  for(int c = 0; c < (int)(NA_TEST_MAX_MACRO_LENGTH - strlen(macroStr)); c++){printf(" ");}\
}

#define printMacroPlain(macro)\
  printMacroHead(#macro)\
  if(strncmp(#macro, NA_STRINGIFY(macro), strlen(#macro)) != 0){\
    if(strlen(NA_STRINGIFY(macro)) == 0){\
      printf("(Defined as empty)");\
    }else{\
      printf(NA_STRINGIFY(macro));\
    }\
  }else{\
    printf("(Undefined)");\
  }\
  printf(NA_NL);

#define printMacroInt(macro)\
  printMacroHead(#macro)\
  printf("%d", (int32)macro);\
  printf(NA_NL);

#define printMacroIntHex(macro, showDec)\
  printMacroHead(#macro)\
  if(showDec)\
    printf("0x%x (%d)", macro, macro);\
  else\
    printf("0x%x", macro);\
  printf(NA_NL);

#define printMacroString(macro)\
  printMacroHead(#macro)\
  printf("%s", macro);\
  printf(NA_NL);

#define printMacroIntSpecial(macro, specialValue, specialString)\
  printMacroHead(#macro)\
  if((int32)macro == (int32)specialValue){\
    printf("%d (%s)", macro, specialString);\
  }else{\
    printf("%d", macro);\
  }\
  printf(NA_NL);

#define printMacroIntSpecialHex(macro, specialValue, specialString)\
  printMacroHead(#macro)\
  if((int32)macro == (int32)specialValue){\
    printf("0x%x (%s)", macro, specialString);\
  }else{\
    printf("0x%x", macro);\
  }\
  printf(NA_NL);

#define printMacroEnumCore(macro, strings, maxValue)\
  if((int32)macro >= (int32)maxValue){\
    printf("%d Invalid value", macro);\
  }else{\
    printf("%d (%s)", macro, strings[macro]);\
  }\

#define printMacroEnum(macro, strings, maxValue)\
  printMacroHead(#macro)\
  printMacroEnumCore(macro, strings, maxValue)\
  printf(NA_NL);

extern const char* na_yesno_strings[];

#define printMacroIntYesNo(macro)\
  printMacroHead(#macro)\
  printMacroEnumCore(macro, na_yesno_strings, 2)\
  printf(NA_NL);

//#define testSuccess(success)\
  //(success ? NA_NULL : naNewStringWithFormat("%d: FAIL" NA_NL, __LINE__))

#define testSuccess(success)\
  if(success){\
    printf("%d: ok" NA_NL, __LINE__);\
  }else{\
    printf("%d: FAIL" NA_NL, __LINE__);\
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
