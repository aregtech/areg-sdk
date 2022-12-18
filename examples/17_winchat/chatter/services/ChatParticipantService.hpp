#pragma once
/************************************************************************
 * \file            chatter/services/ChatParticipantService.hpp
 * \brief           The chat participant service
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "areg/component/NERegistry.hpp"
#include "chatter/services/DirectMessagingClient.hpp"
#include "generated/NECommon.hpp"

class ComponentThread;

class ChatParticipantService  : private   Component
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

    static NERegistry::Model GetModel( const NEDirectMessager::sParticipant & initiator, const NEDirectMessager::ListParticipants & listParticipants, NEMemory::uAlign data );

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
    virtual void startupComponent( ComponentThread & comThread );

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void shutdownComponent( ComponentThread & comThread );

private:
    ChatParticipantService( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread, ChatPrticipantHandler & handlerParticipants );
    virtual ~ChatParticipantService( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    DirectMessagingClient mChatParticipant;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ChatParticipantService & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ChatParticipantService( void );
    ChatParticipantService( const ChatParticipantService & /*src*/ );
    const ChatParticipantService & operator = (const ChatParticipantService & /*src*/);
};

//////////////////////////////////////////////////////////////////////////
// ChatParticipantService inline methods
//////////////////////////////////////////////////////////////////////////

inline ChatParticipantService & ChatParticipantService::self( void )
{
    return (*this);
}
