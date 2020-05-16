
#include "../NAList.h"




NA_RUNTIME_TYPE(NAListElement, NA_NULL, NA_FALSE);



NA_DEF NABool naLocateListData(NAListIterator* iter, const void* data){
  // todo: search in left-right exponential search starting from the current
  // position.

  naResetListIterator(iter);
  while(naIterateList(iter)){
    if(naGetPtrConst(iter->cur->ptr) == data){
      return NA_TRUE;
    }
  }

  // Reaching here, content could not be found.
  return NA_FALSE;
}



NA_DEF NABool naLocateListIndex(NAListIterator* iter, NAInt indx){
  NAList* mutablelist = (NAList*)naGetPtrConst(iter->listptr);

  if(indx < 0){indx += mutablelist->count;}
  if(indx < 0){
    #ifndef NDEBUG
      naError("Negative index underflows the range of the list");
    #endif
    iter->cur = &(mutablelist->sentinel);
    return NA_FALSE;
  }
  if(indx >= (NAInt)mutablelist->count){
    #ifndef NDEBUG
      naError("Index overflows the range of the list");
    #endif
    iter->cur = &(mutablelist->sentinel);
    return NA_FALSE;
  }

  if(indx < ((NAInt)mutablelist->count / 2)){
    // Go forward from start to end
    iter->cur = mutablelist->sentinel.next;
    while(indx){
      iter->cur = iter->cur->next;
      indx--;
    }
  }else{
    // Go backward from end to start
    iter->cur = mutablelist->sentinel.prev;
    indx = indx - mutablelist->count + 1;
    while(indx){
      iter->cur = iter->cur->prev;
      indx++;
    }
  }

  return NA_TRUE;
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
