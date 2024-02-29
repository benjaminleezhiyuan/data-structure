/*!******************************************************************
 * \file      BSTree.h
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course: 
 * \par       Section: 
 * \par      
 * \date      29-02-2024
 * 
 * \brief     This file contains the definition for the BSTree class.
 *********************************************************************/
/******************************************************************************/
//---------------------------------------------------------------------------
#ifndef BSTREE_H
#define BSTREE_H
//---------------------------------------------------------------------------
#include <string>    // std::string
#include <stdexcept> // std::exception

#include "ObjectAllocator.h"

/*!
  The exception class for the AVL/BST classes
*/
class BSTException : public std::exception
{
  public:
    /*!
      Non-default constructor

      \param ErrCode
        The kind of exception (only one currently)

      \param Message
        The human-readable reason for the exception.
    */
    BSTException(int ErrCode, const std::string& Message) :
      error_code_(ErrCode), message_(Message) {
    };

    /*!
      Retrieve the exception code.

      \return
        E_NO_MEMORY
    */
    virtual int code() const {
      return error_code_;
    }

    /*!
      Retrieve the message string

      \return
        The human-readable message.
    */
    virtual const char *what() const throw() {
      return message_.c_str();
    }

    //! Destructor
    virtual ~BSTException() {}

    //! The kinds of exceptions (only one currently)
    enum BST_EXCEPTION{E_NO_MEMORY};

  private:
    int error_code_;      //!< The code of the exception
    std::string message_; //!< Readable message text
};

/*!
  The definition of the BST
*/
template <typename T>
class BSTree
{
  public:
    //! The node structure
    struct BinTreeNode
    {
      BinTreeNode *left;  //!< The left child
      BinTreeNode *right; //!< The right child
      T data;             //!< The data
      int balance_factor; //!< optional for efficient balancing
      unsigned count;     //!< nodes in this subtree for efficient indexing

      //! Default constructor
      BinTreeNode() : left(0), right(0), data(0), balance_factor(0), count(1) {};

      //! Conversion constructor
      BinTreeNode(const T& value) : left(0), right(0), data(value), balance_factor(0), count(1) {};
    };

    //! shorthand
    using BinTree = BinTreeNode*;

    BSTree(ObjectAllocator *oa = 0, bool ShareOA = false);
    BSTree(const BSTree& rhs);
    virtual ~BSTree();
    BSTree& operator=(const BSTree& rhs);
    const BinTreeNode* operator[](int index) const; // for r-values (Extra Credit)
    virtual void insert(const T& value);
    virtual void remove(const T& value);
    void clear();
    bool find(const T& value, unsigned &compares) const;
    bool empty() const;
    unsigned int size() const;
    int height() const;
    BinTree root() const;

  protected:
    BinTree& get_root();
    BinTree make_node(const T& value) const;
    void free_node(BinTree node);
    int tree_height(BinTree tree) const;
    void find_predecessor(BinTree tree, BinTree &predecessor) const;

    BinTree rootNode;
    unsigned int sizeOfTree;
    int heightOfTree;
    ObjectAllocator* OA;
    bool freeOA;
    bool shareOA;

  private:
    // private stuff...
    void InitializeAllocator(ObjectAllocator* oa, bool shareOA);
    void CopyTree(const BinTree& source, BinTree& dest);
    void HandleObjectAllocator(const BSTree& rhs);
    void FreeTree(BinTree tree);
    void FreeSubtree(BinTree tree);
    void InsertNode(BinTree& node, const T& value, int depth);
    void HandleNewNodeCreation(BinTree& node, const T& value, int depth);
    void IncrementNodeCount(BinTree& node);
    void DeleteNode(BinTree& node, const T& value);
    void DecrementNodeCount(BinTree& node);
    void HandleNodeDeletion(BinTree& node);
    void ReplaceNodeWithRightChild(BinTree& node);
    void ReplaceNodeWithLeftChild(BinTree& node);
    bool FindNode(BinTree node, const T& value, unsigned& compares) const;
    BinTree FindNodeAtIndex(BinTree tree, unsigned index) const;
    BinTree FindNodeAtIndexRecursive(BinTree tree, unsigned index) const;
    unsigned int CountLeftSubtreeNodes(BinTree tree) const;
    BinTree AllocateMemory() const;
    BinTree CreateNode(BinTree alloc, const T& value) const;
    int CalculateTreeHeight(BinTree tree) const;
    BinTree FindRightmostNode(BinTree node) const;
};


#include "BSTree.cpp"

#endif
//---------------------------------------------------------------------------
