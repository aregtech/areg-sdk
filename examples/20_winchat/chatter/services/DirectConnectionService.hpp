#pragma once
/************************************************************************
 * \file            chatter/services/DirectConnectionService.hpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "examples/20_winchat/services/DirectConnectionStub.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/String.hpp"

class DirectConnectionService   : public areg::Component
                                , public DirectConnectionStub
{
//////////////////////////////////////////////////////////////////////////
// Statics
//////////////////////////////////////////////////////////////////////////
public:

    static DirectConnectionService * GetService();

    static areg::Model GetModel( const areg::String & nickName, uint32_t cookie, std::any data );

    static areg::String GetGeneratedService( const areg::String & nickName, uint32_t cookie );

    inline void SetOwnerData(const areg::String & nickName, uint32_t cookie );

public:
    DirectConnectionService( const areg::ComponentEntry & entry, areg::ComponentThread & ownerThread );
    virtual ~DirectConnectionService();

//////////////////////////////////////////////////////////////////////////
// DirectConnection Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          Request to setup the direct connection to chat room initiator.
     * \param   initiator           The initiator to setup connection
     * \param   listParticipants    The list of chat-room participants
     * \see     responseConnectoinSetup
     **/
    void requestConnectoinSetup( const DirectConnection::sInitiator & initiator, const DirectConnection::ListParticipants & listParticipants ) override;

    /**
     * \brief   Request call.
     *          The request to add a participant in the direct chat-room.
     * \param   initiator           The initiator to add to chat-room
     * \param   listParticipants    List of participants
     * \see     responseAddParticipant
     **/
    void requestAddParticipant( const DirectConnection::sInitiator & initiator, const DirectConnection::ListParticipants & listParticipants ) override;

    /**
     * \brief   Request call.
     *          Request to remove initiator from chat-room
     * \param   initiator           The initiator to remove from chat-room
     * \param   listParticipants    List of chat-room participants
     * \see     responseRemoveParticipant
     **/
    void requestRemoveParticipant( const DirectConnection::sInitiator & initiator, const DirectConnection::ListParticipants & listParticipants ) override;

    /**
     * \brief   Request call.
     *          Request to close chat-room. There is no response expected.
     * \param   initiator   The initiator to close chat-room.
     * \note    Has no response
     **/
    void requestCloseConnection( const DirectConnection::sInitiator & initiator ) override;

private:
    inline DirectConnectionService & self();

    inline bool isInitiatorValid( const DirectConnection::sInitiator & initiator ) const;

    inline bool exists( const DirectConnection::sInitiator & initiator ) const;

    inline uint64_t getSession( const DirectConnection::ListParticipants & listParticipants );

private:
    static DirectConnectionService *  mService;

    areg::String      mNickName;
    uint32_t    mCookie;
};

inline DirectConnectionService & DirectConnectionService::self()
{   return (*this);                             }

inline void DirectConnectionService::SetOwnerData( const areg::String & nickName, uint32_t cookie )
{   mNickName   = nickName; mCookie = cookie;   }
