/*!******************************************************************
 * \file      BList.cpp
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course: CSD2183
 * \par       Section: A
 * \par
 * \date      31-01-2024
 *
 * \brief     This file contains the definitions for the BList.
 *********************************************************************/

#include "BList.h"

template <typename T, unsigned Size>
size_t BList<T, Size>::nodesize(void)
{
  return sizeof(BNode);
}

template <typename T, unsigned Size>
const typename BList<T, Size>::BNode *BList<T, Size>::GetHead() const
{
  return head_;
}

/**
 * @brief Default constructor for the BList class.
 *
 * Initializes the head and tail pointers to nullptr.
 * Sets the NodeSize to the size of the nodes and ArraySize to the specified Size.
 *
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum number of elements that can be stored in the BList.
 */
template <typename T, unsigned Size>
BList<T, Size>::BList()
{
  // Initialize head and tail pointers to nullptr
  head_ = nullptr;
  tail_ = nullptr;

  // Set NodeSize to the size of the nodes (sizeof BNode) and ArraySize to the specified Size
  stats_.NodeSize = sizeof(BNode);
  stats_.ArraySize = Size;
}

/**
 * @brief Copy constructor for BList class.
 *
 * This constructor creates a deep copy of the given BList object.
 * It iterates through the nodes of the given BList and creates new nodes
 * with the same values. The new nodes are then linked together to form
 * a new BList.
 *
 * @param rhs The BList object to be copied.
 */
template <typename T, unsigned Size>
BList<T, Size>::BList(const BList &rhs) : stats_{rhs.stats_}
{
  // Initialize head and tail pointers to nullptr
  head_ = nullptr;
  tail_ = nullptr;

  // Pointer to traverse the rhs list
  BNode *rhs_current = rhs.head_;

  // Pointer to keep track of the last node created
  BNode *last_new_node = nullptr;

  // Iterate through the rhs list
  while (rhs_current != nullptr)
  {
    // Create a new node
    BNode *new_node = createNode();

    // Copy values from the rhs node to the new node
    for (int i = 0; i < rhs_current->count; ++i)
    {
      new_node->values[i] = rhs_current->values[i];
      new_node->count++;
    }

    // Link the new node with the previous one
    if (last_new_node != nullptr)
    {
      last_new_node->next = new_node;
      new_node->prev = last_new_node;
    }
    else
    {
      // If last_new_node is nullptr, it means this is the first node
      head_ = new_node;
    }

    // Update last_new_node to the newly created node
    last_new_node = new_node;

    // Move to the next node in the rhs list
    rhs_current = rhs_current->next;
  }

  // Update tail_ pointer
  tail_ = last_new_node;
}

/**
 * @brief Destructor for the BList class.
 *
 * This destructor clears the BList by calling the clear() function.
 *
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum size of the BList.
 */
template <typename T, unsigned Size>
BList<T, Size>::~BList()
{
  clear();
}

/**
 * @brief Assignment operator overload for BList.
 *
 * This function assigns the contents of the given BList object to the current BList object.
 * It performs a deep copy of the elements from the given BList to the current BList.
 *
 * @param rhs The BList object to be assigned.
 * @return A reference to the current BList object after assignment.
 */
template <typename T, unsigned Size>
BList<T, Size> &BList<T, Size>::operator=(const BList &rhs)
{
  // Check for self-assignment
  if (this == &rhs)
    // Return a reference to the current BList object after assignment
    return *this;
  // Clear the current list
  clear();

  // Copy statistics
  stats_ = rhs.stats_;

  // Pointer to traverse the rhs list
  BNode *rhs_current = rhs.head_;

  // Pointer to keep track of the last node created
  BNode *last_new_node = nullptr;

  // Iterate through the rhs list
  while (rhs_current != nullptr)
  {
    // Create a new node
    BNode *new_node = createNode();

    // Copy values from the rhs node to the new node
    for (int i = 0; i < rhs_current->count; ++i)
    {
      new_node->values[i] = rhs_current->values[i];
      new_node->count++;
    }

    // Link the new node with the previous one
    if (last_new_node != nullptr)
    {
      last_new_node->next = new_node;
      new_node->prev = last_new_node;
    }
    else
    {
      // If last_new_node is nullptr, it means this is the first node
      head_ = new_node;
    }

    // Update last_new_node to the newly created node
    last_new_node = new_node;

    // Move to the next node in the rhs list
    rhs_current = rhs_current->next;
  }

  // Update tail_ pointer
  tail_ = last_new_node;
  return *this;
}

/**
 * @brief Adds a new element to the end of the BList.
 *
 * This function adds a new element to the end of the BList. If the tail node has available space, the element is added to the tail node. Otherwise, a new node is created and the element is added to it. If the BList is empty, the new node becomes both the head and tail node.
 *
 * @param value The value to be added to the BList.
 */
template <typename T, unsigned Size>
void BList<T, Size>::push_back(const T &value)
{
  if (tail_ && tail_->count < stats_.ArraySize)
  {
    addToTail(value);
  }
  else
  {
    createNewNodeAndAddToTail(value);
  }
  
  ++stats_.ItemCount;
}


/**
 * @brief Adds a new element to the tail of the BList.
 * 
 * @param value The value to be added.
 */
template <typename T, unsigned Size>
void BList<T, Size>::addToTail(const T &value)
{
  tail_->values[tail_->count] = value;
  incrementNode(tail_);
}

/**
 * @brief Creates a new node and adds it to the tail of the BList.
 * 
 * @param value The value to be added to the new node.
 */
template <typename T, unsigned Size>
void BList<T, Size>::createNewNodeAndAddToTail(const T &value)
{
  auto new_node = createNode();
  new_node->values[0] = value;
  incrementNode(new_node);

  if (stats_.NodeCount == 0)
    tail_ = head_ = new_node;
  else
  {
    new_node->prev = tail_;
    tail_->next = new_node;
    tail_ = new_node;
  }
  ++stats_.NodeCount;
}

/**
 * @brief Inserts a new element at the beginning of the BList.
 *
 * This function inserts a new element with the given value at the beginning of the BList.
 * If the head node has available space, the value is added to the head node.
 * Otherwise, a new node is created and the value is added to it.
 *
 * @param value The value to be inserted.
 * @tparam T The type of the elements in the BList.
 * @tparam Size The maximum number of elements that each node can hold.
 */
template <typename T, unsigned Size>
void BList<T, Size>::push_front(const T &value)
{
  if (head_ && head_->count < stats_.ArraySize)
  {
    addToHead(value);
  }
  else
  {
    createNewNodeAndAddToHead(value);
  }
  
  ++stats_.ItemCount;
}

/**
 * @brief Adds a new element to the head of the BList.
 * 
 * @param value The value to be added.
 */
template <typename T, unsigned Size>
void BList<T, Size>::addToHead(const T &value)
{
  for (auto i = head_->count; i > 0; --i)
    head_->values[i] = head_->values[i - 1];
  head_->values[0] = value;
  incrementNode(head_);
}

/**
 * @brief Creates a new node with the given value and adds it to the head of the BList.
 * 
 * @param value The value to be added to the new node.
 */
template <typename T, unsigned Size>
void BList<T, Size>::createNewNodeAndAddToHead(const T &value)
{
  auto new_node = createNode();
  new_node->values[0] = value;
  incrementNode(new_node);

  if (stats_.NodeCount == 0)
    tail_ = head_ = new_node;
  else
  {
    new_node->next = head_;
    head_->prev = new_node;
    head_ = new_node;
  }
  ++stats_.NodeCount;
}


/**
 * @brief Inserts a value into the BList.
 * 
 * If the list is empty, the value is inserted at the front.
 * Otherwise, the value is inserted at the appropriate position based on the order of the elements.
 *
 * @param value The value to be inserted.
 */
template <typename T, unsigned Size>
void BList<T, Size>::insert(const T &value)
{
  if (!head_)
  {
    push_front(value);
    return;
  }

  auto current = findNodeToInsert(value);

  if (current)
  {
    insertValueIntoNode(value, current);
  }
  else // current = nullptr, meaning we are at the tail
  {
    insertValueAtTail(value);
  }
}

/**
 * @brief Finds the node in the BList to insert the given value.
 * 
 * @param value The value to be inserted.
 * @return A pointer to the BNode where the value should be inserted.
 */
template <typename T, unsigned Size>
typename BList<T, Size>::BNode* BList<T, Size>::findNodeToInsert(const T &value)
{
  auto current = head_;
  auto i = 0;
  
  while (current)
  {
    while (i < stats_.ArraySize && current->values[i] < value)
      ++i;

    if (i < current->count)
      break;

    i = 0;
    current = current->next;
  }

  return current;
}

/**
 * @brief Inserts a value into a BNode.
 * 
 * This function inserts a value into a BNode at the appropriate index based on the value's order.
 * If the BNode is full, it splits the node and redistributes the values accordingly.
 * 
 * @param value The value to be inserted.
 * @param node The BNode where the value will be inserted.
 */
template <typename T, unsigned Size>
void BList<T, Size>::insertValueIntoNode(const T &value, BNode* node)
{
  auto index = findInsertionIndex(value, node);

  if (index == 0)
  {
    if (node->prev && node->prev->count < stats_.ArraySize)
    {
      insertAtIndex(node->prev, node->prev->count, value);
    }
    else if (node->count < stats_.ArraySize)
    {
      insertAtIndex(node, index, value);
    }
    else if (node->prev)
    {
      splitNode(node->prev, stats_.ArraySize, value);
    }
    else
    {
      splitNode(node, index, value);
    }
  }
  else
  {
    if (node->count < stats_.ArraySize)
    {
      insertAtIndex(node, index, value);
    }
    else
    {
      splitNode(node, index, value);
    }
  }
}

/**
 * @brief Finds the insertion index for a given value in a BNode.
 * 
 * @param value The value to be inserted.
 * @param node The BNode in which the value will be inserted.
 * @return The index at which the value should be inserted.
 */
template <typename T, unsigned Size>
int BList<T, Size>::findInsertionIndex(const T &value, BNode* node)
{
  auto i = 0;
  while (i < node->count && node->values[i] < value)
    ++i;
  return i;
}

/**
 * @brief Inserts a value at the tail of the BList.
 * 
 * If the tail node has space, the value is inserted directly.
 * If the tail node is full, a new node is created and the value is inserted into it.
 *
 * @param value The value to be inserted.
 */
template <typename T, unsigned Size>
void BList<T, Size>::insertValueAtTail(const T &value)
{
  if (tail_->count < stats_.ArraySize)
  {
    insertAtIndex(tail_, tail_->count, value);
  }
  else
  {
    splitNode(tail_, tail_->count, value);
  }
}

/**
 * @brief Removes an element at the specified index from the BList.
 *
 * This function removes the element at the specified index from the BList.
 * It first retrieves the node containing the element at the given index,
 * and then removes the element from the node's array at the corresponding position.
 * If the node becomes empty after the removal, it is freed.
 *
 * @param index The index of the element to be removed.
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum number of elements that can be stored in each node.
 */
template <typename T, unsigned Size>
void BList<T, Size>::remove(int index)
{
  auto current = head_;
  auto total_index = current->count;
  auto i = index;

  while (total_index <= index)
  {
    i -= current->count;
    current = current->next;
    if (current)
      total_index += current->count;
  }

  removeAtIndex(current, i);
  if (current->count == 0)
    freeNode(current);
}

/**
 * @brief Removes the first occurrence of the specified value from the BList.
 * 
 * If the value is found, it is removed from the BList and the memory is freed if necessary.
 *
 * @param value The value to be removed from the BList.
 */
template <typename T, unsigned Size>
void BList<T, Size>::remove_by_value(const T &value)
{
  auto current = head_;
  auto total_index = 0;
  while (current)
  {
    for (auto i = 0; i < current->count; ++i)
    {
      if (current->values[i] == value)
      {
        removeAtIndex(current, i);
        if (current->count == 0)
          freeNode(current);
        return;
      }
    }
    total_index += current->count;
    current = current->next;
  }
}

/**
 * @brief Finds the index of the first occurrence of the specified value in the BList.
 *
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum number of elements that can be stored in each BNode.
 * @param value The value to search for.
 * @return The index of the first occurrence of the value, or -1 if the value is not found.
 */
template <typename T, unsigned Size>
int BList<T, Size>::find(const T &value) const
{
  auto current = head_;
  auto total_index = 0;
  while (current)
  {
    for (auto i = 0; i < current->count; ++i)
    {
      if (current->values[i] == value)
        return total_index + i;
    }
    total_index += current->count;
    current = current->next;
  }
  return -1;
}

/**
 * @brief Returns a reference to the element at the specified index in the BList.
 *
 * @param index The index of the element to access.
 * @return A reference to the element at the specified index.
 */
template <typename T, unsigned Size>
T &BList<T, Size>::operator[](int index)
{
  return valueAtIndex(index);
}

/**
 * @brief Overloaded subscript operator for accessing elements in the BList.
 *
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum number of elements that can be stored in the BList.
 * @param index The index of the element to access.
 * @return const T& A reference to the element at the specified index.
 */
template <typename T, unsigned Size>
const T &BList<T, Size>::operator[](int index) const
{
  return valueAtIndex(index);
}

/**
 * @brief Returns the number of items in the BList.
 *
 * @return The number of items in the BList.
 */
template <typename T, unsigned Size>
size_t BList<T, Size>::size() const
{
  return stats_.ItemCount;
}

/**
 * @brief Clears the BList by removing all items.
 *
 * This function removes all items from the BList by repeatedly calling the remove() function
 * until the number of items in the BList becomes zero.
 *
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum number of elements that the BList can hold.
 */
template <typename T, unsigned Size>
void BList<T, Size>::clear()
{
  while (stats_.ItemCount > 0)
    remove(0);
}

/**
 * @brief Get the statistics of the BList.
 *
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum number of elements that the BList can hold.
 * @return BListStats The statistics of the BList.
 */
template <typename T, unsigned Size>
BListStats BList<T, Size>::GetStats() const
{
  return stats_;
}

/**
 * @brief Creates a new BNode object.
 *
 * This function creates a new BNode object by allocating memory and copying the values from the given BNode object.
 * If the given BNode object is null, the new BNode object will be initialized with default values.
 *
 * @param rhs The BNode object to copy values from.
 * @return A pointer to the newly created BNode object.
 * @throws BListException if there is not enough memory to create the new BNode object.
 */
template <typename T, unsigned Size>
typename BList<T, Size>::BNode *BList<T, Size>::createNode(const BNode *rhs)
{
  auto new_node = new (std::nothrow) BNode;
  if (!new_node)
    throw BListException{
        BListException::BLIST_EXCEPTION::E_NO_MEMORY, "Not enough memory to create a new node!"};

  if (rhs)
  {
    new_node->next = rhs->next;
    new_node->prev = rhs->prev;
    new_node->count = rhs->count;
    for (auto i = 0; i < rhs->count; ++i)
      new_node->values[i] = rhs->values[i];
  }
  return new_node;
}

/**
 * Retrieves the BNode at the specified index in the BList.
 *
 * @param index The index of the BNode to retrieve.
 * @return The BNode at the specified index.
 * @throws BListException if the index is out of range.
 */
template <typename T, unsigned Size>
typename BList<T, Size>::BNode *BList<T, Size>::getNodeAtIndex(int index) const
{
  if (index < 0 || index >= stats_.NodeCount)
    throw BListException{
        BListException::BLIST_EXCEPTION::E_BAD_INDEX, "Index out of range!"};

  auto current = head_;
  auto i = 0;
  while (i < index)
  {
    current = current->next;
    ++i;
  }
  return current;
}

/**
 * @brief Frees a node from the BList.
 *
 * This function removes a given node from the BList and frees its memory.
 * It updates the previous and next pointers of the adjacent nodes accordingly.
 *
 * @param node The node to be freed.
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum number of elements that can be stored in the BList.
 */
template <typename T, unsigned Size>
void BList<T, Size>::freeNode(BNode *node)
{
  if (node->prev)
    node->prev->next = node->next;
  else
    head_ = node->next;

  if (node->next)
    node->next->prev = node->prev;
  else
    tail_ = node->prev;

  delete node;
  --stats_.NodeCount;

}

/**
 * @brief Increments the count of a BNode.
 *
 * This function increments the count of the given BNode by 1. If the count exceeds the maximum array size,
 * it is set to the maximum array size.
 *
 * @param node Pointer to the BNode to be incremented.
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum size of the BList.
 */
template <typename T, unsigned Size>
void BList<T, Size>::incrementNode(BNode *node)
{
  ++node->count;
  if (node->count > stats_.ArraySize)
    node->count = stats_.ArraySize;
}

/**
 * @brief Splits a node in the BList at the specified index and inserts the given value.
 *
 * @param node The node to be split.
 * @param index The index at which to insert the value.
 * @param value The value to be inserted.
 */
template <typename T, unsigned Size>
void BList<T, Size>::splitNode(BNode *node, int index, const T &value)
{
  auto new_node = createNode();
  setupNewNode(node, new_node);
  
  if (stats_.ArraySize == 1)
  {
    handleSingleArraySize(node, index, value, new_node);
  }
  else
  {
    handleMultipleArraySize(node, index, value, new_node);
  }
  
  updateTailIfNeeded(node, new_node);
  
  ++stats_.ItemCount;
  ++stats_.NodeCount;
}

/**
 * @brief Sets up a new node in the BList.
 * 
 * @param node The current node in the BList.
 * @param new_node The new node to be set up.
 */
template <typename T, unsigned Size>
void BList<T, Size>::setupNewNode(BNode *node, BNode *new_node)
{
  new_node->prev = node;
  if (node->next)
  {
    node->next->prev = new_node;
    new_node->next = node->next;
  }
  node->next = new_node;
}

/**
 * @brief Handles the case when the single array size is modified in the BList.
 * 
 * @param node The BNode where the modification is happening.
 * @param index The index of the value being modified.
 * @param value The new value to be inserted.
 * @param new_node The new BNode that will be created.
 */
template <typename T, unsigned Size>
void BList<T, Size>::handleSingleArraySize(BNode *node, int index, const T &value, BNode *new_node)
{
  if (index == 0)
  {
    new_node->values[0] = node->values[0];
    node->values[0] = value;
  }
  else
  {
    new_node->values[0] = value;
  }
  incrementNode(new_node);
}

/**
 * @brief Handles the case when the BList needs to be split into multiple arrays.
 * 
 * This function is called when a new value is inserted at a specific index in the BList.
 * It splits the current array into two arrays and inserts the new value at the specified index.
 * 
 * @param node The BNode pointer representing the current array.
 * @param index The index at which the new value needs to be inserted.
 * @param value The value to be inserted.
 * @param new_node The BNode pointer representing the new array.
 */
template <typename T, unsigned Size>
void BList<T, Size>::handleMultipleArraySize(BNode *node, int index, const T &value, BNode *new_node)
{
  auto middle = stats_.ArraySize / 2;
  auto shiftIndex = index > middle ? index - middle : 0;

  copyUpperHalfValues(node, new_node, middle);
  adjustCountsAndInsertValue(node, index, value, new_node, middle, shiftIndex);
}

/**
 * @brief Copies the upper half of values from one BNode to another BNode.
 * 
 * @param node The source BNode from which to copy the values.
 * @param new_node The destination BNode to which the values will be copied.
 * @param middle The index representing the middle of the values array in the source BNode.
 */
template <typename T, unsigned Size>
void BList<T, Size>::copyUpperHalfValues(BNode *node, BNode *new_node, int middle)
{
  auto j = 0;
  for (auto i = middle; i < stats_.ArraySize; ++i)
  {
    new_node->values[j++] = node->values[i];
    incrementNode(new_node);
  }
  node->count = middle;
}

/**
 * @brief Adjusts the counts and inserts a value into the BList.
 * 
 * @param node The BNode where the value will be inserted.
 * @param index The index at which the value will be inserted.
 * @param value The value to be inserted.
 * @param new_node The new BNode that will be created if necessary.
 * @param middle The middle index of the BNode.
 * @param shiftIndex The index at which the value will be shifted in the new BNode.
 */
template <typename T, unsigned Size>
void BList<T, Size>::adjustCountsAndInsertValue(BNode *node, int index, const T &value, BNode *new_node, int middle, int shiftIndex)
{
  if (index <= middle)
  {
    shiftValuesForInsertion(node, index);
    node->values[index] = value;
    incrementNode(node);
  }
  else
  {
    if (index == stats_.ArraySize)
    {
      new_node->values[new_node->count] = value;
    }
    else
    {
      shiftValuesForInsertion(new_node, shiftIndex);
      new_node->values[shiftIndex] = value;
    }
    incrementNode(new_node);
  }
}

/**
 * @brief Shifts the values in a BNode for insertion at a specific index.
 * 
 * @param node The BNode in which the values will be shifted.
 * @param index The index at which the values will be inserted.
 */
template <typename T, unsigned Size>
void BList<T, Size>::shiftValuesForInsertion(BNode *node, int index)
{
  for (auto i = node->count; i > index; --i)
    node->values[i] = node->values[i - 1];
}
  
template <typename T, unsigned Size>
void BList<T, Size>::updateTailIfNeeded(BNode *node, BNode *new_node)
{
  if (node == tail_)
    tail_ = new_node;
}

/**
 * @brief Returns the value at the specified index in the BList.
 *
 * @param index The index of the value to retrieve.
 * @return A reference to the value at the specified index.
 */
template <typename T, unsigned Size>
T &BList<T, Size>::valueAtIndex(int index) const
{
  auto current = findNodeContainingIndex(index);
  auto relativeIndex = calculateRelativeIndex(index, current);
  
  return current->values[relativeIndex];
}

/**
 * @brief Finds the BNode containing the given index in the BList.
 * 
 * @param index The index to search for.
 * @return A pointer to the BNode containing the index, or nullptr if not found.
 */
template <typename T, unsigned Size>
typename BList<T, Size>::BNode* BList<T, Size>::findNodeContainingIndex(int index) const
{
  auto current = head_;
  auto total_index = current->count;

  while (total_index <= index)
  {
    current = current->next;
    if (current)
      total_index += current->count;
  }

  return current;
}

/**
 * @brief Calculates the relative index of a given index within a BNode.
 * 
 * The relative index is the index within the BNode, taking into account the previous nodes.
 *
 * @param index The index to calculate the relative index for.
 * @param node The BNode to calculate the relative index within.
 * @return The relative index of the given index within the BNode.
 */
template <typename T, unsigned Size>
int BList<T, Size>::calculateRelativeIndex(int index, BNode* node) const
{
  auto relativeIndex = index;

  for (auto temp = head_; temp != node; temp = temp->next)
    relativeIndex -= temp->count;

  return relativeIndex;
}

/**
 * @brief Inserts a value at the specified index in the BList.
 *
 * @param node The BNode in which the value will be inserted.
 * @param index The index at which the value will be inserted.
 * @param value The value to be inserted.
 */
template <typename T, unsigned Size>
void BList<T, Size>::insertAtIndex(BNode *node, int index, const T &value)
{
  shiftValuesForInsertion(node, index);
  node->values[index] = value;
  incrementNode(node);
  ++stats_.ItemCount;
}

/**
 * @brief Removes the element at the specified index from the BList.
 *
 * This function removes the element at the specified index from the BList.
 * It shifts all the elements after the specified index to the left by one position.
 * The count of elements in the BList is decremented by one.
 *
 * @param node A pointer to the BNode from which the element is to be removed.
 * @param index The index of the element to be removed.
 *
 * @tparam T The type of elements stored in the BList.
 * @tparam Size The maximum number of elements that can be stored in each BNode.
 */
template <typename T, unsigned Size>
void BList<T, Size>::removeAtIndex(BNode *node, int index)
{
  for (auto i = index; i < node->count; ++i)
    node->values[i] = node->values[i + 1];
  --node->count;
  --stats_.ItemCount;
}