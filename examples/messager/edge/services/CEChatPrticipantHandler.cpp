/************************************************************************
 * \file            edge/services/CEChatPrticipantHandler.cpp
 * \brief           The messaging service chat participant handling
 ************************************************************************/

#include "edge/services/CEChatPrticipantHandler.hpp"

//////////////////////////////////////////////////////////////////////////
// CEChatPrticipantHandler class implementation
//////////////////////////////////////////////////////////////////////////

CEChatPrticipantHandler::CEChatPrticipantHandler( const CEString & serviceName
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

CEChatPrticipantHandler::~CEChatPrticipantHandler( void )
{
    ; // do nothing
}

int CEChatPrticipantHandler::AddParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & listParticipants )
{
    CELock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        for ( int i = 0; i < listParticipants.GetSize(); ++ i )
        {
            const NECommonSettings::sParticipant & connection = listParticipants[i];
            if ( findPosition(connection) == NECommon::InvalidIndex )
            {
                mListParticipants.Add(connection);
                ++ result;
            }
        }
    }
    return result;
}

bool CEChatPrticipantHandler::AddParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::sParticipant & participant )
{
    CELock lock( mLock );
    int result = 0;
    if ( (mInitiator == initiator) && (findPosition( participant ) == NECommon::InvalidIndex) )
    {
        mListParticipants.Add( participant );
        ++ result;
    }
    return (result == 1);
}

int CEChatPrticipantHandler::RemoveParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::ListParticipants & listParticipants )
{
    CELock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        for ( int i = 0; i < listParticipants.GetSize(); ++ i )
        {
            const NECommonSettings::sParticipant & connection = listParticipants[i];
            int pos = findPosition(connection);
            if ( pos != NECommon::InvalidIndex )
            {
                mListParticipants.RemoveAt(pos);
                ++ result;
            }
        }
    }
    return result;
}

bool CEChatPrticipantHandler::RemoveParticipant( const NECommonSettings::sInitiator & initiator, const NECommonSettings::sParticipant & participant )
{
    CELock lock( mLock );
    int result = 0;
    if ( mInitiator == initiator )
    {
        int pos = findPosition( participant );
        if ( pos != NECommon::InvalidIndex )
        {
            mListParticipants.RemoveAt( pos );
            ++ result;
        }
    }
    return (result == 1);
}

bool CEChatPrticipantHandler::ParticipantExist( const NECommonSettings::sParticipant & participant ) const
{
    CELock lock( mLock );
    return (findPosition(participant) != NECommon::InvalidIndex);
}

bool CEChatPrticipantHandler::IsEmpty(void) const
{
    return  ( mListParticipants.GetSize() == 0)    ||
            ((mListParticipants.GetSize() == 1) && (mListParticipants[0] == mOwnerConnection));
}

void CEChatPrticipantHandler::Invalidate( void )
{
    CELock lock( mLock );
    mListParticipants.RemoveAll();
    mOwnerConnection    = NECommonSettings::sParticipant();
    mInitiator          = NECommonSettings::sInitiator();
    mConnectionService  = NULL;
    mChatClient         = NULL;
    mIsServicing        = false;
    mWndChat            = static_cast<HWND>(NULL);
}

int CEChatPrticipantHandler::findPosition( const NECommonSettings::sParticipant & participant ) const
{
    int result = NECommon::InvalidIndex;
    for ( int i = 0; i < mListParticipants.GetSize( ); ++ i )
    {
        if ( participant == mListParticipants[i] )
        {
            result = i;
            break;
        }
    }
    return result;
}

void CEChatPrticipantHandler::SetInitiator( const NECommonSettings::sInitiator & initiator )
{
    CELock lock( mLock );
    mInitiator  = initiator;
    if ( mListParticipants.ElementExist(initiator, 0) == false )
        mListParticipants.Add(initiator);
}
