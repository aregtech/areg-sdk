/************************************************************************
* \file            chatter/services/ConnectionHandler.cpp
* \brief           The connection handler. Contains service name and
************************************************************************/

#include "chatter/services/ConnectionHandler.hpp"
#include "examples/20_winchat/services/NEConnectionManager.hpp"

ConnectionHandler::ConnectionHandler()
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

ConnectionHandler::~ConnectionHandler()
{
}

bool ConnectionHandler::IsValid() const
{
    Lock lock(mLock);
    return (GetCookie() != NEConnectionManager::InvalidCookie) && (mNickName.isEmpty() == false);
}

bool ConnectionHandler::AddConnection(const NECommon::sConnection & newConnection)
{
    Lock lock(mLock);
    int pos = areg::common::INVALID_INDEX;
    
    if (newConnection.nickName != mNickName)
    {
        findConnection(newConnection);
        if ( pos == areg::common::INVALID_INDEX )
            mListConnections.add(newConnection);
    }
    return (pos == areg::common::INVALID_INDEX);
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
            if (findConnection(entry) == areg::common::INVALID_INDEX)
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
    if ( pos != areg::common::INVALID_INDEX )
    {
        mListConnections.removeAt( static_cast<uint32_t>(pos) );
    }

    return (pos != areg::common::INVALID_INDEX);
}

bool ConnectionHandler::ConnectionExist(const NECommon::sConnection & connection)
{
    Lock lock(mLock);
    return ( findConnection(connection) != areg::common::INVALID_INDEX);
}

int ConnectionHandler::findConnection(const NECommon::sConnection & connection) const
{
    int result = areg::common::INVALID_INDEX;
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

void ConnectionHandler::ResetConnectionList()
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

void ConnectionHandler::SetConnectCookie( uint32_t cookie )
{
    Lock lock( mLock );
    mConnectCookie  = cookie;
}

uint32_t ConnectionHandler::GetConnectCookie() const
{
    Lock lock( mLock );
    return mConnectCookie;
}

void ConnectionHandler::SetCookie( uint32_t cookie )
{
    Lock lock( mLock );
    mCookie         = cookie;
    mConnectCookie  = cookie;
}

uint32_t ConnectionHandler::GetCookie() const
{
    Lock lock( mLock );
    return (mCookie != NECommon::InvalidCookie ? mCookie : mConnectCookie);
}

uint32_t ConnectionHandler::GetCookieDirect() const
{
    Lock lock( mLock );
    return mCookie;
}
