/************************************************************************
 * \file            register/services/CEConnectionManager.hpp
 * \brief           The connection manager server component implementation
 ************************************************************************/
#pragma once

#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/base/NEMemory.hpp"
#include "shared/generated/CEConnectionManagerStub.hpp"
#include "shared/generated/CECentralMessagerStub.hpp"

class CEComponentThread;

//////////////////////////////////////////////////////////////////////////
// CEConnectionManager class implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Connection manager, server component implementation
 *          to register all client applications connections.
 **/
class CEConnectionManager   : private CEComponent
                            , public  CEConnectionManagerStub
                            , public  CECentralMessagerStub
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static CEComponent * CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner );

    static void DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry );

    static CEConnectionManager * GetService( void );

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
    virtual void RequestConnet( const CEString & nickName, const CEDateTime & dateTime );

    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \param   nickName        DESCRIPTION MISSED
     * \param   cookie          DESCRIPTION MISSED
     * \param   connectCookie   DESCRIPTION MISSED
     * \param   dateRegister    DESCRIPTION MISSED
     * \see     ResponseRegisterConnection
     **/
    virtual void RequestRegisterConnection( const CEString & nickName, const uint32_t & cookie, const uint32_t & connectCookie, const CEDateTime & dateRegister );

    /**
     * \brief   Request call.
     *          Disconnect result.
     * \param   nickName    The nick name of client to disconnect.
     * \param   cookie      The cookie given by connection manager. If invalid, it will search by nickName parameter
     * \param   dateTime    Disconnect request date-time
     * \see     Has no response
     **/
    virtual void RequestDiconnect( const CEString & nickName, const uint32_t & cookie, const CEDateTime & dateTime );

//////////////////////////////////////////////////////////////////////////
// CentralMessager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to sends message that all clients can see.
     * \param   nickName    The nick name of initiator
     * \param   cookie      Cookie given by connection manager. Should not be invalid.
     * \param   newMessage  DESCRIPTION MISSED
     * \param   dateTime    The timestamp when the message is sent
     * \see     Has no response
     **/
    virtual void RequestSendMessage( const CEString & nickName, const uint32_t & cookie, const CEString & newMessage, const CEDateTime & dateTime );

    /**
     * \brief   Request call.
     *          Sent each time when typing a key
     * \param   nickName    The nick name of sender
     * \param   cookie      Cookie given by connection manager.
     * \param   newMessage  The new message sent by client.
     * \see     Has no response
     **/
    virtual void RequestKeyTyping( const CEString & nickName, const uint32_t & cookie, const CEString & newMessage );

public:

    /**
     * \brief   Sets the HWMD value of window object to receive messages.
     **/
    inline void SetHwnd( size_t hWnd );

    /**
     * \brief   Search connection entry in the list. On output, if found, 
     *          the parameter 'connection' contains value of entry
     **/
    bool FindConnection( const CEString & nickName, NEConnectionManager::sConnection & connection );

    /**
     * \brief   Returns true if name is reserved by system.
     **/
    bool IsReservedNickname( const CEString & nickName ) const;

protected:
/************************************************************************/
// CEStubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void StartupServiceInterface( CEComponent & holder );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    CEConnectionManager( CEComponentThread & masterThread, const char * const roleName, NEMemory::uAlign data );
    virtual ~CEConnectionManager( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    static CEConnectionManager *    sService;
    uint64_t    mWnd;
    uint32_t    mCookies;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline CEConnectionManager & self( void );

    inline bool connectionExist( uint32_t cookie ) const;

    inline uint32_t getNextCookie( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    CEConnectionManager( void );
    CEConnectionManager( const CEConnectionManager & /*src*/ );
    const CEConnectionManager & operator = ( const CEConnectionManager & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEConnectionManager inline methods
//////////////////////////////////////////////////////////////////////////

inline CEConnectionManager & CEConnectionManager::self( void )
{   return (*this);     }

inline void CEConnectionManager::SetHwnd( size_t hWnd )
{   mWnd    = hWnd;     }
