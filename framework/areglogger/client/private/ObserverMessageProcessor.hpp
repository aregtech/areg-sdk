#ifndef AREG_AREGLOGGER_CLIENT_PRIVATE_OBSERVERMESSAGEPROCESSOR_HPP
#define AREG_AREGLOGGER_CLIENT_PRIVATE_OBSERVERMESSAGEPROCESSOR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areglogger/client/private/ObserverMessageProcessor.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library API.
 *              The log observer message processor.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    struct LogEntry;
    class MessageEnvelope;
    class DateTime;
    enum class LogPriority : uint16_t;
} // namespace areg

struct ScopeInfo;

namespace areg::logger {
    class LoggerClient;
}

namespace areg::logger {

//////////////////////////////////////////////////////////////////////////
// ObserverMessageProcessor class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Processor for messages received from the log service.
 **/
class ObserverMessageProcessor
{
//////////////////////////////////////////////////////////////////////////
// Default constructor and destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes object.
     * \param   loggerClient    Reference to the LoggerClient instance to process messages for.
     **/
    ObserverMessageProcessor(LoggerClient& loggerClient);
    ~ObserverMessageProcessor() = default;

//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Handles service connection notification with the observer's Cookie ID.
     *
     * \param   msgReceived     Buffer containing the connection message with the observer's Cookie
     *                          ID.
     **/
    void notify_service_connection(const areg::MessageEnvelope& msgReceived);

    /**
     * \brief   Handles notification of the list of connected clients in response to a query.
     *
     * \param   msgReceived     Buffer containing the data of connected clients.
     **/
    void notify_connected_clients(const areg::MessageEnvelope& msgReceived);

    /**
     * \brief   Handles notification of registered scopes with their IDs and message priorities in
     *          response to a query.
     *
     * \param   msgReceived     Buffer containing scope names, IDs, and message priorities for all
     *                          scopes.
     **/
    void notify_log_register_scopes(const areg::MessageEnvelope& msgReceived);

    /**
     * \brief   Handles notification that scope priorities have been updated.
     *
     * \param   msgReceived     Buffer containing scope names, IDs, and updated message priorities.
     **/
    void notify_log_update_scopes(const areg::MessageEnvelope& msgReceived);

    /**
     * \brief   Called when a log source reports that it started or stopped sending its logs.
     *
     * \param   msgReceived     The message to process.
     **/
    void notify_log_source_state(const areg::MessageEnvelope& msgReceived);

    /**
     * \brief   Called when a log source reports that it reloaded its configuration file.
     *
     * \param   msgReceived     The message to process.
     **/
    void notify_log_configuration_restored(const areg::MessageEnvelope& msgReceived);

    /**
     * \brief   Handles a log message notification. Every log message, received from a remote
     *          source or made locally, passes through this method: it is the only place that
     *          queues a log for the database and hands it to the observer.
     *
     * \param   msgReceived     Buffer containing the log message.
     * \return  The identifier the log message gets in the database, or 0 if it is not saved.
     **/
    uint32_t notify_log_message(const areg::MessageEnvelope& msgReceived);

    /**
     * \brief   Returns the database identifier of the log message that `notify_log_message()`
     *          is delivering on the calling thread, or 0 outside a delivery, with the database
     *          off, or if the message was not saved.
     **/
    [[nodiscard]]
    static uint32_t delivered_log_id() noexcept;

    /**
     * \brief   Makes a log message on the observer side and passes it through the same path as a
     *          message received from a remote source. This is the single point of adding a log.
     *
     * \param   cookie      The identifier of the instance the message belongs to.
     * \param   prio        The priority to give the message.
     * \param   timestamp   The moment to stamp the message with. Zero takes the current time.
     * \param   message     The text of the message. Can be nullptr for an empty one.
     * \return  The identifier the log message got in the database, or 0 if it was not saved.
     **/
    uint32_t add_local_log(ITEM_ID cookie, areg::LogPriority prio, TIME64 timestamp, const char * message);

    /**
     * \brief   Queues the deletion of one log message from the database, in order with the logs.
     *          This is the single point of removing a log.
     *
     * \param   logId   The identifier of the log message to delete.
     * \return  Returns true if the deletion is queued.
     **/
    bool remove_log(uint32_t logId);

private:

    //!< Triggered to process client connected message.
    /**
     * \brief   Handles notification when clients connect.
     *
     * \param   msgReceived     Buffer containing connected client data.
     **/
    void _clients_connected(const areg::MessageEnvelope& msgReceived);

    //!< Triggered to process client disconnected message.
    /**
     * \brief   Handles notification when clients disconnect.
     *
     * \param   msgReceived     Buffer containing disconnected client data.
     **/
    void _clients_disconnected(const areg::MessageEnvelope& msgReceived);

    /**
     * \brief   Reads the list of scopes of a scope message and queues it for the database.
     *
     * \param   msgReceived     The message with the number of scopes and the scopes.
     * \param   cookie          The instance the scopes belong to.
     * \param   now             The moment to save with the scopes.
     * \param[out] scopes       The scopes read, to pass to the observer. The caller deletes it.
     * \return  The number of scopes read.
     **/
    uint32_t _save_scopes(const areg::MessageEnvelope& msgReceived, ITEM_ID cookie, const areg::DateTime& now, ScopeInfo*& scopes);

    //!< Initializes the local log message with default values.
    void _init_local_log_message(areg::LogEntry& log, ITEM_ID cookie, TIME64 timestamp = 0) const;

//////////////////////////////////////////////////////////////////////////
// Hidden members.
//////////////////////////////////////////////////////////////////////////
private:
    LoggerClient &  mLoggerClient;  //!< The object of the observer client.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    ObserverMessageProcessor() = delete;
    AREG_NOCOPY_NOMOVE(ObserverMessageProcessor);
};

} // namespace areg::logger

#endif  // AREG_AREGLOGGER_CLIENT_PRIVATE_OBSERVERMESSAGEPROCESSOR_HPP
