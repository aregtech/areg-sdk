#ifndef AREG_IPC_CONNECTIONCONFIGURATION_HPP
#define AREG_IPC_CONNECTIONCONFIGURATION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ConnectionConfiguration.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Remote service connection configuration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/ipc/RemoteServiceDefs.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ConnectionConfiguration class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Loads and manages connection configuration for remote services including protocol type,
 *          host name, and port number.
 **/
class AREG_API ConnectionConfiguration
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes connection configuration with service and connection type names.
     *
     * \param   service         Service name to read configuration properties.
     * \param   connectType     Connection type name to read configuration properties.
     **/
    ConnectionConfiguration( const String & service, const String & connectType );

    /**
     * \brief   Initializes connection configuration with service and connection type enums.
     *
     * \param   service         Service type to read configuration properties.
     * \param   connectType     Connection type to read configuration properties.
     **/
    ConnectionConfiguration(areg::RemoteServiceKind service, areg::ConnectionType connectType);

    virtual ~ConnectionConfiguration() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the remote service name.
     **/
    [[nodiscard]]
    inline const String& remote_service_name() const noexcept;

    /**
     * \brief   Returns the connection type name.
     **/
    [[nodiscard]]
    inline const String& connection_type() const noexcept;

    /**
     * \brief   Returns true if application is configured via file or defaults.
     **/
    [[nodiscard]]
    bool is_configured() const noexcept;

    /**
     * \brief   Returns the connection enabled/disabled flag.
     **/
    [[nodiscard]]
    bool connection_enable_flag() const;

    /**
     * \brief   Sets the connection enabled/disabled flag.
     *
     * \param   is_enabled       Flag indicating whether connection should be enabled.
     **/
    void set_connection_enable(bool is_enabled);

    /**
     * \brief   Returns the connection port number.
     **/
    [[nodiscard]]
    uint16_t connection_port() const noexcept;

    /**
     * \brief   Sets the connection port number.
     *
     * \param   portNr      Port number to set.
     **/
    void set_connection_port(uint16_t portNr);

    /**
     * \brief   Returns the connection address.
     **/
    [[nodiscard]]
    String connection_address() const;

    /**
     * \brief   Sets the connection address.
     *
     * \param   address     Address to set.
     **/
    void set_connection_address(const String& address);

    /**
     * \brief   Sets the connection address and port number.
     *
     * \param   address     Address to set.
     * \param   portNr      Port number to set.
     **/
    void set_connection_data(const String& address, uint16_t portNr);

    /**
     * \brief   Returns the configured SO_SNDBUF size (bytes) for this service connection.
     *          Falls back to the compile-time default (SOCKET_SEND_BUFFER_SIZE) when the
     *          key is absent from areg.init.
     **/
    [[nodiscard]]
    uint32_t socket_send_buffer() const noexcept;

    /**
     * \brief   Returns the configured SO_RCVBUF size (bytes) for this service connection.
     *          Falls back to the compile-time default (SOCKET_RECV_BUFFER_SIZE) when the
     *          key is absent from areg.init.
     **/
    [[nodiscard]]
    uint32_t socket_recv_buffer() const noexcept;

    /**
     * \brief   Returns the configured send/drain batch size for this connection.
     *          Falls back to DEFAULT_BATCH_SIZE when the key is absent from areg.init.
     **/
    [[nodiscard]]
    uint32_t batch_size() const noexcept;

    /**
     * \brief   Returns true if MSG_ZEROCOPY is requested (Linux only).
     *          Falls back to false when the key is absent from areg.init.
     **/
    [[nodiscard]]
    bool zerocopy_enabled() const noexcept;

    /**
     * \brief   Returns the configured zerocopy ring-slot count.
     *          Falls back to DEFAULT_ZEROCOPY_RING_SIZE when the key is absent from areg.init.
     **/
    [[nodiscard]]
    uint32_t zerocopy_ring_size() const noexcept;

    /**
     * \brief   Returns the configured thread-pool pair count.
     *          Falls back to DEFAULT_POOL_PAIRS (0 = no pool) when the key is absent from areg.init.
     **/
    [[nodiscard]]
    uint32_t pool_pairs() const noexcept;

    /**
     * \brief   Returns the configured SO_SNDTIMEO value in milliseconds for this service connection.
     *          Falls back to the compile-time default (SOCKET_SEND_TIMEOUT_MS) when the
     *          key is absent or zero in areg.init.
     **/
    [[nodiscard]]
    uint32_t socket_send_timeout() const noexcept;

    /**
     * \brief   Extracts IP address bytes from connection address.
     *
     * \param[out] field0      First byte of IP address.
     * \param[out] field1      Second byte of IP address.
     * \param[out] field2      Third byte of IP address.
     * \param[out] field3      Fourth byte of IP address.
     * \return  Returns true if valid IP address was extracted.
     **/
    bool connection_ip_address( uint8_t & field0
                               , uint8_t & field1
                               , uint8_t & field2
                               , uint8_t & field3);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
private:
    //! The name of the remote connection service name to look in configuration.
    const String    mServiceName;

    //! The name of the connection type to look in configuration.
    const String    mConnectType;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    ConnectionConfiguration() = delete;
    AREG_NOCOPY_NOMOVE( ConnectionConfiguration );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionConfiguration inline methods.
//////////////////////////////////////////////////////////////////////////

inline const String& ConnectionConfiguration::remote_service_name() const noexcept
{
    return mServiceName;
}

inline const String& ConnectionConfiguration::connection_type() const noexcept
{
    return mConnectType;
}

} // namespace areg
#endif  // AREG_IPC_CONNECTIONCONFIGURATION_HPP
