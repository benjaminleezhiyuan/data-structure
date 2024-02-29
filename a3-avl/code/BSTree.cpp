/*!******************************************************************
 * \file      BSTree.cpp
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course:
 * \par       Section:
 * \par
 * \date      29-02-2024
 *
 * \brief     This file contains the implementation for the BSTree class.
 *********************************************************************/
/******************************************************************************/
#include "BSTree.h"

/**
 * @brief Constructs a new BSTree object.
 *
 * @tparam T The type of elements stored in the BSTree.
 * @param oa Pointer to the ObjectAllocator used for memory management.
 * @param ShareOA Flag indicating whether to share the ObjectAllocator or create a new one.
 */
template <typename T>
BSTree<T>::BSTree(ObjectAllocator *oa, bool ShareOA)
    : rootNode(nullptr), sizeOfTree(0), heightOfTree(-1), OA(ShareOA ? oa : new ObjectAllocator(sizeof(BinTreeNode), OAConfig(true))),
      freeOA(!oa && !ShareOA), shareOA(ShareOA) {}

template <typename T>
/**
 * @brief Copy constructor for BSTree class.
 *
 * @param rhs The BSTree object to be copied.
 */
BSTree<T>::BSTree(const BSTree &rhs)
    : rootNode(nullptr), sizeOfTree(0), heightOfTree(-1), OA(nullptr), freeOA(false), shareOA(false)
{
  InitializeAllocator(rhs.shareOA ? rhs.OA : nullptr, rhs.shareOA);
  CopyTree(rhs.rootNode, rootNode);
  sizeOfTree = rhs.sizeOfTree;
  heightOfTree = rhs.heightOfTree;
}

template <typename T>
/**
 * @brief Initializes the object allocator for the binary search tree.
 *
 * @param oa Pointer to the object allocator.
 * @param shareOA Flag indicating whether to share the object allocator with other data structures.
 */
void BSTree<T>::InitializeAllocator(ObjectAllocator *oa, bool shareOA)
{
  if (shareOA)
  {
    OA = oa;
    freeOA = false;
    shareOA = true;
  }
  else
  {
    OAConfig config(true);
    OA = new ObjectAllocator(sizeof(BinTreeNode), config);
    freeOA = true;
    shareOA = false;
  }
}

/**
 * @brief Destructor for the BSTree class.
 * 
 * This destructor clears the tree by calling the clear() function.
 * If the tree was using dynamic memory allocation for its nodes (freeOA is true),
 * it also deletes the ObjectAllocator (OA) instance.
 * 
 * @tparam T The type of data stored in the tree.
 */
template <typename T>
BSTree<T>::~BSTree()
{
  clear();
  if (freeOA)
    delete OA;
}

/**
 * @brief Assignment operator overload for BSTree.
 * 
 * This function assigns the contents of the right-hand side BSTree object to the left-hand side BSTree object.
 * 
 * @tparam T The type of elements stored in the BSTree.
 * @param rhs The right-hand side BSTree object to be assigned.
 * @return A reference to the left-hand side BSTree object after assignment.
 */
template <typename T>
BSTree<T> &BSTree<T>::operator=(const BSTree &rhs)
{
  if (this == &rhs)
    return *this;

  HandleObjectAllocator(rhs);
  CopyTree(rhs.rootNode, rootNode);
  sizeOfTree = rhs.sizeOfTree;
  heightOfTree = rhs.heightOfTree;

  return *this;
}

template <typename T>
/**
 * @brief Handles the object allocator for the BSTree class.
 * 
 * This function is responsible for handling the object allocator when copying the contents of another BSTree object.
 * 
 * @param rhs The BSTree object to be copied.
 */
void BSTree<T>::HandleObjectAllocator(const BSTree &rhs)
{
  if (rhs.shareOA)
  {
    if (freeOA)
    {
      clear();
      delete OA;
    }

    OA = rhs.OA;
    freeOA = false;
    shareOA = true;
  }
  else
  {
    InitializeAllocator(rhs.OA, false);
  }
}

/**
 * @brief Overloaded subscript operator to access the element at the specified index in the binary search tree.
 * 
 * @tparam T The type of elements stored in the binary search tree.
 * @param index The index of the element to access.
 * @return const typename BSTree<T>::BinTreeNode* A pointer to the node containing the element at the specified index, or nullptr if the index is out of range.
 */
template <typename T>
const typename BSTree<T>::BinTreeNode *BSTree<T>::operator[](int index) const
{
  if (static_cast<unsigned>(index) > sizeOfTree)
    return nullptr;
  else
    return FindNodeAtIndex(rootNode, index);
}

/**
 * @brief Inserts a new node with the given value into the binary search tree.
 * 
 * @tparam T The type of the values stored in the binary search tree.
 * @param value The value to be inserted.
 * @throws BSTException if an error occurs during the insertion process.
 */
template <typename T>
void BSTree<T>::insert(const T &value)
{
  try
  {
    InsertNode(rootNode, value, 0);
  }
  catch (const BSTException &except)
  {
    throw except;
  }
}

/**
 * @brief Removes a node with the specified value from the binary search tree.
 * 
 * This function removes a node with the specified value from the binary search tree.
 * It updates the height of the tree after the removal.
 * 
 * @param value The value to be removed from the tree.
 * @tparam T The type of elements stored in the tree.
 */
template <typename T>
void BSTree<T>::remove(const T &value)
{
  DeleteNode(rootNode, const_cast<T &>(value));
  heightOfTree = tree_height(rootNode);
}

/**
 * @brief Clears the binary search tree.
 * 
 * This function removes all nodes from the binary search tree, 
 * resetting the root node, size, and height to their initial values.
 * 
 * @tparam T The type of elements stored in the binary search tree.
 */
template <typename T>
void BSTree<T>::clear()
{
  if (rootNode)
  {
    FreeTree(rootNode);

    rootNode = nullptr;
    sizeOfTree = 0;
    heightOfTree = -1;
  }
}

/**
 * @brief Finds a value in the binary search tree.
 * 
 * This function searches for a given value in the binary search tree.
 * It returns true if the value is found, and false otherwise.
 * The number of comparisons made during the search is stored in the 'compares' parameter.
 * 
 * @param value The value to search for.
 * @param compares The number of comparisons made during the search.
 * @return True if the value is found, false otherwise.
 */
template <typename T>
bool BSTree<T>::find(const T &value, unsigned &compares) const
{
  return FindNode(rootNode, value, compares);
}

/**
 * @brief Checks if the binary search tree is empty.
 * 
 * @tparam T The type of elements stored in the binary search tree.
 * @return true if the binary search tree is empty, false otherwise.
 */
template <typename T>
bool BSTree<T>::empty() const
{
  return sizeOfTree == 0 ? true : false;
}

/**
 * @brief Returns the number of elements in the binary search tree.
 * 
 * @tparam T The type of elements stored in the binary search tree.
 * @return The number of elements in the binary search tree.
 */
template <typename T>
unsigned int BSTree<T>::size() const
{
  return sizeOfTree;
}

/**
 * Returns the height of the binary search tree.
 * The height of a binary search tree is defined as the maximum number of edges
 * in any path from the root node to a leaf node.
 *
 * @return The height of the binary search tree.
 */
template <typename T>
int BSTree<T>::height() const
{
  return tree_height(rootNode);
}

/**
 * Returns the root node of the binary search tree.
 *
 * @return The root node of the binary search tree.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
  return rootNode;
}

/**
 * @brief Returns a reference to the root node of the binary search tree.
 * 
 * @tparam T The type of elements stored in the binary search tree.
 * @return A reference to the root node of the binary search tree.
 */
template <typename T>
typename BSTree<T>::BinTree &BSTree<T>::get_root()
{
  return rootNode;
}

/**
 * @brief Creates a new node with the given value.
 * 
 * This function allocates memory for a new node and initializes it with the given value.
 * 
 * @param value The value to be stored in the new node.
 * @return The newly created node.
 * @throws BSTException if there is not enough memory to allocate the node.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::make_node(const T &value) const
{
  try
  {
    BinTree alloc = AllocateMemory();

    BinTree node = CreateNode(alloc, value);
    return node;
  }
  catch (const OAException &except)
  {
    throw(BSTException(BSTException::E_NO_MEMORY, except.what()));
  }
}

/**
 * @brief Allocates memory for a binary tree node.
 * 
 * This function allocates memory for a binary tree node using the underlying memory allocator (OA).
 * The allocated memory is then cast to the appropriate type (BinTree) and returned.
 * 
 * @tparam T The type of data stored in the binary tree.
 * @return The allocated memory for the binary tree node.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::AllocateMemory() const
{
  return reinterpret_cast<BinTree>(OA->Allocate());
}

/**
 * @brief Creates a new node in the binary search tree with the given value.
 * 
 * @param alloc The allocator used to allocate memory for the new node.
 * @param value The value to be stored in the new node.
 * @return The newly created node.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::CreateNode(BinTree alloc, const T &value) const
{
  BinTree node = new (alloc) BinTreeNode(value);
  return node;
}

/**
 * @brief Frees a binary tree node.
 * 
 * This function frees the memory occupied by a binary tree node.
 * It first calls the destructor of the node to properly clean up any resources,
 * and then deallocates the memory using the provided allocator.
 * 
 * @param node The node to be freed.
 */
template <typename T>
void BSTree<T>::free_node(BinTree node)
{
  node->~BinTreeNode();
  OA->Free(node);
}

/**
 * Calculates the height of a binary search tree.
 * 
 * @param tree The binary search tree.
 * @return The height of the tree. Returns -1 if the tree is empty.
 */
template <typename T>
int BSTree<T>::tree_height(BinTree tree) const
{
  if (tree == nullptr)
    return -1;

  else
  {
    return CalculateTreeHeight(tree);
  }
}

/**
 * Calculates the height of a binary search tree.
 * 
 * @param tree The binary search tree.
 * @return The height of the binary search tree.
 */
template <typename T>
int BSTree<T>::CalculateTreeHeight(BinTree tree) const
{
  int height_left = tree_height(tree->left);
  int height_right = tree_height(tree->right);
  return std::max(height_left, height_right) + 1;
}

/**
 * Finds the predecessor of a given node in a binary search tree.
 * The predecessor is the node with the largest value that is smaller than the given node.
 *
 * @param tree The binary search tree.
 * @param predecessor Reference to the variable that will store the predecessor node.
 * @tparam T The type of elements stored in the binary search tree.
 */
template <typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree &predecessor) const
{
  predecessor = FindRightmostNode(tree->left);
}

/**
 * Finds the rightmost node in a binary search tree.
 * 
 * @param node The root node of the binary search tree.
 * @return The rightmost node in the binary search tree.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::FindRightmostNode(BinTree node) const
{
  while (node->right != nullptr)
    node = node->right;

  return node;
}

/**
 * @brief Copies the contents of a binary tree from the source to the destination.
 * 
 * This function recursively copies each node of the source binary tree to the destination binary tree.
 * It creates a new node in the destination tree for each node in the source tree and copies the data,
 * count, and balance factor from the source node to the destination node.
 * 
 * @tparam T The type of data stored in the binary tree.
 * @param source The root node of the source binary tree.
 * @param dest The root node of the destination binary tree.
 */
template <typename T>
void BSTree<T>::CopyTree(const BinTree &source, BinTree &dest)
{
  if (source == nullptr)
    dest = nullptr;

  else
  {
    dest = make_node(source->data);
    dest->count = source->count;
    dest->balance_factor = source->balance_factor;
    CopyTree(source->left, dest->left);
    CopyTree(source->right, dest->right);
  }
}

/**
 * @brief Frees the memory occupied by a binary tree.
 * 
 * This function recursively frees the memory occupied by a binary tree.
 * It starts from the root node and frees each node and its children.
 * 
 * @param tree The binary tree to be freed.
 */
template <typename T>
void BSTree<T>::FreeTree(BinTree tree)
{
  FreeSubtree(tree);
}

/**
 * @brief Recursively frees the memory allocated for a binary tree and its subtrees.
 * 
 * @param tree The root of the binary tree to be freed.
 * @tparam T The type of data stored in the binary tree.
 */
template <typename T>
void BSTree<T>::FreeSubtree(BinTree tree)
{
  if (tree == nullptr)
    return;

  FreeSubtree(tree->left);
  FreeSubtree(tree->right);

  free_node(tree);
}

/**
 * @brief Inserts a new node with the given value into the binary search tree.
 * 
 * This function recursively traverses the binary search tree to find the appropriate
 * position to insert the new node. If the tree is empty, a new node is created and
 * assigned the given value. If the value is less than the current node's data, the
 * function is called recursively on the left subtree. Otherwise, the function is called
 * recursively on the right subtree.
 * 
 * @tparam T The type of data stored in the binary search tree.
 * @param node A reference to the current node being considered.
 * @param value The value to be inserted into the binary search tree.
 * @param depth The current depth of the recursion.
 * @throws BSTException if an error occurs during the insertion process.
 */
template <typename T>
void BSTree<T>::InsertNode(BinTree &node, const T &value, int depth)
{
  try
  {
    if (node == nullptr)
    {
      HandleNewNodeCreation(node, value, depth);
      return;
    }

    IncrementNodeCount(node);

    if (value < node->data)
      InsertNode(node->left, value, depth + 1);
    else
      InsertNode(node->right, value, depth + 1);
  }
  catch (const BSTException &except)
  {
    throw except;
  }
}

/**
 * @brief Handles the creation of a new node in the binary search tree.
 * 
 * This function creates a new node with the given value and assigns it to the specified node.
 * It also updates the height and size of the binary search tree if necessary.
 * 
 * @tparam T The type of elements stored in the binary search tree.
 * @param node The node to assign the new node to.
 * @param value The value to be stored in the new node.
 * @param depth The depth of the current node in the binary search tree.
 */
template <typename T>
void BSTree<T>::HandleNewNodeCreation(BinTree &node, const T &value, int depth)
{
  if (depth > heightOfTree)
    ++heightOfTree;
  node = make_node(value);
  ++sizeOfTree;
}

/**
 * @brief Increments the count of a given node in the binary search tree.
 * 
 * @tparam T The type of data stored in the binary search tree.
 * @param node The node whose count needs to be incremented.
 */
template <typename T>
void BSTree<T>::IncrementNodeCount(BinTree &node)
{
  ++node->count;
}

/**
 * @brief Deletes a node with the specified value from the binary search tree.
 * 
 * This function recursively searches for the node with the specified value in the binary search tree.
 * If the node is found, it is deleted from the tree.
 * 
 * @param node The root node of the binary search tree.
 * @param value The value to be deleted from the tree.
 * 
 * @tparam T The type of data stored in the binary search tree.
 */
template <typename T>
void BSTree<T>::DeleteNode(BinTree &node, const T &value)
{
  if (node == nullptr)
    return;
  else if (value < node->data)
  {
    DecrementNodeCount(node);
    DeleteNode(node->left, value);
  }
  else if (value > node->data)
  {
    DecrementNodeCount(node);
    DeleteNode(node->right, value);
  }
  else
  {
    DecrementNodeCount(node);
    HandleNodeDeletion(node);
  }
}

/**
 * Decrements the count of a node in the binary search tree.
 * 
 * @param node The node to decrement the count of.
 */
template <typename T>
void BSTree<T>::DecrementNodeCount(BinTree &node)
{
  if (node != nullptr)
    --node->count;
}

/**
 * @brief Handles the deletion of a node in the binary search tree.
 * 
 * This function is responsible for handling the deletion of a node in the binary search tree.
 * It checks if the node has a left child or a right child, and performs the appropriate replacement
 * or deletion operation accordingly. If the node has both a left and right child, it finds the
 * predecessor of the node, replaces the node's data with the predecessor's data, and then deletes
 * the predecessor's node.
 * 
 * @param node The node to be deleted.
 * @tparam T The type of data stored in the binary search tree.
 */
template <typename T>
void BSTree<T>::HandleNodeDeletion(BinTree &node)
{
  if (node->left == nullptr)
  {
    ReplaceNodeWithRightChild(node);
    --sizeOfTree;
  }
  else if (node->right == nullptr)
  {
    ReplaceNodeWithLeftChild(node);
    --sizeOfTree;
  }
  else
  {
    BinTree pred = nullptr;
    find_predecessor(node, pred);
    node->data = pred->data;
    DeleteNode(node->left, node->data);
  }
}

/**
 * Replaces a node with its right child in a binary search tree.
 * 
 * @param node The node to be replaced.
 * @tparam T The type of data stored in the binary search tree.
 */
template <typename T>
void BSTree<T>::ReplaceNodeWithRightChild(BinTree &node)
{
  BinTree tmp = node;
  node = node->right;
  free_node(tmp);
}

/**
 * Replaces a node with its left child in a binary search tree.
 * 
 * @param node The node to be replaced.
 */
template <typename T>
void BSTree<T>::ReplaceNodeWithLeftChild(BinTree &node)
{
  BinTree tmp = node;
  node = node->left;
  free_node(tmp);
}

/**
 * @brief Finds a node with the specified value in the binary search tree.
 * 
 * This function recursively searches for a node with the specified value in the binary search tree.
 * It returns true if the node is found, and false otherwise.
 * The number of comparisons made during the search is stored in the 'compares' parameter.
 * 
 * @param node The root node of the binary search tree.
 * @param value The value to search for.
 * @param compares The number of comparisons made during the search.
 * @return true if the node with the specified value is found, false otherwise.
 */
template <typename T>
bool BSTree<T>::FindNode(BinTree node, const T &value, unsigned &compares) const
{
  ++compares;

  if (node == nullptr)
    return false;
  else if (value == node->data)
    return true;
  else if (value < node->data)
    return FindNode(node->left, value, compares);
  else
    return FindNode(node->right, value, compares);
}

/**
 * @brief Finds the node at the specified index in the binary search tree.
 * 
 * This function recursively searches for the node at the given index in the binary search tree.
 * The index represents the position of the node in an inorder traversal of the tree.
 * 
 * @param tree The binary search tree.
 * @param index The index of the node to find.
 * @return The node at the specified index, or nullptr if the index is out of range.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::FindNodeAtIndex(BinTree tree, unsigned index) const
{
  return FindNodeAtIndexRecursive(tree, index);
}

/**
 * @brief Recursively finds the node at the specified index in the binary search tree.
 * 
 * This function traverses the binary search tree in an inorder manner and returns the node
 * at the specified index. The index is zero-based, meaning the first node has an index of 0.
 * 
 * @param tree The binary search tree to search in.
 * @param index The index of the node to find.
 * @return The node at the specified index, or nullptr if the index is out of range.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::FindNodeAtIndexRecursive(BinTree tree, unsigned index) const
{
  if (tree == nullptr)
    return nullptr;

  unsigned int left_count = CountLeftSubtreeNodes(tree);

  if (left_count > index)
    return FindNodeAtIndexRecursive(tree->left, index);
  else if (left_count < index)
    return FindNodeAtIndexRecursive(tree->right, index - left_count - 1);
  else
    return tree;
}

/**
 * Counts the number of nodes in the left subtree of a given binary tree.
 * 
 * @param tree The binary tree.
 * @return The number of nodes in the left subtree.
 */
template <typename T>
unsigned int BSTree<T>::CountLeftSubtreeNodes(BinTree tree) const
{
  if (tree->left)
    return tree->left->count;
  else
    return 0;
}
