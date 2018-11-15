
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.

#include "NATree.h"


// Prototypes
NA_HIAPI NATreeLeaf* naIterateTreeCapture(const NATree* tree, NATreeNode* curnode, NAInt previndx, NATreeIterationInfo* info);
NA_HAPI  NATreeLeaf* naIterateTreeBubble(const NATree* tree, NATreeBaseNode* curnode, NATreeIterationInfo* info);



// This function expects a parent node and searches for the next available
// child node after the given previndx. Works recursively until either no
// valid child is found or a leaf is returned
NA_HIDEF NATreeLeaf* naIterateTreeCapture(const NATree* tree, NATreeNode* parent, NAInt previndx, NATreeIterationInfo* info){
  NAInt indx = previndx + info->step;
  while(indx != info->breakindx){
    NANodeChildType childtype = naGetNodeChildType(parent, indx);
    if(naIsNodeChildTypeValid(childtype)){
      // There is another child available
      NATreeBaseNode* child = tree->config->nodeChildGetter(parent, indx);
      if(childtype == NA_TREE_NODE_CHILD_LEAF){
        // We found the next leaf. Good ending
        return (NATreeLeaf*)child;
      }else{
        // We have to go deeper.
        return naIterateTreeCapture(tree, (NATreeNode*)child, info->startindx, info);
      }
    }
    indx += info->step;
  }
  return NA_NULL;
}



// This function takes a given basenode and bubbles up to its parent. This
// function works recursively until either a parent offers a next leaf or
// there are no more parents.
NA_HDEF NATreeLeaf* naIterateTreeBubble(const NATree* tree, NATreeBaseNode* curnode, NATreeIterationInfo* info){
  if(!curnode->parent){
    // We reached the root with no further options. Iteration is over.
    return NA_NULL;
  }
  NAInt indx = tree->config->nodeChildIndexGetter(curnode->parent, curnode);

  NATreeLeaf* leaf = naIterateTreeCapture(tree, curnode->parent, indx, info);
  if(leaf){
    return leaf;
  }else{
    // If non more childs are available, bubble further.
    return naIterateTreeBubble(tree, (NATreeBaseNode*)(curnode->parent), info);
  }
}



NA_HDEF NABool naIterateTreeWithInfo(NATreeIterator* iter, NATreeIterationInfo* info){
  const NATree* tree = (const NATree*)naGetPtrConst(&(iter->tree));
  
  // If the tree has no root, we do not iterate.
  if(!tree->root){
    #ifndef NDEBUG
      if(iter->leaf)
      naCrash("naIterateTree", "Current iterator node is set although no root available");
    #endif
    return NA_FALSE;
  }
  
  NATreeLeaf* leaf;
  if(!iter->leaf){
    // If the iterator is at initial position, we use the root and capture. 
    leaf = naIterateTreeCapture(tree, tree->root, info->startindx, info);
  }else{
    // Otherwise, we use the current leaf and bubble
    leaf = naIterateTreeBubble(tree, (NATreeBaseNode*)(iter->leaf), info);
  }
  #ifndef NDEBUG
    if(leaf && naGetNodeChildType(((NATreeBaseNode*)leaf)->parent, naGetNodeChildIndexBinary(((NATreeBaseNode*)leaf)->parent, ((NATreeBaseNode*)leaf))) != NA_TREE_NODE_CHILD_LEAF)
      naError("naIterateTreeWithInfo", "Result should be a leaf");
  #endif
  naSetTreeIteratorCurLeaf(iter, leaf);
  return (leaf != NA_NULL);
}



// This function searches for the given key and returns the node which is
// closes to it. The result can be a node or a leaf. And only if the leaf
// truely stores the given key, the keyleaffound parameter is set to NA_TRUE.
NA_HDEF NATreeNode* naLocateTreeNode(NATreeIterator* iter, const void* key, NABool* keyleaffound, NAInt* leafindx){
  const NATree* tree;
  #ifndef NDEBUG
    if(naTestFlagi(iter->flags, NA_TREE_ITERATOR_CLEARED))
      naError("naLocateTree", "This iterator has been cleared. You need to make it anew.");
  #endif

  tree = (const NATree*)naGetPtrConst(&(iter->tree));

  if(!tree->root){
    #ifndef NDEBUG
      if(iter->leaf)
      naCrash("naIterateTree", "Current iterator node is set although no root available");
    #endif
    *keyleaffound = NA_FALSE;
    return NA_FALSE;
  }
  
  NATreeBaseNode* curnode = (NATreeBaseNode*)(iter->leaf);
  NATreeNode* topnode;
  
  // Move the iterator to the topmost inner node which contains the given key.
  if(curnode){
    #ifndef NDEBUG
      NAInt testchildindx = tree->config->nodeChildIndexGetter(curnode->parent, curnode);
      if(naGetNodeChildType(curnode->parent, testchildindx) != NA_TREE_NODE_CHILD_LEAF)
        naError("naIterateTree", "current node is not a leaf");
    #endif
    topnode = tree->config->nodeBubbleLocator(curnode->parent, key);
  }else{
    topnode = tree->root;
  }
  
  // Search for the leaf containing key.
  NATreeNode* retnode = tree->config->nodeCaptureLocator(topnode, key, keyleaffound, leafindx);
  #ifndef NDEBUG
    if(((NATreeBaseNode*)retnode)->parent && naGetNodeChildType(((NATreeBaseNode*)retnode)->parent, naGetNodeChildIndexBinary(((NATreeBaseNode*)retnode)->parent, (NATreeBaseNode*)retnode)) != NA_TREE_NODE_CHILD_NODE)
      naError("naLocateTreeNode", "Result should be a node");
  #endif
  return retnode;
}



NA_DEF NABool naAddTreeLeaf(NATreeIterator* iter, const void* key, NAPtr content, NABool replace){
  NABool keyleaffound;
  NAInt childindx;
  NATreeNode* node = naLocateTreeNode(iter, key, &keyleaffound, &childindx);
  NATree* tree = (NATree*)naGetPtrMutable(&(iter->tree));

  if(keyleaffound && !replace){
    naSetTreeIteratorCurLeaf(iter, (NATreeLeaf*)(tree->config->nodeChildGetter(node, childindx)));
    return NA_FALSE;
  }

  NATreeLeaf* leaf = tree->config->leafCoreConstructor(tree, key, content);
  naSetTreeIteratorCurLeaf(iter, leaf);

  if(keyleaffound){
    // We need to replace this node
    NATreeLeaf* oldleaf = (NATreeLeaf*)(tree->config->nodeChildGetter(node, childindx));
    tree->config->nodeChildAdder(((NATreeBaseNode*)node)->parent, (NATreeBaseNode*)leaf, childindx, NA_TREE_NODE_CHILD_LEAF);
    tree->config->leafCoreDestructor(tree, oldleaf);
  }else{
    if(!node){
      // We need to create a root and attach the new leaf to it.
      NATreeNode* root = tree->config->nodeCoreConstructor(tree, key);
      ((NATreeBaseNode*)root)->parent = NA_NULL;
      tree->root = root;
      tree->config->nodeChildAdder(root, (NATreeBaseNode*)leaf, tree->config->nodeChildKeyIndexGetter(root, key), NA_TREE_NODE_CHILD_LEAF);
    }else{
      NANodeChildType childtype = naGetNodeChildType(node, childindx);
      if(childtype == NA_TREE_NODE_CHILD_LEAF){
        // We need to create a node holding both the old leaf and the new one.
        tree->config->leafSplitter(tree, node, childindx, leaf);
      }else{
        #ifndef NDEBUG
          if(childtype == NA_TREE_NODE_CHILD_NODE)
            naError("naAddTreeConst", "Child should not be a node");
        #endif
        // We need to add the new leaf to this node
        tree->config->nodeChildAdder((NATreeNode*)node, (NATreeBaseNode*)leaf, childindx, NA_TREE_NODE_CHILD_LEAF);
      }
    }
  }
  return NA_TRUE;
}



NA_DEF NABool naAddTreeConst(NATreeIterator* iter, const void* key, const void* content, NABool replace){
  return naAddTreeLeaf(iter, key, naMakePtrWithDataConst(content), replace);
}



NA_DEF NABool naAddTreeMutable(NATreeIterator* iter, const void* key, void* content, NABool replace){
  return naAddTreeLeaf(iter, key, naMakePtrWithDataMutable(content), replace);
}



NA_DEF void naRemoveTreeCur(NATreeIterator* iter, NABool advance){
  #ifndef NDEBUG
    if(naTestFlagi(iter->flags, NA_TREE_ITERATOR_CLEARED))
      naError("naLocateTree", "This iterator has been cleared. You need to make it anew.");
    if(!iter->leaf)
      naError("naLocateTree", "Iterator is not at a leaf.");
  #endif
  NATree* tree = (NATree*)naGetPtrMutable(&(iter->tree));
  NATreeLeaf* curleaf = iter->leaf;
  if(advance){naIterateTree(iter);}else{naIterateTreeBack(iter);}
  tree->config->nodeChildRemover(tree, (NATreeBaseNode*)curleaf);
  tree->config->leafCoreDestructor(tree, curleaf);
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
