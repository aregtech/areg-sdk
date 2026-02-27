#pragma once
/************************************************************************
 * \file            chatter/services/ChatParticipantService.hpp
 * \brief           The chat participant service
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "areg/component/Model.hpp"
#include "chatter/services/DirectMessagingClient.hpp"
#include "common/ChatDefs.hpp"

namespace areg { class ComponentThread; }

class ChatParticipantService  : private   areg::Component
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static areg::Model GetModel( const DirectMessager::Participant & initiator, const DirectMessager::ListParticipants & listParticipants, std::any data );

public:
    ChatParticipantService(const areg::ComponentEntry& entry, areg::ComponentThread& ownerThread);

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
    void startupComponent( areg::ComponentThread & comThread ) override;

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    void shutdownComponent( areg::ComponentThread & comThread ) override;

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
