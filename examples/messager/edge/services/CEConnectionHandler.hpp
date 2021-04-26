#pragma once
/************************************************************************
 * \file            edge/services/CEConnectionHandler.hpp
 * \brief           The connection handler. Contains service name and
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/CEDateTime.hpp"
#include "areg/src/base/ESynchObjects.hpp"
#include "shared/NECommonSettings.hpp"

class CEConnectionHandler
{
public:
    CEConnectionHandler( void );
    virtual ~CEConnectionHandler( void );

public:
    inline void SetNickName( const CEString & nickName );

    inline const CEString & GetNickName( void ) const;

    void SetCookie( const uint32_t cookie );

    const uint32_t GetCookie( void ) const;

    void SetConnectCookie( const uint32_t cookie );

    const uint32_t GetConnectCookie( void ) const;

    const uint32_t GetCookieDirect( void ) const;

    inline void SetConnected( const bool isConnected );

    inline bool GetConnected( void ) const;

    inline void SetRegistered( const bool isRegistered );

    inline const bool GetRegistered( void ) const;

    inline void SetTimeConnect( const CEDateTime & dateTime );

    inline const CEDateTime & GetTimeConnect( void ) const;

    inline void SetTimeConnected( const CEDateTime & dateTime );

    inline const CEDateTime & GetTimeConnected( void ) const;

    inline bool HasName( void ) const;

    inline bool CanRegister( void ) const;

    inline bool CanConnect( void ) const;

    inline void RemoveConnections( void );

    inline const NECommonSettings::ListConnections & GetConnectionList( void ) const;

    bool IsValid( void ) const;

    bool AddConnection( const NECommonSettings::sConnection & newConnection );

    int AddConnections( const NECommonSettings::ListConnections & listConnections );

    bool RemoveConnection( const NECommonSettings::sConnection & connection );

    bool ConnectionExist( const NECommonSettings::sConnection & connection );

    void ResetConnectionList( void );

private:

    int findConnection( const NECommonSettings::sConnection & connection ) const;

private:
    NECommonSettings::ListConnections mListConnections;
    CEString        mNickName;
    uint32_t        mCookie;
    uint32_t        mConnectCookie;
    CEDateTime      mTimeConnect;
    CEDateTime      mTimeConnected;
    bool            mIsRegistered;
    bool            mIsConnected;
    mutable CEMutex mLock;

private:
    CEConnectionHandler( const CEConnectionHandler & /*src*/ );
    const CEConnectionHandler & operator = ( const CEConnectionHandler & /*src*/ );
};


inline void CEConnectionHandler::SetNickName(const CEString & nickName)
{
    CELock lock(mLock);
    mIsRegistered = mNickName != nickName ? false : mIsRegistered;
    mNickName   = nickName;
}

inline const CEString & CEConnectionHandler::GetNickName(void) const
{
    CELock lock(mLock);
    return mNickName;
}

inline void CEConnectionHandler::SetConnected(const bool isConnected)
{
    CELock lock(mLock);
    mIsConnected    = isConnected;
    mIsRegistered   = mIsConnected ? mIsRegistered : false;
}

inline bool CEConnectionHandler::GetConnected(void) const
{
    CELock lock(mLock);
    return mIsConnected;
}

inline void CEConnectionHandler::SetRegistered(const bool isRegistered)
{
    CELock lock(mLock);
    mIsRegistered   = isRegistered;
}

inline const bool CEConnectionHandler::GetRegistered(void) const
{
    CELock lock(mLock);
    return mIsRegistered;
}

inline void CEConnectionHandler::SetTimeConnect(const CEDateTime & dateTime)
{
    CELock lock(mLock);
    mTimeConnect    = dateTime;
}

inline const CEDateTime & CEConnectionHandler::GetTimeConnect(void) const
{
    CELock lock(mLock);
    return mTimeConnect;
}

inline void CEConnectionHandler::SetTimeConnected(const CEDateTime & dateTime)
{
    CELock lock(mLock);
    mTimeConnected  = dateTime;
}

inline const CEDateTime & CEConnectionHandler::GetTimeConnected(void) const
{
    CELock lock(mLock);
    return mTimeConnected;
}

inline bool CEConnectionHandler::HasName(void) const
{
    CELock lock(mLock);
    return (mNickName.IsEmpty() == false);
}

inline bool CEConnectionHandler::CanRegister(void) const
{
    CELock lock(mLock);
    return (mNickName.IsEmpty() == false) && (mCookie != NECommonSettings::InvalidCookie) && (mIsRegistered == false);
}

inline bool CEConnectionHandler::CanConnect(void) const
{
    CELock lock(mLock);
    return (mNickName.IsEmpty( ) == false) && (mCookie == NECommonSettings::InvalidCookie);
}

inline void CEConnectionHandler::RemoveConnections(void)
{
    CELock lock(mLock);
    mListConnections.RemoveAll();
}

inline const NECommonSettings::ListConnections & CEConnectionHandler::GetConnectionList(void) const
{
    return mListConnections;
}
