#ifndef AREG_BASE_THREADLOCALSTORAGE_HPP
#define AREG_BASE_THREADLOCALSTORAGE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/ThreadLocalStorage.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread Local Storage class.
 *              Should be initialized when thread starts running.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/LinkedList.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class Thread;

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Manages thread-local storage entries accessed by unique names; one instance per thread.
 **/
class AREG_API ThreadLocalStorage
{
/************************************************************************/
// Friend classes
/************************************************************************/
    friend Thread;

/************************************************************************/
// Internal class declaration
/************************************************************************/
private:
    //////////////////////////////////////////////////////////////////////////
    // ThreadLocalStorage::StorageItem class declaration
    //////////////////////////////////////////////////////////////////////////

    //!< Definition of storage item to store.
    using StorageItem       = std::pair<String, areg::Primitive>;
    //!< Definition of storage list object to store items.
    using StorageList       = LinkedList<ThreadLocalStorage::StorageItem>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the local storage associated with the given thread.
     *
     * \param   owningThread    The thread that owns this local storage.
     **/
    explicit ThreadLocalStorage( Thread & owningThread );
    ~ThreadLocalStorage();

//////////////////////////////////////////////////////////////////////////
// Attributes / Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the name of the local storage (same as the owning thread's name).
     **/
    const String & name() const;

    /**
     * \brief   Returns true if a local storage entry with the given name exists.
     *
     * \param   Key     The name of the local storage entry.
     * \return  Returns true if the entry exists; false otherwise.
     **/
    bool exist(const String & Key) const;

    /**
     * \brief   Returns the owning thread object.
     **/
    [[nodiscard]]
    inline Thread & owner_thread() const noexcept;

    /**
     * \brief   Returns the number of entries in the local storage.
     **/
    [[nodiscard]]
    inline uint32_t size() const noexcept;

    /**
     * \brief   Returns the value of the local storage entry with the given name.
     *
     * \param   Key     The name of the local storage entry.
     * \return  Returns the entry value if found; returns areg::InvalidElement if not found.
     **/
    [[nodiscard]]
    areg::Primitive item( const String & Key ) const;

    /**
     * \brief   Saves a primitive value in the local storage with the given name.
     *
     * \param   Key         The name of the local storage entry.
     * \param   Value       The primitive value to save.
     **/
    void set_item(const String & Key, areg::Primitive Value);

    /**
     * \brief   Saves a pointer value in the local storage with the given name.
     *
     * \param   Key         The name of the local storage entry.
     * \param   Value       The pointer value to save.
     **/
    void set_item(const String & Key, const void * Value);

    /**
     * \brief   Saves a 32-bit unsigned integer value in the local storage with the given name.
     *
     * \param   Key         The name of the local storage entry.
     * \param   Value       The 32-bit unsigned integer value to save.
     **/
    void set_item(const String & Key, uint32_t Value);

    /**
     * \brief   Saves a 64-bit unsigned integer value in the local storage with the given name.
     *
     * \param   Key         The name of the local storage entry.
     * \param   Value       The 64-bit unsigned integer value to save.
     **/
    void set_item(const String & Key, uint64_t Value);

    /**
     * \brief   Saves a floating-point value in the local storage with the given name.
     *
     * \param   Key         The name of the local storage entry.
     * \param   Value       The floating-point value to save.
     **/
    void set_item(const String & Key, double Value);

    /**
     * \brief   Removes the first local storage entry with the given name.
     *
     * \param   Key     The name of the local storage entry to remove.
     * \return  Returns the value of the removed entry; returns a dummy zero value if the entry does
     *          not exist.
     **/
    areg::Primitive remove_item(const String & Key);

    /**
     * \brief   Removes all entries from the local storage.
     **/
    inline void clear();

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    [[nodiscard]]
    inline int32_t _find_item(const String& Key) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   The storage list object, which contains (key; value) pair entries
     **/
    StorageList mStorageList;

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief   The thread object, which is a holder of thread storage.
     **/
    Thread &    mOwningThread;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    ThreadLocalStorage() = delete;
    AREG_NOCOPY_NOMOVE( ThreadLocalStorage );
};

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline Thread & ThreadLocalStorage::owner_thread() const noexcept
{
    return mOwningThread;
}

inline uint32_t ThreadLocalStorage::size() const noexcept
{
    return mStorageList.size();
}

inline void ThreadLocalStorage::clear()
{
    mStorageList.clear();
}

inline int32_t ThreadLocalStorage::_find_item(const String& Key) const noexcept
{
    for (uint32_t i = 0; i < mStorageList.size(); ++i)
    {
        if (mStorageList[i].first == Key)
            return static_cast<int32_t>(i);
    }

    return areg::INVALID_INDEX;
}

} // namespace areg
#endif  // AREG_BASE_THREADLOCALSTORAGE_HPP
