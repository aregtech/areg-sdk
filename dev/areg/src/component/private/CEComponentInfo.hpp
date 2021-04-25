#ifndef AREG_COMPONENT_PRIVATE_CECOMPONENTINFO_HPP
#define AREG_COMPONENT_PRIVATE_CECOMPONENTINFO_HPP
/************************************************************************
 * \file        areg/src/component/private/CEComponentInfo.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Info class declaration.
 *              Contains information of threads binded with component.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

#include "areg/src/base/TEHashMap.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/component/CEComponentAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponentThread;
class CEWorkerThread;
class CEDispatcherThread;

/**
 * \brief   Component Info is an object containing information of
 *          main thread of Component and its worker threads.
 *          The Component Info object gives possibility to send and
 *          receive messages from Worker Threads and provides
 *          communication between Worker Threads binded to the same
 *          Component. The information of threads is saved in Hash Map.
 **/
//////////////////////////////////////////////////////////////////////////
// CEComponentInfo class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEComponentInfo
{
//////////////////////////////////////////////////////////////////////////
// Internal class and types
//////////////////////////////////////////////////////////////////////////
private:
    class AREG_API _CEWorkerThreadMapImpl   : public TEHashMapImpl<const CEThreadAddress &, CEWorkerThread *>
    {
    public:
        /**
         * \brief   Called to calculate the 32-bit hash key value.
         * \ param  Key     The object to calculate 32-bit hash key.
         * \return  Returns 32-bit hash key value.
         **/
        inline unsigned int ImplHashKey(const CEThreadAddress & Key) const
        {
            return static_cast<unsigned int>(Key);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // CEComponentInfo::_CEWorkerThreadMap type declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The Hash Map object to save information of threads
     *          saved in specified Component Info object.
     **/
    typedef TEHashMap<CEThreadAddress, CEWorkerThread*, const CEThreadAddress &, CEWorkerThread*, _CEWorkerThreadMapImpl>   _CEWorkerThreadMap;

    /**
     * \brief   Resource mapping object type. 
     *          As a Key, used thread address (Worker Thread address)
     *          As a value, it saves pointers of Worker Thread object
     *          As a Hash Map used CEComponentInfo::_CEWorkerThreadMap object
     **/
    typedef TELockResourceMap<CEThreadAddress, CEWorkerThread, _CEWorkerThreadMap, TEResourceMapImpl<CEThreadAddress, CEWorkerThread>>     MapWorkerThread;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initialization constructor.
     *          Creates Component Info object by passing the Master Thread
     *          of Component (Component Thread) and the role name of Component.
     * \param   masterThread    The Master Thread of Component (Component Thread)
     * \param   roleName        The Role Name of component, must not be empty
     **/
    CEComponentInfo( CEComponentThread & masterThread, const char * const roleName );

    /**
     * \brief   Destructor
     **/
    ~CEComponentInfo( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns Component Thread object
     **/
    inline CEComponentThread & GetMasterThread( void ) const;

    /**
     * \brief   Returns the address of Component
     **/
    inline const CEComponentAddress & GetAddress( void ) const;

    /**
     * \brief   Returns the Role Name of Component
     **/
    inline const CEString & GetName( void ) const;

    /**
     * \brief   Returns true, if specified thread address is the address of registered Worker Thread.
     * \param   threadAddress   The thread address to check.
     * \return  Returns true, if specified thread address is the address of registered Worker Thread.
     **/
    inline bool IsWorkerThreadAddress( const CEThreadAddress & threadAddress) const;

    /**
     * \brief   Returns true, if Component Info has at least one registered Worker Thread.
     **/
    inline bool HasWorkerThreads( void ) const;

    /**
     * \brief   Returns true if specified Worker Thread is registered in Component Info
     * \param   workerThread    The Worker Thread object to check.
     * \return  Returns true if specified Worker Thread is registered in Component Info.
     **/
    bool IsWorkerThreadRegistered( CEWorkerThread & workerThread ) const;

    /**
     * \brief   Returns true, if specified thread address is the address of Component Thread.
     * \param   threadAddress   The thread address to check.
     * \return  Returns true, if specified thread address is the address of Component Thread.
     **/
    bool IsMasterThreadAddress( const CEThreadAddress & threadAddress ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Registers specified Worker Thread object in Component Info
     *          and returns true, if registration succeeded.
     * \param   workerThread    The Worker Thread to register in Component Info
     **/
    bool RegisterWorkerThread( CEWorkerThread & workerThread );

    /**
     * \brief   Unregisters specified Worker Thread object in Component Info
     *          and returns true, if operation succeeded. The operation may fail,
     *          if thread previously was not registered in Component Info.
     **/
    bool UnregisterWorkerThread( CEWorkerThread & workerThread );

    /**
     * \brief   Looks up for Worker Thread by specified unique Thread address.
     *          If there is registered thread with specified address,
     *          it returns pointer of valid Worker Thread object.
     *          Otherwise, it returns NULL.
     * \param   threadAddress   The unique Worker Thread address to search.
     * \return  Returns pointer of valid Worker Thread if there is registered
     *          Worker Thread with specified unique address. Otherwise, returns NULL.
     **/
    inline CEWorkerThread * FindWorkerThread( const CEThreadAddress & threadAddress ) const;

    /**
     * \brief   Looks up for Worker Thread by specified unique Thread name.
     *          If there is registered thread with specified name,
     *          it returns pointer of valid Worker Thread object.
     *          Otherwise, it returns NULL.
     * \param   threadName  The unique Worker Thread name to search.
     * \return  Returns pointer of valid Worker Thread if there is registered
     *          Worker Thread with specified unique name. Otherwise, returns NULL.
     **/
    inline CEWorkerThread * FindWorkerThread( const char* threadName ) const;

    /**
     * \brief   Looks up for Worker Thread by specified unique Component Path,
     *          which is containing Thread address. The Component Thread address
     *          is extracted from, then search is provided by extracted thread address.
     *          If there is registered thread with extracted address,
     *          it returns pointer of valid Worker Thread object.
     *          Otherwise, it returns NULL.
     * \param   componentPath   The unique path of Component, containing Worker Thread address.
     * \return  Returns pointer of valid Worker Thread if there is registered
     *          Worker Thread with extracted unique address. Otherwise, returns NULL.
     **/
    inline CEWorkerThread * FindThreadByPath( const char* componentPath ) const;

    /**
     * \brief   By specified Event Consumer runtime class ID object, looks up for registered
     *          Event Consumer in the Component Thread and in all Worker Threads.
     *          Returns pointer of valid Dispatcher Thread, where the Event Consumer is registered.
     *          Returns NULL if could not find Event Consumer.
     * \param   whichClass  The runtime class ID of Event Consumer object
     * \return  Returns valid pointer of Dispatcher Thread, where the Event Consumer is registered.
     *          If there is no such Event Consumer registered in Component Thread and in Worker Threads,
     *          it returns NULL.
     **/
    CEDispatcherThread * FindEventConsumer( const CERuntimeClassID & whichClass ) const;

    /**
     * \brief   Returns pointer of first Worker Thread. On output, out_threadAddress
     *          parameter will contain address of first valid thread, or invalid address
     *          CEThreadAddress::INVALID_THREAD_ADDRESS, if Component Info
     *          has no registered Worker Thread, and the function returns NULL.
     * \param   out_threadAddress   On output, if Component Info has registered
     *                              Worker Thread, it will contain valid address.
     *                              Otherwise, it will contain invalid address
     *                              CEThreadAddress::INVALID_THREAD_ADDRESS
     * \return  If Component has Worker Threads, it will return valid pointer and the
     *          Worker Thread object will have address specified in out_threadAddress.
     *          If Component has no Worker Thread, it will return NULL.
     **/
    inline CEWorkerThread * GetFirstWorkerThread( CEThreadAddress & out_threadAddress );

    /**
     * \brief   Returns pointer of next Worker Thread after registered thread specified in
     *          int_out_threadAddress. On input, int_out_threadAddress should be address
     *          of valid thread. On output, if there is next thread, it will contain
     *          address of valid Worker Thread. Otherwise, on output int_out_threadAddress
     *          is invalid CEThreadAddress::INVALID_THREAD_ADDRESS address.
     * \param   int_out_threadAddress   On input, this should be valid address of Worker Thread.
     *                                  On output, if there is next thread, it will be valid address
     *                                  of next Worker Thread. Otherwise, it will be invalid address
     *                                  CEThreadAddress::INVALID_THREAD_ADDRESS
     * \return  If there is next worker thread, the function will return valid pointer of
     *          next Worker Thread object and the address of thread is equal to address
     *          in int_out_threadAddress parameter on output. Otherwise, it returns NULL.
     **/
    inline CEWorkerThread * GetNextWorkerThread( CEThreadAddress & int_out_threadAddress );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The address of Component associated with Component Info object
     **/
    CEComponentAddress  mComponentAddress;

    /**
     * \brief   The Master Thread of Component (Component Thread)
     **/
    CEComponentThread & mMasterThread;

#if _MSC_VER
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The map of Worker Threads binded with Component
     **/
    MapWorkerThread     mWorkerThreadMap;
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    CEComponentInfo( void );
    CEComponentInfo( const CEComponentInfo & );
    const CEComponentInfo & operator = ( const CEComponentInfo & );
};

//////////////////////////////////////////////////////////////////////////
// CEComponentInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEComponentThread& CEComponentInfo::GetMasterThread( void ) const
{   return mMasterThread;       }

inline const CEComponentAddress& CEComponentInfo::GetAddress( void ) const
{   return mComponentAddress;   }

inline const CEString& CEComponentInfo::GetName( void ) const
{   return mComponentAddress.GetComponentName();    }

inline bool CEComponentInfo::IsWorkerThreadAddress( const CEThreadAddress& threadAddress ) const
{   return mWorkerThreadMap.ResourceKeyExist(threadAddress);              }

inline CEWorkerThread* CEComponentInfo::FindWorkerThread( const CEThreadAddress& threadAddress ) const
{   return mWorkerThreadMap.FindResourceObject(threadAddress);    }

inline CEWorkerThread* CEComponentInfo::FindWorkerThread( const char* threadName ) const
{
    CEThread* targetThread = CEThread::FindThreadByName(threadName);
    return (targetThread != NULL ? FindWorkerThread(targetThread->GetThreadAddress()) : static_cast<CEWorkerThread *>(NULL));
}

inline CEWorkerThread* CEComponentInfo::FindThreadByPath( const char* componentPath ) const
{
    CEComponentAddress componentAddress = CEComponentAddress::ConvertPathToAddress(componentPath);
    return FindWorkerThread(componentAddress.GetThreadAddress());
}

inline CEWorkerThread* CEComponentInfo::GetFirstWorkerThread( CEThreadAddress & out_threadAddress )
{   return mWorkerThreadMap.GetResourceFirstKey(out_threadAddress);     }

inline CEWorkerThread* CEComponentInfo::GetNextWorkerThread( CEThreadAddress & int_out_threadAddress )
{   return mWorkerThreadMap.GetResourceNextKey(int_out_threadAddress);  }

inline bool CEComponentInfo::HasWorkerThreads( void ) const
{   return (mWorkerThreadMap.IsEmpty() == false);                       }

#endif  // AREG_COMPONENT_PRIVATE_CECOMPONENTINFO_HPP
