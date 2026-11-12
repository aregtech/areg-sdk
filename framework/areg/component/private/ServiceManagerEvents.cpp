/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/ServiceManagerEvents.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Manager event for client.
 *
 ************************************************************************/
#include "areg/component/private/ServiceManagerEvents.hpp"

#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/private/ServiceManager.hpp"

namespace
{
    //!< Writes to a buffer and keeps whether every write was complete.
    class CheckedStream final : public areg::OutStream
    {
    public:
        explicit CheckedStream( areg::SharedBuffer & target ) noexcept
            : areg::OutStream   ( )
            , mTarget           ( target )
            , mComplete         ( true )
        {
        }

        ~CheckedStream( ) noexcept override = default;

        //!< Invalidates the buffer if one of the writes was not complete.
        void seal( ) noexcept
        {
            if ( mComplete == false )
            {
                mTarget.invalidate( );
            }
        }

        using areg::OutStream::write;

        uint32_t write( const uint8_t * buffer, uint32_t size ) override
        {
            const uint32_t result{ mTarget.write( buffer, size ) };
            mComplete = mComplete && ( result == size );
            return result;
        }

        uint32_t write( const areg::SharedBuffer & buffer ) override
        {
            const uint32_t used{ mTarget.size_used( ) };
            const uint32_t result{ mTarget.write( buffer ) };
            mComplete = mComplete && ( mTarget.size_used( ) - used == static_cast<uint32_t>(sizeof( uint32_t )) + buffer.size_used( ) );
            return result;
        }

        void flush( ) noexcept override
        {
            mTarget.flush( );
        }

        bool ensure_size( uint32_t addSize ) override
        {
            return mTarget.ensure_size( addSize );
        }

    protected:
        //!< Returns no view; every write goes through write().
        uint8_t * write_view( uint32_t /*size*/ ) noexcept override
        {
            return nullptr;
        }

    private:
        areg::SharedBuffer &    mTarget;    //!< The buffer that is written.
        bool                    mComplete;  //!< True while every write was complete.

        CheckedStream( ) = delete;
        AREG_NOCOPY_NOMOVE( CheckedStream );
    };
}

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventData class implementation
//////////////////////////////////////////////////////////////////////////

ServiceManagerEventData ServiceManagerEventData::shutdown_service_manager()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService );
}

ServiceManagerEventData ServiceManagerEventData::register_proxy(const ProxyAddress & addrProxy)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy );
    CheckedStream stream( data.mStream );
    stream << addrProxy << addrProxy.channel();
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregister_proxy( const ProxyAddress & addrProxy, areg::DisconnectReason reason )
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy );
    CheckedStream stream( data.mStream );
    stream << addrProxy << addrProxy.channel() << reason;
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::register_stub(const StubAddress & addrStub)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProvider );
    CheckedStream stream( data.mStream );
    stream << addrStub << addrStub.channel();
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregister_stub( const StubAddress & addrStub, areg::DisconnectReason reason )
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProvider );
    CheckedStream stream( data.mStream );
    stream << addrStub << addrStub.channel() << reason;
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::start_connection(areg::RemoteServiceKind service, uint32_t connectTypes)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection );
    CheckedStream stream( data.mStream );
    stream << service << connectTypes;
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::stop_connection()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection );
}

ServiceManagerEventData ServiceManagerEventData::register_connection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterConnection );
    CheckedStream stream( data.mStream );
    stream << channel;
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::unregister_connection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterConnection );
    CheckedStream stream( data.mStream );
    stream << channel;
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::lost_connection(const Channel & channel)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection );
    CheckedStream stream( data.mStream );
    stream << channel;
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::model_request(bool load, uint32_t modelId, uint64_t request)
{
    ServiceManagerEventData data( load ? ServiceManagerEventData::ServiceManagerCommand::CMD_LoadModel : ServiceManagerEventData::ServiceManagerCommand::CMD_UnloadModel );
    CheckedStream stream( data.mStream );
    stream << modelId << request;
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::thread_report(bool running, uint64_t instance)
{
    ServiceManagerEventData data( running ? ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadRunning : ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadStopped );
    CheckedStream stream( data.mStream );
    stream << instance;
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::watchdog_expired(uint64_t instance, bool isWorker)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_WatchdogExpired );
    CheckedStream stream( data.mStream );
    stream << instance << static_cast<uint32_t>(isWorker ? 1u : 0u);
    stream.seal( );
    return data;
}

ServiceManagerEventData ServiceManagerEventData::deadline_expired()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_DeadlineExpired );
}

ServiceManagerEventData ServiceManagerEventData::router_client_stopped()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_RouterClientStopped );
}

ServiceManagerEventData ServiceManagerEventData::reconcile()
{
    return ServiceManagerEventData( ServiceManagerEventData::ServiceManagerCommand::CMD_Reconcile );
}

ServiceManagerEventData ServiceManagerEventData::user_state(uint32_t userState)
{
    ServiceManagerEventData data( ServiceManagerEventData::ServiceManagerCommand::CMD_UserState );
    CheckedStream stream( data.mStream );
    stream << userState;
    stream.seal( );
    return data;
}

} // namespace areg
