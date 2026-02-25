#pragma once
/************************************************************************
 * \file            chatter/services/ConnectionHandler.hpp
 * \brief           The connection handler. Contains service name and
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "common/ChatDefs.hpp"

class ConnectionHandler
{
public:
    ConnectionHandler();
    virtual ~ConnectionHandler();

public:
    inline void SetNickName( const areg::String & nickName );

    inline const areg::String & GetNickName() const;

    void SetCookie( uint32_t cookie );

    uint32_t GetCookie() const;

    void SetConnectCookie( uint32_t cookie );

    uint32_t GetConnectCookie() const;

    uint32_t GetCookieDirect() const;

    inline void SetConnected( bool isConnected );

    inline bool GetConnected() const;

    inline void SetRegistered( bool isRegistered );

    inline bool GetRegistered() const;

    inline void SetTimeConnect( const areg::DateTime & dateTime );

    inline const areg::DateTime & GetTimeConnect() const;

    inline void SetTimeConnected( const areg::DateTime & dateTime );

    inline const areg::DateTime & GetTimeConnected() const;

    inline bool HasName() const;

    inline bool CanRegister() const;

    inline bool CanConnect() const;

    inline void RemoveConnections();

    inline const chat::ListConnections & GetConnectionList() const;

    bool IsValid() const;

    bool AddConnection( const chat::ConnectionRecord & newConnection );

    int32_t AddConnections( const chat::ListConnections & listConnections );

    bool RemoveConnection( const chat::ConnectionRecord & connection );

    bool ConnectionExist( const chat::ConnectionRecord & connection );

    void ResetConnectionList();

private:

    int32_t findConnection( const chat::ConnectionRecord & connection ) const;

private:
    chat::ListConnections mListConnections;
    areg::String        mNickName;
    uint32_t      mCookie;
    uint32_t      mConnectCookie;
    areg::DateTime      mTimeConnect;
    areg::DateTime      mTimeConnected;
    bool          mIsRegistered;
    bool          mIsConnected;
    mutable Mutex mLock;

private:
    ConnectionHandler( const ConnectionHandler & /*src*/ );
    const ConnectionHandler & operator = ( const ConnectionHandler & /*src*/ );
};


inline void ConnectionHandler::SetNickName(const areg::String & nickName)
{
    Lock lock(mLock);
    mIsRegistered = mNickName != nickName ? false : mIsRegistered;
    mNickName   = nickName;
}

inline const areg::String & ConnectionHandler::GetNickName() const
{
    Lock lock(mLock);
    return mNickName;
}

inline void ConnectionHandler::SetConnected(bool isConnected)
{
    Lock lock(mLock);
    mIsConnected    = isConnected;
    mIsRegistered   = mIsConnected ? mIsRegistered : false;
}

inline bool ConnectionHandler::GetConnected() const
{
    Lock lock(mLock);
    return mIsConnected;
}

inline void ConnectionHandler::SetRegistered(bool isRegistered)
{
    Lock lock(mLock);
    mIsRegistered   = isRegistered;
}

inline bool ConnectionHandler::GetRegistered() const
{
    Lock lock(mLock);
    return mIsRegistered;
}

inline void ConnectionHandler::SetTimeConnect(const areg::DateTime & dateTime)
{
    Lock lock(mLock);
    mTimeConnect    = dateTime;
}

inline const areg::DateTime & ConnectionHandler::GetTimeConnect() const
{
    Lock lock(mLock);
    return mTimeConnect;
}

inline void ConnectionHandler::SetTimeConnected(const areg::DateTime & dateTime)
{
    Lock lock(mLock);
    mTimeConnected  = dateTime;
}

inline const areg::DateTime & ConnectionHandler::GetTimeConnected() const
{
    Lock lock(mLock);
    return mTimeConnected;
}

inline bool ConnectionHandler::HasName() const
{
    Lock lock(mLock);
    return (mNickName.isEmpty() == false);
}

inline bool ConnectionHandler::CanRegister() const
{
    Lock lock(mLock);
    return (mNickName.isEmpty() == false) && (mCookie != chat::InvalidCookie) && (mIsRegistered == false);
}

inline bool ConnectionHandler::CanConnect() const
{
    Lock lock(mLock);
    return (mNickName.isEmpty( ) == false) && (mCookie == chat::InvalidCookie);
}

inline void ConnectionHandler::RemoveConnections()
{
    Lock lock(mLock);
    mListConnections.clear();
}

inline const chat::ListConnections & ConnectionHandler::GetConnectionList() const
{
    return mListConnections;
}
