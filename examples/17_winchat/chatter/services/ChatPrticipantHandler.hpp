#pragma once
/************************************************************************
 * \file            chatter/services/ChatPrticipantHandler.hpp
 * \brief           The messaging service chat participant handling
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"
#include "generated/NECommon.hpp"

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
                         , const NECommon::sInitiator & initiator
                         , const NECommon::ListParticipants & listPaticipants
                         , const NECommon::sParticipant & ownerConnection);
    virtual ~ChatPrticipantHandler( void );

//////////////////////////////////////////////////////////////////////////
// attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    inline bool IsValid( void ) const;

    inline void SetConnectionData( const String & serviceName, const NECommon::sInitiator & initiator, const NECommon::ListParticipants & connectionList);

    inline void SetParticipantsList( const NECommon::ListParticipants & listParaticipants );

    inline const String & GetServiceName( void ) const;

    inline const NECommon::sInitiator & GetInitiator( void ) const;

    inline const NECommon::ListParticipants & GetParticipantList( void ) const;
    
    inline void SetChatWindow(ptr_type hWnd );

    inline ptr_type GetChatWindow( void ) const;

    inline void SetConnectionService( DirectChatService * connectService );

    inline DirectChatService * GetConnectionService( void ) const;

    inline void SetChatClient( DirectMessagingClient * chatClient );

    inline DirectMessagingClient * GetChatClient( void ) const;

    inline void SetConnectionOwner( const NECommon::sParticipant & ownerConnection );

    inline const NECommon::sParticipant & GetConnectionOwner( void ) const;

    void SetInitiator( const NECommon::sInitiator & initiator );

    int AddParticipant( const NECommon::sInitiator & initiator, const NECommon::ListParticipants & listParticipants );

    bool AddParticipant( const NECommon::sInitiator & initiator, const NECommon::sParticipant & participant );

    int RemoveParticipant( const NECommon::sInitiator & initiator, const NECommon::ListParticipants & listParticipants );

    bool RemoveParticipant( const NECommon::sInitiator & initiator, const NECommon::sParticipant & participant );

    bool ParticipantExist( const NECommon::sParticipant & participant ) const;

    bool IsEmpty( void ) const;

    void Invalidate( void );

//////////////////////////////////////////////////////////////////////////
// hidden  methods
//////////////////////////////////////////////////////////////////////////
private:

    int findPosition( const NECommon::sParticipant & participant ) const;

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
private:
    String                      mServiceName;
    NECommon::sParticipant      mOwnerConnection;
    NECommon::sInitiator        mInitiator;
    NECommon::ListParticipants  mListParticipants;
    DirectChatService *         mConnectionService;
    DirectMessagingClient *     mChatClient;
    bool                        mIsServicing;
    ptr_type                    mWndChat;
    mutable Mutex               mLock;

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
    return (mWndChat != 0) && (mConnectionService != nullptr);
}

inline void ChatPrticipantHandler::SetConnectionData( const String & serviceName, const NECommon::sInitiator & initiator, const NECommon::ListParticipants & listParticipants )
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

void ChatPrticipantHandler::SetParticipantsList( const NECommon::ListParticipants & listParaticipants )
{
    Lock lock( mLock );
    mListParticipants = listParaticipants;
}

inline const NECommon::sInitiator & ChatPrticipantHandler::GetInitiator( void ) const
{
    Lock lock( mLock );
    return mInitiator;
}

inline const NECommon::ListParticipants & ChatPrticipantHandler::GetParticipantList( void ) const
{
    Lock lock( mLock );
    return mListParticipants;
}

inline void ChatPrticipantHandler::SetChatWindow(ptr_type hWnd )
{
    Lock lock( mLock );
    mWndChat    = hWnd;
}

inline ptr_type ChatPrticipantHandler::GetChatWindow( void ) const
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

inline void ChatPrticipantHandler::SetConnectionOwner( const NECommon::sParticipant & ownerConnection )
{
    Lock lock( mLock );
    mOwnerConnection    = ownerConnection;
}

inline const NECommon::sParticipant & ChatPrticipantHandler::GetConnectionOwner( void ) const
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
