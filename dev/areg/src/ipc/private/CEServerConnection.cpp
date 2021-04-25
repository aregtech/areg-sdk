/************************************************************************
 * \file        areg/src/ipc/private/CEServerConnection.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Server Connection class declaration.
 ************************************************************************/
#include "areg/src/ipc/private/CEServerConnection.hpp"
#include "areg/src/ipc/private/NEConnection.hpp"
#include "areg/src/component/NEService.hpp"
#include "areg/src/base/CERemoteMessage.hpp"

const int       CEServerConnection::MASTER_LIST_SIZE    = 64;

CEServerConnection::CEServerConnection( void )
    : CESocketConnectionBase( )
    , mServerSocket         ( )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( CEServerConnection::MASTER_LIST_SIZE )
    , mLock                 ( )
{
    ; // do nothing
}

CEServerConnection::CEServerConnection(const char * hostName, unsigned short portNr)
    : CESocketConnectionBase( )
    , mServerSocket         ( hostName, portNr )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( CEServerConnection::MASTER_LIST_SIZE )
    , mLock                 ( )
{
    ; // do nothing
}

CEServerConnection::CEServerConnection(const NESocket::CEInterlockedValue & serverAddress)
    : CESocketConnectionBase( )
    , mServerSocket         ( serverAddress )
    , mCookieGenerator      ( static_cast<ITEM_ID>(NEService::CookieFirstValid) )
    , mAcceptedConnections  ( )
    , mCookieToSocket       ( )
    , mSocketToCookie       ( )
    , mMasterList           ( CEServerConnection::MASTER_LIST_SIZE )
    , mLock                 ( )
{
    ; // do nothing
}

CEServerConnection::~CEServerConnection(void)
{
    ; // do nothing
}

bool CEServerConnection::CreateSocket(const char * hostName, unsigned short portNr)
{
    CELock lock(mLock);
    return mServerSocket.CreateSocket(hostName, portNr);
}

bool CEServerConnection::CreateSocket(void)
{
    CELock lock(mLock);
    return mServerSocket.CreateSocket();
}

void CEServerConnection::CloseSocket(void)
{
    CELock lock(mLock);
    mMasterList.RemoveAll();
    mCookieToSocket.RemoveAll();
    mSocketToCookie.RemoveAll();
    mAcceptedConnections.RemoveAll();
    mCookieGenerator = static_cast<ITEM_ID>(NEService::CookieFirstValid);

    mServerSocket.CloseSocket();
}

bool CEServerConnection::ServerListen(int maxQueueSize /*= NESocket::MAXIMUM_LISTEN_QUEUE_SIZE */)
{
    return mServerSocket.ListenConnection(maxQueueSize);
}

SOCKETHANDLE CEServerConnection::WaitForConnectionEvent(NESocket::CEInterlockedValue & out_addrNewAccepted)
{
    return mServerSocket.WaitForConnectionEvent(out_addrNewAccepted, static_cast<const SOCKETHANDLE *>(mMasterList), mMasterList.GetSize());
}

bool CEServerConnection::AcceptConnection(CESocketAccepted & clientConnection)
{
    CELock lock(mLock);
    bool result = false;

    if ( mServerSocket.IsValid() )
    {
        if ( clientConnection.IsValid() )
        {
            const SOCKETHANDLE hSocket = clientConnection.GetHandle();
            if ( mMasterList.FindElement(hSocket) == -1)
            {
                ASSERT(mAcceptedConnections.Find( hSocket ) == NULL);
                ASSERT(mSocketToCookie.Find(hSocket) == NULL);

                ITEM_ID cookie = mCookieGenerator ++;
                ASSERT(cookie >= static_cast<ITEM_ID>(NEService::CookieFirstValid));

                mAcceptedConnections.SetKey(hSocket, clientConnection);
                mCookieToSocket.SetKey(cookie, hSocket);
                mSocketToCookie.SetKey(hSocket, cookie);
                mMasterList.Add( hSocket );
                result = true;
            }
            else
            {
                ASSERT(mAcceptedConnections.Find( hSocket ) != NULL);
                ASSERT(mSocketToCookie.Find(hSocket) != NULL);
                result = true;
            }
        }
    }

    return result;
}

void CEServerConnection::RejectConnection(CESocketAccepted & clientConnection)
{
    ITEM_ID cookie = GetCookie(clientConnection.GetHandle());
    CERemoteMessage msgReject = NEConnection::CreateRejectNotify(cookie);
    SendMessage(msgReject, clientConnection);
    CloseConnection(clientConnection);
}

void CEServerConnection::CloseAllConnection(void)
{
    CELock lock( mLock );
    CERemoteMessage msgBeyClient;
    if ( msgBeyClient.InitializeMessage( NEConnection::MessageByeClient.rbHeader ) != NULL )
    {
        msgBeyClient.SetSequenceNr( NEService::SEQUENCE_NUMBER_ANY );
        msgBeyClient.SetSource( NEService::COOKIE_ROUTER );
        msgBeyClient.BufferCompletionFix();
    }

    for ( MAPPOS pos = mAcceptedConnections.GetStartPosition(); pos != NULL; pos = mAcceptedConnections.GetNextPosition(pos) )
    {
        CESocketAccepted clientConnection = mAcceptedConnections.GetValueAt(pos);
        msgBeyClient.SetTarget( GetCookie(clientConnection) );
        SendMessage(msgBeyClient, clientConnection);
    }
    mMasterList.RemoveAll();
    mCookieToSocket.RemoveAll();
    mSocketToCookie.RemoveAll();
    mAcceptedConnections.RemoveAll();

    mCookieGenerator    = static_cast<ITEM_ID>(NEService::CookieFirstValid);
}

ITEM_ID CEServerConnection::GetCookie(const CESocketAccepted & clientSocket) const
{
    return GetCookie(clientSocket.GetHandle());
}

ITEM_ID CEServerConnection::GetCookie(SOCKETHANDLE socketHandle) const
{
    CELock lock( mLock );

    MAPPOS pos = mSocketToCookie.Find( socketHandle );
    return ( pos != NULL ? mSocketToCookie.GetValueAt(pos) : NEService::COOKIE_UNKNOWN );
}

CESocketAccepted CEServerConnection::GetClientByCookie(ITEM_ID clientCookie) const
{
    CELock lock( mLock );

    CESocketAccepted result;
    MAPPOS pos = mCookieToSocket.Find(clientCookie);
    if ( pos != NULL )
        result = GetClientByHandle( mCookieToSocket.GetValueAt(pos) );

    return result;
}

CESocketAccepted CEServerConnection::GetClientByHandle(SOCKETHANDLE clientSocket) const
{
    CELock lock( mLock );

    CESocketAccepted result;
    MAPPOS pos = mAcceptedConnections.Find(clientSocket);
    if ( pos != NULL )
        result = mAcceptedConnections.GetAt(clientSocket);

    return result;
}


void CEServerConnection::CloseConnection(CESocketAccepted & clientConnection)
{
    CELock lock( mLock );

    SOCKETHANDLE hSocket= clientConnection.GetHandle();
    MAPPOS pos = mSocketToCookie.Find(hSocket);
    ITEM_ID cookie = pos != NULL ? mSocketToCookie.GetValueAt(pos) : NEService::COOKIE_UNKNOWN;

    mSocketToCookie.RemoveKey(hSocket);
    mCookieToSocket.RemoveKey(cookie);
    mAcceptedConnections.RemoveKey(hSocket);
    mMasterList.RemoveElement(hSocket, 0);

    clientConnection.CloseSocket();
}
