#pragma once
/************************************************************************
 * \file            chatter/services/ConnectionService.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/EventTemplate.hpp"

#include <functional>
#include <utility>

namespace areg { class SyncEvent; }

/**
 * \brief   A task to run in the thread of the connection service, and the event
 *          to signal when the task completes.
 **/
class ServiceTask
{
public:
    ServiceTask( ) = default;
    ServiceTask( std::function<void()> task, areg::SyncEvent * done );
    ServiceTask( const ServiceTask & src ) = default;
    ServiceTask & operator = ( const ServiceTask & src ) = default;

    std::function<void()>   mTask;
    areg::SyncEvent *       mDone{ nullptr };
};

AREG_DECLARE_EVENT( ServiceTask, ServiceTaskEvent, ServiceTaskConsumer );

class ConnectionService  final  : public  areg::Component
                                , private ServiceTaskConsumer
{
public:
    ConnectionService(const areg::ComponentEntry& entry, areg::ComponentThread& ownerThread);

    /**
     * \brief   Runs the task in the thread of the connection service and waits until it completes.
     *          Call it from another thread, never from the service thread.
     * \param   task    The task to run.
     * \return  Returns true if the task ran, false if the service is not running.
     **/
    static bool RunInServiceThread( const std::function<void()> & task );

    /**
     * \brief   Creates a client of the service in the thread of the connection service.
     * \param   args    The arguments of the client constructor.
     * \return  Returns the created client, or nullptr if the service is not running.
     **/
    template<class Client, typename ... Args>
    static Client * CreateClient( Args && ... args );

    /**
     * \brief   Deletes the client in the thread of the connection service and resets the pointer.
     * \param   client  The client to delete. Can be nullptr.
     **/
    template<class Client>
    static void DeleteClient( Client * & client );

protected:

/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    void startup_component( areg::ComponentThread & comThread ) override;

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    void shutdown_component( areg::ComponentThread & comThread ) noexcept override;

/************************************************************************/
// ServiceTaskConsumer overrides
/************************************************************************/
    /**
     * \brief   Runs the task in the service thread and signals the caller.
     * \param   data    The task to run and the event to signal.
     **/
    void process_event( const ServiceTask & data ) final;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ConnectionService & self();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ConnectionService();
    ConnectionService( const ConnectionService & /*src*/ );
    const ConnectionService & operator = ( const ConnectionService & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionService inline methods
//////////////////////////////////////////////////////////////////////////

inline ConnectionService & ConnectionService::self()
{   return (*this);         }

template<class Client, typename ... Args>
inline Client * ConnectionService::CreateClient( Args && ... args )
{
    Client * client{ nullptr };
    ConnectionService::RunInServiceThread( [&]( ) { client = new Client( std::forward<Args>( args )... ); } );
    return client;
}

template<class Client>
inline void ConnectionService::DeleteClient( Client * & client )
{
    if ( client != nullptr )
    {
        Client * obj{ client };
        client = nullptr;
        ConnectionService::RunInServiceThread( [obj]( ) { delete obj; } );
    }
}
