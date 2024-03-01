/*!******************************************************************
 * \file      AVLTree.cpp
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course: 
 * \par       Section: 
 * \par      
 * \date      29-02-2024
 * 
 * \brief     This file contains the implementation for the AVLTree class.
  *********************************************************************/ 
/******************************************************************************/
#include "AVLTree.h"


/******************************************************************************/
/*!
\brief
  This function inserts a value into the AVLTree.
\param value, the data to insert.
*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::insert(const T &value)
{
  Stack visited_nodes;
  AVLInsert(BSTree<T>::get_root(), value, visited_nodes);
}

/******************************************************************************/
/*!
\brief
  This function lets the client know if we are doing efficient balancing.
\return true if implemented else false.
*/
/******************************************************************************/
template <typename T>
bool AVLTree<T>::ImplementedBalanceFactor(void)
{
  return false;
}
/******************************************************************************/
/*!
\brief
  This function removes a value from the AVLTree.
\param value, the data to remove.
*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::remove(const T &value)
{
  Stack visited_nodes;
  AVLRemove(BSTree<T>::get_root(), value, visited_nodes);
}

/******************************************************************************/
/*!
\brief
  This is the constructor for a AVLTree.
\param oa, the object allocator(OA) to use.
\param ShareOA, this boolean decides whether this AVLTree will share it's OA.
*/
/******************************************************************************/
template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator *OA, bool ShareOA)
    : BSTree<T>{OA, ShareOA}
{
}

/******************************************************************************/
/*!
\brief
  This function recursively inserts a value into the AVLTree.
\param tree, current node.
\param value, the data to insert.
\param visited, visited nodes.
*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::AVLInsert(BinTree &tree, const T &value, Stack &visited)
{
  if (tree == nullptr)
  {
    tree = BSTree<T>::make_node(value);
    ++this->size_;
    AVLBalance(visited);
  }
  else
  {
    visited.push(&tree);
    ++tree->count;
    if (value < tree->data)
      InsertLeft(tree, value, visited);
    else if (value > tree->data)
      InsertRight(tree, value, visited);
  }
}

/**
 * Inserts a new node with the given value to the left subtree of the specified tree.
 * This function uses the AVLInsert function to maintain the AVL tree property.
 *
 * @param tree The tree to insert the node into.
 * @param value The value to be inserted.
 * @param visited A stack to keep track of visited nodes during the insertion process.
 */
template <typename T>
void AVLTree<T>::InsertLeft(BinTree &tree, const T &value, Stack &visited)
{
  AVLInsert(tree->left, value, visited);
}

/**
 * Inserts a new node with the specified value as the right child of the given tree.
 * This function uses the AVLInsert function to maintain the AVL tree property.
 *
 * @param tree The tree to insert the new node into.
 * @param value The value to be inserted.
 * @param visited A stack to keep track of visited nodes during the insertion process.
 */
template <typename T>
void AVLTree<T>::InsertRight(BinTree &tree, const T &value, Stack &visited)
{
  AVLInsert(tree->right, value, visited);
}

/******************************************************************************/
/*!
\brief
  This function recursively removes a value from the AVLTree.
\param tree, current node.
\param value, the data to remove.
\param visited, visited nodes.
*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::AVLRemove(BinTree &tree, const T &value, Stack &visited)
{
    if (tree == nullptr)
        return;
    else if (value < tree->data)
    {
        visited.push(&tree);
        --tree->count;
        LeftRemove(tree, value, visited);
    }
    else if (value > tree->data)
    {
        visited.push(&tree);
        --tree->count;
        RightRemove(tree, value, visited);
    }
    else
    {
        --tree->count;
        if (tree->left == nullptr)
        {
            BinTree temp = tree;
            tree = tree->right;
            BSTree<T>::free_node(temp);
            --this->size_;
        }
        else if (tree->right == nullptr)
        {
            BinTree temp = tree;
            tree = tree->left;
            BSTree<T>::free_node(temp);
            --this->size_;
        }
        else
        {
            BinTree pred = nullptr;
            BSTree<T>::find_predecessor(tree, pred);
            tree->data = pred->data;
            AVLRemove(tree->left, tree->data, visited);
        }
        AVLBalance(visited);
    }
}

/**
 * Removes the specified value from the left subtree of the given AVL tree.
 * 
 * @param tree The AVL tree.
 * @param value The value to be removed.
 * @param visited The stack to keep track of visited nodes.
 */
template <typename T>
void AVLTree<T>::LeftRemove(BinTree &tree, const T &value, Stack &visited)
{
    AVLRemove(tree->left, value, visited);
}

/**
 * Removes the specified value from the right subtree of the given AVL tree.
 * This function is called recursively to remove the value from the right subtree.
 *
 * @param tree The AVL tree.
 * @param value The value to be removed.
 * @param visited A stack to keep track of visited nodes during the removal process.
 */
template <typename T>
void AVLTree<T>::RightRemove(BinTree &tree, const T &value, Stack &visited)
{
    AVLRemove(tree->right, value, visited);
}


/******************************************************************************/
/*!
\brief
  This function balances the AVLTree along a given path.
\param visited, visited nodes aka path.
*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::AVLBalance(Stack &visited)
{
    while (!visited.empty())
    {
        BinTree *node = visited.top();
        visited.pop();
        int height_left = BSTree<T>::tree_height((*node)->left);
        int height_right = BSTree<T>::tree_height((*node)->right);

        if (std::abs(height_left - height_right) < 2)
            continue;

        if (height_right > height_left)
        {
            if ((*node)->right)
            {
                RightBalance(node);
                AVLRecount(*node);
            }
        }
        else if (height_right < height_left)
        {
            if ((*node)->left)
            {
                LeftBalance(node);
                AVLRecount(*node);
            }
        }
    }
}

/**
 * Performs right balancing on the AVL tree.
 * This function is called when the right subtree of a node becomes unbalanced.
 * It checks if a right rotation or a left rotation is required to balance the tree.
 * If the left subtree of the right child of the given node is taller than the right subtree,
 * a right rotation is performed on the right child.
 * Otherwise, a left rotation is performed on the given node.
 *
 * @param node A pointer to the node that needs right balancing.
 */
template <typename T>
void AVLTree<T>::RightBalance(BinTree *node)
{
    if (BSTree<T>::tree_height((*node)->right->left) > BSTree<T>::tree_height((*node)->right->right))
        RightRotate((*node)->right);
    LeftRotate((*node));
}

/**
 * Performs a left balance operation on the AVL tree.
 * This operation is used to maintain the balance of the tree after a node insertion or deletion.
 * It checks if the left subtree of the given node is unbalanced and performs necessary rotations to restore balance.
 * 
 * @param node A pointer to the node that needs left balancing.
 */
template <typename T>
void AVLTree<T>::LeftBalance(BinTree *node)
{
    if (BSTree<T>::tree_height((*node)->left->left) < BSTree<T>::tree_height((*node)->left->right))
        LeftRotate((*node)->left);
    RightRotate((*node));
}


/******************************************************************************/
/*!
\brief
  This function performs a simple left rotation on a given tree.
\param tree, the pivot node.
*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::LeftRotate(BinTree &tree)
{
    if (tree == nullptr || tree->right == nullptr)
        return;

    // Store the necessary nodes for rotation
    BinTree pivot = tree;
    BinTree right_child = pivot->right;
    BinTree left_subtree_of_right_child = right_child->left;

    // Perform the rotation
    right_child->left = pivot;
    pivot->right = left_subtree_of_right_child;
    tree = right_child;
}


/******************************************************************************/
/*!
\brief
  This function performs a simple right rotation on a given tree.
\param tree, the pivot node.
*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::RightRotate(BinTree &tree)
{
    if (tree == nullptr || tree->left == nullptr)
        return;

    // Store the necessary nodes for rotation
    BinTree pivot = tree;
    BinTree left_child = pivot->left;
    BinTree right_subtree_of_left_child = left_child->right;

    // Perform the rotation
    left_child->right = pivot;
    pivot->left = right_subtree_of_left_child;
    tree = left_child;
}


/******************************************************************************/
/*!
\brief
  This function recursively counts the number of nodes in the subtree.
\param tree, the parent node.
*/
/******************************************************************************/
template <typename T>
unsigned int AVLTree<T>::CountTree(BinTree &tree)
{
    if (tree == nullptr)
        return 0;

    // Count the number of nodes in the subtree rooted at the current node
    unsigned int left_count = CountTree(tree->left);
    unsigned int right_count = CountTree(tree->right);

    // Return the total count including the current node
    return 1 + left_count + right_count;
}


/******************************************************************************/
/*!
\brief
  This function recursively counts the number of nodes in the subtree for every
  subtree.
\param tree, the parent node.
*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::AVLRecount(BinTree &tree)
{
    if (tree == nullptr)
        return;

    // Count the number of nodes in the subtree
    tree->count = 1 + CountTree(tree->left) + CountTree(tree->right);

    // Recursively update counts for left and right subtrees
    AVLRecount(tree->left);
    AVLRecount(tree->right);
}

