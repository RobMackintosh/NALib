// This file is part of NALib, a collection of C and C++ source code
// intended for didactical purposes. Full license notice at the bottom.



// This file contains inline implementations of the file NAMemory.h
// Do not include this file directly! It will automatically be included when
// including "NAMemory.h"



#if NA_SYSTEM == NA_SYSTEM_WINDOWS
  #include <Windows.h>
#elif NA_SYSTEM == NA_SYSTEM_MAC_OS_X
  #include <unistd.h>
  #include "malloc/malloc.h"
#endif

#include "../NASystem.h"
#include "../NAMathOperators.h"
#include "NAValueHelper.h"


// //////////////////////////////////////
// Accessing informations about the memory system
// //////////////////////////////////////


NA_IDEF NAUInt naGetSystemMemoryPagesize(){
  #if NA_SYSTEM == NA_SYSTEM_WINDOWS
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (NAUInt)info.dwPageSize;
  #else
    return (NAUInt)sysconf(_SC_PAGESIZE);
  #endif
}



NA_IDEF NAUInt naGetSystemMemoryPagesizeMask(){
  return ~(NAUInt)(naGetSystemMemoryPagesize() - NA_ONE);
}



// Zero fill
//
// The following function returns for a given negative bytesize, how big a
// buffer must be, to include the appended zero bytes.
//
// The allocation functions of NALib expect a positive or negative bytesize.
// If the bytesize is negative, the absolute value is used to allocate
// sufficient space but a certain number of bytes is appended to the memory
// block which will be initialized with binary zero but will not be visible to
// the programmer.
// 
// The following holds true:
//
// - The additional bytes are all guaranteed to be initialized with binary
//   zero.
// - There are AT LEAST as many bytes appended as one address requires.
//   Or more precise: 4 Bytes on 32 Bit systems, 8 Bytes on 64 Bit systems
// - There are as many bytes allocated such that the total bytesize of the
//   allocated block is a multiple of an address bytesize, meaning 4 or 8 Bytes
//   depending on the system (32 Bit or 64 Bit).
// - The total bytesize (desired space plus null bytes) is at minimum 2 times
//   the number of bytes needed for an address.
// - When using naMalloc, the part without the additional bytes might partially
//   become initialized with binary zero.
// Example for a 32bit system:
// 0 is a zero-filled byte.
// x is a desired byte,
// z is a desired byte which initially gets overwritten with zero.
// 0:  0000 0000
// 1:  z000 0000
// 2:  zz00 0000
// 3:  zzz0 0000
// 4:  zzzz 0000
// 5:  xxxx z000 0000
// 8:  xxxx zzzz 0000
// 9:  xxxx xxxx z000 0000
// 12: xxxx xxxx zzzz 0000
// 13: xxxx xxxx xxxx z000 0000

NA_HIDEF NAInt naGetNullTerminationBytesize(NAInt bytesize){
  NAInt returnbytesize;
  #ifndef NDEBUG
    if((bytesize >= NA_ZERO))
      naError("naGetNullTerminationBytesize", "size is not negative");
    if(bytesize == NA_INVALID_MEMORY_BYTESIZE)
      naError("naGetNullTerminationBytesize", "invalid size given");
  #endif
  returnbytesize = (-bytesize - NA_ONE) + (NA_SYSTEM_ADDRESS_BYTES << 1) - ((-bytesize - NA_ONE) % NA_SYSTEM_ADDRESS_BYTES);
  #ifndef NDEBUG
    if(returnbytesize < NA_ZERO)
      naError("naGetNullTerminationBytesize", "given negative size is too close to the minimal integer value");
  #endif
  return returnbytesize;
}








// ////////////////////////////////////////////
// Basic Memory allocation and freeing
// ////////////////////////////////////////////



NA_IDEF void* naMalloc(NAInt bytesize){
  NAByte* ptr; // Declaration before implementation. Needed for C90.
  NAInt fullsize;
  // ptr is declared as NAByte to simplify accessing individual bytes later
  // in this functions.

  #ifndef NDEBUG
    if(bytesize == NA_ZERO)
      naError("naMalloc", "size is zero.");
  #endif

  if(bytesize > NA_ZERO){
    ptr = (NAByte*)malloc((size_t)bytesize);
  }else{
    #ifndef NDEBUG
      if(bytesize == NA_INT_MIN)
        naError("naMalloc", "given negative size owerflows NAInt type.");
    #endif
    fullsize = naGetNullTerminationBytesize(bytesize);
    #ifndef NDEBUG
      if(fullsize < -bytesize)
        naError("naMalloc", "given size including zero filled endbytes overflows NAInt type.");
    #endif
    ptr = (NAByte*)malloc((size_t)fullsize);
    #ifndef NDEBUG
    if (!ptr)
      {naCrash("naMalloc", "Out of memory."); return NA_NULL;}
    #endif
    *(NAUInt*)(&(ptr[fullsize - 2 * NA_SYSTEM_ADDRESS_BYTES])) = NA_ZERO;
    *(NAUInt*)(&(ptr[fullsize - 1 * NA_SYSTEM_ADDRESS_BYTES])) = NA_ZERO;
  }

  return ptr;
}


NA_IDEF void naFree(void* ptr){
  free(ptr);
}



NA_IDEF void* naMallocAligned(NAUInt bytesize, NAUInt align){
  void* retptr;
  // Usually, aligned memory can be created in unix like systems using the
  // following three methods. Unfortunately, none of them work reliably on
  // Mac OS X.
  // - aligned_alloc under C11 is unreliable when using clang
  // - posix_memalign returns misaligned pointers in Snow Leopard
  // - malloc_zone_memalign same thing as posix_memalign
  // Therefore, a custom implementation is used which is costly but what
  // the hell.
    

  #if NA_SYSTEM == NA_SYSTEM_WINDOWS
    retptr = _aligned_malloc(bytesize, align);
  #else
    // Allocate a full align and a pointer more than required.
    void* mem = malloc(bytesize + align + sizeof(void*));
    // make a pointer point to the first byte being aligned within the memory
    // allocated in mem which comes after align bytes and a pointer size.
    void** ptr = (void**)((size_t)((NAByte*)mem + align + sizeof(void*)) & ~(align - NA_ONE));
    // Store the pointer to the original allocation pointer in the element
    // right before the first returned byte.
    ptr[-1] = mem;
    retptr = ptr;
  #endif
  
  #ifndef NDEBUG
    if(((NAUInt)retptr & (NAUInt)(align - NA_ONE)) != NA_ZERO)
      naError("naMallocAligned", "pointer unaligned.");
  #endif
  
  return retptr;
}



NA_IDEF void* naMallocPageAligned(NAUInt bytesize){
  // Note that due to some strange reason, aligned_alloc and valloc not work
  // with clang. Therefore, a simple call to naMallocAligned is used.
  return naMallocAligned(bytesize, naGetSystemMemoryPagesize());
}



NA_IDEF void naFreeAligned(void* ptr){
  #if NA_SYSTEM == NA_SYSTEM_WINDOWS
    _aligned_free(ptr);
  #else
    free(((void**)ptr)[-1]);
  #endif
}







// //////////////////////////
// NARuntime
// //////////////////////////


NA_API void*              naNewStruct(NATypeInfo* typeidentifier);


// This needs to be in this inline implementation file as it needs to be
// accessible to the compiler at all times.
struct NATypeInfo{
  void*             curpool;  // The actual type of this entry is hidden.
  NAUInt            typesize;
  NAMutator         destructor;
};










// //////////////////////////
// NAMemoryCleanup
// //////////////////////////


// For the NAPointer struct, we need to have the information available about
// how the memory was originally alloced. We store it as the high-order part
// of the refcount which is why we need to bit-shift it to the left.
// To simplify matters, also the NAPtr struct stores this information as left-
// shifted bits in the flags when NDEBUG ist undefined.
#define NA_MEMORY_CLEANUP_BITS            3
#define NA_MEMORY_CLEANUP_MASK            ((1 << NA_MEMORY_CLEANUP_BITS) - 1)
#define NA_REFCOUNT_DATA_CLEANUP_BITSHIFT   (NA_SYSTEM_ADDRESS_BITS - NA_MEMORY_CLEANUP_BITS)
#define NA_REFCOUNT_STRUCT_CLEANUP_BITSHIFT (NA_SYSTEM_ADDRESS_BITS - 2 * NA_MEMORY_CLEANUP_BITS)



#ifndef NDEBUG
  NA_HIDEF NABool naIsCleanupValid(NAMemoryCleanup cleanup){
    return ((cleanup & NA_MEMORY_CLEANUP_MASK) == cleanup);
  }
#endif
NA_HIDEF NAMemoryCleanup naGetFlagCleanupData(NAUInt flag){
  return (NAMemoryCleanup)((flag >> NA_REFCOUNT_DATA_CLEANUP_BITSHIFT) & NA_MEMORY_CLEANUP_MASK);
}
NA_HIDEF NAMemoryCleanup naGetFlagCleanupStruct(NAUInt flag){
  return (NAMemoryCleanup)((flag >> NA_REFCOUNT_STRUCT_CLEANUP_BITSHIFT) & NA_MEMORY_CLEANUP_MASK);
}



NA_IDEF void naCleanupMemory(void* data, NAMemoryCleanup cleanup){

  // Clear the data based on the data cleanup
  switch(cleanup){
  case NA_MEMORY_CLEANUP_NONE:
    break;
  case NA_MEMORY_CLEANUP_FREE:
    free(data);
    break;
  case NA_MEMORY_CLEANUP_FREE_ALIGNED:
    #if NA_SYSTEM == NA_SYSTEM_WINDOWS
      _aligned_free(data);
    #else
      free(data);
    #endif
    break;
  case NA_MEMORY_CLEANUP_NA_FREE:
    naFree(data);
    break;
  case NA_MEMORY_CLEANUP_NA_FREE_ALIGNED:
    naFreeAligned(data);
    break;
#ifdef __cplusplus 
  case NA_MEMORY_CLEANUP_DELETE:
    delete data;
    break;
  case NA_MEMORY_CLEANUP_DELETE_BRACK:
    delete [] data;
    break;
#endif
  case NA_MEMORY_CLEANUP_NA_DELETE:
    naDelete(data);
    break;
  default:
    #ifndef NDEBUG
      naError("naCleanupMemory", "Invalid cleanup");
    #endif
    break;
  }

}






// //////////////////////////
// NARefCount
// //////////////////////////

struct NARefCount{
  NAUInt count;      // Reference count. Including flags
  #ifndef NDEBUG
    NAMemoryCleanup structcleanup;
    NAMemoryCleanup datacleanup;
  #endif
};

// The refcount field also stores two cleanup hints in the 2*3 bits on the
// higher endian side. This is necessary due to the automatic deletion of a
// pointer and the struct the refcount is stored in when refcount reaches zero.
// It could be stored in a separate flag but the author decided to do it with
// a mask.
//
// Note that when NDEBUG is undefined, there is an additional flag just to
// check whether a reference counted value had been released properly.


#ifndef NDEBUG
  #define NA_REFCOUNT_DELETE_FROM_RELEASE (NA_ONE << (NA_REFCOUNT_STRUCT_CLEANUP_BITSHIFT - 1))
  #define NA_REFCOUNT_FLAG_BITS   (2 * NA_MEMORY_CLEANUP_BITS + 1)
#else
  #define NA_REFCOUNT_FLAG_BITS   (2 * NA_MEMORY_CLEANUP_BITS)
#endif

#define NA_REFCOUNT_FLAGS_BITSHIFT      (NA_SYSTEM_ADDRESS_BITS - NA_REFCOUNT_FLAG_BITS)
#define NA_REFCOUNT_MASK      ((NA_ONE << NA_REFCOUNT_FLAGS_BITSHIFT) - NA_ONE)


NA_IDEF NARefCount* naInitRefCount(NARefCount* refcount, NAMemoryCleanup structcleanup, NAMemoryCleanup datacleanup){
  #ifndef NDEBUG
    if(!naIsCleanupValid(structcleanup))
      naError("naInitRefCount", "structcleanup method invalid");
    if(!naIsCleanupValid(datacleanup))
      naError("naInitRefCount", "datacleanup method invalid");
    #ifdef __cplusplus 
      if(structcleanup == NA_MEMORY_CLEANUP_DELETE_BRACK)  
        naError("naInitRefCount", "This smartptrcleanup method does not make sense");
    #endif
  #endif
  refcount->count  = 1
               | ((NAUInt)datacleanup << NA_REFCOUNT_DATA_CLEANUP_BITSHIFT)
               | ((NAUInt)structcleanup << NA_REFCOUNT_STRUCT_CLEANUP_BITSHIFT);
  #ifndef NDEBUG
    refcount->structcleanup = structcleanup;
    refcount->datacleanup = datacleanup;
  #endif
  return refcount;
}



NA_HIDEF NAUInt naGetRefCountCount(const NARefCount* refcount){
  return refcount->count & NA_REFCOUNT_MASK;
}



NA_IDEF NAMemoryCleanup naGetRefCountCleanupData(const NARefCount* refcount){
  #ifndef NDEBUG
    if(!refcount)
      {naCrash("naGetRefCountCleanupData", "refcount is Null-Pointer."); return NA_MEMORY_CLEANUP_NONE;}
    if(naGetFlagCleanupData(refcount->count) != refcount->datacleanup)
      naError("naGetRefCountCleanupData", "Consistency problem: flags overwritten");
  #endif
  return naGetFlagCleanupData(refcount->count);
}



NA_IDEF NAMemoryCleanup naGetRefCountCleanupStruct(const NARefCount* refcount){
  #ifndef NDEBUG
    if(!refcount)
      {naCrash("naGetRefCountCleanupStruct", "refcount is Null-Pointer."); return NA_MEMORY_CLEANUP_NONE;}
    if(naGetFlagCleanupStruct(refcount->count) != refcount->structcleanup)
      naError("naGetRefCountCleanupStruct", "Consistency problem: flags overwritten");
  #endif
  return naGetFlagCleanupStruct(refcount->count);
}



NA_IDEF NARefCount* naRetainRefCount(NARefCount* refcount){
  #ifndef NDEBUG
    if(!refcount){
      naCrash("naRetainRefCount", "refcount is Null-Pointer.");
      return NA_NULL;
    }else{
      // The next test can detect some erroneous behaviour in the code. Note
      // however that most likely the true cause of the error did occur long
      // before reaching here.
      if(naGetRefCountCount(refcount) == NA_ZERO)
        {naCrash("naRetainRefCount", "Retaining NARefCount with a count of 0"); return NA_NULL;}
    }
  #endif
  refcount->count++;
  #ifndef NDEBUG
    // If refcount now suddenly becomes zero, there was either an error earlier
    // or the object has been retained too many times. Overflow.
    if(naGetRefCountCount(refcount) == NA_ZERO)
      naError("naRetainRefCount", "Reference count overflow");
  #endif
  return refcount;
}



NA_IDEF NABool naReleaseRefCount(NARefCount* refcount, void* data, NAMutator destructor){
  #ifndef NDEBUG
    if(!refcount){
      naCrash("naReleaseRefCount", "refcount is Null-Pointer.");
      return NA_FALSE;
    }
    // The next test can detect some erroneous behaviour in the code. Note
    // however that most likely the true cause of the error did occur long
    // before reaching here.
    if(naGetRefCountCount(refcount) == NA_ZERO)
      {naCrash("naReleaseRefCount", "Releasing NARefCount with a count of 0"); return NA_FALSE;}
  #endif
  // Note that the author decided to always count to zero, even if it is clear
  // that the pointer will eventually be freed and the data will be lost in
  // nirvana. But often times in debugging, when retaining and releasing is not
  // done correctly, an NAPointer is released too often. When refcount is 0 and
  // NDEBUG is not defined, this can be detected!
  refcount->count--;
  if(naGetRefCountCount(refcount) == NA_ZERO){
    #ifndef NDEBUG
      refcount->count |= NA_REFCOUNT_DELETE_FROM_RELEASE;
      if(!data && destructor)
        naError("naReleaseRefCount", "destructor available but no data.");
    #endif
    
    // Call the destructor on the data if available.
    if(data){
      if(destructor){destructor(data);}
      naCleanupMemory(data, naGetRefCountCleanupData(refcount));
    }

    // Cleanup the struct as marked.
    naCleanupMemory(refcount, naGetRefCountCleanupStruct(refcount));
        
    // Note that this is the only position in the whole NALib code where an
    // NAPointer is deleted. Therefore you could theoretically include the
    // code written in naDestructPointer directly here to save one function
    // call. But as NALib assumes that NAPointer is only used when really
    // needed for reference counting, it is nontheless put into a separate
    // function such that this function can be inlined for sure and thus
    // becomes very fast.
    return NA_TRUE;
  }
  // Note that other programming languages have incorporated this very idea
  // of self-organized reference-counting pointers deeply within its core.
  // Their runtime-systems keep giant pools of free objects at hand and take
  // care of detecting and collecting unused objects. In C and C++, no such
  // mechanisms exist and must be implemented manually. NARuntime is a small
  // example of such a system.
  return NA_FALSE;
}




// //////////////////////////
// NAPtr
// //////////////////////////

struct NAPtr{
  union{                        // A union storing either ...
    const void* constd;         // ... const data or ...
    void*       d;              // ... non-const (mutable) data.
  } data;
  #ifndef NDEBUG
    NAUInt flags;              // This field stores some flags.
  #endif
};
// Note that this is one of the very, very rare situations, where a union type
// actually makes sense.
//
// Also note that NAPtr will be fully optimized to a simple C-pointer
// when NDEBUG is defined.
//
// Authors note:
// The decision to include so many debugging information arose over many
// iterations. Mainly NAString was the cause of pushing debugging information
// deeply into the core functionality of NALib. They should be here to help
// with very fundamental datastructures like an NAByteArray but not to distract
// on higher levels. Further more, questions like "Is String Null-Terminated"
// should not be decided during (release) runtime. Therefore, the NAPtr
// implementation both supports but also limits the use of debug information
// depending whether you compile for debug or release.
//
// There would be the possibility to add such debug information to any pointer
// returned by naMalloc and similar functions. The information could be hidden
// in fields not visible to the user. But as naMalloc is basically just a
// wrapper for malloc, the two must be interchangable. Trying to use debug
// fields of a pointer which is not allocated with naMalloc may be more
// devastating than helpful. Therefore, the NAPtr struct was added which
// provides both a useful extension to differentiate between const and mutable
// as well as additional debugging functions for all who want to really make
// sure their code is doing what is expected.
//
// If you simply want to store a pointer without any knowledge of const or
// mutable and without any debugging information, just use a normal C-pointer.





// The following macros and functions are only available when debugging and
// are defined to be helper functions. They are needed when debugging to
// tag NAPtr values with various information.
#ifndef NDEBUG
  #define NA_PTR_CONST_DATA                 0x01
  #define NA_PTR_CLEANED                    0x02

  // Marks an NAPtr to know what the cleanup method shall be. This will later
  // on be used when cleaning the NAPtr. It the used method will not correspond
  // to the method marked, a debug warning will be emitted.
  NA_HIDEF void naMarkPtrCleanup(NAPtr* ptr, NAMemoryCleanup cleanup){
    if(!naIsCleanupValid(cleanup))
      naError("naMarkPtrCleanup", "Invalid cleanup option");
    ptr->flags |= (NAUInt)(cleanup) << NA_REFCOUNT_DATA_CLEANUP_BITSHIFT;
  }
  
  
#endif




NA_IDEF NAPtr naMakeNullPtr(){
  NAPtr ptr;
  ptr.data.d = NA_NULL;
  #ifndef NDEBUG
    ptr.flags = NA_ZERO; // Do not mark a null pointer as const. Otherwise many
                         // more errors will spawn.
    naMarkPtrCleanup(&ptr, NA_MEMORY_CLEANUP_NONE);
  #endif
  return ptr;
}



// Note: bytesize 0 ist not allowed. Allowing it would introduce an if statement
// which should not exist at this low level function. A debug error will fire
// and you have to deal with zero sizes in higher level functions.
NA_IDEF NAPtr naMakePtrWithBytesize(NAInt bytesize){
  NAPtr ptr;
  #ifndef NDEBUG
    if(bytesize == NA_ZERO)
      naError("naMakePtrWithBytesize", "bytesize is zero.");
  #endif
  ptr.data.d = naMalloc(bytesize);
  #ifndef NDEBUG
    ptr.flags = NA_ZERO;
    naMarkPtrCleanup(&ptr, NA_MEMORY_CLEANUP_NA_FREE);
  #endif
  return ptr;
}



NA_IDEF void naClearPtr(NAPtr* ptr){
  #ifndef NDEBUG
    if(ptr->flags & NA_PTR_CLEANED)
      naError("naClearPtr", "NAPtr has already been cleaned once.");
    if(naGetFlagCleanupData(ptr->flags) != NA_MEMORY_CLEANUP_NONE)
      naError("naClearPtr", "Ptr has NOT the NONE cleanup hint. Use the appropriate clear function naFreePtr, naFreeAlignedPtr or naDeletePtr.");
  #endif
  // As this is the NONE cleanup, we do nothing when NDEBUG is defined.
  #ifndef NDEBUG
    ptr->flags |= NA_PTR_CLEANED;
  #else
    NA_UNUSED(ptr);
  #endif
}



NA_IDEF void naFreePtr(NAPtr* ptr){
  #ifndef NDEBUG
    if(ptr->flags & NA_PTR_CLEANED)
      naError("naFreePtr", "NAPtr has already been cleaned once.");
    if(naIsPtrConst(ptr))
      naError("naFreePtr", "Trying to free a const pointer");
    if(naGetFlagCleanupData(ptr->flags) != NA_MEMORY_CLEANUP_NA_FREE)
      naError("naFreePtr", "Ptr has not the NA_FREE cleanup hint");
  #endif
  free(ptr->data.d);
  #ifndef NDEBUG
    ptr->flags |= NA_PTR_CLEANED;
  #endif
}



NA_IDEF void naFreeAlignedPtr(NAPtr* ptr){
  #ifndef NDEBUG
    if(ptr->flags & NA_PTR_CLEANED)
      naError("naFreeAlignedPtr", "NAPtr has already been cleaned once.");
    if(naIsPtrConst(ptr))
      naError("naFreeAlignedPtr", "Trying to free-aligned a const pointer");
    if(naGetFlagCleanupData(ptr->flags) != NA_MEMORY_CLEANUP_NA_FREE_ALIGNED)
      naError("naFreeAlignedPtr", "Ptr has not the NA_FREE_ALIGNED cleanup hint");
  #endif
  #if NA_SYSTEM == NA_SYSTEM_WINDOWS
    _aligned_free(ptr->data.d);
  #else
    free(ptr->data.d);
  #endif
  #ifndef NDEBUG
    ptr->flags |= NA_PTR_CLEANED;
  #endif
}



NA_IDEF void naNaFreePtr(NAPtr* ptr){
  #ifndef NDEBUG
    if(ptr->flags & NA_PTR_CLEANED)
      naError("naFreePtr", "NAPtr has already been cleaned once.");
    if(naIsPtrConst(ptr))
      naError("naFreePtr", "Trying to free a const pointer");
    if(naGetFlagCleanupData(ptr->flags) != NA_MEMORY_CLEANUP_NA_FREE)
      naError("naFreePtr", "Ptr has not the NA_FREE cleanup hint");
  #endif
  naFree(ptr->data.d);
  #ifndef NDEBUG
    ptr->flags |= NA_PTR_CLEANED;
  #endif
}



NA_IDEF void naNaFreeAlignedPtr(NAPtr* ptr){
  #ifndef NDEBUG
    if(ptr->flags & NA_PTR_CLEANED)
      naError("naFreeAlignedPtr", "NAPtr has already been cleaned once.");
    if(naIsPtrConst(ptr))
      naError("naFreeAlignedPtr", "Trying to free-aligned a const pointer");
    if(naGetFlagCleanupData(ptr->flags) != NA_MEMORY_CLEANUP_NA_FREE_ALIGNED)
      naError("naFreeAlignedPtr", "Ptr has not the NA_FREE_ALIGNED cleanup hint");
  #endif
  naFreeAligned(ptr->data.d);
  #ifndef NDEBUG
    ptr->flags |= NA_PTR_CLEANED;
  #endif
}



#ifdef __cplusplus 

  NA_IDEF void naDeletePtr(NAPtr* ptr){
    #ifndef NDEBUG
      if(ptr->flags & NA_PTR_CLEANED)
        naError("naDeletePtr", "NAPtr has already been cleaned once.");
      if(naIsPtrConst(ptr))
        naError("naDeletePtr", "Trying to delete a const pointer");
      if(naGetFlagCleanupData(ptr->flags) != NA_MEMORY_CLEANUP_DELETE)
        naError("naDeletePtr", "Ptr has not the DELETE cleanup hint");
    #endif
    delete ptr->data.d;
    #ifndef NDEBUG
      ptr->flags |= NA_PTR_CLEANED;
    #endif
  }



  NA_IDEF void naDeleteBrackPtr(NAPtr* ptr){
    #ifndef NDEBUG
      if(ptr->flags & NA_PTR_CLEANED)
        naError("naDeleteBrackPtr", "NAPtr has already been cleaned once.");
      if(naIsPtrConst(ptr))
        naError("naDeleteBrackPtr", "Trying to delete a const pointer");
      if(naGetFlagCleanupData(ptr->flags) != NA_MEMORY_CLEANUP_DELETE_BRACK)
        naError("naDeleteBrackPtr", "Ptr has not the DELETE_BRACK cleanup hint");
    #endif
    delete [] ptr->data.d;
    #ifndef NDEBUG
      ptr->flags |= NA_PTR_CLEANED;
    #endif
  }

#endif


NA_IDEF void naNaDeletePtr(NAPtr* ptr){
  #ifndef NDEBUG
    if(ptr->flags & NA_PTR_CLEANED)
      naError("naNaDeletePtr", "NAPtr has already been cleaned once.");
    if(naIsPtrConst(ptr))
      naError("naNaDeletePtr", "Trying to delete a const pointer");
    if(naGetFlagCleanupData(ptr->flags) != NA_MEMORY_CLEANUP_NA_DELETE)
      naError("naNaDeletePtr", "Ptr has not the NA_DELETE cleanup hint");
  #endif
  naDelete(ptr->data.d);
  #ifndef NDEBUG
    ptr->flags |= NA_PTR_CLEANED;
  #endif
}



NA_IDEF NAPtr naMakePtrWithDataConst(const void* data){
  NAPtr ptr;
  ptr.data.constd = data;
  #ifndef NDEBUG
    ptr.flags = NA_PTR_CONST_DATA;
    naMarkPtrCleanup(&ptr, NA_MEMORY_CLEANUP_NONE);
  #endif
  return ptr;
}



NA_IDEF NAPtr naMakePtrWithDataMutable(void* data, NAMemoryCleanup cleanuphint){
  NAPtr ptr;
  ptr.data.d = data;
  #ifndef NDEBUG
    ptr.flags = NA_ZERO;
    naMarkPtrCleanup(&ptr, cleanuphint);
  #else
    NA_UNUSED(cleanuphint);
  #endif
  return ptr;
}



NA_IDEF NAPtr naMakePtrWithExtraction(const NAPtr* srcptr, NAUInt byteoffset, NAUInt bytesizehint){
  NAPtr dstptr;
  #ifndef NDEBUG
    if((NAInt)byteoffset < NA_ZERO)
      naError("naMakePtrWithExtraction", "offset seems to be negative but should be unsigned.");
    if((NAInt)bytesizehint < NA_ZERO)
      naError("naMakePtrWithExtraction", "bytesizehint seems to be negative but should be unsigned.");
  #endif
  dstptr.data.d = &(((NAByte*)(srcptr->data.d))[byteoffset]);
  #ifndef NDEBUG
    // Now, we set the sizes and decide if certain flags still are valid.
    dstptr.flags = srcptr->flags & NA_PTR_CONST_DATA;
    naMarkPtrCleanup(&dstptr, NA_MEMORY_CLEANUP_NONE);
  #else
    NA_UNUSED(bytesizehint);
  #endif
  return dstptr;
}



NA_IDEF const void* naGetPtrConst(const NAPtr* ptr){
  #ifndef NDEBUG
    if(!ptr){
      naCrash("naGetPtrConst", "ptr is Null-Pointer.");
      return NA_NULL;
    }
  #endif
  return ptr->data.constd;
}



NA_IDEF void* naGetPtrMutable(NAPtr* ptr){
  #ifndef NDEBUG
    if(!ptr){
      naCrash("naGetPtrMutable", "ptr is Null-Pointer.");
      return NA_NULL;
    }
    if(ptr->flags & NA_PTR_CONST_DATA)
      naCrash("naGetPtrMutable", "Mutable access to const ptr.");
  #endif
  return ptr->data.d;
}



NA_IDEF NABool naIsPtrConst(const NAPtr* ptr){
  #ifndef NDEBUG
    return (ptr->flags & NA_PTR_CONST_DATA);
  #else
    NA_UNUSED(ptr);
    return NA_FALSE;
  #endif
}








// //////////////////////////
// NASmartPtr
// //////////////////////////

struct NASmartPtr{
  NARefCount refcount;      // Reference count. Including flags
  NAPtr ptr;
};





NA_IDEF NASmartPtr* naInitSmartPtrConst(NASmartPtr* sptr, NAMemoryCleanup smartptrcleanup, const void* data){
  #ifndef NDEBUG
    if(!naIsCleanupValid(smartptrcleanup))
      naError("naInitSmartPtr", "smartptrcleanup method invalid");
    #ifdef __cplusplus 
      if(smartptrcleanup == NA_MEMORY_CLEANUP_DELETE_BRACK)  
        naError("naInitSmartPtr", "This smartptrcleanup method does not make sense");
    #endif
  #endif
  sptr->ptr = naMakePtrWithDataConst(data);
  naInitRefCount(&(sptr->refcount), smartptrcleanup, NA_MEMORY_CLEANUP_NONE);
  return sptr;
}



NA_IDEF NASmartPtr* naInitSmartPtrMutable(NASmartPtr* sptr, NAMemoryCleanup smartptrcleanup, void* data, NAMemoryCleanup datacleanup){
  #ifndef NDEBUG
    if(!naIsCleanupValid(smartptrcleanup))
      naError("naInitSmartPtr", "smartptrcleanup method invalid");
    #ifdef __cplusplus 
      if(smartptrcleanup == NA_MEMORY_CLEANUP_DELETE_BRACK)  
        naError("naInitSmartPtr", "This smartptrcleanup method does not make sense");
    #endif
    if(!naIsCleanupValid(datacleanup))
      naError("naInitSmartPtr", "datacleanup method invalid");
  #endif
  sptr->ptr = naMakePtrWithDataMutable(data, datacleanup);
  naInitRefCount(&(sptr->refcount), smartptrcleanup, datacleanup);
  return sptr;
}




NA_IDEF NASmartPtr* naRetainSmartPtr(NASmartPtr* sptr){
  #ifndef NDEBUG
    if(!sptr){
      naCrash("naRetainSmartPtr", "sptr is Null-Pointer.");
      return NA_NULL;
    }
  #endif
  return (NASmartPtr*)naRetainRefCount(&(sptr->refcount));
}



NA_IDEF NABool naReleaseSmartPtr(NASmartPtr* sptr, NAMutator destructor){
  #ifndef NDEBUG
    if(!sptr){
      naCrash("naReleaseSmartPtr", "sptr is Null-Pointer.");
      return NA_FALSE;
    }
  #endif

  if(naGetRefCountCleanupData(&(sptr->refcount)) == NA_MEMORY_CLEANUP_NONE){
    return naReleaseRefCount(&(sptr->refcount), NA_NULL, NA_NULL);
  }else{
    return naReleaseRefCount(&(sptr->refcount), naGetSmartPtrMutable(sptr), destructor);
  }
}



NA_IDEF const void* naGetSmartPtrConst(const NASmartPtr* sptr){
  return naGetPtrConst(&(sptr->ptr));
}



NA_IDEF void* naGetSmartPtrMutable(NASmartPtr* sptr){
  return naGetPtrMutable(&(sptr->ptr));
}








// //////////////////////////
// NAPointer
// //////////////////////////

struct NAPointer{
  NASmartPtr sptr;
  NAMutator destructor;
};

// Prototype of the destruct function defined in the implementation file.
NA_RUNTIME_TYPE(NAPointer, NA_NULL);




NA_IDEF NAPointer* naNewPointerConst(const void* data){
  NAPointer* pointer; // Declaration before definition. Needed for C90
  pointer = naNew(NAPointer);
  // As sptr is the first entry of pointer, we can trick sptr to delete the
  // object when refcount becomes zero and delete the whole NAPointer object
  // in the end.
  naInitSmartPtrConst(&(pointer->sptr), NA_MEMORY_CLEANUP_NA_DELETE, data);
  pointer->destructor = NA_NULL;
  return pointer;
}



NA_IDEF NAPointer* naNewPointerMutable(void* data, NAMemoryCleanup datacleanup, NAMutator destructor){
  NAPointer* pointer; // Declaration before definition. Needed for C90
  #ifndef NDEBUG
    if(!naIsCleanupValid(datacleanup))
      naError("naNewPointer", "cleanup method invalid");
  #endif
  pointer = naNew(NAPointer);
  // As sptr is the first entry of pointer, we can trick sptr to delete the
  // object when refcount becomes zero and delete the whole NAPointer object
  // in the end.
  naInitSmartPtrMutable(&(pointer->sptr), NA_MEMORY_CLEANUP_NA_DELETE, data, datacleanup);
  pointer->destructor = destructor;
  return pointer;
}



NA_HIDEF void naDestructPointer(NAPointer* pointer){
  if(pointer->destructor){
    pointer->destructor(naGetSmartPtrMutable(&(pointer->sptr)));
  }
}



NA_IDEF NAPointer* naRetainPointer(NAPointer* pointer){
  #ifndef NDEBUG
    if(!pointer)
      {naCrash("naRetainPointer", "pointer is Null-Pointer."); return NA_NULL;}
  #endif
  naRetainSmartPtr(&(pointer->sptr));
  return pointer;
}



NA_IDEF NABool naReleasePointer(NAPointer* pointer){
  #ifndef NDEBUG
    if(!pointer){
      naCrash("naReleasePointer", "pointer is Null-Pointer.");
      return NA_FALSE;
    }
  #endif
  return naReleaseSmartPtr(&(pointer->sptr), pointer->destructor);
}




NA_IDEF const void* naGetPointerConst(const NAPointer* pointer){
  return naGetSmartPtrConst(&(pointer->sptr));
}



NA_IDEF void* naGetPointerMutable(NAPointer* pointer){
  return naGetSmartPtrMutable(&(pointer->sptr));
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
