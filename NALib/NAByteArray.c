
// This file is part of NALib, a collection of C and C++ source code
// intended for didactical purposes. Full license notice at the bottom.

#include "NAByteArray.h"
#include "NAValueHelper.h"



NA_DEF NAByteArray* naInitByteArrayWithSize(NAByteArray* array, NAInt size){
  #ifndef NDEBUG
    if(!array)
      {naCrash("naInitByteArrayWithSize", "array is Null-Pointer."); return NA_NULL;}
    if(size == NA_INVALID_MEMORY_SIZE)
      naError("naInitByteArrayWithSize", "Invalid size given.");
  #endif
  if(!size){  // if size is zero
    array = naInitByteArray(array);
  }else{
    NAMemoryBlock* newstorageblock = naAlloc(NAMemoryBlock);
    *newstorageblock = naMakeMemoryBlockWithBytesize(size);
    array->storage = naNewPointerMutable(newstorageblock, NA_POINTER_CLEANUP_FREE, (NAFunc)naClearMemoryBlock);
    array->memblock = naMakeMemoryBlockWithExtraction(newstorageblock, 0, naGetMemoryBlockBytesize(newstorageblock));
    array->indx = NA_INVALID_MEMORY_INDEX;
  }
  return array;
}



NA_DEF NAByteArray* naInitByteArrayWithConstBuffer(NAByteArray* array, const void* buffer, NAInt bytesize){
  NAMemoryBlock* newstorageblock;
  #ifndef NDEBUG
    if(!array)
      {naCrash("naInitByteArrayWithConstBuffer", "array is Null-Pointer."); return NA_NULL;}
    if(!buffer)
      naError("naInitByteArrayWithConstBuffer", "buffer is Null-Pointer.");
  #endif
  newstorageblock = naAlloc(NAMemoryBlock);
  if(naIsIntZero(bytesize)){
    *newstorageblock = naMakeMemoryBlock();
  }else{
    *newstorageblock = naMakeMemoryBlockWithConstBuffer(buffer, bytesize);
  }
  array->storage = naNewPointerMutable(newstorageblock, NA_POINTER_CLEANUP_FREE, NA_NULL);
  array->memblock = naMakeMemoryBlockWithExtraction(newstorageblock, 0, naGetMemoryBlockBytesize(newstorageblock));
  array->indx = NA_INVALID_MEMORY_INDEX;
  return array;
}



NA_DEF NAByteArray* naInitByteArrayWithMutableBuffer(NAByteArray* array, void* buffer, NAInt bytesize, NABool takeownership){
  NAMemoryBlock* newstorageblock;
  #ifndef NDEBUG
    if(!array)
      {naCrash("naInitByteArrayWithMutableBuffer", "array is Null-Pointer."); return NA_NULL;}
    if(!buffer)
      naError("naInitByteArrayWithMutableBuffer", "buffer is Null-Pointer.");
  #endif
  newstorageblock = naAlloc(NAMemoryBlock);
  if(naIsIntZero(bytesize)){
    *newstorageblock = naMakeMemoryBlock();
  }else{
    *newstorageblock = naMakeMemoryBlockWithMutableBuffer(buffer, bytesize);
  }
  if(takeownership){
    array->storage = naNewPointerMutable(newstorageblock, NA_POINTER_CLEANUP_FREE, (NAFunc)naClearMemoryBlock);
  }else{
    array->storage = naNewPointerMutable(newstorageblock, NA_POINTER_CLEANUP_FREE, NA_NULL);
  }
  array->memblock = naMakeMemoryBlockWithExtraction(newstorageblock, 0, naGetMemoryBlockBytesize(newstorageblock));
  array->indx = NA_INVALID_MEMORY_INDEX;
  return array;
}



NA_DEF NAByteArray* naInitByteArrayExtraction(NAByteArray* dstarray, const NAByteArray* srcarray, NAInt offset, NAInt size){
  NAMemoryBlock newmemblock;
  NAUInt positiveoffset;
  NAUInt positivesize;

  #ifndef NDEBUG
    if(!dstarray)
      {naCrash("naInitByteArrayExtraction", "dstarray is Null-Pointer."); return NA_NULL;}
    if(!srcarray){
      naCrash("naInitByteArrayExtraction", "srcarray is Null-Pointer."); return NA_NULL;}
  #endif

  // Note that dstarray may be equal to srcarray. If so, dstarray is assumed
  // to be a valid bytearray. Otherwise, dstarray is expected to be
  // uninitialized.

  naMakeIntegerRangePositiveInSize(&positiveoffset, &positivesize, offset, size, naGetMemoryBlockBytesize(&(srcarray->memblock)));
  
  if(!positivesize){return naInitByteArray(dstarray);}
  
  // The resulting array has content!
  newmemblock = naMakeMemoryBlockWithExtraction(&(srcarray->memblock), positiveoffset, positivesize);
  if(dstarray != srcarray){
    dstarray->storage = naRetainPointer(srcarray->storage);
  }
  dstarray->memblock = newmemblock;
  
  dstarray->indx = NA_INVALID_MEMORY_INDEX;
  return dstarray;
}



NA_DEF void naDecoupleByteArray(NAByteArray* array, NABool appendnulltermination){
  // Declaration before implementation. Needed for C90.
  NAInt arraysize;
  NAByte* buf;
  #ifndef NDEBUG
    if(!array){
      naCrash("naDecoupleByteArray", "array is Null-Pointer.");
      return;
    }
  #endif
  // Note: Do not use realloc as ptr may point to a subset of NAPointer.
  // Instead, create a new object and copy manually.
  arraysize = (NAInt)naGetMemoryBlockBytesize(&(array->memblock));
  if(!arraysize){
    naEmptyByteArray(array);
    return;
  }
  if(appendnulltermination){arraysize = -arraysize;}
  buf = naMalloc(arraysize);
  naCopyn(buf, naGetMemoryBlockConstPointer(&(array->memblock)), naAbsi(arraysize));
  naClearByteArray(array);

  // Note that now, you could initialize the array with a call to
  //
   naInitByteArrayWithMutableBuffer(array, buf, arraysize, NA_TRUE);
  //
  // But code-sanity checks do not recognize this function to be owning the
  // buffer and therefore, it is should be done explicitely.
  //
  // But not for now.
  // todo.
}





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
