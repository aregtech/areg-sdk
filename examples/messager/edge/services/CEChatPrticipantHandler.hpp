#pragma once
/************************************************************************
 * \file            edge/services/CEChatPrticipantHandler.hpp
 * \brief           The messaging service chat participant handling
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/ESynchObjects.hpp"
#include "shared/NECommonSettings.hpp"

#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// Dependencies
//////////////////////////////////////////////////////////////////////////
class CEDirectChatService;
class CEDirectMessagingClient;

//////////////////////////////////////////////////////////////////////////
// CEChatPrticipantHandler class declaration
//////////////////////////////////////////////////////////////////////////

class CEChatPrticipantHandler
{
//////////////////////////////////////////////////////////////////////////
// constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    CEChatPrticipantHandler( const CEString & serviceName
                           , const NECommonSettings::sInitiator & initiator
                           , const NECommonSettings::ListParticipants & listPaticipants
                           , const NECommonSettings::sParticipant & ownerConnection);
    virtual ~CEChatPrticipantHandler( void );

//////////////////////////////////////////////////////////////////////////
// attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    inline bool IsValid( void ) const;

    inline void SetConnectionData( const CEString & serviceName, const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & connectionList);

    inline void SetParticipantsList( const NECommonSettings::ListParticipants & listParaticipants );

    inline const CEString & GetServiceName( void ) const;

    inline const NECommonSettings::sInitiator & GetInitiator( void ) const;

    inline const NECommonSettings::ListParticipants & GetParticipantList( void ) const;
    
    inline void SetChatWindow( HWND hWnd );

    inline HWND GetChatWindow( void ) const;

    inline void SetConnectionService( CEDirectChatService * connectService );

    inline CEDirectChatService * GetConnectionService( void ) const;

    inline void SetChatClient( CEDirectMessagingClient * chatClient );

    inline CEDirectMessagingClient * GetChatClient( void ) const;

    inline void SetConnectionOwner( const NECommonSettings::sParticipant & ownerConnection );

    inline const NECommonSettings::sParticipant & GetConnectionOwner( void ) const;

    void SetInitiator( const NECommonSettings::sInitiator & initiator );

    int AddParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & listParticipants );

    bool AddParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::sParticipant & participant );

    int RemoveParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & listParticipants );

    bool RemoveParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::sParticipant & participant );

    bool ParticipantExist( const NECommonSettings::sParticipant & participant ) const;

    bool IsEmpty( void ) const;

    void Invalidate( void );

//////////////////////////////////////////////////////////////////////////
// hidden  methods
//////////////////////////////////////////////////////////////////////////
private:

    int findPosition( const NECommonSettings::sParticipant & participant ) const;

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
private:
    CEString                                mServiceName;
    NECommonSettings::sParticipant         mOwnerConnection;
    NECommonSettings::sInitiator           mInitiator;
    NECommonSettings::ListParticipants     mListParticipants;
    CEDirectChatService *                   mConnectionService;
    CEDirectMessagingClient *               mChatClient;
    bool                                    mIsServicing;
    HWND                                    mWndChat;
    mutable CEMutex                         mLock;

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEChatPrticipantHandler( void );
    CEChatPrticipantHandler( const CEChatPrticipantHandler & /*src*/ );
    const CEChatPrticipantHandler & operator = ( const CEChatPrticipantHandler & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEChatPrticipantHandler class inline methods
//////////////////////////////////////////////////////////////////////////

inline bool CEChatPrticipantHandler::IsValid( void ) const
{
    return (mWndChat != NULL) && (mConnectionService != NULL);
}

inline void CEChatPrticipantHandler::SetConnectionData( const CEString & serviceName, const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & listParticipants )
{
    CELock lock( mLock );
    mServiceName        = serviceName;
    mInitiator          = initiator;
    mListParticipants   = listParticipants;
}

inline const CEString & CEChatPrticipantHandler::GetServiceName( void ) const
{
    CELock lock(mLock);
    return mServiceName;
}

void CEChatPrticipantHandler::SetParticipantsList( const NECommonSettings::ListParticipants & listParaticipants )
{
    CELock lock( mLock );
    mListParticipants = listParaticipants;
}

inline const NECommonSettings::sInitiator & CEChatPrticipantHandler::GetInitiator( void ) const
{
    CELock lock( mLock );
    return mInitiator;
}

inline const NECommonSettings::ListParticipants & CEChatPrticipantHandler::GetParticipantList( void ) const
{
    CELock lock( mLock );
    return mListParticipants;
}

inline void CEChatPrticipantHandler::SetChatWindow( HWND hWnd )
{
    CELock lock( mLock );
    mWndChat    = hWnd;
}

inline HWND CEChatPrticipantHandler::GetChatWindow( void ) const
{
    CELock lock( mLock );
    return mWndChat;
}

inline void CEChatPrticipantHandler::SetConnectionService( CEDirectChatService * connectService )
{
    CELock lock( mLock );
    mConnectionService  = connectService;
}

inline CEDirectChatService * CEChatPrticipantHandler::GetConnectionService( void ) const
{
    CELock lock( mLock );
    return mConnectionService;
}

inline void CEChatPrticipantHandler::SetConnectionOwner( const NECommonSettings::sParticipant & ownerConnection )
{
    CELock lock( mLock );
    mOwnerConnection    = ownerConnection;
}

inline const NECommonSettings::sParticipant & CEChatPrticipantHandler::GetConnectionOwner( void ) const
{
    CELock lock( mLock );
    return mOwnerConnection;
}

inline void CEChatPrticipantHandler::SetChatClient( CEDirectMessagingClient * chatClient )
{
    CELock lock( mLock );
    mChatClient = chatClient;
}

inline CEDirectMessagingClient * CEChatPrticipantHandler::GetChatClient( void ) const
{
    CELock lock( mLock );
    return mChatClient;
}
