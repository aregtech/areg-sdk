/************************************************************************
 * \file            register/services/ConnectionManager.hpp
 * \brief           The connection manager server component implementation
 ************************************************************************/
#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "areg/base/NEMemory.hpp"
#include "generated/ConnectionManagerStub.hpp"
#include "generated/CentralMessagerStub.hpp"

class ComponentThread;

//////////////////////////////////////////////////////////////////////////
// ConnectionManager class implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Connection manager, server component implementation
 *          to register all client applications connections.
 **/
class ConnectionManager : private Component
                        , public  ConnectionManagerStub
                        , public  CentralMessagerStub
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

    static ConnectionManager * getService( void );

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          The request to connect to system
     * \param   nickName    The nick name of client
     * \param   dateTime    Client connection request date time in UTC
     * \see     ResponseConnect
     **/
    virtual void requestConnect( const String & nickName, const DateTime & dateTime );

    /**
     * \brief   Request call.
     *          Request to register direct connection service.
     * \param   nickName        The nick name of connection used in service
     * \param   cookie          The cookie value to set
     * \param   connectCookie   The cookie to use for registration. 
     * \param   dateRegister    The connection registration time-stamp
     * \see     responseRegisterConnection
     **/
    virtual void requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister );

    /**
     * \brief   Request call.
     *          Disconnect result.
     * \param   nickName    The nick name of client to disconnect.
     * \param   cookie      The cookie given by connection manager. If invalid, it will search by nickName parameter
     * \param   dateTime    Disconnect request date-time
     * \see     Has no response
     **/
    virtual void requestDisconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime );

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
    virtual void requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime );

    /**
     * \brief   Request call.
     *          Sent each time when typing a key
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie assigned to initiator
     * \param   newMessage  The message typed.
     * \note    Has no response
     **/
    virtual void requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage );

public:

    /**
     * \brief   Sets the HWMD value of window object to receive messages.
     **/
    inline void SetHwnd( size_t hWnd );

    /**
     * \brief   Search connection entry in the list. On output, if found, 
     *          the parameter 'connection' contains value of entry
     **/
    bool FindConnection( const String & nickName, NEConnectionManager::sConnection & connection );

    /**
     * \brief   Returns true if name is reserved by system.
     **/
    bool IsReservedNickname( const String & nickName ) const;

protected:
/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void startupServiceInterface( Component & holder );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    ConnectionManager( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread, NEMemory::uAlign data );
    virtual ~ConnectionManager( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    static ConnectionManager *    sService;
    uint64_t    mWnd;
    uint32_t    mCookies;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ConnectionManager & self( void );

    inline bool connectionExist( uint32_t cookie ) const;

    inline uint32_t getNextCookie( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ConnectionManager( void );
    ConnectionManager( const ConnectionManager & /*src*/ );
    const ConnectionManager & operator = ( const ConnectionManager & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionManager inline methods
//////////////////////////////////////////////////////////////////////////

inline ConnectionManager & ConnectionManager::self( void )
{   return (*this);     }

inline void ConnectionManager::SetHwnd( size_t hWnd )
{   mWnd    = hWnd;     }
