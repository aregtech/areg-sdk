#pragma once
/************************************************************************
 * \file            edge/services/CEDirectChatService.hpp
 * \brief           The direct chat service with direct connection clients
 ************************************************************************/

#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/CEComponent.hpp"
#include "shared/generated/CEDirectMessagerStub.hpp"
#include "edge/services/CEDirectMessagingClient.hpp"

#include "areg/src/component/NERegistry.hpp"
#include "areg/src/base/TEResourceMap.hpp"
#include "shared/generated/NEDirectConnection.hpp"
#include "shared/NECommonSettings.hpp"
#include "edge/NEDistributedApp.hpp"

class CEDirectConnectionClient;
class CEChatPrticipantHandler;

class CEDirectChatService   : public CEComponent
                            , public CEDirectMessagerStub
                          
{
private:
    typedef TEHashMap<CEString, CEDirectChatService *, const CEString &, CEDirectChatService *>     HashMapDirectConnections;
    typedef TELockResourceMap<CEString, CEDirectChatService *, HashMapDirectConnections>            MapDirectConnections;
    typedef TEArrayList<CEDirectConnectionClient *>                                                 ListDirectConnectionClients;


//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    static CEComponent * CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner );

    static void DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry );

    static NERegistry::CEModel GetModel( const NEDirectMessager::sParticipant & initiator, const NEDirectMessager::ListParticipants & listParticipants, const NEMemory::uAlign data );

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to join chat. The participant should be in the list of connections
     * \param   participant The participant to join chat. The participant should be in the connection list.
     * \param   timeConnect The timestamp when the request was sent.
     * \see     ResponseChatJoin
     **/
    virtual void RequestChatJoin( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeConnect );

    /**
     * \brief   Request call.
     *          Request to send a message
     * \param   sender      DESCRIPTION MISSED
     * \param   msgText     Message text
     * \param   timeSent    The connection session
     * \see     Has no response
     **/
    virtual void RequestMessageSend( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent );

    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \param   participant DESCRIPTION MISSED
     * \param   msgText     DESCRIPTION MISSED
     * \see     Has no response
     **/
    virtual void RequestMessageType( const NEDirectMessager::sParticipant & participant, const CEString & msgText );

    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \param   participant DESCRIPTION MISSED
     * \param   timeLeave   DESCRIPTION MISSED
     * \see     Has no response
     **/
    virtual void RequestChatLeave( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeave );

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
    CEDirectChatService( CEComponentThread & masterThread, const NERegistry::CEComponentEntry & entry, CEChatPrticipantHandler & handlerParticipant );
    virtual ~CEDirectChatService( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    static MapDirectConnections _mapDirectConnections;

    CEChatPrticipantHandler &   mPaticipantsHandler;
    ListDirectConnectionClients mListClients;
    CEDirectMessagingClient     mChatParticipant;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline CEDirectChatService & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    CEDirectChatService( void );
    CEDirectChatService( const CEDirectChatService & /*src*/ );
    const CEDirectChatService & operator = ( const CEDirectChatService & /*src*/ );
};

inline CEDirectChatService & CEDirectChatService::self( void )
{   return (*this); }
