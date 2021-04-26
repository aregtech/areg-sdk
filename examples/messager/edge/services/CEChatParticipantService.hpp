#pragma once
/************************************************************************
 * \file            edge/services/CEChatParticipantService.hpp
 * \brief           The chat participant service
 ************************************************************************/

#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/component/NERegistry.hpp"
#include "edge/services/CEDirectMessagingClient.hpp"
#include "shared/NECommonSettings.hpp"

class CEComponentThread;

class CEChatParticipantService  : private   CEComponent
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static CEComponent * CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner );

    static void DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry );

    static NERegistry::CEModel GetModel( const NEDirectMessager::sParticipant & initiator, const NEDirectMessager::ListParticipants & listParticipants, NEMemory::uAlign data );

protected:
/************************************************************************/
// CEComponent overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    virtual void StartupComponent( CEComponentThread & comThread );

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void ShutdownComponent( CEComponentThread & comThread );

private:
    CEChatParticipantService( CEComponentThread & masterThread, const NERegistry::CEComponentEntry & entry, CEChatPrticipantHandler & handlerParticipants );
    virtual ~CEChatParticipantService( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectMessagingClient mChatParticipant;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline CEChatParticipantService & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    CEChatParticipantService( void );
    CEChatParticipantService( const CEChatParticipantService & /*src*/ );
    const CEChatParticipantService & operator = (const CEChatParticipantService & /*src*/);
};

//////////////////////////////////////////////////////////////////////////
// CEChatParticipantService inline methods
//////////////////////////////////////////////////////////////////////////

inline CEChatParticipantService & CEChatParticipantService::self( void )
{
    return (*this);
}
