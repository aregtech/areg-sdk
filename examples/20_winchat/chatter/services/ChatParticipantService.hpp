#pragma once
/************************************************************************
 * \file            chatter/services/ChatParticipantService.hpp
 * \brief           The chat participant service
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/Model.hpp"
#include "chatter/services/DirectMessagingClient.hpp"
#include "common/ChatDefs.hpp"

namespace areg { class ComponentThread; }

class ChatParticipantService final  : private   areg::Component
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static areg::Model GetModel( const DirectMessager::Participant & initiator, const DirectMessager::ListParticipants & listParticipants, std::any data );

public:
    ChatParticipantService(const areg::ComponentEntry& entry, areg::ComponentThread& ownerThread);

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    DirectMessagingClient mChatParticipant;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ChatParticipantService & self();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ChatParticipantService();
    ChatParticipantService( const ChatParticipantService & /*src*/ );
    const ChatParticipantService & operator = (const ChatParticipantService & /*src*/);
};

//////////////////////////////////////////////////////////////////////////
// ChatParticipantService inline methods
//////////////////////////////////////////////////////////////////////////

inline ChatParticipantService & ChatParticipantService::self()
{
    return (*this);
}
