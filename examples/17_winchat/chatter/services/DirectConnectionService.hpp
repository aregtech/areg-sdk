#pragma once
/************************************************************************
 * \file            chatter/services/DirectConnectionService.hpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/DirectConnectionStub.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/String.hpp"

class DirectConnectionService   : public Component
                                , public DirectConnectionStub
{

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

    static DirectConnectionService * GetService( void );

    static NERegistry::Model GetModel( const String & nickName, uint32_t cookie, const NEMemory::uAlign data );

    static String GetGeneratedService( const String & nickName, uint32_t cookie );

    inline void SetOwnerData(const String & nickName, uint32_t cookie );

private:
    DirectConnectionService( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread, NEMemory::uAlign data );
    virtual ~DirectConnectionService( void );

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
     *          Request to setup the direct connection to chat room initiator.
     * \param   initiator           The initiator to setup connection
     * \param   listParticipants    The list of chat-room participants
     * \see     responseConnectoinSetup
     **/
    virtual void requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants ) override;

    /**
     * \brief   Request call.
     *          The request to add a participant in the direct chat-room.
     * \param   initiator           The initiator to add to chat-room
     * \param   listParticipants    List of participants
     * \see     responseAddParticipant
     **/
    virtual void requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants ) override;

    /**
     * \brief   Request call.
     *          Request to remove initiator from chat-room
     * \param   initiator           The initiator to remove from chat-room
     * \param   listParticipants    List of chat-room participants
     * \see     responseRemoveParticipant
     **/
    virtual void requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants ) override;

    /**
     * \brief   Request call.
     *          Request to close chat-room. There is no response expected.
     * \param   initiator   The initiator to close chat-room.
     * \note    Has no response
     **/
    virtual void requestCloseConnection( const NEDirectConnection::sInitiator & initiator ) override;

private:
    inline DirectConnectionService & self( void );

    inline bool isInitiatorValid( const NEDirectConnection::sInitiator & initiator ) const;

    inline bool exists( const NEDirectConnection::sInitiator & initiator ) const;

    inline uint64_t getSession( const NEDirectConnection::ListParticipants & listParticipants );

private:
    static DirectConnectionService *  mService;

    String      mNickName;
    uint32_t    mCookie;
};

inline DirectConnectionService & DirectConnectionService::self( void )
{   return (*this);                             }

inline void DirectConnectionService::SetOwnerData( const String & nickName, uint32_t cookie )
{   mNickName   = nickName; mCookie = cookie;   }
