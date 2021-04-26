#pragma once
/************************************************************************
 * \file            edge/services/CEDirectConnectionService.hpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEComponent.hpp"
#include "shared/generated/CEDirectConnectionStub.hpp"

#include "areg/src/base/CEDateTime.hpp"
#include "areg/src/base/CEString.hpp"
#include <windows.h>

class CEDirectConnectionService : public CEComponent
                                , public CEDirectConnectionStub
{

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    static CEComponent * CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner );

    static void DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry );

    static CEDirectConnectionService * GetService( void );

    static NERegistry::CEModel GetModel( const CEString & nickName, uint32_t cookie, const NEMemory::uAlign data );

    static CEString GetGeneratedService( const CEString & nickName, uint32_t cookie );

    inline void SetOwnerData(const CEString & nickName, uint32_t cookie );

private:
    CEDirectConnectionService( CEComponentThread & masterThread, const char * const roleName, NEMemory::uAlign data );
    virtual ~CEDirectConnectionService( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// DirectConnection Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \param   initiator           DESCRIPTION MISSED
     * \param   listParticipants    DESCRIPTION MISSED
     * \see     ResponseConnectoinSetup
     **/
    virtual void RequestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );

    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \param   initiator           DESCRIPTION MISSED
     * \param   listParticipants    DESCRIPTION MISSED
     * \see     ResponseAddParticipant
     **/
    virtual void RequestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );

    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \param   initiator           DESCRIPTION MISSED
     * \param   listParticipants    DESCRIPTION MISSED
     * \see     ResponseRemoveParticipant
     **/
    virtual void RequestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );

    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \param   initiator   DESCRIPTION MISSED
     * \see     Has no response
     **/
    virtual void RequestCloseConnection( const NEDirectConnection::sInitiator & initiator );

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

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void ShutdownServiceIntrface ( CEComponent & holder );

    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client      The address of proxy client, which connection status is changed.
     * \param   isConnected Flag, indicating whether client is connected or disconnected.
     *                      When client disconnects, all listeners are removed.
     **/
    virtual void ClientConnected( const CEProxyAddress & client, bool isConnected );

private:
    inline CEDirectConnectionService & self( void );

    inline bool isInitiatorValid( const NEDirectConnection::sInitiator & initiator ) const;

    inline bool exists( const NEDirectConnection::sInitiator & initiator ) const;

    inline uint64_t getSession( const NEDirectConnection::ListParticipants & listParticipants );

private:
    static CEDirectConnectionService *  mService;

    CEString        mNickName;
    uint32_t        mCookie;
};

inline CEDirectConnectionService & CEDirectConnectionService::self( void )
{   return (*this);                             }

inline void CEDirectConnectionService::SetOwnerData( const CEString & nickName, uint32_t cookie )
{   mNickName   = nickName; mCookie = cookie;   }
