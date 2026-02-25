#pragma once
/************************************************************************
 * \file            chatter/services/ChatPrticipantHandler.hpp
 * \brief           The messaging service chat participant handling
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "common/ChatDefs.hpp"

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
    ChatPrticipantHandler( const areg::String & serviceName
                         , const chat::sInitiator & initiator
                         , const chat::ListParticipants & listPaticipants
                         , const chat::Participant & ownerConnection);
    virtual ~ChatPrticipantHandler();

//////////////////////////////////////////////////////////////////////////
// attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    inline bool IsValid() const;

    inline void SetConnectionData( const areg::String & serviceName, const chat::sInitiator & initiator, const chat::ListParticipants & connectionList);

    inline void SetParticipantsList( const chat::ListParticipants & listParaticipants );

    inline const areg::String & GetServiceName() const;

    inline const chat::sInitiator & GetInitiator() const;

    inline const chat::ListParticipants & GetParticipantList() const;
    
    inline void SetChatWindow(ptr_type hWnd );

    inline ptr_type GetChatWindow() const;

    inline void SetConnectionService( DirectChatService * connectService );

    inline DirectChatService * GetConnectionService() const;

    inline void SetChatClient( DirectMessagingClient * chatClient );

    inline DirectMessagingClient * GetChatClient() const;

    inline void SetConnectionOwner( const chat::Participant & ownerConnection );

    inline const chat::Participant & GetConnectionOwner() const;

    void SetInitiator( const chat::sInitiator & initiator );

    int32_t AddParticipant( const chat::sInitiator & initiator, const chat::ListParticipants & listParticipants );

    bool AddParticipant( const chat::sInitiator & initiator, const chat::Participant & participant );

    int32_t RemoveParticipant( const chat::sInitiator & initiator, const chat::ListParticipants & listParticipants );

    bool RemoveParticipant( const chat::sInitiator & initiator, const chat::Participant & participant );

    bool ParticipantExist( const chat::Participant & participant ) const;

    bool IsEmpty() const;

    void Invalidate();

//////////////////////////////////////////////////////////////////////////
// hidden  methods
//////////////////////////////////////////////////////////////////////////
private:

    int32_t findPosition( const chat::Participant & participant ) const;

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
private:
    areg::String                      mServiceName;
    chat::Participant      mOwnerConnection;
    chat::sInitiator        mInitiator;
    chat::ListParticipants  mListParticipants;
    DirectChatService *         mConnectionService;
    DirectMessagingClient *     mChatClient;
    bool                        mIsServicing;
    ptr_type                    mWndChat;
    mutable Mutex               mLock;

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ChatPrticipantHandler();
    ChatPrticipantHandler( const ChatPrticipantHandler & /*src*/ );
    const ChatPrticipantHandler & operator = ( const ChatPrticipantHandler & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ChatPrticipantHandler class inline methods
//////////////////////////////////////////////////////////////////////////

inline bool ChatPrticipantHandler::IsValid() const
{
    Lock lock(mLock);
    return (mWndChat != 0) && (mConnectionService != nullptr);
}

inline void ChatPrticipantHandler::SetConnectionData( const areg::String & serviceName, const chat::sInitiator & initiator, const chat::ListParticipants & listParticipants )
{
    Lock lock( mLock );
    mServiceName        = serviceName;
    mInitiator          = initiator;
    mListParticipants   = listParticipants;
}

inline const areg::String & ChatPrticipantHandler::GetServiceName() const
{
    Lock lock(mLock);
    return mServiceName;
}

void ChatPrticipantHandler::SetParticipantsList( const chat::ListParticipants & listParaticipants )
{
    Lock lock( mLock );
    mListParticipants = listParaticipants;
}

inline const chat::sInitiator & ChatPrticipantHandler::GetInitiator() const
{
    Lock lock( mLock );
    return mInitiator;
}

inline const chat::ListParticipants & ChatPrticipantHandler::GetParticipantList() const
{
    Lock lock( mLock );
    return mListParticipants;
}

inline void ChatPrticipantHandler::SetChatWindow(ptr_type hWnd )
{
    Lock lock( mLock );
    mWndChat    = hWnd;
}

inline ptr_type ChatPrticipantHandler::GetChatWindow() const
{
    Lock lock( mLock );
    return mWndChat;
}

inline void ChatPrticipantHandler::SetConnectionService( DirectChatService * connectService )
{
    Lock lock( mLock );
    mConnectionService  = connectService;
}

inline DirectChatService * ChatPrticipantHandler::GetConnectionService() const
{
    Lock lock( mLock );
    return mConnectionService;
}

inline void ChatPrticipantHandler::SetConnectionOwner( const chat::Participant & ownerConnection )
{
    Lock lock( mLock );
    mOwnerConnection    = ownerConnection;
}

inline const chat::Participant & ChatPrticipantHandler::GetConnectionOwner() const
{
    Lock lock( mLock );
    return mOwnerConnection;
}

inline void ChatPrticipantHandler::SetChatClient( DirectMessagingClient * chatClient )
{
    Lock lock( mLock );
    mChatClient = chatClient;
}

inline DirectMessagingClient * ChatPrticipantHandler::GetChatClient() const
{
    Lock lock( mLock );
    return mChatClient;
}
