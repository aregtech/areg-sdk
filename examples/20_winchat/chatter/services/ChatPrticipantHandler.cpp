/************************************************************************
 * \file            chatter/services/ChatPrticipantHandler.cpp
 * \brief           The messaging service chat participant handling
 ************************************************************************/

#include "chatter/services/ChatPrticipantHandler.hpp"
#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
// ChatPrticipantHandler class implementation
//////////////////////////////////////////////////////////////////////////

ChatPrticipantHandler::ChatPrticipantHandler( const areg::String & serviceName
                                            , const chat::sInitiator & initiator
                                            , const chat::ListParticipants & listParticipants
                                            , const chat::Participant & ownerConnection )
    : mServiceName      ( serviceName )
    , mOwnerConnection  ( ownerConnection )
    , mInitiator        ( initiator )
    , mListParticipants ( listParticipants )
    , mConnectionService( nullptr )
    , mChatClient       ( nullptr )
    , mIsServicing      ( ownerConnection == initiator )
    , mWndChat          ( static_cast<ptr_type>(0) )
    , mLock             ( false )
{
}

ChatPrticipantHandler::~ChatPrticipantHandler()
{
}

int32_t ChatPrticipantHandler::AddParticipant( const chat::sInitiator & initiator, const chat::ListParticipants & listParticipants )
{
    areg::Lock lock( mLock );
    int32_t result = 0;
    if ( mInitiator == initiator )
    {
        for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
        {
            const chat::Participant & connection = listParticipants[i];
            if ( findPosition(connection) == areg::INVALID_INDEX )
            {
                mListParticipants.add(connection);
                ++ result;
            }
        }
    }
    return result;
}

bool ChatPrticipantHandler::AddParticipant( const chat::sInitiator & initiator, const chat::Participant & participant )
{
    areg::Lock lock( mLock );
    int32_t result = 0;
    if ( (mInitiator == initiator) && (findPosition( participant ) == areg::INVALID_INDEX) )
    {
        mListParticipants.add( participant );
        ++ result;
    }
    return (result == 1);
}

int32_t ChatPrticipantHandler::RemoveParticipant( const chat::sInitiator & initiator, const chat::ListParticipants & listParticipants )
{
    areg::Lock lock( mLock );
    int32_t result = 0;
    if ( mInitiator == initiator )
    {
        for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
        {
            const chat::Participant & connection = listParticipants[i];
            int32_t pos = findPosition(connection);
            if ( pos != areg::INVALID_INDEX )
            {
                mListParticipants.removeAt(static_cast<uint32_t>(pos));
                ++ result;
            }
        }
    }
    return result;
}

bool ChatPrticipantHandler::RemoveParticipant( const chat::sInitiator & initiator, const chat::Participant & participant )
{
    areg::Lock lock( mLock );
    int32_t result = 0;
    if ( mInitiator == initiator )
    {
        int32_t pos = findPosition( participant );
        if ( pos != areg::INVALID_INDEX )
        {
            mListParticipants.removeAt( static_cast<uint32_t>(pos) );
            ++ result;
        }
    }
    return (result == 1);
}

bool ChatPrticipantHandler::ParticipantExist( const chat::Participant & participant ) const
{
    areg::Lock lock( mLock );
    return (findPosition(participant) != areg::INVALID_INDEX);
}

bool ChatPrticipantHandler::IsEmpty() const
{
    areg::Lock lock(mLock);
    uint32_t size = mListParticipants.getSize( );
    if (size == 1)
    {
        const chat::Participant & part = mListParticipants[0u];
        return (mOwnerConnection == part);
    }
    else
    {
        return (size == 0);
    }
}

void ChatPrticipantHandler::Invalidate()
{
    areg::Lock lock( mLock );
    mListParticipants.clear();
    mOwnerConnection    = chat::Participant();
    mInitiator          = chat::sInitiator();
    mConnectionService  = nullptr;
    mChatClient         = nullptr;
    mIsServicing        = false;
    mWndChat            = static_cast<ptr_type>(0);
}

int32_t ChatPrticipantHandler::findPosition( const chat::Participant & participant ) const
{
    int32_t result = areg::INVALID_INDEX;
    for (uint32_t i = 0; i < mListParticipants.getSize( ); ++ i )
    {
        if ( participant == mListParticipants[i] )
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

void ChatPrticipantHandler::SetInitiator( const chat::sInitiator & initiator )
{
    areg::Lock lock( mLock );
    mInitiator  = initiator;
    if ( mListParticipants.contains(initiator, 0) == false )
        mListParticipants.add(initiator);
}
