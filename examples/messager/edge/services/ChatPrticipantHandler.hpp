#pragma once
/************************************************************************
 * \file            edge/services/ChatPrticipantHandler.hpp
 * \brief           The messaging service chat participant handling
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/ESynchObjects.hpp"
#include "shared/NECommonSettings.hpp"

#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// Dependencies
//////////////////////////////////////////////////////////////////////////
class DirectChatService;
class DirectMessagingClient;

//////////////////////////////////////////////////////////////////////////
// ChatPrticipantHandler class declaration
//////////////////////////////////////////////////////////////////////////

class ChatPrticipantHandler
{
//////////////////////////////////////////////////////////////////////////
// constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ChatPrticipantHandler( const String & serviceName
                           , const NECommonSettings::sInitiator & initiator
                           , const NECommonSettings::ListParticipants & listPaticipants
                           , const NECommonSettings::sParticipant & ownerConnection);
    virtual ~ChatPrticipantHandler( void );

//////////////////////////////////////////////////////////////////////////
// attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    inline bool IsValid( void ) const;

    inline void SetConnectionData( const String & serviceName, const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & connectionList);

    inline void SetParticipantsList( const NECommonSettings::ListParticipants & listParaticipants );

    inline const String & GetServiceName( void ) const;

    inline const NECommonSettings::sInitiator & GetInitiator( void ) const;

    inline const NECommonSettings::ListParticipants & GetParticipantList( void ) const;
    
    inline void SetChatWindow( HWND hWnd );

    inline HWND GetChatWindow( void ) const;

    inline void SetConnectionService( DirectChatService * connectService );

    inline DirectChatService * GetConnectionService( void ) const;

    inline void SetChatClient( DirectMessagingClient * chatClient );

    inline DirectMessagingClient * GetChatClient( void ) const;

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
    String                                mServiceName;
    NECommonSettings::sParticipant         mOwnerConnection;
    NECommonSettings::sInitiator           mInitiator;
    NECommonSettings::ListParticipants     mListParticipants;
    DirectChatService *                   mConnectionService;
    DirectMessagingClient *               mChatClient;
    bool                                    mIsServicing;
    HWND                                    mWndChat;
    mutable Mutex                         mLock;

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ChatPrticipantHandler( void );
    ChatPrticipantHandler( const ChatPrticipantHandler & /*src*/ );
    const ChatPrticipantHandler & operator = ( const ChatPrticipantHandler & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ChatPrticipantHandler class inline methods
//////////////////////////////////////////////////////////////////////////

inline bool ChatPrticipantHandler::IsValid( void ) const
{
    return (mWndChat != NULL) && (mConnectionService != NULL);
}

inline void ChatPrticipantHandler::SetConnectionData( const String & serviceName, const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & listParticipants )
{
    Lock lock( mLock );
    mServiceName        = serviceName;
    mInitiator          = initiator;
    mListParticipants   = listParticipants;
}

inline const String & ChatPrticipantHandler::GetServiceName( void ) const
{
    Lock lock(mLock);
    return mServiceName;
}

void ChatPrticipantHandler::SetParticipantsList( const NECommonSettings::ListParticipants & listParaticipants )
{
    Lock lock( mLock );
    mListParticipants = listParaticipants;
}

inline const NECommonSettings::sInitiator & ChatPrticipantHandler::GetInitiator( void ) const
{
    Lock lock( mLock );
    return mInitiator;
}

inline const NECommonSettings::ListParticipants & ChatPrticipantHandler::GetParticipantList( void ) const
{
    Lock lock( mLock );
    return mListParticipants;
}

inline void ChatPrticipantHandler::SetChatWindow( HWND hWnd )
{
    Lock lock( mLock );
    mWndChat    = hWnd;
}

inline HWND ChatPrticipantHandler::GetChatWindow( void ) const
{
    Lock lock( mLock );
    return mWndChat;
}

inline void ChatPrticipantHandler::SetConnectionService( DirectChatService * connectService )
{
    Lock lock( mLock );
    mConnectionService  = connectService;
}

inline DirectChatService * ChatPrticipantHandler::GetConnectionService( void ) const
{
    Lock lock( mLock );
    return mConnectionService;
}

inline void ChatPrticipantHandler::SetConnectionOwner( const NECommonSettings::sParticipant & ownerConnection )
{
    Lock lock( mLock );
    mOwnerConnection    = ownerConnection;
}

inline const NECommonSettings::sParticipant & ChatPrticipantHandler::GetConnectionOwner( void ) const
{
    Lock lock( mLock );
    return mOwnerConnection;
}

inline void ChatPrticipantHandler::SetChatClient( DirectMessagingClient * chatClient )
{
    Lock lock( mLock );
    mChatClient = chatClient;
}

inline DirectMessagingClient * ChatPrticipantHandler::GetChatClient( void ) const
{
    Lock lock( mLock );
    return mChatClient;
}
