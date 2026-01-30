#ifndef AREG_BASE_IESYNCOBJECT_HPP
#define AREG_BASE_IESYNCOBJECT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/IESyncObject.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Base Synchronization object interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NECommon.hpp"

//////////////////////////////////////////////////////////////////////////
// IESyncObject class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class of synchronization objects. Contains locking and unlocking 
 *          functions  and has basic attributes for all synchronization objects.
 **/
class AREG_API IESyncObject
{
//////////////////////////////////////////////////////////////////////////
// Class typed and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Type of supported Synchronization objects
     **/
    enum class eSyncObject  : short
    {
          SoUnknown     = -1    //!< Unknown type of synchronization object.
        , SoMutex       =  0    //!< Synchronization object is a Mutex.
        , SoEvent               //!< Synchronization object is an Event.
        , SoSemaphore           //!< Synchronization object is a Semaphore.
        , SoCritical            //!< Synchronization object is a Critical Section
        , SoSpinlock            //!< Synchronization object is a Spin-Lock.
        , SoReslock             //!< A synchronization object to use to access resources.
        , SoNolock              //!< Empty / Dummy Synchronization object, no real locking operations are supported
        , SoTimer               //!< Synchronization object is a Waiting Timer.
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Protected constructor, initialize synchronization object.
     *          Cannot be called directly. Only child classes that have 
     *          overwritten pure virtual functions.
     * \param	synchObjectType	Type of synchronization object
     **/
    explicit IESyncObject( IESyncObject::eSyncObject syncObjectType );

public:
    /**
     * \brief   Public destructor. The system synchronization objects are automatically closed
     *          and resources are freed when destructor is closed.
     **/
    virtual ~IESyncObject( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts Synchronization object instance to void pointer
     **/
    inline operator SYNCHANDLE (void);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the synchronization object
     *          Handle value.
     **/
    inline SYNCHANDLE getHandle( void ) const;

    /**
     * \brief   Returns type of synchronization object
     **/
    inline IESyncObject::eSyncObject getObjectType( void ) const;

    /**
     * \brief   Returns true if a synchronization object is valid.
     **/
    inline bool isValid( void ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Locks synchronization object and if succeeds, returns true.
     *          The thread is locked if it successfully owns synchronization object.
     *          For different Synchronization objects, this function call may
     *          perform different actions.
     * \param	timeout	    The timeout in milliseconds to wait if synchronization
     *                      object cannot be owned by thread. Otherwise function
     *                      returns immediately.
     *                      WAIT_INFINITE means endless timeout to wait to
     *                      own synchronization object
     * \return	If thread successfully owns object, returns true. Otherwise returns false.
     **/
    virtual bool lock( unsigned int timeout = NECommon::WAIT_INFINITE );

    /**
     * \brief   Unlocks / Release current thread ownership of synchronization object
     * \return  Returns true if thread ownership is successfully released.
     **/
    virtual bool unlock( void );

//////////////////////////////////////////////////////////////////////////
// Operations, private
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Destroys the synchronization object. Normally called in the destructor.
     **/
    void _osDestroySyncObject( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Handle to synchronization object
     **/
    SYNCHANDLE                       mSyncObject;

    /**
     * \brief   Synchronization object type
     **/
    const IESyncObject::eSyncObject mSyncObjectType;

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    IESyncObject( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( IESyncObject );
};

//////////////////////////////////////////////////////////////////////////
// Inline functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESyncObject class inline functions
//////////////////////////////////////////////////////////////////////////
inline IESyncObject::operator SYNCHANDLE ( void )
{
    return mSyncObject;
}

inline SYNCHANDLE IESyncObject::getHandle( void ) const
{
    return mSyncObject;
}

inline IESyncObject::eSyncObject IESyncObject::getObjectType( void ) const
{
    return mSyncObjectType;
}

inline bool IESyncObject::isValid( void ) const
{
    return (mSyncObjectType == IESyncObject::eSyncObject::SoNolock) || (mSyncObject != nullptr);
}

#endif  // AREG_BASE_IESYNCOBJECT_HPP
