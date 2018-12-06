

// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.


#include "NABuffer.h"


NA_HAPI void naDeallocBuffer(NABuffer* buffer);
NA_RUNTIME_TYPE(NABuffer, naDeallocBuffer, NA_TRUE);



#define NA_BUFFER_FLAG_RANGE_FIXED       0x01



void naDestructBufferTreeLeaf(NAPtr leafdata, NAPtr configdata){
  NA_UNUSED(configdata);
  NABufferPart* part = naGetPtrMutable(&leafdata);
  naDelete(part);
}



NAPtr naConstructBufferTreeNode(const void* key, NAPtr configdata){
  NABufferTreeNodeData* nodedata = naAlloc(NABufferTreeNodeData);
  nodedata->len1 = 0;
  nodedata->len2 = 0;
  return naMakePtrWithDataMutable(nodedata);
}



void naDestructBufferTreeNode(NAPtr nodedata, NAPtr configdata){
  naFree(naGetPtrMutable(&nodedata));
}



NABool naUpdateBufferTreeNode(NAPtr parentdata,  NAPtr* childdatas, NAInt childindx, NAInt childmask){
  NABufferTreeNodeData* parentnodedata = (NABufferTreeNodeData*)naGetPtrMutable(&parentdata);

  NAInt prevlen1 = parentnodedata->len1;
  if(childmask & 0x01){
    NABufferPart* leafdata = (NABufferPart*)naGetPtrMutable(&(childdatas[0]));
    parentnodedata->len1 = leafdata->bytesize;
  }else{
    NABufferTreeNodeData* childdata = (NABufferTreeNodeData*)naGetPtrMutable(&(childdatas[0]));
    parentnodedata->len1 = childdata->len1 + childdata->len2;
  }

  NAInt prevlen2 = parentnodedata->len2;
  if(childmask & 0x02){
    NABufferPart* leafdata = (NABufferPart*)naGetPtrMutable(&(childdatas[1]));
    parentnodedata->len2 = leafdata->bytesize;
  }else{
    NABufferTreeNodeData* childdata = (NABufferTreeNodeData*)naGetPtrMutable(&(childdatas[1]));
    parentnodedata->len2 = childdata->len1 + childdata->len2;
  }

  return (parentnodedata->len1 != prevlen1) || (parentnodedata->len2 != prevlen2);
}



NA_HDEF NABool naSearchBufferNode(void* token, NAPtr data, NAInt* nextindx){
  NABufferSearchToken* searchtoken = (NABufferSearchToken*)token;
  NABufferTreeNodeData* nodedata = (NABufferTreeNodeData*)naGetPtrMutable(&data);

  if((searchtoken->searchoffset < searchtoken->curoffset) || (searchtoken->searchoffset >= searchtoken->curoffset + nodedata->len1 + nodedata->len2)){
    *nextindx = -1;
  }else{
    if(searchtoken->searchoffset < searchtoken->curoffset + nodedata->len1){
      *nextindx = 0;
    }else{
      searchtoken->curoffset += nodedata->len1;
      *nextindx = 1;
    }
  }
  return NA_TRUE;
}



NA_HDEF NABool naSearchBufferLeaf(void* token, NAPtr data, NABool* matchfound){
  NABufferSearchToken* searchtoken = (NABufferSearchToken*)token;
  NABufferPart* leafdata = (NABufferPart*)naGetPtrMutable(&data);

  if((searchtoken->searchoffset >= searchtoken->curoffset) && (searchtoken->searchoffset < searchtoken->curoffset + leafdata->bytesize)){
    *matchfound = NA_TRUE;
  }else{
    *matchfound = NA_FALSE;
  }
  return NA_FALSE;
}



NA_HDEF void naInitBufferStruct(NABuffer* buffer){
  buffer->flags = 0;
  buffer->range = naMakeRangeiWithStartAndEnd(0, 0);
  NATreeConfiguration* config = naCreateTreeConfiguration(NA_TREE_KEY_NOKEY | NA_TREE_BALANCE_AVL);
  naSetTreeConfigurationNodeCallbacks(config, naConstructBufferTreeNode, naDestructBufferTreeNode, naUpdateBufferTreeNode);
  naSetTreeConfigurationLeafCallbacks(config, NA_NULL, naDestructBufferTreeLeaf);
  naSetTreeConfigurationTokenCallbacks(config, naSearchBufferNode, naSearchBufferLeaf);
  naInitTree(&(buffer->parts), config);
  #ifndef NDEBUG
    buffer->itercount = 0;
  #endif
  naReleaseTreeConfiguration(config);
}



// This is the filler method of the secure memory input source descriptor
//NA_HIDEF void naFillBufferPartSecureMemory(void* data, void* dst, NARangei range){
//  NA_UNUSED(data);
//  NA_UNUSED(dst);
//  NA_UNUSED(range);
////  NA_UNUSED(data);
////  naNulln(dst, range.length);
//}



// This is a special buffer creation method which is not visible to the
// programmer.
NA_HDEF NABuffer* naNewBufferMemorySourceBuffer(NABool secure){
  NA_UNUSED(secure);
//  NABufferSourceDescriptor desc;
//
//  NABuffer* buffer = naNew(NABuffer);
//  naInitBufferStruct(buffer);
//
//  // Setting up the source descriptor
//  naNulln(&desc, sizeof(desc));
//  if(secure){desc.filler = naFillBufferPartSecureMemory;}
//  // Creating the source
//  buffer->source = naNewBufferSource(desc);
//  buffer->srcoffset = 0;
//
//  buffer->newlineencoding = NA_NEWLINE_NATIVE;
//  buffer->endianness = NA_ENDIANNESS_UNKNOWN;
//  buffer->converter = naMakeEndiannessConverter(buffer->endianness, NA_ENDIANNESS_NATIVE);
//
//  return buffer;
  return NA_NULL;
}



NA_DEF NABuffer* naNewBuffer(NABool securememory){
  NA_UNUSED(securememory);
  // todo: secure source

  NABuffer* buffer = naNew(NABuffer);
  naInitBufferStruct(buffer);

  buffer->enhancesource = NA_NULL;
  buffer->enhancesourceoffset = 0;

  buffer->range = naMakeRangeiWithStartAndEnd(0, 0);
  buffer->flags = 0;

  buffer->newlineencoding = NA_NEWLINE_NATIVE;
  buffer->endianness = NA_ENDIANNESS_UNKNOWN;
  buffer->converter = naMakeEndiannessConverter(buffer->endianness, NA_ENDIANNESS_NATIVE);

  return buffer;
}



// Uses range to point into containingrange. Negative offset and length measure
// bytes from the end of the buffer.
NA_HIAPI NARangei naMakeRangeiAbsolute(NARangei range, NARangei containingrange){
  NAInt start;
  NAInt end;
  #ifndef NDEBUG
    if(!naIsLengthValueUseful(containingrange.length))
      naError("naMakeRangeiAbsolute", "Length of containing range is not useful.");
  #endif
  start = containingrange.origin + (range.origin < 0) * containingrange.length + range.origin;
  end = ((range.length >= 0) ? start : naGetRangeiEnd(containingrange)) + range.length;
  #ifndef NDEBUG
    if(!naContainsRangeiOffset(containingrange, start))
      naError("naMakeRangeiAbsolute", "Resulting range underflows containing range.");
    if(!naContainsRangeiOffset(containingrange, naMakeMaxWithEndi(end)))
      naError("naMakeRangeiAbsolute", "Resulting range overflows containing range.");
    if(end < start)
      naError("naMakeRangeiAbsolute", "Resulting range has negative length.");
  #endif
  return naMakeRangeiWithStartAndEnd(start, end);
}



NA_DEF NABuffer* naNewBufferExtraction(NABuffer* srcbuffer, NARangei range){
  NABuffer* buffer = naNew(NABuffer);
  naInitBufferStruct(buffer);

  NARangei absoluterange = naMakeRangeiAbsolute(range, srcbuffer->range);

  buffer->enhancesource = NA_NULL;
  buffer->enhancesourceoffset = 0;

  buffer->range = naMakeRangei(0, absoluterange.length);
  buffer->flags |= NA_BUFFER_FLAG_RANGE_FIXED;

  NABufferSource* bufsource = naCreateBufferSource(NA_NULL, srcbuffer);
    buffer->enhancesource = naRetainBufferSource(bufsource);
    buffer->enhancesourceoffset = -range.origin;
  naReleaseBufferSource(bufsource);

  // Add the const data to the list.
  NABufferPart* part = naNewBufferPartSparse(bufsource, absoluterange);
  naAddTreeFirstMutable(&(buffer->parts), part);

  buffer->flags = srcbuffer->flags | NA_BUFFER_FLAG_RANGE_FIXED;

  buffer->newlineencoding = srcbuffer->newlineencoding;
  buffer->endianness = srcbuffer->endianness;
  buffer->converter = naMakeEndiannessConverter(buffer->endianness, NA_ENDIANNESS_NATIVE);

  return buffer;
}



NA_DEF NABuffer* naNewBufferCopy(const NABuffer* srcbuffer, NARangei range, NABool securememory){
  NA_UNUSED(srcbuffer);
  NA_UNUSED(range);
  NA_UNUSED(securememory);
//  NABufferIterator srciter;
//  NABufferIterator dstiter;
//  NABuffer* buffer = naNewBuffer(securememory);
//
//  if(range.length == 0){return buffer;}
//
//  srciter = naMakeBufferAccessor(srcbuffer);
//  dstiter = naMakeBufferModifier(buffer);
//
//  naPrepareBuffer(&dstiter, naMakeRangei(0, range.length), NA_FALSE);
//  naIterateBuffer(&srciter, 1);
//
//  while(range.length){
//    NABufferPart* dstpart = naGetListCurMutable(&(dstiter.partiter));
//    const NABufferPart* srcpart = naGetListCurConst(&(srciter.partiter));
//    NAInt remainingsrc = naGetBufferPartEnd(srcpart) - range.origin;
//    NAInt remainingdst = naGetBufferPartEnd(dstpart) - naTellBuffer(&dstiter);
//    NAInt remaining = naMini(remainingsrc, remainingdst);
////    remaining = naMini(remaining, range.length);
//    naCopyn(naGetBufferPartDataPointerMutable(dstpart, naTellBuffer(&dstiter)), naGetBufferPartDataPointerConst(srcpart, range.origin), remaining);
//    naIterateBuffer(&srciter, remaining);
//    naIterateBuffer(&dstiter, remaining);
//    range.origin += remaining;
//    range.length -= remaining;
//  }
//
//  naClearBufferIterator(&srciter);
//  naClearBufferIterator(&dstiter);
//
//  return buffer;
  return NA_NULL;
}



NA_DEF NABuffer* naNewBufferPlain(){
//  NABuffer* buffer = naNew(NABuffer);
//  naInitBufferStruct(buffer);
//
//  // The source is a Null pointer.
//  buffer->source = NA_NULL;
//  buffer->srcoffset = 0;
//
//  buffer->newlineencoding = NA_NEWLINE_NATIVE;
//  buffer->endianness = NA_ENDIANNESS_UNKNOWN;
//  buffer->converter = naMakeEndiannessConverter(buffer->endianness, NA_ENDIANNESS_NATIVE);
//
//  return buffer;
  return NA_NULL;
}



NA_DEF NABuffer* naNewBufferWithSameSource(NABuffer* srcbuffer){
  NA_UNUSED(srcbuffer);
//  NABuffer* buffer = naNew(NABuffer);
//  naInitBufferStruct(buffer);
//
//  buffer->source = naRetain(srcbuffer->source);
//  buffer->srcoffset = srcbuffer->srcoffset;
//
//  buffer->flags = srcbuffer->flags;
//
//  buffer->newlineencoding = srcbuffer->newlineencoding;
//  buffer->endianness = srcbuffer->endianness;
//  buffer->converter = naMakeEndiannessConverter(buffer->endianness, NA_ENDIANNESS_NATIVE);
//
//  return buffer;
  return NA_NULL;
}



// This is the filler method of the file input source descriptor
NA_HIDEF void naFillBufferPartFile(void* data, void* dst, NARangei range){
  naReadFileBytes(data, dst, range.length);
}



NA_DEF NABuffer* naNewBufferWithInputFile(const char* filename){
  NARangei range;
  NAFile* file;

  NABuffer* buffer = naNew(NABuffer);
  naInitBufferStruct(buffer);

  file = naCreateFileReadingFilename(filename);
  range = naMakeRangei(0, (NAInt)naComputeFileBytesize(file));

  NABuffer* filebuffer = naNewBuffer(NA_FALSE);
  NABufferSource* readsource = naCreateBufferSource(naFillBufferPartFile, NA_NULL);
    naSetBufferSourceData(readsource, file, (NAMutator)naReleaseFile);
    naSetBufferSourceLimit(readsource, range);
    filebuffer->enhancesource = naRetainBufferSource(readsource);
    filebuffer->enhancesourceoffset = 0;
  naReleaseBufferSource(readsource);

  NABufferSource* bufsource = naCreateBufferSource(NA_NULL, filebuffer);
    buffer->enhancesource = naRetainBufferSource(bufsource);
    buffer->enhancesourceoffset = 0;
  naReleaseBufferSource(bufsource);
  naRelease(filebuffer);

  naEnsureBufferRange(buffer, 0, range.length);
  buffer->flags |= NA_BUFFER_FLAG_RANGE_FIXED;

  buffer->newlineencoding = NA_NEWLINE_NATIVE;
  buffer->endianness = NA_ENDIANNESS_UNKNOWN;
  buffer->converter = naMakeEndiannessConverter(buffer->endianness, NA_ENDIANNESS_NATIVE);

  return buffer;
}



NA_DEF NABuffer* naNewBufferWithConstData(const void* data, NAInt bytesize){
  NABufferPart* part;
  NARangei range;

  NABuffer* buffer = naNew(NABuffer);
  naInitBufferStruct(buffer);

  range = naMakeRangeiWithStartAndEnd(0, (NAInt)bytesize);

  // Add the const data to the list.
  part = naNewBufferPartWithConstData(data, bytesize);
  naAddTreeFirstMutable(&(buffer->parts), part);

  buffer->enhancesource = NA_NULL;
  buffer->enhancesourceoffset = 0;

  buffer->range = range;
  buffer->flags |= NA_BUFFER_FLAG_RANGE_FIXED;

  buffer->newlineencoding = NA_NEWLINE_NATIVE;
  buffer->endianness = NA_ENDIANNESS_UNKNOWN;
  buffer->converter = naMakeEndiannessConverter(buffer->endianness, NA_ENDIANNESS_NATIVE);

  return buffer;
}



NA_DEF NABuffer* naNewBufferWithMutableData(void* data, NAInt bytesize, NAMutator destructor){
  NABufferPart* part;
  NARangei range;

  NABuffer* buffer = naNew(NABuffer);
  naInitBufferStruct(buffer);

  range = naMakeRangeiWithStartAndEnd(0, (NAInt)bytesize);

    // Add the mutable data to the list.
    part = naNewBufferPartWithMutableData(data, bytesize, destructor);
    naAddTreeFirstMutable(&(buffer->parts), part);

  buffer->enhancesource = NA_NULL;
  buffer->enhancesourceoffset = 0;

  buffer->range = range;
  buffer->flags |= NA_BUFFER_FLAG_RANGE_FIXED;

  buffer->newlineencoding = NA_NEWLINE_NATIVE;
  buffer->endianness = NA_ENDIANNESS_UNKNOWN;
  buffer->converter = naMakeEndiannessConverter(buffer->endianness, NA_ENDIANNESS_NATIVE);

  return buffer;
}



NA_DEF NABuffer* naNewBufferWithCustomSource(NABufferSource* source){
  NA_UNUSED(source);
//  NABuffer* buffer = naNew(NABuffer);
//  naInitBufferStruct(buffer);
//
//  buffer->source = naNewBufferSource(desc);
//  buffer->srcoffset = 0;
//
//  buffer->newlineencoding = NA_NEWLINE_NATIVE;
//  buffer->endianness = NA_ENDIANNESS_UNKNOWN;
//  buffer->converter = naMakeEndiannessConverter(buffer->endianness, NA_ENDIANNESS_NATIVE);
//
//  return buffer;
  return NA_NULL;
}



NA_HDEF void naDeallocBuffer(NABuffer* buffer){
  #ifndef NDEBUG
    if(buffer->itercount)
      naError("naDeallocBuffer", "There are still iterators running. Did you forgot naClearBufferIterator?");
  #endif
  if(buffer->enhancesource){naRelease(buffer->enhancesource);}
  naEmptyTree(&(buffer->parts));
}



NA_DEF NABool naIsBufferEmpty(const NABuffer* buffer){
  return naIsRangeiEmpty(buffer->range);
}



NA_DEF NARangei naGetBufferRange(const NABuffer* buffer){
  return buffer->range;
}



NA_DEF void naFixBufferRange(NABuffer* buffer){
  buffer->flags |= NA_BUFFER_FLAG_RANGE_FIXED;
}



NA_DEF NABool naHasBufferFixedRange(const NABuffer* buffer){
  return (buffer->flags & NA_BUFFER_FLAG_RANGE_FIXED);
}



NA_DEF void naExtendBufferRange(NABuffer* buffer, NAInt bytesatstart, NAInt bytesatend){
  #ifndef NDEBUG
    if(bytesatstart < 0)
      naError("naExtendBufferRange", "bytesatstart should not be negative");
    if(bytesatend < 0)
      naError("naExtendBufferRange", "bytesatend should not be negative");
  #endif
  naEnsureBufferRange(buffer, buffer->range.origin - bytesatstart, naGetRangeiEnd(buffer->range) + bytesatend);
}



NA_DEF NANewlineEncoding naGetBufferNewlineEncoding(NABuffer* buffer){
  return buffer->newlineencoding;
}



NA_DEF void naSetBufferNewlineEncoding(NABuffer* buffer, NANewlineEncoding newlineencoding){
  buffer->newlineencoding = newlineencoding;
}



NA_DEF NAInt naGetBufferEndianness(NABuffer* buffer){
  return buffer->endianness;
}



NA_DEF void naSetBufferEndianness(NABuffer* buffer, NAInt endianness){
  buffer->endianness = endianness;
  buffer->converter = naMakeEndiannessConverter(endianness, NA_ENDIANNESS_NATIVE);
}



NA_DEF NAInt naSearchBufferByteOffset(const NABuffer* buffer, NAByte byte, NAInt startoffset, NABool forward){
  NA_UNUSED(buffer);
  NA_UNUSED(byte);
  NA_UNUSED(startoffset);
  NA_UNUSED(forward);
//  NAListIterator iter;
//
//  NAInt retindex = NA_INVALID_MEMORY_INDEX;
//  if(naIsBufferEmpty(buffer)){return NA_INVALID_MEMORY_INDEX;}
//
//  iter = naMakeListAccessor(&(buffer->parts));
//  naLocateListLast(&iter);
////  naLocateListPosition(&iter, naGetListCurPosition(&(buffer->iter)));
//  naLocateBufferPartOffset(&iter, startoffset);
//
//  while(!naIsListAtInitial(&iter)){
//    const NABufferPart* part;
//    const NAByte* curbyte;
//
//    part = naGetListCurConst(&iter);
//    #ifndef NDEBUG
//      if(naIsBufferPartSparse(part))
//        naError("naSearchBufferByteOffset", "sparse part detected.");
//    #endif
//    curbyte = naGetBufferPartDataPointerConst(part, startoffset);
//    if(forward){
//      NAInt endoffset = naGetBufferPartEnd(part);
//      while(startoffset < endoffset){
//        if(*curbyte == byte){
//          retindex = startoffset;
//          break;
//        }
//        curbyte++;
//        startoffset++;
//      }
//    }else{
//      NAInt beginoffset = naGetBufferPartStart(part) - 1;
//      while(startoffset > beginoffset){
//        if(*curbyte == byte){
//          retindex = startoffset;
//          break;
//        }
//        curbyte--;
//        startoffset--;
//      }
//    }
//    if(retindex != NA_INVALID_MEMORY_INDEX){break;}
//    if(forward){naIterateList(&iter);}else{naIterateListBack(&iter);}
//  }
//
//  naClearListIterator(&iter);
//  return retindex;
  return 0;
}



NA_DEF NABool naEqualBufferToBuffer(const NABuffer* buffer1, const NABuffer* buffer2, NABool casesensitive){
  NA_UNUSED(buffer1);
  NA_UNUSED(buffer2);
  NA_UNUSED(casesensitive);
//  NABool resultequal;
//  NAListIterator iter1;
//  NAListIterator iter2;
//  const NABufferPart* part1;
//  const NABufferPart* part2;
//  NAInt offset1;
//  NAInt offset2;
//
//  if(buffer1 == buffer2){return NA_TRUE;}
//  if(naGetBufferRange(buffer1).length != naGetBufferRange(buffer2).length){return NA_FALSE;}
//  resultequal = NA_TRUE;;
//
//  iter1 = naMakeListAccessor(&(buffer1->parts));
//  iter2 = naMakeListAccessor(&(buffer2->parts));
//  naLocateListFirst(&iter1);
//  naLocateListFirst(&iter2);
//  part1 = naGetListCurConst(&iter1);
//  part2 = naGetListCurConst(&iter2);
//
//  offset1 = naGetBufferRange(buffer1).origin;
//  offset2 = naGetBufferRange(buffer2).origin;
//
//  while(part1 && part2){
//    NAInt remainingbytes1;
//    NAInt remainingbytes2;
//    NAInt remainingbytes;
//    const NAByte* bytes1;
//    const NAByte* bytes2;
//
//    #ifndef NDEBUG
//      if(naIsBufferPartSparse(part1))
//        naError("naEqualBufferToBuffer", "Buffer 1 has sparse part");
//      if(naIsBufferPartSparse(part2))
//        naError("naEqualBufferToBuffer", "Buffer 2 has sparse part");
//    #endif
//    remainingbytes1 = naGetBufferPartEnd(part1) - offset1;
//    remainingbytes2 = naGetBufferPartEnd(part2) - offset2;
//    remainingbytes = naMini(remainingbytes1, remainingbytes2);
//    bytes1 = naGetBufferPartDataPointerConst(part1, offset1);
//    bytes2 = naGetBufferPartDataPointerConst(part2, offset2);
//    if(bytes1 != bytes2){
//      if(!naEqualUTF8CStringLiterals((NAUTF8Char*)bytes1, (NAUTF8Char*)bytes2, remainingbytes, casesensitive)){
//        resultequal = NA_FALSE;
//        break;
//      }
//    }
//    offset1 += remainingbytes;
//    offset2 += remainingbytes;
//    if(remainingbytes1 == remainingbytes){
//      naIterateList(&iter1);
//      part1 = naGetListCurConst(&iter1);;
//    }
//    if(remainingbytes2 == remainingbytes){
//      naIterateList(&iter2);
//      part2 = naGetListCurConst(&iter2);;
//    }
//  }
//
//  naClearListIterator(&iter1);
//  naClearListIterator(&iter2);
//  return resultequal;
  return NA_FALSE;
}



NA_DEF NABool naEqualBufferToData(const NABuffer* buffer, const void* data, NAInt databytesize, NABool casesensitive){
  NA_UNUSED(buffer);
  NA_UNUSED(data);
  NA_UNUSED(databytesize);
  NA_UNUSED(casesensitive);
//  NABool resultequal = NA_TRUE;
//  const NABufferPart* part;
//  NAInt offset;
//  NAInt remaininglength;
//  const NAByte* bytes;
//  NAListIterator iter;
//
//  if(databytesize != buffer->range.length){return NA_FALSE;}
//
//  bytes = (const NAByte*)data;
//
//  iter = naMakeListAccessor(&(buffer->parts));
//  naLocateListFirst(&iter);
//  part = naGetListCurConst(&iter);
//
//  offset = naGetBufferRange(buffer).origin;
//  remaininglength = buffer->range.length;
//
//  while(remaininglength){
//    NAInt remainingbytes;
//    const NAByte* bufferbytes;
//
//    #ifndef NDEBUG
//      if(naIsBufferPartSparse(part))
//        naError("naEqualBufferToData", "Buffer has sparse part");
//    #endif
//    remainingbytes = naGetBufferPartEnd(part) - offset;
//    bufferbytes = naGetBufferPartDataPointerConst(part, offset);
//    if(bufferbytes != bytes){
//      if(!naEqualUTF8CStringLiterals((NAUTF8Char*)bufferbytes, (NAUTF8Char*)bytes, remainingbytes, casesensitive)){
//        resultequal = NA_FALSE;
//        break;
//      }
//    }
//    offset += remainingbytes;
//    bytes += remainingbytes;
//    remaininglength -= remainingbytes;
//    naIterateList(&iter);
//    part = naGetListCurConst(&iter);;
//  }
//
//  naClearListIterator(&iter);
//  return resultequal;
  return NA_FALSE;
}



NA_DEF void naAppendBufferToBuffer(NABuffer* dstbuffer, const NABuffer* srcbuffer){
  NA_UNUSED(dstbuffer);
  NA_UNUSED(srcbuffer);
//  NABufferIterator iter = naMakeBufferModifier(dstbuffer);
//  naWriteBufferBuffer(&iter, srcbuffer, naGetBufferRange(srcbuffer));
//  naClearBufferIterator(&iter);
  return;
}



NA_DEF void naCacheBufferRange(NABuffer* buffer, NARangei range, NABool forcevolatile){
  if(range.length){
    NABufferIterator iter = naMakeBufferModifier(buffer);
    NABool found = naLocateBuffer(&iter, range.origin);
//    #ifndef NDEBUG
//      if(!found)
//        naError("naCacheBufferRange", "could not find range origin in buffer");
//    #else
      NA_UNUSED(found);
//    #endif
    naPrepareBuffer(&iter, range.length, forcevolatile);
    naClearBufferIterator(&iter);
  }
}



NA_DEF void naDismissBufferRange(NABuffer* buffer, NARangei range){
  NA_UNUSED(buffer);
  NA_UNUSED(range);
//  naUnlinkBufferRange(buffer, range);
  return;
}




// Copyright (c) NALib, Tobias Stamm
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
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
