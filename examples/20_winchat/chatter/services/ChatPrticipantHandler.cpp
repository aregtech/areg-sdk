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

ChatPrticipantHandler::ChatPrticipantHandler( const String & serviceName
                                            , const chat::sInitiator & initiator
                                            , const chat::ListParticipants & listParticipants
                                            , const chat::sParticipant & ownerConnection )
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

int ChatPrticipantHandler::AddParticipant( const chat::sInitiator & initiator, const chat::ListParticipants & listParticipants )
{
    Lock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
        {
            const chat::sParticipant & connection = listParticipants[i];
            if ( findPosition(connection) == areg::INVALID_INDEX )
            {
                mListParticipants.add(connection);
                ++ result;
            }
        }
    }
    return result;
}

bool ChatPrticipantHandler::AddParticipant( const chat::sInitiator & initiator, const chat::sParticipant & participant )
{
    Lock lock( mLock );
    int result = 0;
    if ( (mInitiator == initiator) && (findPosition( participant ) == areg::INVALID_INDEX) )
    {
        mListParticipants.add( participant );
        ++ result;
    }
    return (result == 1);
}

int ChatPrticipantHandler::RemoveParticipant( const chat::sInitiator & initiator, const chat::ListParticipants & listParticipants )
{
    Lock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
        {
            const chat::sParticipant & connection = listParticipants[i];
            int pos = findPosition(connection);
            if ( pos != areg::INVALID_INDEX )
            {
                mListParticipants.removeAt(static_cast<uint32_t>(pos));
                ++ result;
            }
        }
    }
    return result;
}

bool ChatPrticipantHandler::RemoveParticipant( const chat::sInitiator & initiator, const chat::sParticipant & participant )
{
    Lock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        int pos = findPosition( participant );
        if ( pos != areg::INVALID_INDEX )
        {
            mListParticipants.removeAt( static_cast<uint32_t>(pos) );
            ++ result;
        }
    }
    return (result == 1);
}

bool ChatPrticipantHandler::ParticipantExist( const chat::sParticipant & participant ) const
{
    Lock lock( mLock );
    return (findPosition(participant) != areg::INVALID_INDEX);
}

bool ChatPrticipantHandler::IsEmpty() const
{
    Lock lock(mLock);
    uint32_t size = mListParticipants.getSize( );
    if (size == 1)
    {
        const chat::sParticipant & part = mListParticipants[0u];
        return (mOwnerConnection == part);
    }
    else
    {
        return (size == 0);
    }
}

void ChatPrticipantHandler::Invalidate()
{
    Lock lock( mLock );
    mListParticipants.clear();
    mOwnerConnection    = chat::sParticipant();
    mInitiator          = chat::sInitiator();
    mConnectionService  = nullptr;
    mChatClient         = nullptr;
    mIsServicing        = false;
    mWndChat            = static_cast<ptr_type>(0);
}

int ChatPrticipantHandler::findPosition( const chat::sParticipant & participant ) const
{
    int result = areg::INVALID_INDEX;
    for (uint32_t i = 0; i < mListParticipants.getSize( ); ++ i )
    {
        if ( participant == mListParticipants[i] )
        {
            result = static_cast<int>(i);
            break;
        }
    }

    return result;
}

void ChatPrticipantHandler::SetInitiator( const chat::sInitiator & initiator )
{
    Lock lock( mLock );
    mInitiator  = initiator;
    if ( mListParticipants.contains(initiator, 0) == false )
        mListParticipants.add(initiator);
}
