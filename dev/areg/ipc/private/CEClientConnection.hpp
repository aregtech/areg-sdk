#ifndef AREG_IPC_PRIVATE_CECLIENTCONNECTION_HPP
#define AREG_IPC_PRIVATE_CECLIENTCONNECTION_HPP

/************************************************************************
 * \file        areg/ipc/private/CEClientConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Client Connection class declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/private/CESocketConnectionBase.hpp"

#include "areg/base/CESocketClient.hpp"

//////////////////////////////////////////////////////////////////////////
// CEClientConnection class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Client Socket is used to connect to remote host server,
 *          send and receive data. Before sending or receiving any data,
 *          the socket should be created and as soon as connection is not needed,
 *          it should be closed.
 *          Client socket is using only TCP/IP connection. All other types
 *          and protocols are out of scope of this class and are not considered.
 **/
class AREG_API CEClientConnection   : private   CESocketConnectionBase
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates instance with invalid socket object. Before sending
     *          or receiving data, the socket should be created and connected to remote host.
     **/
    CEClientConnection( void );

    /**
     * \brief   Creates instance of object with invalid socket object. Before sending
     *          or receiving data, the socket should be created and connected to remote host.
     *          When instantiated, it resolves host name and port number. If succeeded to resolve,
     *          it sets resolved IP-address and port number as socket address. If passed hostName is NULL,
     *          it resolve connection for localhost.
     * \param   hostName    Host name or IP-address of remote server to connect. If NULL, the connection
     *                      is setup for localhost.
     * \param   portNr      Port number of remote server to connect, should not be invalid port.
     **/
    CEClientConnection( const char * hostName, unsigned int portNr );

    /**
     * \brief   Creates instance of object with invalid socket object. Before sending
     *          or receiving data, the socket should be created and connected to remote host.
     *          Specified remoteAddress is a remote server address to connect.
     * \param   remoteAddress   Address of remote host to connect.
     **/
    CEClientConnection( const NESocket::CEInterlockedValue & remoteAddress );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEClientConnection( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns cookie of client connection set by server.
     *          Cookie is checked when sending or receiving data as
     *          source or target in Remote Buffer.
     **/
    ITEM_ID GetCookie( void ) const;

    /**
     * \brief   Sets cookie of client connection set by server
     *          Cookie is checked when sending or receiving data as
     *          source or target in Remote Buffer.
     **/
    void SetCookie( ITEM_ID newCookie );

    /**
     * \brief   Return Socket Address object.
     **/
    const NESocket::CEInterlockedValue & GetAddress( void ) const;

    /**
     * \brief   Sets Socket Address. If hostName is not IP-address, it will 
     *          try to resolve first then set. The isServer parameter is needed
     *          to resolve address either for server or for client.
     *          For accepted sockets this call plays no role, because the
     *          the address automatically is resolved when accepting connection.
     * \param   hostName    Host name or IP-address to set. If name is specified,
     *                      first it will be resolved to get IP-address.
     * \param   portNr      Valid port number of socket connection.
     * \return  Returns true if succeeded to resolve and set Socket Address.
     **/
    bool SetAddress( const char * hostName, unsigned short portNr );

    /**
     * \brief   Sets socket address. The address should be either invalid
     *          or already resolved with IP-address.
     * \param   newAddress  The new address to set.
     **/
    void SetAddress( const NESocket::CEInterlockedValue & newAddress );

    /**
     * \brief   Returns true if existing socket descriptor is valid.
     *          The function is not checking socket descriptor validation.
     **/
    bool IsValid( void ) const;

    /**
     * \brief   Returns handle of socket.
     **/
    SOCKETHANDLE GetSocketHandle( void ) const;

public:

    /**
     * \brief   Before sending or receiving any data from remote host,
     *          call this method to create new socket descriptor and 
     *          connect to specified remote host and port.
     * \param   hostName    The name of host to connect.
     * \param   portNr      The valid port number to connect.
     * \return  Returns true if operation succeeded.
     **/
    bool CreateSocket( const char * hostName, unsigned short portNr );

    /**
     * \brief   Before sending or receiving any data from remote host,
     *          call this method to create new socket descriptor and 
     *          connects to existing socket address. The remote host address
     *          and port number should be already set in socket address.
     * \return  Returns true if operation succeeded.
     **/
    bool CreateSocket( void );

    /**
     * \brief   Closes existing socket.
     *          The call will disconnect from remote server.
     **/
    void CloseSocket( void );

public:
    /**
     * \brief   If socket is valid, sends data using existing socket connection and returns length in bytes
     *          of data in Remote Buffer. And returns negative number if either socket is invalid,
     *          or failed to send data to remote host. No data will be sent, if Remote Buffer is empty.
     *          Before sending data, the method will check and validate existing checksum in buffer
     *          structure. And if checksum is invalid, the data will not be sent to remote target.
     *          If checksum is invalid, the returned value is zero.
     *          Note:   The returned value of sent data (used data length) will be different of total buffer length.
     *          Note:   If Remote Buffer is empty, nothing will be sent.
     *          Note:   The call is blocking and method will not return until all data are not sent
     *                  or if data sending fails.
     *          Note:   Check and set checksum before sending data.
     * \param   in_message  The instance of buffer to send. The checksum number of Remote Buffer object
     *                      will be checked before sending. If checksum is invalid, the data will not be sent.
     * \return  Returns length in bytes of data in Remote Buffer sent to remote host. 
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or Remote Buffer object is empty.
     **/
    int SendMessage( const CERemoteMessage & in_message ) const;

    /**
     * \brief   If socket is valid, receives data using existing socket connection and returns length in bytes
     *          of data in Remote Buffer. And returns negative number if either socket is invalid,
     *          or failed to receive data from remote host. If Remote Buffer data is empty or checksum is,
     *          not matching, it will return zero.
     *          Note:   The returned value of received data (used data length) will be different of total buffer length.
     *          Note:   If received Remote Buffer was empty, on output out_message in invalid.
     *          Note:   The call is blocking and method will not return until all data are not received
     *                  or if data receiving fails.
     * \param   out_message The instance of Remote Buffer to receive data. The checksum number of Remote Buffer object
     *                      will be checked after receiving data. If checksum is invalid, the data will invalidated and dropped.
     * \return  Returns length in bytes of data in Remote Buffer received from remote host.
     *          Returns negative number if socket is not valid of failed to send.
     *          Returns zero, if checksum in Remote Buffer was not validated or data in Remote Buffer object is empty.
     **/
    int ReceiveMessage( CERemoteMessage & out_message ) const;

    /**
     * \brief   Sets socket in read-only more, i.e. no send message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    bool DisableSend( void );

    /**
     * \brief   Sets socket in write-only more, i.e. no receive message is possible anymore.
     * \return  Returns true if operation succeeds.
     **/
    bool DisableReceive( void );

    /**
     * \brief   Called by connection service to start connection with remote Service Broker.
     * \return  Returns true if succeeded to connect to remote Service Broker.
     **/
    bool RequestConnectServer( void );

    /**
     * \brief   Called by connection service to disconnect from remote Service Broker.
     *          Once disconnected, all services receive disconnect notification and stop communication
     *          via disconnected Service Broker.
     * \return  Returns true if there was an active connection and the system succeeded to disconnect.
     *          Returns false, if there was no active connection.
     **/
    bool RequestDisconnectServer( void );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The client connection socket
     **/
    CESocketClient  mClientSocket;

    /**
     * \brief   Client connection cookie
     **/
    ITEM_ID     mCookie;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEClientConnection( const CEClientConnection & /*src*/ );
    const CEClientConnection & operator = ( const CEClientConnection & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEClientConnection class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool CEClientConnection::SetAddress(const char * hostName, unsigned short portNr)
{   return mClientSocket.SetAddress(hostName, portNr, false);                   }

inline ITEM_ID CEClientConnection::GetCookie( void ) const
{   return mCookie;                                                             }

inline void CEClientConnection::SetCookie( ITEM_ID newCookie )
{   mCookie = newCookie;                                                        }

inline const NESocket::CEInterlockedValue & CEClientConnection::GetAddress( void ) const
{   return mClientSocket.GetAddress();                                          }

inline void CEClientConnection::SetAddress( const NESocket::CEInterlockedValue & newAddress )
{   mClientSocket.SetAddress(newAddress);                                       }

inline bool CEClientConnection::IsValid( void ) const
{   return mClientSocket.IsValid();                                             }

inline bool CEClientConnection::DisableSend( void )
{   return mClientSocket.DisableSend( );                                        }

inline bool CEClientConnection::DisableReceive( void )
{   return mClientSocket.DisableReceive();                                      }

inline SOCKETHANDLE CEClientConnection::GetSocketHandle( void ) const
{   return mClientSocket.GetHandle();                                           }

inline int CEClientConnection::SendMessage(const CERemoteMessage & in_message) const
{   return CESocketConnectionBase::SendMessage( in_message, mClientSocket);     }

inline int CEClientConnection::ReceiveMessage(CERemoteMessage & out_message) const
{   return CESocketConnectionBase::ReceiveMessage( out_message, mClientSocket); }

#endif  // AREG_IPC_PRIVATE_CECLIENTCONNECTION_HPP
