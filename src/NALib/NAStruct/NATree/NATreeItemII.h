
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.


// /////////////////////////////////////
// Item
// /////////////////////////////////////

NA_HIDEF void naInitTreeItem(NATreeItem* item){
  // We do not initialize the parent. Code must use naSetTreeItemParent.
  #ifndef NDEBUG
    item->itercount = 0;
  #else
    NA_UNUSED(item);
  #endif
}



NA_HIDEF void naClearTreeItem(NATreeItem* item){
  #ifndef NDEBUG
    if(item->itercount)
      naError("There are still iterators running on this item. Did you forget a call to naClearTreeIterator?");
  #else
    NA_UNUSED(item);
  #endif
}



NA_HIDEF NATreeNode* naGetTreeItemParent(const NATreeItem* item){
  return item->parent;
}



NA_HIDEF void naSetTreeItemParent(NATreeItem* item, NATreeNode* parent){
  item->parent = parent;
}



NA_HIDEF NABool naIsTreeItemRoot(const NATree* tree, const NATreeItem* item){
  NA_UNUSED(tree);
  return (naGetTreeItemParent(item) == NA_NULL);
}



// /////////////////////////////////////
// Node
// /////////////////////////////////////

NA_HIDEF NATreeItem* naGetTreeNodeItem(NATreeNode* node){
  return &(node->item);
}



NA_HIDEF void naInitTreeNode(const NATree* tree, NATreeNode* node, const void* key){
  naInitTreeItem(naGetTreeNodeItem(node));
  node->flags = 0;

  if(tree->config->keyAssigner){
    tree->config->keyAssigner(naGetTreeNodeKey(tree, node), key);
  }

  for(NAInt i = 0; i < tree->config->childpernode; i++){
    naSetTreeNodeChild(tree, node, i, NA_NULL);
  }

  if(tree->config->nodeDataConstructor){
    naSetTreeNodeData(tree, node, tree->config->nodeDataConstructor(&key));
  }else{
    naSetTreeNodeData(tree, node, naMakePtrNull());
  }
}



NA_HIDEF void naClearTreeNode(NATreeNode* node){
  naClearTreeItem(naGetTreeNodeItem(node));
}



NA_HIDEF void naDestructNodeData(const NATree* tree, NAPtr data){
  if(tree->config->nodeDataDestructor){
    tree->config->nodeDataDestructor(data);
  }
}



NA_HIDEF void naDestructTreeNode(NATree* tree, NATreeNode* node, NABool recursive){
  #ifndef NDEBUG
    if(!node)
      naCrash("node shall not be Null");
  #endif
  
  if(recursive){
    for(NAInt i = 0; i < tree->config->childpernode; i++){
      NATreeItem* child = naGetTreeNodeChild(tree, node, i);
      if(naIsNodeChildLeaf(node, i)){
        naDestructTreeLeaf(tree, (NATreeLeaf*)child);
      }else{
        naDestructTreeNode(tree, (NATreeNode*)child, NA_TRUE);
      }
    }
  }

  naDestructNodeData(tree, naGetTreeNodeData(tree, node));
  naClearTreeNode(node);
  tree->config->nodeDestructor(node);
}



NA_HIDEF NABool naIsNodeChildLeaf(NATreeNode* node, NAInt childindx){
  return (NABool)((node->flags >> childindx) & 0x01);
}



NA_HIDEF void naMarkNodeChildLeaf(NATreeNode* node, NAInt childindx, NABool isleaf){
  node->flags &= ~(1 << childindx);
  node->flags |= (NAInt)isleaf << childindx;
}



NA_HIDEF void* naGetTreeNodeKey(const NATree* tree, NATreeNode* node){
  // We thank the power of pointer arithmetic!
  return ((char*)node) + tree->config->nodeKeyOffset; 
}



NA_HIDEF NAPtr naGetTreeNodeData(const NATree* tree, NATreeNode* node){
  // We thank the power of pointer arithmetic!
  return *(NAPtr*)(((char*)node) + tree->config->nodeUserDataOffset); 
}



NA_HIDEF void naSetTreeNodeData(const NATree* tree, NATreeNode* node, NAPtr newdata){
  // We thank the power of pointer arithmetic!
  *(NAPtr*)(((char*)node) + tree->config->nodeUserDataOffset) = newdata; 
}



NA_HIDEF NATreeItem* naGetTreeNodeChild(const NATree* tree, NATreeNode* parent, NAInt childindx){
  #ifndef NDEBUG
    if(childindx < 0)
      naError("childindx must be >= 0");
    if(childindx >= tree->config->childpernode)
      naError("childindx out of bounds");
  #endif
  // We thank the power of pointer arithmetic!
  return ((NATreeItem**)(((char*)parent) + tree->config->nodeChildsOffset))[childindx]; 
}



NA_HIDEF void naSetTreeNodeChild(const NATree* tree, NATreeNode* parent, NAInt childindx, NATreeItem* newchild){
  ((NATreeItem**)(((char*)parent) + tree->config->nodeChildsOffset))[childindx] = newchild;
}



NA_HDEF NAInt naGetTreeNodeChildIndex(const NATree* tree, NATreeNode* parent, NATreeItem* child){
  NAInt retvalue;
  #ifndef NDEBUG
    if(!child)
      naError("Child should not be Null");
  #endif
  
  // We thank the power of pointer arithmetic!
  NATreeItem** childs = ((NATreeItem**)(((char*)parent) + tree->config->nodeChildsOffset));
  
  // We assume, the child MUST be a child of this parent. Therefore we only
  // search until childpernode - 1 and just return the last possibility in
  // good belief.
  for(retvalue = 0; retvalue < tree->config->childpernode - 1; retvalue++){
    if(childs[retvalue] == child){break;}
  }
  
  #ifndef NDEBUG
    if(childs[retvalue] != child)
      naError("returned childindex does not correspond to the desired child");
  #endif
  return retvalue;
}



// /////////////////////////////////////
// Leaf
// /////////////////////////////////////

NA_HIDEF NATreeItem* naGetTreeLeafItem(NATreeLeaf* leaf){
  return &(leaf->item);
}



NA_HIDEF void naInitTreeLeaf(NATree* tree, NATreeLeaf* leaf, const void* key, NAPtr constructordata){
  naInitTreeItem(naGetTreeLeafItem(leaf));
  
  if(tree->config->keyAssigner){
    tree->config->keyAssigner(naGetTreeLeafKey(tree, leaf), key);
  }
  naSetTreeLeafData(tree, leaf, naConstructLeafData(tree, key, constructordata));
}



NA_HIDEF void naClearTreeLeaf(NATreeLeaf* leaf){
  naClearTreeItem(naGetTreeLeafItem(leaf));
}



NA_HIDEF void naDestructLeafData(const NATree* tree, NAPtr data){
  if(tree->config->leafDataDestructor){
    tree->config->leafDataDestructor(data);
  }
}



NA_HIDEF void naDestructTreeLeaf(NATree* tree, NATreeLeaf* leaf){
  #ifndef NDEBUG
    if(!leaf)
      naCrash("leaf shall not be Null");
  #endif
  naDestructLeafData(tree, naGetTreeLeafData(tree, leaf));
  naClearTreeLeaf(leaf);
  tree->config->leafDestructor(leaf);
}



NA_HIDEF NAPtr naConstructLeafData(NATree* tree, const void* key, NAPtr constructordata){
  if(tree->config->leafDataConstructor){
    return tree->config->leafDataConstructor(key, constructordata);
  }else{
    // If there is no constructor, we plainly store the given constructordata.
    return constructordata;
  }
}



NA_HIDEF void* naGetTreeLeafKey(const NATree* tree, NATreeLeaf* leaf){
  return ((char*)leaf) + tree->config->leafKeyOffset; 
}



NA_HIDEF NAPtr naGetTreeLeafData(const NATree* tree, NATreeLeaf* leaf){
  // We thank the power of pointer arithmetic!
  return *(NAPtr*)(((char*)leaf) + tree->config->leafUserDataOffset); 
}



NA_HIDEF void naSetTreeLeafData(const NATree* tree, NATreeLeaf* leaf, NAPtr newcontent){
  // We thank the power of pointer arithmetic!
  *(NAPtr*)(((char*)leaf) + tree->config->leafUserDataOffset) = newcontent; 
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
