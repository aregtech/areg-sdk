#ifndef AREG_BASE_IESYNCHOBJECT_HPP
#define AREG_BASE_IESYNCHOBJECT_HPP
/************************************************************************
 * \file        areg/src/base/IESynchObject.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Base Synchronization object interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

/**
 * \details The synchronization classes are used to synchronize
 *          data access from different threads as well as
 *          to send a trigger to notify a thread about
 *          particular event. This is base synchronization object interface.
 **/

/**
 * \brief   Base class of synchronization objects.
 *          Contains locking and unlocking function interface
 *          and has basic attributes for all synchronization objects.
 **/
//////////////////////////////////////////////////////////////////////////
// IESynchObject class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API IESynchObject
{
//////////////////////////////////////////////////////////////////////////
// Class typed and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Type of supported Synchronization objects
     **/
    typedef enum E_SyncObject
    {
          SO_UNKNOWN    = -1    //!< Unknown type of synchronization object
        , SO_MUTEX      =  0    //!< Synchronization object is a Mutex
        , SO_EVENT              //!< Synchronization object is an Event
        , SO_SEMAPHORE          //!< Synchronization object is a Semaphore
        , SO_TIMER              //!< Synchronization object is a Waiting Timer
        , SO_CRITICAL           //!< Synchronization object is a Critical Section
        , SO_RES_LOCK           //!< Simple synchronization object to lock resource access. Cannot be used to wait event.
        , SO_NOLOCK             //!< Empty / Dummy Synchronization object, no real locking operations are supported
    } eSyncObject;

    /**
     * \brief   IESynchObject::WAIT_INFINITE
     *          Infinitive timeout waiting time -1 / 0xFFFFFFFF
     *          until a certain waiting event did not happen
     **/
    static const unsigned int   WAIT_INFINITE       /* = 0xFFFFFFFF*/;

    /**
     * \brief   IESynchObject::DO_NOT_WAIT
     *          Do not wait for certain waiting event
     **/
    static const unsigned int   DO_NOT_WAIT         /* = 0x00000000 */;

    /**
     * \brief   IESynchObject::WAIT_1_MS
     *          Waiting time 1 millisecond, the minimum waiting time.
     **/
    static const unsigned int   WAIT_1_MS           /* = 1 */;

    /**
     * \brief   IESynchObject::WAIT_1_SEC
     *          Waiting time 1 second
     **/
    static const unsigned int   WAIT_1_SEC          /* = 1000 */;

    /**
     * \brief   IESynchObject::WAIT_1_MIN
     *          Waiting time 1 minute
     **/
    static const unsigned int   WAIT_1_MIN          /* = 60000 */;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Protected constructor, cannot be called directly
     *          Only from child classes, which has overwritten
     *          pure virtual functions
     * \param	synchObjectType	Type of synchronization object
     **/
    IESynchObject( IESynchObject::eSyncObject synchObjectType );

public:
    /**
     * \brief   Public destructor. If Synchronization object is valid,
     *          it will close it automatically.
     **/
    virtual ~IESynchObject( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts Synchronization object instance to void pointer
     **/
    operator void * (void);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the synchronization object
     *          Handle value.
     **/
    void * GetHandle( void ) const;

    /**
     * \brief   Returns type of synchronization object
     **/
    IESynchObject::eSyncObject GetSyncObjectType( void ) const;

    /**
     * \brief   Returns true if a synchronization object is valid.
     **/
    inline bool IsValid( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
// Pure virtual functions to overwrite
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Locks synchronization object and if succeeds, returns true.
     *          The thread is locked if it successfully owns synchronization object.
     *          For different Synchronization objects, this function call may
     *          perform different actions.
     * \param	timeout	    Timeout in milliseconds to wait if synchronization
     *                      object cannot be owned by thread. Otherwise function
     *                      returns immediately.
     *                      WAIT_INFINITE means endless timeout to wait to
     *                      own synchronization object
     * \return	If thread successfully owns object, returns true. Otherwise returns false.
     **/
    virtual bool Lock( unsigned int timeout = IESynchObject::WAIT_INFINITE );

    /**
     * \brief   Unlocks / Release current thread ownership of synchronization object
     * \return  Returns true if thread ownership is successfully released.
     **/
    virtual bool Unlock( void );

//////////////////////////////////////////////////////////////////////////
// Operations, private
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Destroys the synchronization object. Normally called in the destructor.
     **/
    void _destroySynchObject( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Synchronization object type
     **/
    const eSyncObject   mSynchObjectType;

    /**
     * \brief   Handle to synchronization object
     **/
    void *              mSynchObject;

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    IESynchObject( void );
    IESynchObject( const IESynchObject & /*src*/ );
    const IESynchObject & operator = ( const IESynchObject & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// Inline functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESynchObject class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool IESynchObject::IsValid( void ) const
{   return (mSynchObjectType != IESynchObject::SO_NOLOCK ? mSynchObject != static_cast<void *>(NULL) : true);   }

#endif  // AREG_BASE_IESYNCHOBJECT_HPP
