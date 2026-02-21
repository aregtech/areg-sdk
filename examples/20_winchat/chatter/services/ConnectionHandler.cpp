/************************************************************************
* \file            chatter/services/ConnectionHandler.cpp
* \brief           The connection handler. Contains service name and
************************************************************************/

#include "chatter/services/ConnectionHandler.hpp"
#include "examples/20_winchat/services/ConnectionManager.hpp"

ConnectionHandler::ConnectionHandler()
    : mListConnections  ( )
    , mNickName         ( )
    , mCookie           ( ConnectionManager::InvalidCookie )
    , mConnectCookie    ( ConnectionManager::InvalidCookie )
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
    return (GetCookie() != ConnectionManager::InvalidCookie) && (mNickName.isEmpty() == false);
}

bool ConnectionHandler::AddConnection(const chat::sConnection & newConnection)
{
    Lock lock(mLock);
    int32_t pos = NECommon::INVALID_INDEX;
    
    if (newConnection.nickName != mNickName)
    {
        findConnection(newConnection);
        if ( pos == NECommon::INVALID_INDEX )
            mListConnections.add(newConnection);
    }
    return (pos == NECommon::INVALID_INDEX);
}

int32_t ConnectionHandler::AddConnections(const chat::ListConnections & listConnections)
{
    Lock lock(mLock);
    int32_t result = 0;
    for (uint32_t i = 0; i < listConnections.getSize(); ++i)
    {
        const chat::sConnection & entry = listConnections.getAt(i);
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

bool ConnectionHandler::RemoveConnection(const chat::sConnection & connection)
{
    Lock lock(mLock);
    int32_t pos = findConnection(connection);
    if ( pos != NECommon::INVALID_INDEX )
    {
        mListConnections.removeAt( static_cast<uint32_t>(pos) );
    }

    return (pos != NECommon::INVALID_INDEX);
}

bool ConnectionHandler::ConnectionExist(const chat::sConnection & connection)
{
    Lock lock(mLock);
    return ( findConnection(connection) != NECommon::INVALID_INDEX);
}

int32_t ConnectionHandler::findConnection(const chat::sConnection & connection) const
{
    int32_t result = NECommon::INVALID_INDEX;
    for (uint32_t i = 0; i < mListConnections.getSize(); ++i)
    {
        const chat::sConnection & entry = mListConnections.getAt(i);
        if (entry.nickName == connection.nickName)
        {
            result = static_cast<int32_t>(i);
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
    
    mCookie         = ConnectionManager::InvalidCookie;
    mConnectCookie  = ConnectionManager::InvalidCookie;
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
    return (mCookie != chat::InvalidCookie ? mCookie : mConnectCookie);
}

uint32_t ConnectionHandler::GetCookieDirect() const
{
    Lock lock( mLock );
    return mCookie;
}
