#ifndef AREG_BASE_CHUNKQUEUE_HPP
#define AREG_BASE_CHUNKQUEUE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/ChunkQueue.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, a FIFO queue of fixed blocks that never throws: a push fails
 *              instead, and an element never moves while it is in the queue.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <new>
#include <type_traits>
#include <utility>

namespace areg {

/**
 * \brief   A not synchronized FIFO queue that keeps its elements in blocks of BLOCK_SIZE
 *          elements. A push allocates at most one block with a non-throwing allocation and
 *          returns false if it fails. An element never moves while it is in the queue, so a
 *          reference to it stays valid until it is popped. One emptied block is kept for the
 *          next push, the other emptied blocks are freed.
 *
 *          Elements are copied or moved in, and their copy, move and destruction must not
 *          throw.
 *
 * \tparam  VALUE       The type of the elements.
 * \tparam  BLOCK_SIZE  The number of elements of one block, at least 1.
 **/
template <typename VALUE, uint32_t BLOCK_SIZE = 16u>
class ChunkQueue
{
    static_assert(BLOCK_SIZE != 0u, "a block holds at least one element");
    static_assert(std::is_nothrow_copy_constructible<VALUE>::value, "the copy of an element must not throw");
    static_assert(std::is_nothrow_move_constructible<VALUE>::value, "the move of an element must not throw");
    static_assert(std::is_nothrow_destructible<VALUE>::value, "the destruction of an element must not throw");

//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    //!< One block of elements.
    struct Block
    {
        Block * next{ nullptr };                                    //!< The next block, or nullptr.
        alignas(VALUE) unsigned char bytes[sizeof(VALUE) * BLOCK_SIZE]; //!< The space of the elements.

        //!< Returns the element at the index of the block.
        inline VALUE * at(uint32_t index) noexcept
        {
            return std::launder(reinterpret_cast<VALUE *>(bytes) + index);
        }
    };

    //!< Iterates the elements from the first to the last.
    template <typename ELEM>
    class IteratorBase
    {
    public:
        inline IteratorBase(Block * block, uint32_t index) noexcept
            : mBlock(block)
            , mIndex(index)
        {
        }

        inline ELEM & operator * () const noexcept
        {
            return *mBlock->at(mIndex);
        }

        inline ELEM * operator -> () const noexcept
        {
            return mBlock->at(mIndex);
        }

        inline IteratorBase & operator ++ () noexcept
        {
            if (++mIndex == BLOCK_SIZE)
            {
                mBlock = mBlock->next;
                mIndex = 0u;
            }

            return *this;
        }

        inline bool operator == (const IteratorBase & other) const noexcept
        {
            return (mBlock == other.mBlock) && (mIndex == other.mIndex);
        }

        inline bool operator != (const IteratorBase & other) const noexcept
        {
            return (mBlock != other.mBlock) || (mIndex != other.mIndex);
        }

    private:
        Block *     mBlock; //!< The block of the element, nullptr at the end.
        uint32_t    mIndex; //!< The index of the element in the block.
    };

public:
    //!< Iterates the elements, modifiable.
    using Iterator      = IteratorBase<VALUE>;
    //!< Iterates the elements, read only.
    using ConstIterator = IteratorBase<const VALUE>;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty queue. Allocates nothing.
     **/
    ChunkQueue() noexcept = default;

    /**
     * \brief   Takes the elements and the blocks of the source, which is empty after the call.
     **/
    inline ChunkQueue(ChunkQueue && source) noexcept;

    /**
     * \brief   Destroys the elements and frees the blocks.
     **/
    inline ~ChunkQueue();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Destroys the own elements and takes the elements and the blocks of the source,
     *          which is empty after the call.
     **/
    inline ChunkQueue & operator = (ChunkQueue && source) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Appends a copy of the element at the end.
     * \return  False if no block could be allocated; the queue is unchanged.
     **/
    [[nodiscard]]
    inline bool push_back(const VALUE & value) noexcept;

    /**
     * \brief   Moves the element to the end.
     * \return  False if no block could be allocated; the queue and the element are unchanged.
     **/
    [[nodiscard]]
    inline bool push_back(VALUE && value) noexcept;

    /**
     * \brief   Returns the first element. The queue must not be empty.
     **/
    [[nodiscard]]
    inline VALUE & front() noexcept;

    /**
     * \brief   Returns the first element. The queue must not be empty.
     **/
    [[nodiscard]]
    inline const VALUE & front() const noexcept;

    /**
     * \brief   Destroys the first element. The queue must not be empty.
     **/
    inline void pop_front() noexcept;

    /**
     * \brief   Returns true if the queue has no element.
     **/
    [[nodiscard]]
    inline bool empty() const noexcept;

    /**
     * \brief   Returns the number of elements.
     **/
    [[nodiscard]]
    inline uint32_t size() const noexcept;

    /**
     * \brief   Destroys every element. Keeps one block for the next push.
     **/
    inline void clear() noexcept;

    /**
     * \brief   Returns the position of the first element.
     **/
    [[nodiscard]]
    inline Iterator begin() noexcept;

    /**
     * \brief   Returns the position after the last element.
     **/
    [[nodiscard]]
    inline Iterator end() noexcept;

    /**
     * \brief   Returns the position of the first element.
     **/
    [[nodiscard]]
    inline ConstIterator begin() const noexcept;

    /**
     * \brief   Returns the position after the last element.
     **/
    [[nodiscard]]
    inline ConstIterator end() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    //!< Returns the slot of a new last element, or nullptr if no block could be allocated.
    inline VALUE * _append_slot() noexcept;

    //!< Destroys the elements and frees every block.
    inline void _free() noexcept;

    //!< Forgets the blocks without freeing them.
    inline void _forget() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The block of the first element, or nullptr.
    Block *     mHead   { nullptr };
    //!< The block of the last element, or nullptr.
    Block *     mTail   { nullptr };
    //!< An emptied block kept for the next push, or nullptr.
    Block *     mSpare  { nullptr };
    //!< The index of the first element in its block.
    uint32_t    mHeadPos{ 0u };
    //!< The index after the last element in its block.
    uint32_t    mTailPos{ 0u };
    //!< The number of elements.
    uint32_t    mCount  { 0u };

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ChunkQueue(const ChunkQueue &) = delete;
    ChunkQueue & operator = (const ChunkQueue &) = delete;
};

//////////////////////////////////////////////////////////////////////////
// ChunkQueue class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, uint32_t BLOCK_SIZE>
inline ChunkQueue<VALUE, BLOCK_SIZE>::ChunkQueue(ChunkQueue && source) noexcept
    : mHead     (source.mHead)
    , mTail     (source.mTail)
    , mSpare    (source.mSpare)
    , mHeadPos  (source.mHeadPos)
    , mTailPos  (source.mTailPos)
    , mCount    (source.mCount)
{
    source._forget();
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline ChunkQueue<VALUE, BLOCK_SIZE>::~ChunkQueue()
{
    _free();
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline ChunkQueue<VALUE, BLOCK_SIZE> & ChunkQueue<VALUE, BLOCK_SIZE>::operator = (ChunkQueue && source) noexcept
{
    if (this != &source)
    {
        _free();
        mHead   = source.mHead;
        mTail   = source.mTail;
        mSpare  = source.mSpare;
        mHeadPos= source.mHeadPos;
        mTailPos= source.mTailPos;
        mCount  = source.mCount;
        source._forget();
    }

    return *this;
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline bool ChunkQueue<VALUE, BLOCK_SIZE>::push_back(const VALUE & value) noexcept
{
    VALUE * const slot{ _append_slot() };
    if (slot == nullptr)
        return false;

    ::new (static_cast<void *>(slot)) VALUE(value);
    ++mTailPos;
    ++mCount;
    return true;
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline bool ChunkQueue<VALUE, BLOCK_SIZE>::push_back(VALUE && value) noexcept
{
    VALUE * const slot{ _append_slot() };
    if (slot == nullptr)
        return false;

    ::new (static_cast<void *>(slot)) VALUE(std::move(value));
    ++mTailPos;
    ++mCount;
    return true;
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline VALUE & ChunkQueue<VALUE, BLOCK_SIZE>::front() noexcept
{
    return *mHead->at(mHeadPos);
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline const VALUE & ChunkQueue<VALUE, BLOCK_SIZE>::front() const noexcept
{
    return *mHead->at(mHeadPos);
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline void ChunkQueue<VALUE, BLOCK_SIZE>::pop_front() noexcept
{
    mHead->at(mHeadPos)->~VALUE();
    --mCount;
    if ((++mHeadPos == BLOCK_SIZE) || (mCount == 0u))
    {
        Block * const done{ mHead };
        mHead   = (mCount == 0u ? nullptr : done->next);
        mHeadPos= 0u;
        if (mHead == nullptr)
        {
            mTail   = nullptr;
            mTailPos= 0u;
        }

        if (mSpare == nullptr)
        {
            mSpare = done;
        }
        else
        {
            delete done;
        }
    }
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline bool ChunkQueue<VALUE, BLOCK_SIZE>::empty() const noexcept
{
    return (mCount == 0u);
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline uint32_t ChunkQueue<VALUE, BLOCK_SIZE>::size() const noexcept
{
    return mCount;
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline void ChunkQueue<VALUE, BLOCK_SIZE>::clear() noexcept
{
    while (mCount != 0u)
    {
        pop_front();
    }
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline typename ChunkQueue<VALUE, BLOCK_SIZE>::Iterator ChunkQueue<VALUE, BLOCK_SIZE>::begin() noexcept
{
    return Iterator(mHead, mHeadPos);
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline typename ChunkQueue<VALUE, BLOCK_SIZE>::Iterator ChunkQueue<VALUE, BLOCK_SIZE>::end() noexcept
{
    return ((mTail == nullptr) || (mTailPos == BLOCK_SIZE) ? Iterator(nullptr, 0u) : Iterator(mTail, mTailPos));
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline typename ChunkQueue<VALUE, BLOCK_SIZE>::ConstIterator ChunkQueue<VALUE, BLOCK_SIZE>::begin() const noexcept
{
    return ConstIterator(mHead, mHeadPos);
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline typename ChunkQueue<VALUE, BLOCK_SIZE>::ConstIterator ChunkQueue<VALUE, BLOCK_SIZE>::end() const noexcept
{
    return ((mTail == nullptr) || (mTailPos == BLOCK_SIZE) ? ConstIterator(nullptr, 0u) : ConstIterator(mTail, mTailPos));
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline VALUE * ChunkQueue<VALUE, BLOCK_SIZE>::_append_slot() noexcept
{
    if ((mTail == nullptr) || (mTailPos == BLOCK_SIZE))
    {
        Block * block{ mSpare };
        if (block == nullptr)
        {
            block = new (std::nothrow) Block;
            if (block == nullptr)
                return nullptr;
        }

        mSpare = nullptr;
        block->next = nullptr;
        if (mTail == nullptr)
        {
            mHead   = block;
            mHeadPos= 0u;
        }
        else
        {
            mTail->next = block;
        }

        mTail   = block;
        mTailPos= 0u;
    }

    return mTail->at(mTailPos);
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline void ChunkQueue<VALUE, BLOCK_SIZE>::_free() noexcept
{
    clear();
    delete mSpare;
    mSpare = nullptr;
}

template <typename VALUE, uint32_t BLOCK_SIZE>
inline void ChunkQueue<VALUE, BLOCK_SIZE>::_forget() noexcept
{
    mHead   = nullptr;
    mTail   = nullptr;
    mSpare  = nullptr;
    mHeadPos= 0u;
    mTailPos= 0u;
    mCount  = 0u;
}

} // namespace areg

#endif  // AREG_BASE_CHUNKQUEUE_HPP
