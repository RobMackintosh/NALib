
// This file is part of NALib, a collection of C and C++ source code
// intended for didactical purposes. Full license notice at the bottom.

#ifndef NA_SYSTEM_INCLUDED
#define NA_SYSTEM_INCLUDED
#ifdef __cplusplus 
  extern "C"{
#endif


// This is the base of the whole NALib implementation. Almost all files
// include this file. It contains the most basic type definitions and
// preprocessor macros.


#define NA_VERSION 11


// The various Systems:
#define NA_SYSTEM_MAC_OS_X  0
#define NA_SYSTEM_WINDOWS   1
#define NA_SYSTEM_UNKNOWN   2
#define NA_SYSTEM_COUNT     3

// The various Endianness-Settings:
#define NA_ENDIANNESS_NATIVE  0
#define NA_ENDIANNESS_LITTLE  1
#define NA_ENDIANNESS_BIG     2
#define NA_ENDIANNESS_UNKNOWN 3 // Behaves the same as native.
#define NA_ENDIANNESS_COUNT   4
// Note that when expecting the endianness as an argument, it will have the
// type NAInt. Unfortunately, these macros can not be defined as enums.



// //////////////////////////////////////////
// Preprocessor directives to figure out what system this is.
//
// Currently, there are two system configurations assumed:
// - Mac OS X with GCC or Clang
// - Windows with Microsoft Visual Studio compiler
//
// Note that the author is completely aware that the system is not bound to
// a specific compiler. But these combinations are the ones having been used
// during the last 10 years by the author both for hobby and job.
//
// In the future, there might be more or different macros
//
// //////////////////////////////////////////

// Interesting read: http://sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined _WIN32
  #define NA_SYSTEM NA_SYSTEM_WINDOWS
  #define NA_SYSTEM_ENDIANNESS NA_ENDIANNESS_LITTLE
  #if defined _WIN64
    #define NA_SYSTEM_ADDRESS_BITS 64
  #else
    #define NA_SYSTEM_ADDRESS_BITS 32
  #endif

#elif defined __APPLE__ && __MACH__
  #define NA_SYSTEM NA_SYSTEM_MAC_OS_X
  #if defined __LITTLE_ENDIAN__
    #define NA_SYSTEM_ENDIANNESS NA_ENDIANNESS_LITTLE
  #elif defined __BIG_ENDIAN__
    #define NA_SYSTEM_ENDIANNESS NA_ENDIANNESS_BIG
  #endif
  #if defined __LP64__
    #define NA_SYSTEM_ADDRESS_BITS 64
  #else
    #define NA_SYSTEM_ADDRESS_BITS 32
  #endif
  
#else
  #warning "System not detected"
#endif

#ifndef NA_SYSTEM_ADDRESS_BITS
  #warning "Address bits not detected, trying 32."
  #define NA_SYSTEM_ADDRESS_BITS 32
#endif

#ifndef NA_SYSTEM_ENDIANNESS
  #warning "Endian format not detected, trying native."
  #define NA_SYSTEM_ENDIANNESS NA_ENDIANNESS_UNKNOWN
#endif

// The number of bytes per address
#define NA_SYSTEM_ADDRESS_BYTES (NA_SYSTEM_ADDRESS_BITS >> 3)



// NALib detects, what c standards are used.
#if defined __STDC__
  #define NA_C89
  #define NA_C90
#endif
#if defined __STDC_VERSION__
  #if __STDC_VERSION__ >= 199409L
    #define NA_C94
  #endif
  #if __STDC_VERSION__ >= 199901L
    #define NA_C99
  #endif
  #if __STDC_VERSION__ >= 201112L
    #define NA_C11
  #endif
#endif



// ////////////////////////////////////////////////////////////
// System dependant mapping of global functions and macros
// ////////////////////////////////////////////////////////////

// The definition of NA_RESTRICT and NA_INLINE are just mappings of built-in
// keywords on different systems.
//
// A function declared with NA_LINKER_NO_EXPORT will not be exported when
// building a library. This means that this function will not be listed in the
// .lib file on windows and will not be accessible anywhere when including the
// (.dll or .dylib) library. A function declared with NA_LINKER_EXPORT will
// explicitely be exported. This is a system-independent implementation which
// allows the programmer to define the exporting on a very low granularity.
// There exist other methods to define the exporting in supplementary files.
//
// As NALib is not intended to be compiled as a library, all functions are
// either declared static inline or with NA_LINKER_NO_EXPORT. NA_LINKER_EXPORT
// will never be used but you may use it in your own code.
//
// Usually, the NA_LINKER_NO_EXPORT macro is defined in some other, hidden file
// as it shall not be visible to the end user and will only be used on hidden
// parts. This is not necessary but makes the code clean and helps detect
// functions which should not be accessible to the user. In NALib, this does
// not matter and therefore the NA_LINKER_NO_EXPORT macro is defined here together
// with the NA_LINKER_EXPORT macro.

#if defined NA_C99
  #define NA_INLINE             inline
  #define NA_RESTRICT           restrict
#endif

#if NA_SYSTEM == NA_SYSTEM_WINDOWS
  #if !defined NA_RESTRICT
    #define NA_RESTRICT          __restrict
  #endif
  #if !defined NA_INLINE
    #define NA_INLINE           _inline
  #endif
  #define NA_LINKER_NO_EXPORT
  #define NA_LINKER_EXPORT      __declspec(dllexport)
#elif NA_SYSTEM == NA_SYSTEM_MAC_OS_X
  #if !defined NA_RESTRICT
    #define NA_RESTRICT         __restrict__
  #endif
  #define NA_LINKER_NO_EXPORT   __attribute__ ((visibility("hidden")))
  #define NA_LINKER_EXPORT      __attribute__ ((visibility("default")))
#else
#endif

// The following macros mark the declarations and definitions of symbols
// in NALib:
//
// NA_API marks the declaration of a function which is intended to be used by
// the programmer. Its counterpart is NA_DEF which marks the implementation of
// the appropriate function. NA_API functions can only be found in header
// files. NA_DEF functions can only be found in implementation files.
//
// Note that API stands for "Application Programming Interface" and DEF stands
// for "Definition".
//
// NA_IAPI is short for NA_INLINE_API and NA_IDEF is short for NA_INLINE_DEF.
// These two prefixes mark inline functions in NALib. But note that inline
// functions do not need to be static. Even more so, the declaration of an
// inline function should not be declared with the inline keyword. But as
// compilers tend to be difficult and there are some versions out there which
// may emit warnings, both macros are defined "static inline" in NALib. Saves
// a lot of trouble. The static keyword may lead to slightly bigger binaries.
// But so far, the author never had problems with that.
//
// Also note that visibility attributes have no effect on inline functions as
// they MUST be visible to the compiler in order for it to inline the code.
// Hence in NALib, any visibility attribute is omitted with inline functions
// as otherwise compilers would constantly emit annoying warnings.
//
// Again, note that the following macros are just used for NALib, they may
// very well differ in your own implementation!
#define NA_API    NA_LINKER_NO_EXPORT
#define NA_DEF    NA_LINKER_NO_EXPORT
#define NA_IAPI   static NA_INLINE
#define NA_IDEF   static NA_INLINE

// Additionally, there are definitions of so called HELPER functions.
// Helper functions should not be accessible or visible to the user. Helper
// functions are called from other functions and usually are declared in
// the same instance and defined once, and only once in an implementation file,
// not a header file. Again, in NALib, things are a little more transparent so
// these macros are a mere hint for the programmer, not more.
#define NA_HAPI   NA_LINKER_NO_EXPORT
#define NA_HDEF   NA_LINKER_NO_EXPORT
#define NA_HIAPI  static NA_INLINE
#define NA_HIDEF  static NA_INLINE
// Authors comment: Note that all symbols are declared and defined very
// restrictive. Meaning: No function or variable of NALib will show up in a
// library which is built with NALib.


#ifndef va_copy
  // va_copy is missing on some systems. This is the definition which most
  // probably works:
  #define va_copy(d,s) ((d) = (s))
#endif




// /////////////////////////
// The integer types
// /////////////////////////

// We test if the current system has a (positive) integer encoding suitable for
// NALib. Some obscure compilers might handle this differently.
#if (0x01000000 >> 24) != 0x01
  #warning "Unknown integer number encoding. NALib might not compile or run."
#endif

// We test what encoding is used for negative integers. With this knowledge,
// certain tasks might be speeded up a bit.
#if   (-1 & 3) == 3
  #define NA_SIGNED_INTEGER_USES_TWOS_COMPLEMENT
#elif (-1 & 3) == 2
  #define NA_SIGNED_INTEGER_USES_ONES_COMPLEMENT
#elif (-1 & 3) == 1
  #define NA_SIGNED_INTEGER_USES_SIGN_MAGNITUDE
#else
  #warning "Invalid signed integer encoding. NALib might not work properly."
#endif

// Definition of basic integer types. Depending on the C-standard and depending
// on the system the typedefs are provided differently in NALib.
//
// Note that these types do not have the NA prefix. The author decided that
// these types are too fundamental such that they do not need to be marked with
// the prefix. They should not collide with other typedefs found in other
// sourcecodes. If the do collide, there would be a serious problem and the
// implementation is likely incompatible. Please send the author an email if
// that happends.

#if defined NA_C99
  #include <stdint.h>
  typedef uint64_t          uint64;
  typedef uint32_t          uint32;
  typedef uint16_t          uint16;
  typedef uint8_t           uint8;
  typedef int64_t           int64;
  typedef int32_t           int32;
  typedef int16_t           int16;
  typedef int8_t            int8;
#elif NA_SYSTEM == NA_SYSTEM_WINDOWS
  typedef unsigned __int64  uint64;
  typedef unsigned __int32  uint32;
  typedef unsigned __int16  uint16;
  typedef unsigned __int8   uint8;
  typedef signed __int64    int64;
  typedef signed __int32    int32;
  typedef signed __int16    int16;
  typedef signed __int8     int8;
#elif NA_SYSTEM == NA_SYSTEM_MAC_OS_X
  #include <sys/types.h>
  typedef u_int64_t         uint64;
  typedef u_int32_t         uint32;
  typedef u_int16_t         uint16;
  typedef u_int8_t          uint8;
  typedef int64_t           int64;
  typedef int32_t           int32;
  typedef int16_t           int16;
  typedef int8_t            int8;
#else
  #warning "Don't know how to define basic integer types."
#endif

// Note that the following macros might also be defined using the values
// provided in the <limits.h> or <climits> header file. In NALib, the are
// provided manually to not be dependent on the various definitions of
// char, short, int, long and long long.

#if defined NA_C99
  // In C99, the values are defined in stdint.h (included above)
  #define NA_UINT8_MAX  UINT8_MAX
  #define NA_UINT16_MAX UINT16_MAX
  #define NA_UINT32_MAX UINT32_MAX
  #define NA_UINT64_MAX UINT64_MAX
  #define NA_INT8_MAX   INT8_MAX
  #define NA_INT8_MIN   INT8_MIN
  #define NA_INT16_MAX  INT16_MAX
  #define NA_INT16_MIN  INT16_MIN
  #define NA_INT32_MAX  INT32_MAX
  #define NA_INT32_MIN  INT32_MIN
  #define NA_INT64_MAX  INT64_MAX
  #define NA_INT64_MIN  INT64_MIN
#else
  #define NA_UINT8_MAX  ((uint8) (0xffu))
  #define NA_UINT16_MAX ((uint16)(0xffffu))
  #define NA_UINT32_MAX ((uint32)(0xffffffffu))
  #define NA_UINT64_MAX ((uint64)(0xffffffffffffffffuLL))
  // note that the - 1 is needed to avoid warnings on some compilers
  #define NA_INT8_MAX   ((int8) (0x7f))
  #define NA_INT8_MIN   ((int8) (0x81 - 1))
  #define NA_INT16_MAX  ((int16)(0x7fff))
  #define NA_INT16_MIN  ((int16)(0x8001 - 1))
  #define NA_INT32_MAX  ((int32)(0x7fffffff))
  #define NA_INT32_MIN  ((int32)(0x80000001 - 1))
  #define NA_INT64_MAX  ((int64)(0x7fffffffffffffffLL))
  #define NA_INT64_MIN  ((int64)(0x8000000000000001LL - 1LL))
#endif


// ////////////////////////////////////
// Other Basic types used in NALib
// ////////////////////////////////////

// Currently, NALib assumes a Byte to consist of precisely 8 bits. This is
// reflected in various enumerations and fundamental types like NAByte and
// NAUTF8Char. With this macro, we make sure, our code compiles just on the
// systems which have 8-Bit Bytes.
#include <limits.h>
#if CHAR_BIT != 8
  #error "NALib can not work properly with chars unequal 8 bits."
#endif

// An NAByte is a type definition of a Byte.
//
// Defining an NAByte as an uint8 can be very handy. In NALib, the NAByte type
// is often used when a memory block needs to be accessed byte by byte. You
// could also use a void-pointer for that but void-pointers are sometimes just
// a little too cumbersome to work with and do not always have a size defined
// depending on the standard used. Furthermore, a pointer to an uint8 can be
// displayed by a debugger while a pointer to void can not.
// Why not using the signed variant? Because there are many implementations
// using enumerations which go beyound 127 and do not want to use negative
// numbers.
typedef uint8 NAByte;


// NAInt
//
// The NAInt type is an integer of the size which is needed for storing an
// address. This means that this type is dependent on the system NALib is
// compiled for. It can for example be 32 Bits on one and 64 Bits on another
// architecture.
//
// NAInt will be used as the default integer type in NALib. Many fundamental
// functions will return NAInt or expect it as an argument. Note that the NAInt
// type is signed!
//
// Starting with NALib Version 9, there also exists NAUInt, the unsigned
// variant. Many functions have been changed which explicitely require or
// return an unsigned integer. This helps detecting errors as a compiler can
// emit warnings on sign differences.
//
// The signed variant NAInt is also used by memory and array functions. This
// means that in 32-Bit systems, only about 2 billion entries can be accessed
// with arrays when using NALib. The author is perfectly aware of the fact
// that most standard library functions use an unsigned integer for size_t,
// and that NALib therefore will have some minor limitations regarding memory
// usage. But the author has been convinced by the simplicity and elegance of
// just using signed integers during the last couple of years. It makes
// programming and debugging so much easier. And in 64 Bit systems, the memory
// limitations will not be a problem for a couple of years coming.
//
// Note that integers with explicitely defined sizes are used. The definition
// of just "int" would be unreliable.
//
// In addition to the type, there is the definition of a printf-argument macro.
// Use the macro for example like this:
//
// printf("The point is at X coordinate %" NA_PRIi " NA_NL, pos.x);
// printf("The array has %" NA_PRIu " entries." NA_NL, naGetArrayCount(array));
//
// Some more printf arguments can be found in the NAString.h header file.
//
// One serious drawback of a system-dependent definition is that NALib might
// behave differently depending on the system it is compiled for. But it will
// only behave differently in cases where the behaviour would become difficult
// anyway. Meaning: You will most likely run into out-of-memory problems first.
//
// Note that a different solution would be to always go for int64 and convert
// between the different integer types. But compiling NALib on a non-64-Bit
// system then would definitely be a waste of space with lots of implicit
// arithmetic conversions.
//
#if NA_SYSTEM_ADDRESS_BITS == 32
  typedef int32 NAInt;
  typedef uint32 NAUInt;
  #define NA_PRIi "d"
  #define NA_PRIu "u"
  #define NA_INT_MAX NA_INT32_MAX
  #define NA_INT_MIN NA_INT32_MIN
  #define NA_ZERO 0
  #define NA_ONE  1
#elif NA_SYSTEM_ADDRESS_BITS == 64
  typedef int64 NAInt;
  typedef uint64 NAUInt;
  #define NA_PRIi "lld"
  #define NA_PRIu "llu"
  #define NA_INT_MAX NA_INT64_MAX
  #define NA_INT_MIN NA_INT64_MIN
  #define NA_ZERO 0LL
  #define NA_ONE  1LL
#else
  #warning "NAInt undefined"
#endif



// NABool
// Note that in NALib, the definition of NABool is explicitely set to "int" and
// not char or unsigned char or not even NAInt. Just plain old "int".
// This is unusual but most probably the easiest way to tell the compiler to 
// use whatever he likes as long as it's fast. it is in the believe of the
// author, that in modern computers, speed is more important than space as
// opposed to earlier times where wasting space was a complete no-go. When
// using lots of Boolean values, there are better ways to store them than in
// NABools anyway: Use C-style-masks.
//
// Also note that there is no distinction between C and C++. Therefore, NALib
// never uses the bool type or the true and false keywords. Also the macros
// available in C11 are unused.
//
typedef int NABool;
#define NA_TRUE    1
#define NA_FALSE   0




// These functions are here for core functions like memory functions to
// be very explicit in how the code should compile
NA_IDEF NABool naIsIntStrictlyPositive(NAInt x){return (x  > NA_ZERO);}
NA_IDEF NABool naIsIntZero(NAInt x)            {return (x == NA_ZERO);}
NA_IDEF NABool naIsIntNegative(NAInt x)        {return (x  < NA_ZERO);}


// Note that in NALib, there is no base typedef for a void* or a const void*
// pointer. Some libraries or frameworks define them with names such as
// NAVoid, NAPtr, NABuffer, NABuf, NAIn, NAOut, NAInOut or such alike. The
// author believes that void* pointers should only be typedef'd if they
// describe a distinct purpose, like for example if a returned void* points
// to a group of structs sharing some common properties.
//
// If void* is used plainly as a pointer to an untyped address, the author
// thinks that it is best to just write void*, not some abscure type name.
// Therefore, you won't find any type definition of NAVoid here.




// The following macro is used to suppress compiler warnings in functions
// which do not make any use of a given parameter. This happens when function
// signatures are chosen to match predefined function pointer types or shall
// in general have a specific look. By using the given parameter as an
// expression and casting the result to void makes the compiler think that
// the parameter is used but has the explicit order to not use any
// evaluated value.
//
// Be careful with this when using the volatile keyword.

#define NA_UNUSED(x) (void)(x)


// The definition of NA_NULL is usually set to the NULL found in stdlib. The
// new C11 standard however has a new keyword. Let's use it if it is available!
// Note that stdlib is needed anyway for malloc, free and exit which is most
// probably needed anyway.
//
// Also note that in the comments of NALib as well as in the error messages,
// it will oftem times be called "Null-Pointer". In the implementation
// however, NA_NULL is used.
//
// The NORETURN macro expands to _Noreturn only in C11

#include <stdlib.h>
#ifdef NA_C11
  #define NA_NULL nullptr
  #define NA_NORETURN _Noreturn
#else
  #define NA_NULL NULL
  #define NA_NORETURN
#endif





// /////////////////////////////////
// Debugging:
//
// The following definitions will only be defined if NDEBUG is undefined.
// Many functions of NALib will perform many tests which will slow down
// the performance more or less considerably. If NDEBUG is defined however,
// no tests are performed whatsoever.
//
// When it comes to debugging, the macro NDEBUG is the only one being somewhat
// standardized across all systems and compilers. It literally tells the
// compiler to NOT-DEBUG, which means: When the macro is undefined, code
// encapsulated in #ifndef NDEBUG will be compiled, otherwise not.
//
// Usually in IDEs (Integrated Development Environments), there are at least
// two configurations per application: Debug and Release, sometimes also called
// Development and Deployment. The Release-Configuration usually automatically
// has the NDEBUG macro defined as a preprocessor build option. Not every IDE
// does this by default though, so you might have to add it manually to the
// build options.
//
// Note that in NALib, all tests performed by code encapsulated by
// #ifndef NDEBUG will emit errors on sterr in the following format:
// Function_name: Errormessage\n
// Therefore, a programmer can simply set a breakpoint in the denoted function
// and start debugging.
//
// If due to some reason, setting a breakpoint does not work in the desired
// function, you can simply set a breakpoint in the naError or naCrash function.
// These two functions are explicitely NOT inlined and are defined in a separate
// implementation file (and therefore a separate translation unit) due to that
// reason.
//
// Also note that in NALib, code executed within NDEBUG will not alter the
// runtime execution. In other words: Except from speed differences and outputs
// to strerr, NALib will NOT behave differently depending on NDEBUG being
// defined or not. The only remark is that calls to naCrash will eventually
// lead to an exit(EXIT_FAILURE) call before the real error is executed.
// If you use code sanity checking (for example clang analyzer) you should
// therefore always check both with and without NDEBUG defined. Otherwise,
// the analyzer might miss some errors.
// /////////////////////////////////

#ifndef NDEBUG

  // Prints an error. When this function gets called, the ongoing of the
  // application is undefined. Sometimes, the error might affect everything
  // which comes after it, sometimes, the error will just result in a NaN or
  // even be corrected automatically. Nontheless, any error should be
  // considered a potential risk for the application to eventually crash.
  void naError(const char* functionsymbol, const char* message);


  // Prints a crash message.
  // This function is used when the application experiences a critical error
  // like dereferencing an invalid pointer. The application will almost
  // certainly crash few steps after this function call.
  NA_NORETURN void naCrash(const char* functionsymbol, const char* message);

#endif






// This is the type of a constructor and destructor callback which is used by
// some naInitXXX as well as some naClearXXX functions.
// See readme for detailed informations.
typedef void* (*NAConstructor)(void *);
typedef void  (*NADestructor) (void *);
typedef void  (*NAMutator)    (void *);



typedef struct NAStructInfo NAStructInfo;
struct NAStructInfo{
  NAUInt structsize;
  NAConstructor constructor;
  NADestructor destructor;
};


#ifdef __cplusplus 
  } // extern "C"
#endif
#endif // NA_SYSTEM_INCLUDED

// Copyright (c) NALib, Tobias Stamm, Manderim GmbH
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the source-code.
//
// In case the source-code of this software is inaccessible to the end-user,
// the above copyright notice and this permission notice shall be included
// in any source-code which is dependent on this software and is accessible
// to the end-user.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
