/*!******************************************************************
 * \file      AVLTree.h
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course: 
 * \par       Section: 
 * \par      
 * \date      29-02-2024
 * 
 * \brief     This file contains the definition for the AVL Tree class.
 *********************************************************************/
/******************************************************************************/
//---------------------------------------------------------------------------
#ifndef AVLTREE_H
#define AVLTREE_H
//---------------------------------------------------------------------------
#include <stack>
#include "BSTree.h"

/*!
  Definition for the AVL Tree
*/
template <typename T>
class AVLTree : public BSTree<T>
{
  public:
    AVLTree(ObjectAllocator *oa = 0, bool ShareOA = false);
    virtual ~AVLTree() = default; // DO NOT IMPLEMENT
    virtual void insert(const T& value) override;
    virtual void remove(const T& value) override;

    // Returns true if efficiency implemented
    static bool ImplementedBalanceFactor(void);

  private:
    using BinTree = typename BSTree<T>::BinTree;
    void LeftRotate(BinTree& tree);
    using Stack = std::stack<BinTree *>;
    void AVLBalance(Stack& visited);
    // private stuff
    
    unsigned int CountTree(BinTree& tree);
    void AVLRecount(BinTree& tree);
    void InsertRight(BinTree &tree, const T &value, Stack &visited);
    void RightRotate(BinTree& tree);

    // Helper functions
    void InsertLeft(BinTree &tree, const T &value, Stack &visited);

    // Helper functions for removal
    void LeftRemove(BinTree &tree, const T &value, Stack &visited);
    void RightRemove(BinTree &tree, const T &value, Stack &visited);
    
    void RightBalance(BinTree *node);
    void LeftBalance(BinTree *node);
    void AVLRemove(BinTree& tree, const T& value, Stack& visited);
    void AVLInsert(BinTree& tree, const T& value, Stack& visited);
    
    
};

#include "AVLTree.cpp"

#endif
//---------------------------------------------------------------------------
