/*!******************************************************************
 * \file      ChHashTable.cpp
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course: 
 * \par       Section: A
 * \par      
 * \date      29-03-2024
 * 
 * \brief       
 *********************************************************************/

#include "ChHashTable.h"
#include <cmath>

/******************************************************************************/
/*!
\brief
  This is the constructor for a ChHashTable.
\param Config, hash table configuration settings.
\param allocator, client object allocator.
*/
/******************************************************************************/
template <typename T>
ChHashTable<T>::ChHashTable(const HTConfig &Config, ObjectAllocator *allocator)
    : oa{allocator}, config{Config}, stats{}
{
  head = new ChHTHeadNode[config.InitialTableSize_];

  stats.HashFunc_ = config.HashFunc_;
  stats.Allocator_ = allocator;
  stats.TableSize_ = config.InitialTableSize_;
}

/******************************************************************************/
/*!
\brief
  This is the destructor for a ChHashTable.
*/
/******************************************************************************/
template <typename T>
ChHashTable<T>::~ChHashTable()
{
  clear();
  delete[] head;
}

/******************************************************************************/
/*!
\brief
  Checks if the table needs to be resized based on the current load factor.
*/
/******************************************************************************/
template <typename T>
void ChHashTable<T>::check_resize()
{
  const auto current_load_factor =
      ((stats.Count_ + 1) / static_cast<double>(stats.TableSize_));

  if (current_load_factor > config.MaxLoadFactor_)
    grow_table();
}

/******************************************************************************/
/*!
\brief
  Finds the index in the table for a given key using the hash function.
\param Key, the key for which the index is calculated.
\return unsigned, the index in the table.
*/
/******************************************************************************/
template <typename T>
unsigned ChHashTable<T>::find_index(const char *Key) const
{
  return config.HashFunc_(Key, stats.TableSize_);
}

/******************************************************************************/
/*!
\brief
  Searches for a key in a given linked list.
\param Key, the key to search for.
\param list, the linked list to search in.
\return bool, true if the key is found, false otherwise.
*/
/******************************************************************************/
template <typename T>
bool ChHashTable<T>::find_key_in_list(const char *Key, ChHTNode *list) const
{
  while (list)
  {
    ++stats.Probes_;

    if (strncmp(Key, list->Key, MAX_KEYLEN) == 0)
      return true;

    list = list->Next;
  }

  return false;
}

/******************************************************************************/
/*!
\brief
  Inserts a new node into a given linked list.
\param Key, the key for the new node.
\param Data, the data for the new node.
\param list, the linked list to insert into.
*/
/******************************************************************************/
template <typename T>
void ChHashTable<T>::insert_into_list(const char *Key, const T &Data, ChHTNode *&list)
{
  ChHTNode *new_node = make_node(Data);
  strncpy(new_node->Key, Key, MAX_KEYLEN);

  new_node->Next = list;
  list = new_node;
}

/******************************************************************************/
/*!
\brief
  Inserts a data into the hash table with a key.
\param Key, the key for the data.
\param Data, the data to insert.
*/
/******************************************************************************/
template <typename T>
void ChHashTable<T>::insert(const char *Key, const T &Data)
{
  try
  {
    check_resize();

    unsigned index = find_index(Key);
    ChHTHeadNode *table_head = &head[index];
    ChHTNode *list = table_head->Nodes;

    ++stats.Probes_;

    if (find_key_in_list(Key, list))
      throw HashTableException(HashTableException::E_DUPLICATE, "Trying to insert duplicate item!");

    insert_into_list(Key, Data, table_head->Nodes);

    ++table_head->Count;
    ++stats.Count_;
  }
  catch (HashTableException &e)
  {
    throw e;
  }
}

/******************************************************************************/
/*!
\brief
  Removes a node from a given linked list.
\param current, pointer to the node to be removed.
\param previous, pointer to the node before the one to be removed.
\param table_head, pointer to the head node of the linked list.
*/
/******************************************************************************/
template <typename T>
void ChHashTable<T>::remove_node_from_list(ChHTNode *current, ChHTNode *previous, ChHTHeadNode *table_head)
{
  if (previous)
    previous->Next = current->Next;
  else
    table_head->Nodes = current->Next;

  remove_node(current);

  --table_head->Count;
  --stats.Count_;
}

/******************************************************************************/
/*!
\brief
  Removes the data from the hash table with a given key.
\param Key, the key for the data to remove.
*/
/******************************************************************************/
template <typename T>
void ChHashTable<T>::remove(const char *Key)
{
  unsigned index = find_index(Key);
  ChHTHeadNode *table_head = &head[index];
  ChHTNode *current = table_head->Nodes;
  ChHTNode *previous = nullptr;

  while (current)
  {
    ++stats.Probes_;

    if (strncmp(Key, current->Key, MAX_KEYLEN) == 0)
    {
      remove_node_from_list(current, previous, table_head);
      return;
    }

    previous = current;
    current = current->Next;
  }
}

/******************************************************************************/
/*!
\brief
  Searches for a key in a given linked list and returns the data if found.
\param Key, the key to search for.
\param list, the linked list to search in.
\returns const T&, the data associated with the key.
\throws HashTableException, if the key is not found.
*/
/******************************************************************************/
template <typename T>
const T &ChHashTable<T>::search_key_in_list(const char *Key, ChHTNode *list) const
{
  while (list)
  {
    ++stats.Probes_;

    if (strncmp(Key, list->Key, MAX_KEYLEN) == 0)
      return list->Data;

    list = list->Next;
  }

  throw HashTableException(HashTableException::E_ITEM_NOT_FOUND, "Key not found!");
}

/******************************************************************************/
/*!
\brief
  Finds a data from the hash table with a key.
\returns the data if key is found, else a
  HashTableException::E_ITEM_NOT_FOUND will be thrown.
*/
/******************************************************************************/
template <typename T>
const T &ChHashTable<T>::find(const char *Key) const
{
  unsigned index = find_index(Key);
  ChHTNode *list = head[index].Nodes;

  return search_key_in_list(Key, list);
}

/******************************************************************************/
/*!
\brief
  This function clears the hash table.
*/
/******************************************************************************/
template <typename T>
void ChHashTable<T>::clear()
{
  for (unsigned i = 0; i < stats.TableSize_; ++i)
  {
    ChHTNode *list = head[i].Nodes;

    while (list)
    {
      ChHTNode *temp = list->Next;
      remove_node(list);
      list = temp;
    }

    head[i].Nodes = nullptr;
  }

  stats.Count_ = 0;
}

/******************************************************************************/
/*!
\brief
  This function returns the Hash table's statistics.
\returns HTStats, the stats of this Hash table.
*/
/******************************************************************************/
template <typename T>
HTStats ChHashTable<T>::GetStats() const
{
  return stats;
}

/******************************************************************************/
/*!
\brief
  This function returns the table node.
\returns ChHTHeadNode*, the head node of the table.
*/
/******************************************************************************/
template <typename T>
const typename ChHashTable<T>::ChHTHeadNode *ChHashTable<T>::GetTable() const
{
  return head;
}

/******************************************************************************/
/*!
\brief
  This function creates a new node with given data.
\param data, the data of the node.
\return ChHTNode*, a pointer to the new node.
*/
/******************************************************************************/
template <typename T>
typename ChHashTable<T>::ChHTNode *ChHashTable<T>::make_node(const T &data)
{
  try
  {
    if (oa)
    {
      ChHTNode *alloc = reinterpret_cast<ChHTNode *>(oa->Allocate());
      ChHTNode *node = new (alloc) ChHTNode(data);
      return node;
    }

    ChHTNode *node = new ChHTNode(data);
    return node;
  }
  catch (std::bad_alloc &e)
  {
    throw(HashTableException(HashTableException::E_NO_MEMORY,
                             "Unable to allocate memory!"));
  }
}

/******************************************************************************/
/*!
\brief
  This function frees a given node.
\param ChHTNode*, the node to free.
*/
/******************************************************************************/
template <typename T>
void ChHashTable<T>::remove_node(ChHTNode *&node)
{

  if (config.FreeProc_)
    config.FreeProc_(node->Data);

  if (oa)
    oa->Free(node);
  else
    delete node;
}

/******************************************************************************/
/*!
\brief
  This function grows the size of the table.
*/
/******************************************************************************/
template <typename T>
void ChHashTable<T>::grow_table()
{
  try
  {
    unsigned old_table_size = stats.TableSize_;
    double factor = std::ceil(stats.TableSize_ * config.GrowthFactor_);
    unsigned new_table_size = GetClosestPrime(static_cast<unsigned>(factor));
    stats.TableSize_ = new_table_size;


    ChHTHeadNode *new_table = new ChHTHeadNode[stats.TableSize_];
    for (unsigned i = 0; i < old_table_size; ++i)
    {

      ChHTNode *list = head[i].Nodes;
      while (list)
      {
        ++stats.Probes_;
        ChHTNode *temp = list->Next;
        unsigned index = stats.HashFunc_(list->Key, stats.TableSize_);

        if (new_table[index].Nodes)
        {
          ChHTNode *new_list = new_table[index].Nodes;

          while (new_list)
          {
            ++stats.Probes_;

            if (strncmp(new_list->Key, list->Key, MAX_KEYLEN) == 0)
              break;
            new_list = new_list->Next;
          }
        }
        list->Next = new_table[index].Nodes;
        new_table[index].Nodes = list;

        list = temp;
      }
    }
    delete[] head;
    head = new_table;
    ++stats.Expansions_;
  }
  catch (std::bad_alloc &e)
  {
    throw(HashTableException(HashTableException::E_NO_MEMORY,
                             "Unable to allocate memory!"));
  }
}
