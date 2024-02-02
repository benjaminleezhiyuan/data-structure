/*!******************************************************************
 * \file      ObjectAllocator.cpp
 * \author    Benjamin Lee
 * \par       DP email: benjaminzhiyuan.lee\@digipen.edu.sg
 * \par       Course: CSD2183
 * \par       Section: B
 * \par
 * \date      31-01-2024
 *
 * \brief
 *********************************************************************/

#include "ObjectAllocator.h"
#include <cstring>

/*!
 * \brief Calculates the alignment requirements based on the specified configuration.
 */
void ObjectAllocator::CalculateAlignment()
{
    if (Config_.Alignment_ > 1)
    {
        // Only need to align if alignment is 2 and above
        Config_.InterAlignSize_ = Config_.Alignment_ - ((Stats_.ObjectSize_ + Config_.PadBytes_ + Config_.PadBytes_ + Config_.HBlockInfo_.size_) % Config_.Alignment_);
        Config_.LeftAlignSize_ = Config_.Alignment_ - ((Config_.PadBytes_ + Config_.HBlockInfo_.size_ + sizeof(GenericObject *)) % Config_.Alignment_);
    }
}

/*!
 * \brief Calculates the page size based on the object size and configuration.
 * 
 * \param ObjectSize The size of each object.
 */
void ObjectAllocator::CalculatePageSize(size_t ObjectSize)
{
    Stats_.ObjectSize_ = ObjectSize;

    Stats_.PageSize_ = sizeof(GenericObject *) + Config_.LeftAlignSize_ + Config_.ObjectsPerPage_ * (Config_.HBlockInfo_.size_ + Config_.PadBytes_ + Stats_.ObjectSize_ + Config_.PadBytes_ + Config_.InterAlignSize_) - Config_.InterAlignSize_;
}


/*!
 * \brief ObjectAllocator constructor.
 * 
 * \param ObjectSize The size of each object.
 * \param config Configuration parameters for ObjectAllocator.
 */
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config)
    : PageList_(nullptr),
      FreeList_(nullptr),
      Config_(config),
      Stats_(OAStats())
{
    CalculateAlignment();
    CalculatePageSize(ObjectSize);

    // If not using new/delete
    if (!Config_.UseCPPMemManager_)
    {
        Newpage();
    }
}

/*!
 * \brief Initializes memory blocks on a page.
 * 
 * \param page Pointer to the page.
 * \param index Index of the block on the page.
 */
void ObjectAllocator::InitializeMemoryBlocks(char *page, unsigned int index)
{
    char *memory = page + sizeof(GenericObject *) + Config_.LeftAlignSize_ + Config_.HBlockInfo_.size_ + Config_.PadBytes_ +
                   index * (Config_.HBlockInfo_.size_ + Config_.PadBytes_ + Stats_.ObjectSize_ + Config_.PadBytes_ + Config_.InterAlignSize_);

    InitializeBlockMemory(memory, index == Config_.ObjectsPerPage_ - 1);
}

/*!
 * \brief Allocates memory for a new page and initializes its blocks.
 * 
 * \return Pointer to the allocated page memory.
 */
char *ObjectAllocator::AllocatePageMemory()
{
    try
    {
        return new char[Stats_.PageSize_];
    }
    catch (const std::bad_alloc &)
    {
        throw OAException(OAException::E_NO_MEMORY, "No Physical Memory Available");
    }
}

/*!
 * \brief Initializes the memory of a block.
 * 
 * \param memory Pointer to the memory block.
 * \param isLastBlock Indicates if the block is the last one on the page.
 */
void ObjectAllocator::InitializeBlockMemory(char *memory, bool isLastBlock)
{
    std::memset(memory, UNALLOCATED_PATTERN, Stats_.ObjectSize_);
    std::memset(memory + Stats_.ObjectSize_, PAD_PATTERN, Config_.PadBytes_);

    if (!isLastBlock)
    {
        std::memset(memory + Stats_.ObjectSize_ + Config_.PadBytes_, ALIGN_PATTERN, Config_.InterAlignSize_);
        std::memset(memory + Stats_.ObjectSize_ + Config_.PadBytes_ + Config_.InterAlignSize_ + Config_.HBlockInfo_.size_, PAD_PATTERN, Config_.PadBytes_);
    }

    GenericObject *currentBlock = reinterpret_cast<GenericObject *>(memory);
    currentBlock->Next = FreeList_;
    FreeList_ = currentBlock;
    Stats_.FreeObjects_++;
}

/*!
 * \brief Initializes the header of a page.
 * 
 * \param page Pointer to the page.
 */
void ObjectAllocator::InitializePageHeader(char *page)
{
    std::memset(page + sizeof(GenericObject *), ALIGN_PATTERN, Config_.LeftAlignSize_);
    std::memset(page + sizeof(GenericObject *) + Config_.LeftAlignSize_ + Config_.HBlockInfo_.size_, PAD_PATTERN, Config_.PadBytes_);

    GenericObject *currentPage = reinterpret_cast<GenericObject *>(page);
    currentPage->Next = PageList_;
    PageList_ = currentPage;
    Stats_.PagesInUse_++;
}

/*!
 * \brief Allocates a new page and initializes its blocks.
 */
void ObjectAllocator::Newpage()
{
    char *page = AllocatePageMemory();

    for (unsigned int i = 0; i < Config_.ObjectsPerPage_; i++)
    {
        InitializeMemoryBlocks(page, i);
    }

    InitializePageHeader(page);
}

/*!
 * \brief ObjectAllocator destructor.
 */
ObjectAllocator::~ObjectAllocator()
{
    // If not using new/delete
    if (!Config_.UseCPPMemManager_)
    {
        while (PageList_)
        {
            GenericObject *tmp = PageList_->Next;
            delete[] reinterpret_cast<char *>(PageList_);
            PageList_ = tmp;
        }
    }
}

/*!
 * \brief Allocates memory for a new object.
 * 
 * \param label Optional label for debugging.
 * 
 * \return Pointer to the allocated object.
 */
void *ObjectAllocator::Allocate(const char *label)
{
    if (!Config_.UseCPPMemManager_)
    {
        CheckAndAllocateMemory();

        if (++Stats_.ObjectsInUse_ > Stats_.MostObjects_)
        {
            Stats_.MostObjects_ = Stats_.ObjectsInUse_;
        }

        Stats_.Allocations_++;
        Stats_.FreeObjects_--;

        GenericObject *allocatedObject = FreeList_;
        FreeList_ = FreeList_->Next;

        InitializeAllocatedMemory(allocatedObject);
        SetHeaderInfo(allocatedObject, label);

        return allocatedObject;
    }
    else
    {
        return AllocateUsingCPP();
    }
}

/*!
 * \brief Checks for free memory and allocates a new page if needed.
 */
void ObjectAllocator::CheckAndAllocateMemory()
{
    if (!FreeList_)
    {
        if (Stats_.PagesInUse_ == Config_.MaxPages_)
        {
            throw OAException(OAException::E_NO_PAGES, "No Logical Memory Available");
        }
        else
        {
            Newpage();
        }
    }
}

/*!
 * \brief Initializes the memory of an allocated object.
 * 
 * \param allocatedObject Pointer to the allocated object.
 */
void ObjectAllocator::InitializeAllocatedMemory(GenericObject *allocatedObject)
{
    std::memset(allocatedObject, ALLOCATED_PATTERN, Stats_.ObjectSize_);
}


/*!
 * \brief Sets header information for an allocated object.
 * 
 * \param allocatedObject Pointer to the allocated object.
 * \param label Optional label for debugging.
 */
void ObjectAllocator::SetHeaderInfo(GenericObject *allocatedObject, const char *label)
{
    char *header = reinterpret_cast<char *>(allocatedObject) - Config_.PadBytes_ - Config_.HBlockInfo_.size_;

    if (Config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbBasic)
    {
        SetBasicHeaderInfo(header);
    }
    else if (Config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbExtended)
    {
        SetExtendedHeaderInfo(header);
    }
    else if (Config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbExternal)
    {
        SetExternalHeaderInfo(header, label);
    }
}
/*!
 * \brief Sets basic header information for an allocated object.
 * 
 * \param header Pointer to the header of the allocated object.
 */
void ObjectAllocator::SetBasicHeaderInfo(char *header)
{
    *reinterpret_cast<unsigned int *>(header) = Stats_.Allocations_;
    *(header + sizeof(unsigned int)) |= 0x01;
}

/*!
 * \brief Sets extended header information for an allocated object.
 * 
 * \param header Pointer to the header of the allocated object.
 */
void ObjectAllocator::SetExtendedHeaderInfo(char *header)
{
    (*reinterpret_cast<unsigned short *>(header + Config_.HBlockInfo_.additional_))++;
    *reinterpret_cast<unsigned int *>(header + Config_.HBlockInfo_.additional_ + sizeof(unsigned short)) = Stats_.Allocations_;
    *(header + Config_.HBlockInfo_.additional_ + sizeof(unsigned short) + sizeof(unsigned int)) |= 0x01;
}

/*!
 * \brief Sets external header information for an allocated object.
 * 
 * \param header Pointer to the header of the allocated object.
 * \param label Optional label for debugging.
 */
void ObjectAllocator::SetExternalHeaderInfo(char *header, const char *label)
{
    try
    {
        MemBlockInfo *ext = new MemBlockInfo;
        ext->in_use = true;
        ext->alloc_num = Stats_.Allocations_;

        if (label)
        {
            ext->label = new char[strlen(label) + 1];
            strcpy(ext->label, label);
        }

        char **head = reinterpret_cast<char **>(header);
        *head = reinterpret_cast<char *>(ext);
    }
    catch (const std::bad_alloc &)
    {
        throw OAException(OAException::E_NO_MEMORY, "No Physical Memory Available");
    }
}

/*!
 * \brief Allocates memory for an object using new/delete.
 * 
 * \return Pointer to the allocated object.
 */
void *ObjectAllocator::AllocateUsingCPP()
{
    try
    {
        char *data = new char;
        if (++Stats_.ObjectsInUse_ > Stats_.MostObjects_)
        {
            Stats_.MostObjects_ = Stats_.ObjectsInUse_;
        }
        Stats_.Allocations_++;
        return data;
    }
    catch (const std::bad_alloc &)
    {
        throw OAException(OAException::E_NO_MEMORY, "No Physical Memory!");
    }
}

/*!
 * \brief Frees memory allocated for an object.
 * 
 * \param Object Pointer to the object to be freed.
 */
void ObjectAllocator::Free(void *Object)
{
    if (!Config_.UseCPPMemManager_)
    {
        CheckDoubleFree(Object);
        CheckBadBoundary(Object);

        // Validate and deallocate memory block
        ValidateAndDeallocate(Object);
    }
    else
    {
        DeleteUsingCPP(Object);
    }
}

/*!
 * \brief Checks for double-free and throws an exception if detected.
 * 
 * \param Object Pointer to the object being freed.
 */
void ObjectAllocator::CheckDoubleFree(void *Object)
{
    // Check for double free
    for (size_t i = 0; i < Stats_.ObjectSize_; i++)
    {
        if (*(reinterpret_cast<unsigned char *>(Object) + i) == FREED_PATTERN || *(reinterpret_cast<unsigned char *>(Object) + i) == UNALLOCATED_PATTERN)
        {
            throw OAException(OAException::E_MULTIPLE_FREE, "Multiple Free Detected");
        }
    }
}

/*!
 * \brief Checks for invalid object boundary and throws an exception if detected.
 * 
 * \param Object Pointer to the object being freed.
 */
void ObjectAllocator::CheckBadBoundary(void *Object)
{
    // Check bad boundary
    GenericObject *currpage = FindPageForObject(Object);

    if (!IsValidBoundary(Object, currpage))
    {
        throw OAException(OAException::E_BAD_BOUNDARY, "Invalid Object Boundary");
    }
}

/*!
 * \brief Validates and deallocates memory for a freed object.
 * 
 * \param Object Pointer to the object being freed.
 */
void ObjectAllocator::ValidateAndDeallocate(void *Object)
{
    // Validate and deallocate memory block
    DeallocateMemory(Object);
    UpdateHeaderInfo(Object);
    MarkAsFreed(Object);
}

/*!
 * \brief Checks if the given memory address is within the page.
 * 
 * \param Object Pointer to the object being checked.
 * \param page Pointer to the page.
 * 
 * \return True if the memory is within the page, false otherwise.
 */
bool ObjectAllocator::IsWithinPage(void *Object, GenericObject *page) const
{
    // Check if the given object is within the page
    return (Object >= reinterpret_cast<char *>(page) &&
            Object < reinterpret_cast<char *>(page) + Stats_.PageSize_);
}

/*!
 * \brief Finds the page containing the given object.
 * 
 * \param Object Pointer to the object.
 * 
 * \return Pointer to the page containing the object.
 */
GenericObject *ObjectAllocator::FindPageForObject(void *Object) const
{
    // Find the page containing the given object
    GenericObject *currpage = PageList_;

    while (currpage)
    {
        if (IsWithinPage(Object, currpage))
        {
            break;
        }
        currpage = currpage->Next;
    }

    return currpage;
}

/*!
 * \brief Checks if the object boundary within the page is valid.
 * 
 * \param Object Pointer to the object.
 * \param currpage Pointer to the page.
 * 
 * \return True if the boundary is valid, false otherwise.
 */
bool ObjectAllocator::IsValidBoundary(void *Object, GenericObject *currpage) const
{
    // Validate object boundary within the page
    size_t withinpage = reinterpret_cast<char *>(Object) - (reinterpret_cast<char *>(currpage) + sizeof(GenericObject *) + Config_.LeftAlignSize_ + Config_.PadBytes_ + Config_.HBlockInfo_.size_);

    if (Config_.Alignment_ != 0)
    {
        return (withinpage % Config_.Alignment_ == 0);
    }
    else
    {
        return (withinpage % (Stats_.ObjectSize_ + Config_.PadBytes_ + Config_.InterAlignSize_ + Config_.HBlockInfo_.size_ + Config_.PadBytes_) == 0);
    }
}

/*!
 * \brief Deallocates memory for a freed object.
 * 
 * \param Object Pointer to the object being freed.
 */
void ObjectAllocator::DeallocateMemory(void *Object)
{
    // Deallocate memory block
    char *header = reinterpret_cast<char *>(Object) - Config_.PadBytes_ - Config_.HBlockInfo_.size_;

    if (Config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbExternal)
    {
        DeleteExternalHeaderInfo(header);
    }
}

/*!
 * \brief Deletes external header information for an allocated object.
 * 
 * \param header Pointer to the header of the allocated object.
 */
void ObjectAllocator::DeleteExternalHeaderInfo(char *header)
{
    // Delete external header info
    MemBlockInfo **extinfo = reinterpret_cast<MemBlockInfo **>(header);
    (*extinfo)->in_use = false;
    (*extinfo)->alloc_num = 0;
    delete[] (*extinfo)->label;
    delete *extinfo;
    *extinfo = nullptr;
}

/*!
 * \brief Updates header information based on block type for a freed object.
 * 
 * \param Object Pointer to the object being freed.
 */
void ObjectAllocator::UpdateHeaderInfo(void *Object)
{
    // Update header info based on block type
    char *header = reinterpret_cast<char *>(Object) - Config_.PadBytes_ - Config_.HBlockInfo_.size_;

    if (Config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbBasic)
    {
        UpdateBasicHeaderInfo(header);
    }
    else if (Config_.HBlockInfo_.type_ == OAConfig::HBLOCK_TYPE::hbExtended)
    {
        UpdateExtendedHeaderInfo(header);
    }
}

/*!
 * \brief Updates basic header information for a freed object.
 * 
 * \param header Pointer to the header of the freed object.
 */
void ObjectAllocator::UpdateBasicHeaderInfo(char *header)
{
    // Update basic header info
    *reinterpret_cast<unsigned int *>(header) = 0x00;
    *(header + sizeof(unsigned int)) &= 0xFE;
}

/*!
 * \brief Updates extended header information for a freed object.
 * 
 * \param header Pointer to the header of the freed object.
 */
void ObjectAllocator::UpdateExtendedHeaderInfo(char *header)
{
    // Update extended header info
    *reinterpret_cast<unsigned int *>(header + Config_.HBlockInfo_.additional_ + sizeof(unsigned short)) = 0;
    *(header + Config_.HBlockInfo_.additional_ + sizeof(unsigned short) + sizeof(unsigned int)) &= 0xFE;
}

/*!
 * \brief Marks a freed memory block with a freed pattern and adds it to the free list.
 * 
 * \param Object Pointer to the object being freed.
 */
void ObjectAllocator::MarkAsFreed(void *Object)
{
    // Mark memory block as freed
    std::memset(Object, FREED_PATTERN, Stats_.ObjectSize_);

    // Add the block to the free list
    GenericObject *object = reinterpret_cast<GenericObject *>(Object);
    object->Next = FreeList_;
    FreeList_ = object;

    Stats_.FreeObjects_++;
    Stats_.ObjectsInUse_--;
    Stats_.Deallocations_++;
}

/*!
 * \brief Deletes memory block using new/delete for a freed object.
 * 
 * \param Object Pointer to the object being freed.
 */
void ObjectAllocator::DeleteUsingCPP(void *Object)
{
    // Delete memory block using new/delete
    delete reinterpret_cast<char *>(Object);
    Stats_.ObjectsInUse_--;
    Stats_.Deallocations_++;
}

/*!
 * \brief Dumps memory in use and calls a callback function for each block.
 * 
 * \param fn Callback function for each block in use.
 * 
 * \return The number of blocks in use.
 */
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
    // Calls the callback fn for each block still in use
    GenericObject *currentPage = PageList_;
    unsigned int count = 0;

    while (currentPage)
    {
        for (unsigned int i = 0; i < Config_.ObjectsPerPage_; i++)
        {
            char *memory = GetMemoryAddressInPage(currentPage, i);

            if (!IsInFreeList(memory))
            {
                fn(memory, Stats_.ObjectSize_);
                count++;
            }
        }
        currentPage = currentPage->Next;
    }

    return count;
}

/*!
 * \brief Gets the memory address within the page for the given object index.
 * 
 * \param currentPage Pointer to the current page.
 * \param objectIndex Index of the object on the page.
 * 
 * \return The memory address within the page.
 */
char *ObjectAllocator::GetMemoryAddressInPage(GenericObject *currentPage, unsigned int objectIndex) const
{
    // Get the memory address within the page for the given object index
    return reinterpret_cast<char *>(currentPage) + sizeof(GenericObject *) + Config_.LeftAlignSize_ +
           Config_.HBlockInfo_.size_ + Config_.PadBytes_ +
           objectIndex * (Config_.HBlockInfo_.size_ + Config_.PadBytes_ +
                          Stats_.ObjectSize_ + Config_.PadBytes_ + Config_.InterAlignSize_);
}

/*!
 * \brief Checks if the given memory address is in the free list.
 * 
 * \param memory Pointer to the memory address.
 * 
 * \return True if the memory is in the free list, false otherwise.
 */
bool ObjectAllocator::IsInFreeList(char *memory) const
{
    // Check if the given memory address is in the free list
    GenericObject *freeObject = FreeList_;

    while (freeObject)
    {
        if (reinterpret_cast<char *>(freeObject) == memory)
        {
            return true;
        }
        freeObject = freeObject->Next;
    }

    return false;
}

/*!
 * \brief Validates pages and calls a callback function for each potentially corrupted block.
 * 
 * \param fn Callback function for each potentially corrupted block.
 * 
 * \return The number of potentially corrupted blocks.
 */
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
    // Calls the callback fn for each block that is potentially corrupted
    GenericObject *currentPage = PageList_;
    unsigned int count = 0;

    while (currentPage)
    {
        for (unsigned int i = 0; i < Config_.ObjectsPerPage_; i++)
        {
            char *memory = GetMemoryAddressInPage(currentPage, i);

            if (IsMemoryCorrupted(memory))
            {
                fn(memory, Stats_.ObjectSize_);
                count++;
            }
        }
        currentPage = currentPage->Next;
    }

    return count;
}

/*!
 * \brief Checks if the memory block is potentially corrupted.
 * 
 * \param memory Pointer to the memory block.
 * 
 * \return True if the memory block is potentially corrupted, false otherwise.
 */
bool ObjectAllocator::IsMemoryCorrupted(char *memory) const
{
    // Check if the memory block is potentially corrupted
    unsigned char *pre = reinterpret_cast<unsigned char *>(memory) - Config_.PadBytes_;
    unsigned char *post = reinterpret_cast<unsigned char *>(memory) + Stats_.ObjectSize_;

    for (unsigned int u = 0; u < Config_.PadBytes_; u++)
    {
        if (*(pre + u) != PAD_PATTERN || *(post + u) != PAD_PATTERN)
        {
            return true;
        }
    }

    return false;
}

/*!
 * \brief Removes a page from the free list.
 * 
 * \param pageAddr Pointer to the page to be removed.
 */
void ObjectAllocator::RemoveFromPageList(GenericObject* pageAddr)
{
    // Store head node 
    GenericObject* temp = this->FreeList_, * prev = nullptr;

    // If head node itself holds the key or multiple occurrences of key 
    while (temp != nullptr && IsInPage(pageAddr, reinterpret_cast<unsigned char*>(temp)))
    {
        this->FreeList_ = temp->Next;   // Changed head 
        temp = this->FreeList_;         // Change Temp 
        this->Stats_.FreeObjects_--;
    }

    // Delete occurrences other than head 
    while (temp != NULL)
    {
        // Search for the key to be deleted, keep track of the 
        // previous node as we need to change 'prev->next' 
        while (temp != nullptr && !IsInPage(pageAddr, reinterpret_cast<unsigned char*>(temp)))
        {
            prev = temp;
            temp = temp->Next;
        }

        // If key was not present in linked list 
        if (temp == nullptr) return;

        // Unlink the node from linked list 
        prev->Next = temp->Next;

        this->Stats_.FreeObjects_--;
        //Update Temp for next iteration of outer loop 
        temp = prev->Next;
    }
}

/*!
 * \brief Frees a page.
 * 
 * \param temp Pointer to the page to be freed.
 */
void ObjectAllocator::FreePage(GenericObject* temp)
{
    RemoveFromPageList(temp);
    delete[] reinterpret_cast<unsigned char*>(temp);
    this->Stats_.PagesInUse_--;
    
}

/*!
 * \brief Checks if the given address is within the page.
 * 
 * \param pageAddr Pointer to the page.
 * \param addr Pointer to the address.
 * 
 * \return True if the address is within the page, false otherwise.
 */
bool ObjectAllocator::IsInPage(GenericObject* pageAddr, unsigned char* addr) const
{
    return (addr >= reinterpret_cast<unsigned char*>(pageAddr) &&
        addr < reinterpret_cast<unsigned char*>(pageAddr) + Stats_.PageSize_);
}

/*!
 * \brief Checks if the page is empty.
 * 
 * \param page Pointer to the page.
 * 
 * \return True if the page is empty, false otherwise.
 */
bool ObjectAllocator::IsPageEmpty(GenericObject* page) const
{
    // Walk though the linked list.
    GenericObject* freeList = this->FreeList_;
    unsigned freeInPage = 0;
    while (freeList)
    {
        if (IsInPage(page, reinterpret_cast<unsigned char*>(freeList)))
        {
            if (++freeInPage >= Config_.ObjectsPerPage_)
                return true;
        }
        freeList = freeList->Next;
    }
    return false;
}

/*!
 * \brief Frees empty pages.
 * 
 * \return The number of freed empty pages.
 */
unsigned ObjectAllocator::FreeEmptyPages()
{
    if (this->PageList_ == nullptr)
        return 0;
    // Return value
    unsigned emptyPages = 0;
    
    // Store head node 
    GenericObject* temp = this->PageList_, * prev = nullptr;

    // If head node itself holds the key or multiple occurrences of key 
    while (temp != nullptr && IsPageEmpty(temp))
    {
        this->PageList_ = temp->Next;   // Changed head 
        FreePage(temp);
        temp = this->PageList_;         // Change Temp 
        emptyPages++;
    }

    // Delete occurrences other than head 
    while (temp != nullptr)
    {
        // Search for the key to be deleted, keep track of the 
        // previous node as we need to change 'prev->next' 
        while (temp != nullptr && !IsPageEmpty(temp))
        {
            prev = temp;
            temp = temp->Next;
        }

        // If key was not present in linked list 
        if (temp == nullptr) return emptyPages;

        // Unlink the node from linked list 
        prev->Next = temp->Next;
        FreePage(temp);

        //Update Temp for next iteration of outer loop 
        temp = prev->Next;
        emptyPages++;
    }
    return emptyPages;
}

/*!
 * \brief Sets the debug state of ObjectAllocator.
 * 
 * \param State The debug state to be set.
 */
void ObjectAllocator::SetDebugState(bool State)
{
    Config_.DebugOn_ = State;
}

/*!
 * \brief Gets the pointer to the free list.
 * 
 * \return The pointer to the free list.
 */
const void *ObjectAllocator::GetFreeList() const
{
    return FreeList_;
}

/*!
 * \brief Gets the pointer to the page list.
 * 
 * \return The pointer to the page list.
 */
const void *ObjectAllocator::GetPageList() const
{
    return PageList_;
}

/*!
 * \brief Gets the configuration of ObjectAllocator.
 * 
 * \return The configuration of ObjectAllocator.
 */
OAConfig ObjectAllocator::GetConfig() const
{
    return Config_;
}

/*!
 * \brief Gets the statistics of ObjectAllocator.
 * 
 * \return The statistics of ObjectAllocator.
 */
OAStats ObjectAllocator::GetStats() const
{
    return Stats_;
}