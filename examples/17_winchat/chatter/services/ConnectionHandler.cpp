/************************************************************************
* \file            chatter/services/ConnectionHandler.cpp
* \brief           The connection handler. Contains service name and
************************************************************************/

#include "chatter/services/ConnectionHandler.hpp"
#include "generated/NEConnectionManager.hpp"

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

bool ConnectionHandler::AddConnection(const NECommon::sConnection & newConnection)
{
    Lock lock(mLock);
    int pos = NECommon::INVALID_INDEX;
    
    if (newConnection.nickName != mNickName)
    {
        findConnection(newConnection);
        if ( pos == NECommon::INVALID_INDEX )
            mListConnections.add(newConnection);
    }
    return (pos == NECommon::INVALID_INDEX);
}

int ConnectionHandler::AddConnections(const NECommon::ListConnections & listConnections)
{
    Lock lock(mLock);
    int result = 0;
    for (uint32_t i = 0; i < listConnections.getSize(); ++i)
    {
        const NECommon::sConnection & entry = listConnections.getAt(i);
        if (entry.nickName != mNickName)
        {
            if (findConnection(entry) == NECommon::INVALID_INDEX)
            {
                mListConnections.add(entry);
                ++ result;
            }
        }
    }

    return result;
}

bool ConnectionHandler::RemoveConnection(const NECommon::sConnection & connection)
{
    Lock lock(mLock);
    int pos = findConnection(connection);
    if ( pos != NECommon::INVALID_INDEX )
        mListConnections.removeAt(pos);

    return (pos != NECommon::INVALID_INDEX);
}

bool ConnectionHandler::ConnectionExist(const NECommon::sConnection & connection)
{
    Lock lock(mLock);
    return ( findConnection(connection) != NECommon::INVALID_INDEX);
}

int ConnectionHandler::findConnection(const NECommon::sConnection & connection) const
{
    int result = NECommon::INVALID_INDEX;
    for (uint32_t i = 0; i < mListConnections.getSize(); ++i)
    {
        const NECommon::sConnection & entry = mListConnections.getAt(i);
        if (entry.nickName == connection.nickName)
        {
            result = static_cast<int>(i);
            break;
        }
    }

    return result;
}

void ConnectionHandler::ResetConnectionList( void )
{
    Lock lock( mLock );

    mListConnections.clear( );
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
    return (mCookie != NECommon::InvalidCookie ? mCookie : mConnectCookie);
}

const uint32_t ConnectionHandler::GetCookieDirect( void ) const
{
    Lock lock( mLock );
    return mCookie;
}
