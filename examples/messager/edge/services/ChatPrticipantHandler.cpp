/************************************************************************
 * \file            edge/services/ChatPrticipantHandler.cpp
 * \brief           The messaging service chat participant handling
 ************************************************************************/

#include "edge/services/ChatPrticipantHandler.hpp"

//////////////////////////////////////////////////////////////////////////
// ChatPrticipantHandler class implementation
//////////////////////////////////////////////////////////////////////////

ChatPrticipantHandler::ChatPrticipantHandler( const String & serviceName
                                                , const NECommonSettings::sInitiator & initiator
                                                , const NECommonSettings::ListParticipants & listParticipants 
                                                , const NECommonSettings::sParticipant & ownerConnection )
    : mServiceName      ( serviceName )
    , mOwnerConnection  ( ownerConnection )
    , mInitiator        ( initiator )
    , mListParticipants ( listParticipants )
    , mConnectionService( NULL )
    , mChatClient       ( NULL )
    , mIsServicing      ( ownerConnection == initiator )
    , mWndChat          ( static_cast<HWND>(NULL) )
    , mLock             ( false )
{
    ; // do nothing
}

ChatPrticipantHandler::~ChatPrticipantHandler( void )
{
    ; // do nothing
}

int ChatPrticipantHandler::AddParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & listParticipants )
{
    Lock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        for ( int i = 0; i < listParticipants.getSize(); ++ i )
        {
            const NECommonSettings::sParticipant & connection = listParticipants[i];
            if ( findPosition(connection) == NECommon::InvalidIndex )
            {
                mListParticipants.add(connection);
                ++ result;
            }
        }
    }
    return result;
}

bool ChatPrticipantHandler::AddParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::sParticipant & participant )
{
    Lock lock( mLock );
    int result = 0;
    if ( (mInitiator == initiator) && (findPosition( participant ) == NECommon::InvalidIndex) )
    {
        mListParticipants.add( participant );
        ++ result;
    }
    return (result == 1);
}

int ChatPrticipantHandler::RemoveParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & listParticipants )
{
    Lock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        for ( int i = 0; i < listParticipants.getSize(); ++ i )
        {
            const NECommonSettings::sParticipant & connection = listParticipants[i];
            int pos = findPosition(connection);
            if ( pos != NECommon::InvalidIndex )
            {
                mListParticipants.removeAt(pos);
                ++ result;
            }
        }
    }
    return result;
}

bool ChatPrticipantHandler::RemoveParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::sParticipant & participant )
{
    Lock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        int pos = findPosition( participant );
        if ( pos != NECommon::InvalidIndex )
        {
            mListParticipants.removeAt( pos );
            ++ result;
        }
    }
    return (result == 1);
}

bool ChatPrticipantHandler::ParticipantExist( const NECommonSettings::sParticipant & participant ) const
{
    Lock lock( mLock );
    return (findPosition(participant) != NECommon::InvalidIndex);
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
    mOwnerConnection    = NECommonSettings::sParticipant();
    mInitiator          = NECommonSettings::sInitiator();
    mConnectionService  = NULL;
    mChatClient         = NULL;
    mIsServicing        = false;
    mWndChat            = static_cast<HWND>(NULL);
}

int ChatPrticipantHandler::findPosition( const NECommonSettings::sParticipant & participant ) const
{
    int result = NECommon::InvalidIndex;
    for ( int i = 0; i < mListParticipants.getSize( ); ++ i )
    {
        if ( participant == mListParticipants[i] )
        {
            result = i;
            break;
        }
    }
    return result;
}

void ChatPrticipantHandler::SetInitiator( const NECommonSettings::sInitiator & initiator )
{
    Lock lock( mLock );
    mInitiator  = initiator;
    if ( mListParticipants.exist(initiator, 0) == false )
        mListParticipants.add(initiator);
}
