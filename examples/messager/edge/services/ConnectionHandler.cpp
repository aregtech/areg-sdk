/************************************************************************
* \file            edge/services/ConnectionHandler.cpp
* \brief           The connection handler. Contains service name and
************************************************************************/

#include "edge/services/ConnectionHandler.hpp"
#include "shared/generated/NEConnectionManager.hpp"

ConnectionHandler::ConnectionHandler(void)
    : mListConnections  ( )
    , mNickName         ( )
    , mCookie           ( NEConnectionManager::InvalidCookie )
    , mConnectCookie    ( NEConnectionManager::InvalidCookie )
    , mTimeConnect      ( )
    , mTimeConnected    ( )
    , mIsRegistered     ( false )
    , mIsConnected      ( false )
    , mLock             ( false )
{
}

ConnectionHandler::~ConnectionHandler(void)
{
}

bool ConnectionHandler::IsValid(void) const
{
    Lock lock(mLock);
    return (GetCookie() != NEConnectionManager::InvalidCookie) && (mNickName.isEmpty() == false);
}

bool ConnectionHandler::AddConnection(const NECommonSettings::sConnection & newConnection)
{
    Lock lock(mLock);
    int pos = NECommon::InvalidIndex;
    
    if (newConnection.nickName != mNickName)
    {
        findConnection(newConnection);
        if ( pos == NECommon::InvalidIndex )
            mListConnections.add(newConnection);
    }
    return (pos == NECommon::InvalidIndex);
}

int ConnectionHandler::AddConnections(const NECommonSettings::ListConnections & listConnections)
{
    Lock lock(mLock);
    int result = 0;
    for (int i = 0; i < listConnections.getSize(); ++i)
    {
        const NECommonSettings::sConnection & entry = listConnections.getAt(i);
        if (entry.nickName != mNickName)
        {
            if (findConnection(entry) == NECommon::InvalidIndex)
            {
                mListConnections.add(entry);
                ++ result;
            }
        }
    }
    return result;
}

bool ConnectionHandler::RemoveConnection(const NECommonSettings::sConnection & connection)
{
    Lock lock(mLock);
    int pos = findConnection(connection);
    if ( pos != NECommon::InvalidIndex )
        mListConnections.removeAt(pos);
    return (pos != NECommon::InvalidIndex);
}

bool ConnectionHandler::ConnectionExist(const NECommonSettings::sConnection & connection)
{
    Lock lock(mLock);
    return ( findConnection(connection) != NECommon::InvalidIndex);
}

int ConnectionHandler::findConnection(const NECommonSettings::sConnection & connection) const
{
    int result = NECommon::InvalidIndex;
    for (int i = 0; i < mListConnections.getSize(); ++i)
    {
        const NECommonSettings::sConnection & entry = mListConnections.getAt(i);
        if (entry.nickName == connection.nickName)
        {
            result = i;
            break;
        }
    }
    return result;
}

void ConnectionHandler::ResetConnectionList( void )
{
    Lock lock( mLock );

    mListConnections.removeAll( );
    mNickName.clear( );
    
    mCookie         = NEConnectionManager::InvalidCookie;
    mConnectCookie  = NEConnectionManager::InvalidCookie;
    mTimeConnect    = DateTime( );
    mTimeConnected  = DateTime( );
    mIsRegistered   = false;
    mIsConnected    = false;
}

void ConnectionHandler::SetConnectCookie( const uint32_t cookie )
{
    Lock lock( mLock );
    mConnectCookie  = cookie;
}

const uint32_t ConnectionHandler::GetConnectCookie( void ) const
{
    Lock lock( mLock );
    return mConnectCookie;
}

void ConnectionHandler::SetCookie( const uint32_t cookie )
{
    Lock lock( mLock );
    mCookie         = cookie;
    mConnectCookie  = cookie;
}

const uint32_t ConnectionHandler::GetCookie( void ) const
{
    Lock lock( mLock );
    return (mCookie != NECommonSettings::InvalidCookie ? mCookie : mConnectCookie);
}

const uint32_t ConnectionHandler::GetCookieDirect( void ) const
{
    Lock lock( mLock );
    return mCookie;
}
