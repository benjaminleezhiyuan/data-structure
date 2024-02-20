/*!******************************************************************
 * \file      BList.h
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course: CSD2183
 * \par       Section: A
 * \par
 * \date      31-01-2024
 *
 * \brief     This file contains the declarations for the BList.
 *********************************************************************/

////////////////////////////////////////////////////////////////////////////////
#ifndef BLIST_H
#define BLIST_H
////////////////////////////////////////////////////////////////////////////////

#include <string> // error strings

/*!
  The exception class for BList
*/
class BListException : public std::exception
{
private:
  int m_ErrCode;             //!< One of E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR
  std::string m_Description; //!< Description of the exception

public:
  /*!
    Constructor

    \param ErrCode
      The error code for the exception.

    \param Description
      The description of the exception.
  */
  BListException(int ErrCode, const std::string &Description) : m_ErrCode(ErrCode), m_Description(Description){};

  /*!
    Get the kind of exception

    \return
      One of E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR
  */
  virtual int code() const
  {
    return m_ErrCode;
  }

  /*!
    Get the human-readable text for the exception

    \return
      The description of the exception
  */
  virtual const char *what() const throw()
  {
    return m_Description.c_str();
  }

  /*!
    Destructor is "implemented" because it needs to be virtual
  */
  virtual ~BListException()
  {
  }

  //! The reason for the exception
  enum BLIST_EXCEPTION
  {
    E_NO_MEMORY,
    E_BAD_INDEX,
    E_DATA_ERROR
  };
};

/*!
  Statistics about the BList
*/
struct BListStats
{
  //!< Default constructor
  BListStats() : NodeSize(0), NodeCount(0), ArraySize(0), ItemCount(0){};

  /*!
    Non-default constructor

    \param nsize
      Size of the node

    \param ncount
      Number of nodes in the list

    \param asize
      Number of elements in each node (array)

    \param count
      Number of items in the list

  */
  BListStats(size_t nsize, int ncount, int asize, int count) : NodeSize(nsize), NodeCount(ncount), ArraySize(asize), ItemCount(count){};

  size_t NodeSize; //!< Size of a node (via sizeof)
  int NodeCount;   //!< Number of nodes in the list
  int ArraySize;   //!< Max number of items in each node
  int ItemCount;   //!< Number of items in the entire list
};

/*!
  The BList class
*/
template <typename T, unsigned Size = 1>
class BList
{

public:
  /*!
    Node struct for the BList
  */
  struct BNode
  {
    BNode *next;    //!< pointer to next BNode
    BNode *prev;    //!< pointer to previous BNode
    int count;      //!< number of items currently in the node
    T values[Size]; //!< array of items in the node

    //!< Default constructor
    BNode() : next(0), prev(0), count(0) {}
  };

  BList();                            // default constructor
  BList(const BList &rhs);            // copy constructor
  ~BList();                           // destructor
  BList &operator=(const BList &rhs); // assign operator

  // arrays will be unsorted, if calling either of these
  void push_back(const T &value);
  void push_front(const T &value);

  // arrays will be sorted, if calling this
  void insert(const T &value);

  void remove(int index);
  void remove_by_value(const T &value);

  int find(const T &value) const; // returns index, -1 if not found

  T &operator[](int index);             // for l-values
  const T &operator[](int index) const; // for r-values

  size_t size() const; // total number of items (not nodes)
  void clear();        // delete all nodes

  static size_t nodesize(); // so the allocator knows the size

  // For debugging
  const BNode *GetHead() const;
  BListStats GetStats() const;

private:
  BNode *head_; //!< points to the first node
  BNode *tail_; //!< points to the last node

  // Other private data and methods you may need ...
  BListStats stats_;
  BNode *createNode(const BNode *rhs = nullptr);
  BNode *getNodeAtIndex(int index) const;
  void freeNode(BNode *node);
  void incrementNode(BNode *node);
  void splitNode(BNode *node, int index, const T &value);
  T &valueAtIndex(int index) const;
  void insertAtIndex(BNode *node, int index, const T &value);
  void removeAtIndex(BNode *node, int index);

  // Insert helper
  BNode *findNodeToInsert(const T &value);
  void insertValueIntoNode(const T &value, BNode* node);
  int findInsertionIndex(const T &value, BNode* node);
  void insertValueAtTail(const T &value);

  // Split helper
  void setupNewNode(BNode *node, BNode *new_node);
  void handleSingleArraySize(BNode *node, int index, const T &value, BNode *new_node);
  void handleMultipleArraySize(BNode *node, int index, const T &value, BNode *new_node);
  void copyUpperHalfValues(BNode *node, BNode *new_node, int middle);
  void adjustCountsAndInsertValue(BNode *node, int index, const T &value, BNode *new_node, int middle, int shiftIndex);
  void shiftValuesForInsertion(BNode *node, int index);
  void updateTailIfNeeded(BNode *node, BNode *new_node);

  BNode* findNodeContainingIndex(int index) const;
  int calculateRelativeIndex(int index, BNode* node) const;

  void addToHead(const T &value);
  void createNewNodeAndAddToHead(const T &value);
  void addToTail(const T &value);
  void createNewNodeAndAddToTail(const T &value);
};

#include "BList.cpp"

#endif // BLIST_H
