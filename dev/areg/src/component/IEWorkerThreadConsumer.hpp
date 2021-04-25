#ifndef AREG_COMPONENT_IEWORKERTHREADCONSUMER_HPP
#define AREG_COMPONENT_IEWORKERTHREADCONSUMER_HPP
/************************************************************************
 * \file        areg/src/component/IEWorkerThreadConsumer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Worker Thread Consumer.
 *              This object is required by every worker thread to
 *              trigger register / unregister methods when thread
 *              starts and stops.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

#include "areg/src/base/CEString.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponentThread;
class CEWorkerThread;

//////////////////////////////////////////////////////////////////////////
// IEWorkerThreadConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Worker Thread Consumer is required by Worker Thread when
 *              it starts and stops running. Pass object to worker thread
 *              to register and register event listeners and start receiving
 *              events from component thread.
 * 
 * \details     If an object is an instance of Worker Thread Consumer,
 *              it will get register / unregister calls to notify
 *              Thread Start / Stop. Set listeners in register method to
 *              be able to receive notification messages. Every consumer
 *              should have name. This is done because if a component has
 *              binded several worker threads, it should be able to
 *              differentiate and assign appropriate consumer depending
 *              on name. The consumer name is also part of 
 *              Worker Thread Entry object. For details, see NERegistry
 *              namespace.
 **/
class AREG_API IEWorkerThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates consumer object and sets name.
     * \param   consumerName    The name of consumer bind to worker thread.
     **/
    IEWorkerThreadConsumer( const char* const consumerName );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IEWorkerThreadConsumer( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Consumer name of Worker Thread.
     *          Is required if component contains few worker threads
     *          and needs few consumers for every single worker thread.
     **/
    inline const char * GetConsumerName( void ) const;

    /**
     * \brief   Compares passed name with the name of consumer
     *          and returns true if names are equal.
     * \param   consumerName    The name to check.
     * \return  Returns true if passed name is the name of consumer.
     **/
    inline bool IsEqualName( const char* consumerName ) const;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEWorkerThreadConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered by Worker Thread when starts running.
     *          Make initializations and add event consumers in this
     *          method to start receiving events.
     * \param   workThread      The Worker Thread object to notify startup
     * \param   masterThread    The component thread, which owns worker thread.
     **/
    virtual void RegisterEventConsumers( CEWorkerThread & workThread, CEComponentThread & masterThread ) = 0;

    /**
     * \brief   Triggered by Worker Thread when stops running.
     *          Make cleanups and remove event consumers in this
     *          method to stop receiving events.
     * \param   workThread  The Worker Thread object to notify stop
     **/
    virtual void UnregisterEventConsumers( CEWorkerThread & workThread ) = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of consumer. Is a fixed name and cannot be changed
     **/
    const CEString  mConsumerName;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IEWorkerThreadConsumer( void );
    IEWorkerThreadConsumer(const IEWorkerThreadConsumer & /*src*/ );
    const IEWorkerThreadConsumer& operator = (const IEWorkerThreadConsumer & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// IEWorkerThreadConsumer class inline function implementation
//////////////////////////////////////////////////////////////////////////
inline const char* IEWorkerThreadConsumer::GetConsumerName( void ) const
{   return static_cast<const char *>(mConsumerName);    }

inline bool IEWorkerThreadConsumer::IsEqualName( const char * consumerName ) const
{   return (consumerName != NULL ? mConsumerName == consumerName : false);  }

#endif  // AREG_COMPONENT_IEWORKERTHREADCONSUMER_HPP
