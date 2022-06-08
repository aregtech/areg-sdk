/************************************************************************
 * \file            chatter/services/ChatPrticipantHandler.cpp
 * \brief           The messaging service chat participant handling
 ************************************************************************/

#include "chatter/services/ChatPrticipantHandler.hpp"

//////////////////////////////////////////////////////////////////////////
// ChatPrticipantHandler class implementation
//////////////////////////////////////////////////////////////////////////

ChatPrticipantHandler::ChatPrticipantHandler( const String & serviceName
                                            , const NECommon::sInitiator & initiator
                                            , const NECommon::ListParticipants & listParticipants 
                                            , const NECommon::sParticipant & ownerConnection )
    : mServiceName      ( serviceName )
    , mOwnerConnection  ( ownerConnection )
    , mInitiator        ( initiator )
    , mListParticipants ( listParticipants )
    , mConnectionService( nullptr )
    , mChatClient       ( nullptr )
    , mIsServicing      ( ownerConnection == initiator )
    , mWndChat          ( nullptr )
    , mLock             ( false )
{
    ; // do nothing
}

ChatPrticipantHandler::~ChatPrticipantHandler( void )
{
    ; // do nothing
}

int ChatPrticipantHandler::AddParticipant( const NECommon::sInitiator & initiator, const NECommon::ListParticipants & listParticipants )
{
    Lock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
        {
            const NECommon::sParticipant & connection = listParticipants[i];
            if ( findPosition(connection) == NECommon::INVALID_INDEX )
            {
                mListParticipants.add(connection);
                ++ result;
            }
        }
    }
    return result;
}

bool ChatPrticipantHandler::AddParticipant( const NECommon::sInitiator & initiator, const NECommon::sParticipant & participant )
{
    Lock lock( mLock );
    int result = 0;
    if ( (mInitiator == initiator) && (findPosition( participant ) == NECommon::INVALID_INDEX) )
    {
        mListParticipants.add( participant );
        ++ result;
    }
    return (result == 1);
}

int ChatPrticipantHandler::RemoveParticipant( const NECommon::sInitiator & initiator, const NECommon::ListParticipants & listParticipants )
{
    Lock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        for (uint32_t i = 0; i < listParticipants.getSize(); ++ i )
        {
            const NECommon::sParticipant & connection = listParticipants[i];
            int pos = findPosition(connection);
            if ( pos != NECommon::INVALID_INDEX )
            {
                mListParticipants.removeAt(pos);
                ++ result;
            }
        }
    }
    return result;
}

bool ChatPrticipantHandler::RemoveParticipant( const NECommon::sInitiator & initiator, const NECommon::sParticipant & participant )
{
    Lock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        int pos = findPosition( participant );
        if ( pos != NECommon::INVALID_INDEX )
        {
            mListParticipants.removeAt( pos );
            ++ result;
        }
    }
    return (result == 1);
}

bool ChatPrticipantHandler::ParticipantExist( const NECommon::sParticipant & participant ) const
{
    Lock lock( mLock );
    return (findPosition(participant) != NECommon::INVALID_INDEX);
}

bool ChatPrticipantHandler::IsEmpty(void) const
{
    return  ( mListParticipants.getSize() == 0)    ||
            ((mListParticipants.getSize() == 1) && (mListParticipants[0] == mOwnerConnection));
}

void ChatPrticipantHandler::Invalidate( void )
{
    Lock lock( mLock );
    mListParticipants.removeAll();
    mOwnerConnection    = NECommon::sParticipant();
    mInitiator          = NECommon::sInitiator();
    mConnectionService  = nullptr;
    mChatClient         = nullptr;
    mIsServicing        = false;
    mWndChat            = nullptr;
}

int ChatPrticipantHandler::findPosition( const NECommon::sParticipant & participant ) const
{
    int result = NECommon::INVALID_INDEX;
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

void ChatPrticipantHandler::SetInitiator( const NECommon::sInitiator & initiator )
{
    Lock lock( mLock );
    mInitiator  = initiator;
    if ( mListParticipants.exist(initiator, 0) == false )
        mListParticipants.add(initiator);
}
