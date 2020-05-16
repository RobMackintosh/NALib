
typedef struct NATreeBinNode NATreeBinNode;
struct NATreeBinNode{
  NATreeNode node;
  NATreeItem* childs[2];
  union{
    double d;
    NAInt i;
  } key;
  NAPtr userdata;
};
NA_EXTERN_RUNTIME_TYPE(NATreeBinNode);

typedef struct NATreeBinLeaf NATreeBinLeaf;
struct NATreeBinLeaf{
  NATreeLeaf leaf;
  union{
    double d;
    NAInt i;
  } key;
  NAPtr userdata;
};
NA_EXTERN_RUNTIME_TYPE(NATreeBinLeaf);

#include <stddef.h>
#define NODE_CHILDS_OFFSET_BIN     offsetof(NATreeBinNode, childs)
#define LEAF_KEY_OFFSET_BIN        offsetof(NATreeBinLeaf, key)
#define NODE_KEY_OFFSET_BIN        offsetof(NATreeBinNode, key)
#define LEAF_USERDATA_OFFSET_BIN   offsetof(NATreeBinLeaf, userdata)
#define NODE_USERDATA_OFFSET_BIN   offsetof(NATreeBinNode, userdata)

NA_HAPI  NAInt naGetChildIndexBinDouble(NATreeNode* parentnode, const void* childkey);
NA_HAPI  NAInt naGetKeyIndexBinDouble(const void* basekey, const void* testkey, const void* data);
NA_HAPI  NABool naTestKeyBinDouble(const void* lowerlimit, const void* upperlimit, const void* key);
NA_HAPI NABool naTestKeyLeafContainBinDouble(NATreeLeaf* leaf, const void* key);

NA_HAPI  NAInt naGetChildIndexBinNAInt(NATreeNode* parentnode, const void* childkey);
NA_HAPI  NAInt naGetKeyIndexBinNAInt(const void* basekey, const void* key, const void* data);
NA_HAPI  NABool naTestKeyBinNAInt(const void* lowerlimit, const void* upperlimit, const void* key);
NA_HAPI NABool naTestKeyLeafContainBinNAInt(NATreeLeaf* leaf, const void* key);

NA_HAPI  void naDestructTreeNodeBin(NATreeNode* node);
NA_HAPI  void naDestructTreeLeafBin(NATreeLeaf* leaf);

NA_HAPI  NATreeNode* naLocateBubbleBin(const NATree* tree, NATreeItem* item, const void* key);
NA_HAPI  NATreeNode* naRemoveLeafBin(NATree* tree, NATreeLeaf* leaf);
NA_HAPI  NATreeLeaf* naInsertLeafBin(NATree* tree, NATreeItem* existingItem, const void* key, NAPtr content, NATreeLeafInsertOrder insertOrder);

NA_HAPI  void naInitNodeAVL(NATreeBinNode* binnode);
NA_HAPI  void naGrowAVL(NATree* tree, NATreeBinNode* binnode, NAInt childindx);
NA_HAPI  void naShrinkAVL(NATree* tree, NATreeBinNode* binnode, NAInt childindx);

NA_HIAPI NATreeItem* naGetBinNodeItem(NATreeBinNode* binnode);






NA_HIDEF NATreeNode* naGetBinNodeNode(NATreeBinNode* binnode){
  return &(binnode->node);
}
NA_HIDEF NATreeLeaf* naGetBinLeafLeaf(NATreeBinLeaf* binleaf){
  return &(binleaf->leaf);
}



NA_HIDEF NATreeItem* naGetBinNodeItem(NATreeBinNode* binnode){
  return naGetTreeNodeItem(naGetBinNodeNode(binnode));
}
NA_HIDEF NATreeItem* naGetBinLeafItem(NATreeBinLeaf* binleaf){
  return naGetTreeLeafItem(naGetBinLeafLeaf(binleaf));
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
