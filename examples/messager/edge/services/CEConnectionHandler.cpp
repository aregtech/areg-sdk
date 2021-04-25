/************************************************************************
* \file            edge/services/CEConnectionHandler.cpp
* \brief           The connection handler. Contains service name and
************************************************************************/

#include "edge/services/CEConnectionHandler.hpp"
#include "shared/generated/NEConnectionManager.hpp"

CEConnectionHandler::CEConnectionHandler(void)
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

CEConnectionHandler::~CEConnectionHandler(void)
{
}

bool CEConnectionHandler::IsValid(void) const
{
    CELock lock(mLock);
    return (GetCookie() != NEConnectionManager::InvalidCookie) && (mNickName.IsEmpty() == false);
}

bool CEConnectionHandler::AddConnection(const NECommonSettings::sConnection & newConnection)
{
    CELock lock(mLock);
    int pos = NECommon::InvalidIndex;
    
    if (newConnection.nickName != mNickName)
    {
        findConnection(newConnection);
        if ( pos == NECommon::InvalidIndex )
            mListConnections.Add(newConnection);
    }
    return (pos == NECommon::InvalidIndex);
}

int CEConnectionHandler::AddConnections(const NECommonSettings::ListConnections & listConnections)
{
    CELock lock(mLock);
    int result = 0;
    for (int i = 0; i < listConnections.GetSize(); ++i)
    {
        const NECommonSettings::sConnection & entry = listConnections.GetAt(i);
        if (entry.nickName != mNickName)
        {
            if (findConnection(entry) == NECommon::InvalidIndex)
            {
                mListConnections.Add(entry);
                ++ result;
            }
        }
    }
    return result;
}

bool CEConnectionHandler::RemoveConnection(const NECommonSettings::sConnection & connection)
{
    CELock lock(mLock);
    int pos = findConnection(connection);
    if ( pos != NECommon::InvalidIndex )
        mListConnections.RemoveAt(pos);
    return (pos != NECommon::InvalidIndex);
}

bool CEConnectionHandler::ConnectionExist(const NECommonSettings::sConnection & connection)
{
    CELock lock(mLock);
    return ( findConnection(connection) != NECommon::InvalidIndex);
}

int CEConnectionHandler::findConnection(const NECommonSettings::sConnection & connection) const
{
    int result = NECommon::InvalidIndex;
    for (int i = 0; i < mListConnections.GetSize(); ++i)
    {
        const NECommonSettings::sConnection & entry = mListConnections.GetAt(i);
        if (entry.nickName == connection.nickName)
        {
            result = i;
            break;
        }
    }
    return result;
}

void CEConnectionHandler::ResetConnectionList( void )
{
    CELock lock( mLock );

    mListConnections.RemoveAll( );
    mNickName.Clear( );
    
    mCookie         = NEConnectionManager::InvalidCookie;
    mConnectCookie  = NEConnectionManager::InvalidCookie;
    mTimeConnect    = CEDateTime( );
    mTimeConnected  = CEDateTime( );
    mIsRegistered   = false;
    mIsConnected    = false;
}

void CEConnectionHandler::SetConnectCookie( const uint32_t cookie )
{
    CELock lock( mLock );
    mConnectCookie  = cookie;
}

const uint32_t CEConnectionHandler::GetConnectCookie( void ) const
{
    CELock lock( mLock );
    return mConnectCookie;
}

void CEConnectionHandler::SetCookie( const uint32_t cookie )
{
    CELock lock( mLock );
    mCookie         = cookie;
    mConnectCookie  = cookie;
}

const uint32_t CEConnectionHandler::GetCookie( void ) const
{
    CELock lock( mLock );
    return (mCookie != NECommonSettings::InvalidCookie ? mCookie : mConnectCookie);
}

const uint32_t CEConnectionHandler::GetCookieDirect( void ) const
{
    CELock lock( mLock );
    return mCookie;
}
