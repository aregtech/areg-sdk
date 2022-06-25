#pragma once
/************************************************************************
 * \file            chatter/services/ConnectionHandler.hpp
 * \brief           The connection handler. Contains service name and
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/SynchObjects.hpp"
#include "generated/NECommon.hpp"

class ConnectionHandler
{
public:
    ConnectionHandler( void );
    virtual ~ConnectionHandler( void );

public:
    inline void SetNickName( const String & nickName );

    inline const String & GetNickName( void ) const;

    void SetCookie( const uint32_t cookie );

    const uint32_t GetCookie( void ) const;

    void SetConnectCookie( const uint32_t cookie );

    const uint32_t GetConnectCookie( void ) const;

    const uint32_t GetCookieDirect( void ) const;

    inline void SetConnected( const bool isConnected );

    inline bool GetConnected( void ) const;

    inline void SetRegistered( const bool isRegistered );

    inline const bool GetRegistered( void ) const;

    inline void SetTimeConnect( const DateTime & dateTime );

    inline const DateTime & GetTimeConnect( void ) const;

    inline void SetTimeConnected( const DateTime & dateTime );

    inline const DateTime & GetTimeConnected( void ) const;

    inline bool HasName( void ) const;

    inline bool CanRegister( void ) const;

    inline bool CanConnect( void ) const;

    inline void RemoveConnections( void );

    inline const NECommon::ListConnections & GetConnectionList( void ) const;

    bool IsValid( void ) const;

    bool AddConnection( const NECommon::sConnection & newConnection );

    int AddConnections( const NECommon::ListConnections & listConnections );

    bool RemoveConnection( const NECommon::sConnection & connection );

    bool ConnectionExist( const NECommon::sConnection & connection );

    void ResetConnectionList( void );

private:

    int findConnection( const NECommon::sConnection & connection ) const;

private:
    NECommon::ListConnections mListConnections;
    String        mNickName;
    uint32_t        mCookie;
    uint32_t        mConnectCookie;
    DateTime      mTimeConnect;
    DateTime      mTimeConnected;
    bool            mIsRegistered;
    bool            mIsConnected;
    mutable Mutex mLock;

private:
    ConnectionHandler( const ConnectionHandler & /*src*/ );
    const ConnectionHandler & operator = ( const ConnectionHandler & /*src*/ );
};


inline void ConnectionHandler::SetNickName(const String & nickName)
{
    Lock lock(mLock);
    mIsRegistered = mNickName != nickName ? false : mIsRegistered;
    mNickName   = nickName;
}

inline const String & ConnectionHandler::GetNickName(void) const
{
    Lock lock(mLock);
    return mNickName;
}

inline void ConnectionHandler::SetConnected(const bool isConnected)
{
    Lock lock(mLock);
    mIsConnected    = isConnected;
    mIsRegistered   = mIsConnected ? mIsRegistered : false;
}

inline bool ConnectionHandler::GetConnected(void) const
{
    Lock lock(mLock);
    return mIsConnected;
}

inline void ConnectionHandler::SetRegistered(const bool isRegistered)
{
    Lock lock(mLock);
    mIsRegistered   = isRegistered;
}

inline const bool ConnectionHandler::GetRegistered(void) const
{
    Lock lock(mLock);
    return mIsRegistered;
}

inline void ConnectionHandler::SetTimeConnect(const DateTime & dateTime)
{
    Lock lock(mLock);
    mTimeConnect    = dateTime;
}

inline const DateTime & ConnectionHandler::GetTimeConnect(void) const
{
    Lock lock(mLock);
    return mTimeConnect;
}

inline void ConnectionHandler::SetTimeConnected(const DateTime & dateTime)
{
    Lock lock(mLock);
    mTimeConnected  = dateTime;
}

inline const DateTime & ConnectionHandler::GetTimeConnected(void) const
{
    Lock lock(mLock);
    return mTimeConnected;
}

inline bool ConnectionHandler::HasName(void) const
{
    Lock lock(mLock);
    return (mNickName.isEmpty() == false);
}

inline bool ConnectionHandler::CanRegister(void) const
{
    Lock lock(mLock);
    return (mNickName.isEmpty() == false) && (mCookie != NECommon::InvalidCookie) && (mIsRegistered == false);
}

inline bool ConnectionHandler::CanConnect(void) const
{
    Lock lock(mLock);
    return (mNickName.isEmpty( ) == false) && (mCookie == NECommon::InvalidCookie);
}

inline void ConnectionHandler::RemoveConnections(void)
{
    Lock lock(mLock);
    mListConnections.clear();
}

inline const NECommon::ListConnections & ConnectionHandler::GetConnectionList(void) const
{
    return mListConnections;
}
