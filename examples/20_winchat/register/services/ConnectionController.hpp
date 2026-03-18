/************************************************************************
 * \file            register/services/ConnectionController.hpp
 * \brief           The connection manager server component implementation
 ************************************************************************/
#pragma once

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "examples/20_winchat/services/ConnectionManagerProviderBase.hpp"
#include "examples/20_winchat/services/CentralMessagerProviderBase.hpp"

namespace areg { class ComponentThread; }

//////////////////////////////////////////////////////////////////////////
// ConnectionController class implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Connection manager, server component implementation
 *          to register all client applications connections.
 **/
class ConnectionController  : public    areg::Component
                            , public    ConnectionManagerProviderBase
                            , public    CentralMessagerProviderBase
{
public:
    ConnectionController( const areg::ComponentEntry & entry, areg::ComponentThread & ownerThread );
    virtual ~ConnectionController();

//////////////////////////////////////////////////////////////////////////
// ConnectionController Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          The request to connect to system
     * \param   nickName    The nick name of client
     * \param   dateTime    Client connection request date time in UTC
     * \see     ResponseConnect
     **/
    void requestConnect( const areg::String & nickName, const areg::DateTime & dateTime ) override;

    /**
     * \brief   Request call.
     *          Request to register direct connection service.
     * \param   nickName        The nick name of connection used in service
     * \param   cookie          The cookie value to set
     * \param   connectCookie   The cookie to use for registration. 
     * \param   dateRegister    The connection registration time-stamp
     * \see     responseRegisterConnection
     **/
    void requestRegisterConnection( const areg::String & nickName, uint32_t cookie, uint32_t connectCookie, const areg::DateTime & dateRegister ) override;

    /**
     * \brief   Request call.
     *          Disconnect result.
     * \param   nickName    The nick name of client to disconnect.
     * \param   cookie      The cookie given by connection manager. If invalid, it will search by nickName parameter
     * \param   dateTime    Disconnect request date-time
     * \see     Has no response
     **/
    void requestDisconnect( const areg::String & nickName, uint32_t cookie, const areg::DateTime & dateTime ) override;

//////////////////////////////////////////////////////////////////////////
// CentralMessager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to sends message that all clients can see.
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie given by connection manager. Should not be invalid.
     * \param   newMessage  The message sent by connected initiator
     * \param   dateTime    The time-stamp create on local host of initiator
     * \note    Has no response
     **/
    void requestSendMessage( const areg::String & nickName, uint32_t cookie, const areg::String & newMessage, const areg::DateTime & dateTime ) override;

    /**
     * \brief   Request call.
     *          Sent each time when typing a key
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie assigned to initiator
     * \param   newMessage  The message typed.
     * \note    Has no response
     **/
    void requestKeyTyping( const areg::String & nickName, uint32_t cookie, const areg::String & newMessage ) override;

public:

    /**
     * \brief   Sets the HWMD value of window object to receive messages.
     **/
    inline void SetHwnd( HWND hWnd );

    /**
     * \brief   Search connection entry in the list. On output, if found, 
     *          the parameter 'connection' contains value of entry
     **/
    bool FindConnection( const areg::String & nickName, ConnectionManager::ConnectionRecord & connection );

    /**
     * \brief   Returns true if name is reserved by system.
     **/
    bool IsReservedNickname( const areg::String & nickName ) const;

    static ConnectionController* getConnectionService();

protected:
/************************************************************************/
// ProviderBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Provider,
     *                  which started up.
     **/
    void startupServiceInterface( areg::Component & holder ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    HWND        mWnd;
    uint32_t    mCookies;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ConnectionController & self();

    inline bool connectionExist( uint32_t cookie ) const;

    inline uint32_t getNextCookie();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ConnectionController();
    ConnectionController( const ConnectionController & /*src*/ );
    const ConnectionController & operator = ( const ConnectionController & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionController inline methods
//////////////////////////////////////////////////////////////////////////

inline ConnectionController & ConnectionController::self()
{   return (*this);     }

inline void ConnectionController::SetHwnd( HWND hWnd )
{   mWnd    = hWnd;     }
