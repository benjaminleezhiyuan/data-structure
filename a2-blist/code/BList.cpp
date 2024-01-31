/*!******************************************************************
 * \file      BList.cpp
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course: CSD2183
 * \par       Section: A
 * \par      
 * \date      31-01-2024
 * 
 * \brief  
 *********************************************************************/

#include "BList.h"

template <typename T, unsigned Size>
size_t BList<T, Size>::nodesize(void)
{
  return sizeof(BNode);
}

template <typename T, unsigned Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
  return head_;
}
