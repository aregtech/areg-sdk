#ifndef AREG_BASE_EVENTENVELOPE_HPP
#define AREG_BASE_EVENTENVELOPE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/EventEnvelope.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Event transport buffer. Extends SharedBuffer to carry a binary
 *              EventHeader at the start of the allocation, followed by the
 *              serialized event payload.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SharedBuffer.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventEnvelope class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Zero-copy event transport buffer. The underlying heap block is
 *          shared across copies (O(1) copy cost via shared_ptr). The binary
 *          layout in the heap allocation is:
 *
 *              [EventHeader 120B][envData 4B][payload...]
 *
 *          EventHeader is always at byte offset 0. The payload stream begins at
 *          data_offset() = offsetof(RawEnvelope, envData) = 120.
 *          header_size() = sizeof(RawEnvelope) = 124.
 *
 *          The class exposes typed accessors grouped as follows:
 *            - Bulk endpoint access  : consumer(), provider(), raw_service()
 *            - Consumer endpoint     : consumer_id/number/thread/version_*
 *            - Provider endpoint     : provider_id/number/thread/version_*
 *            - Shared service fields : service_role/item
 *            - Event routing fields  : target, source, channel, message_id, sequence,
 *                                      result, event_type, call_type, priority, checksum
 *
 *          EventEnvelope is NOT final: Event and its subclasses extend it.
 **/
class AREG_API EventEnvelope : public SharedBuffer
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class Socket;

//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
private:
    static const SequenceNumber     _INVALID_SEQUENCE;
    static const areg::Endpoint     _INVALID_ENDPOINT;
    static const areg::RawService   _INVALID_SERVICE;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs an uninitialised envelope with the given block-growth step.
     *
     * \param   blockSize   Allocation increment, rounded up to areg::BLOCK_SIZE.
     **/
    explicit EventEnvelope(uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs an envelope; pre-allocates the header region when init is true.
     *
     * \param   init        When true, reserves at least blockSize bytes and zeroes the header.
     * \param   blockSize   Allocation increment, rounded up to areg::BLOCK_SIZE.
     **/
    explicit EventEnvelope(bool init, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs an envelope and reserves the given number of bytes.
     *
     * \param   reserveSize Bytes to reserve (header + payload space).
     * \param   blockSize   Allocation increment, rounded up to areg::BLOCK_SIZE.
     **/
    EventEnvelope(uint32_t reserveSize, uint32_t blockSize);

    /**
     * \brief   Constructs an envelope from a raw byte block (e.g. received from socket).
     *          The caller is responsible for ensuring the block starts with a valid EventHeader.
     *
     * \param   buffer      Pointer to raw data (EventHeader + payload).
     * \param   size        Total byte count.
     * \param   blockSize   Allocation increment, rounded up to areg::BLOCK_SIZE.
     **/
    EventEnvelope(const uint8_t* buffer, uint32_t size, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs an envelope from an existing EventHeader and reserves additional space.
     *          Fast path when header data is already available.
     *
     * \param   evtHeader   Event header to copy into the buffer.
     * \param   reserve     Extra bytes to reserve for the payload.
     * \param   blockSize   Allocation increment, rounded up to areg::BLOCK_SIZE.
     **/
    EventEnvelope(const areg::EventHeader& evtHeader, uint32_t reserve, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs an envelope, allocates header + payload space, and writes raw
     *          eventType and priority values into the EventHeader. Used by Event subclasses
     *          that carry a placement-new'd DataClass payload in the tail after the 120-byte
     *          header. Raw types avoid a dependency on areg/component/EventDefs.hpp.
     *
     * \param   eventType       Raw EventType value (uint16_t) to store in the header.
     * \param   prio            Raw EventPriority value (uint8_t) to store in the header.
     * \param   reserveSize     Extra bytes to reserve after the header for the DataClass payload.
     * \param   blockSize       Allocation increment, rounded up to areg::BLOCK_SIZE.
     **/
    EventEnvelope(uint16_t eventType, uint8_t prio, uint32_t reserveSize = 0, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Shares the underlying byte buffer with src (O(1), no data copy).
     **/
    EventEnvelope(const EventEnvelope& src) = default;

    EventEnvelope(EventEnvelope&& src) noexcept = default;

    virtual ~EventEnvelope() noexcept = default;

//////////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////////
public:
    EventEnvelope& operator = (const EventEnvelope& src) = default;

    EventEnvelope& operator = (EventEnvelope&& src) noexcept = default;

    /**
     * \brief   Reads an EventEnvelope from an InStream: reads the header first,
     *          then the payload.
     **/
    friend inline const InStream& operator >> (const InStream& stream, EventEnvelope& input);

    /**
     * \brief   Writes an EventEnvelope to an OutStream: writes the header first,
     *          then the payload. The internal1, internal2, and custom fields are
     *          zeroed on the wire — they are LOCAL-ONLY and must never be transmitted.
     **/
    friend inline OutStream& operator << (OutStream& stream, const EventEnvelope& output);

    /**
     * \brief   Copy-serialises src into the EventEnvelope payload.
     *          Appends src.size_used() raw bytes to the envelope's current write
     *          position. No length prefix is written. The envelope owns its own
     *          independent copy of the payload bytes.
     *
     * \param   env     The destination envelope (mutable: payload grows).
     * \param   src     Source buffer whose payload bytes are copied in.
     * \return  Reference to env for chaining.
     **/
    friend inline EventEnvelope& operator << (EventEnvelope& env, const SharedBuffer& src) noexcept;

    /**
     * \brief   Zero-copy payload view. Exposes the EventEnvelope's payload as a
     *          read-only SharedBuffer view. The two objects share the same heap block
     *          (reference-counted; no byte copy). dst.is_view() == true after this call.
     *          The read cursor in dst is reset to the beginning of the payload.
     *
     *          If the envelope has no payload the call is a no-op and dst is left
     *          in its current state.
     *
     * \param   env     Source envelope (const: only the shared_ptr refcount changes).
     * \param   dst     Target SharedBuffer that receives the view.
     * \return  const reference to env for chaining.
     **/
    friend inline const EventEnvelope& operator >> (const EventEnvelope& env, SharedBuffer& dst) noexcept;

//////////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////////
public:
    virtual void destroy_event() noexcept;

//////////////////////////////////////////////////////////////////////////////
// Header bulk access
//////////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns a pointer to the raw heap block cast as RawEnvelope, or nullptr.
     **/
    [[nodiscard]]
    inline const areg::RawEnvelope* raw_envelope() const noexcept;

    /**
     * \brief   Returns a mutable pointer to the EventHeader at the start of the buffer,
     *          or nullptr if the buffer is not allocated.
     **/
    [[nodiscard]]
    inline areg::EventHeader* header() noexcept;

    /**
     * \brief   Returns a const pointer to the EventHeader at the start of the buffer,
     *          or nullptr if the buffer is not allocated.
     **/
    [[nodiscard]]
    inline const areg::EventHeader* header() const noexcept;

    /**
     * \brief   Returns a const reference to the consumer Endpoint.
     **/
    [[nodiscard]]
    inline const areg::Endpoint& consumer() const noexcept;

    /**
     * \brief   Overwrites the consumer Endpoint in the EventHeader.
     *
     * \param   src     Endpoint to store as the consumer.
     **/
    inline void set_consumer(const areg::Endpoint& src) noexcept;

    /**
     * \brief   Returns a const reference to the provider Endpoint.
     **/
    [[nodiscard]]
    inline const areg::Endpoint& provider() const noexcept;

    /**
     * \brief   Overwrites the provider Endpoint in the EventHeader.
     *
     * \param   src     Endpoint to store as the provider.
     **/
    inline void set_provider(const areg::Endpoint& src) noexcept;

    /**
     * \brief   Returns a const reference to the shared RawService identity.
     **/
    [[nodiscard]]
    inline const areg::RawService& raw_service() const noexcept;

    /**
     * \brief   Overwrites the shared RawService identity in the EventHeader.
     *
     * \param   src     RawService struct to store.
     **/
    inline void set_raw_service(const areg::RawService& src) noexcept;

//////////////////////////////////////////////////////////////////////////////
// Consumer endpoint fields
//////////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the routing cookie of the consumer endpoint.
     **/
    [[nodiscard]]
    inline uint32_t consumer_id() const noexcept;

    /**
     * \brief   Sets the routing cookie of the consumer endpoint.
     **/
    inline void set_consumer_id(uint32_t id) noexcept;

    /**
     * \brief   Returns the unique magic number of the consumer endpoint.
     **/
    [[nodiscard]]
    inline uint32_t consumer_number() const noexcept;

    /**
     * \brief   Sets the unique magic number of the consumer endpoint.
     **/
    inline void set_consumer_number(uint32_t number) noexcept;

    /**
     * \brief   Returns the thread name hash (CRC32) of the consumer endpoint.
     **/
    [[nodiscard]]
    inline uint32_t consumer_thread() const noexcept;

    /**
     * \brief   Sets the thread name hash of the consumer endpoint.
     **/
    inline void set_consumer_thread(uint32_t thread) noexcept;

    /**
     * \brief   Returns the service version major of the consumer endpoint.
     **/
    [[nodiscard]]
    inline uint16_t consumer_version_major() const noexcept;

    /**
     * \brief   Sets the service version major of the consumer endpoint.
     **/
    inline void set_consumer_version_major(uint16_t major) noexcept;

    /**
     * \brief   Returns the service version minor of the consumer endpoint.
     **/
    [[nodiscard]]
    inline uint16_t consumer_version_minor() const noexcept;

    /**
     * \brief   Sets the service version minor of the consumer endpoint.
     **/
    inline void set_consumer_version_minor(uint16_t minor) noexcept;

    /**
     * \brief   Returns the service version patch of the consumer endpoint.
     **/
    [[nodiscard]]
    inline uint16_t consumer_version_patch() const noexcept;

    /**
     * \brief   Sets the service version patch of the consumer endpoint.
     **/
    inline void set_consumer_version_patch(uint16_t patch) noexcept;

    /**
     * \brief   Returns the ServiceType of the consumer endpoint.
     **/
    [[nodiscard]]
    inline uint16_t consumer_type() const noexcept;

    /**
     * \brief   Sets the ServiceType of the consumer endpoint.
     **/
    inline void set_consumer_type(uint16_t type) noexcept;

//////////////////////////////////////////////////////////////////////////////
// Provider endpoint fields
//////////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the routing cookie of the provider endpoint.
     **/
    [[nodiscard]]
    inline uint32_t provider_id() const noexcept;

    /**
     * \brief   Sets the routing cookie of the provider endpoint.
     **/
    inline void set_provider_id(uint32_t id) noexcept;

    /**
     * \brief   Returns the unique magic number of the provider endpoint.
     **/
    [[nodiscard]]
    inline uint32_t provider_number() const noexcept;

    /**
     * \brief   Sets the unique magic number of the provider endpoint.
     **/
    inline void set_provider_number(uint32_t number) noexcept;

    /**
     * \brief   Returns the thread name hash (CRC32) of the provider endpoint.
     **/
    [[nodiscard]]
    inline uint32_t provider_thread() const noexcept;

    /**
     * \brief   Sets the thread name hash of the provider endpoint.
     **/
    inline void set_provider_thread(uint32_t thread) noexcept;

    /**
     * \brief   Returns the service version major of the provider endpoint.
     **/
    [[nodiscard]]
    inline uint16_t provider_version_major() const noexcept;

    /**
     * \brief   Sets the service version major of the provider endpoint.
     **/
    inline void set_provider_version_major(uint16_t major) noexcept;

    /**
     * \brief   Returns the service version minor of the provider endpoint.
     **/
    [[nodiscard]]
    inline uint16_t provider_version_minor() const noexcept;

    /**
     * \brief   Sets the service version minor of the provider endpoint.
     **/
    inline void set_provider_version_minor(uint16_t minor) noexcept;

    /**
     * \brief   Returns the service version patch of the provider endpoint.
     **/
    [[nodiscard]]
    inline uint16_t provider_version_patch() const noexcept;

    /**
     * \brief   Sets the service version patch of the provider endpoint.
     **/
    inline void set_provider_version_patch(uint16_t patch) noexcept;

    /**
     * \brief   Returns the ServiceType of the provider endpoint.
     **/
    [[nodiscard]]
    inline uint16_t provider_type() const noexcept;

    /**
     * \brief   Sets the ServiceType of the provider endpoint.
     **/
    inline void set_provider_type(uint16_t type) noexcept;

//////////////////////////////////////////////////////////////////////////////
// Shared service interface fields (RawService)
//////////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the role name hash of the shared service interface.
     *          Corresponds to the ServiceAddress magic number.
     **/
    [[nodiscard]]
    inline uint32_t service_role() const noexcept;

    /**
     * \brief   Sets the role name hash of the shared service interface.
     **/
    inline void set_service_role(uint32_t role) noexcept;

    /**
     * \brief   Returns the service item hash of the shared service interface.
     *          Corresponds to the ServiceItem magic number.
     **/
    [[nodiscard]]
    inline uint32_t service_item() const noexcept;

    /**
     * \brief   Sets the service item hash of the shared service interface.
     **/
    inline void set_service_item(uint32_t item) noexcept;

//////////////////////////////////////////////////////////////////////////////
// Event routing fields
//////////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the routing destination cookie (mtrouter reads at offset 16).
     *          Request → provider.id; Response/Attribute → consumer.id.
     **/
    [[nodiscard]]
    inline uint32_t target() const noexcept;

    /**
     * \brief   Sets the routing destination cookie.
     **/
    inline void set_target(uint32_t dst) noexcept;

    /**
     * \brief   Returns the routing source cookie.
     *          Request → consumer.id; Response/Attribute → provider.id.
     **/
    [[nodiscard]]
    inline uint32_t source() const noexcept;

    /**
     * \brief   Sets the routing source cookie.
     **/
    inline void set_source(uint32_t src) noexcept;

    /**
     * \brief   Returns the outbound routing channel magic.
     *          0 = local (ServiceManager routes); CRC32(RC_thread_name) = IPC.
     **/
    [[nodiscard]]
    inline uint32_t channel() const noexcept;

    /**
     * \brief   Sets the outbound routing channel magic.
     **/
    inline void set_channel(uint32_t ch) noexcept;

    /**
     * \brief   Returns the message or attribute ID stored in the EventHeader.
     **/
    [[nodiscard]]
    inline uint32_t message_id() const noexcept;

    /**
     * \brief   Sets the message or attribute ID in the EventHeader.
     *
     * \param   msgId   Message ID value to store.
     **/
    inline void set_message_id(uint32_t msgId) noexcept;

    /**
     * \brief   Returns the sequence number stored in the EventHeader.
     **/
    [[nodiscard]]
    inline const SequenceNumber& sequence() const noexcept;

    /**
     * \brief   Sets the sequence number in the EventHeader.
     *
     * \param   seq     Sequence number to store.
     **/
    inline void set_sequence(const SequenceNumber& seq) noexcept;

    /**
     * \brief   Returns the response result code stored in the EventHeader; zero on the request path.
     **/
    [[nodiscard]]
    inline uint32_t result() const noexcept;

    /**
     * \brief   Sets the response result code in the EventHeader.
     *
     * \param   newResult   Result value to store.
     **/
    inline void set_result(uint32_t newResult) noexcept;

    /**
     * \brief   Returns the raw event type value (uint16_t) from the EventHeader.
     *          Cast to areg::EventType at the areg/component layer.
     **/
    [[nodiscard]]
    inline uint16_t event_type() const noexcept;

    /**
     * \brief   Sets the raw event type value in the EventHeader.
     *
     * \param   evtType     Raw EventType value (uint16_t) to store.
     **/
    inline void set_event_type(uint16_t evtType) noexcept;

    /**
     * \brief   Returns the raw call type value (uint8_t) from the EventHeader.
     *          Cast to areg::EventCallType at the areg/component layer.
     **/
    [[nodiscard]]
    inline uint8_t call_type() const noexcept;

    /**
     * \brief   Sets the raw call type value in the EventHeader.
     *
     * \param   callType    Raw EventCallType value (uint8_t) to store.
     **/
    inline void set_call_type(uint8_t callType) noexcept;

    /**
     * \brief   Returns the raw priority value (uint8_t) from the EventHeader.
     *          Cast to areg::EventPriority at the areg/component layer.
     **/
    [[nodiscard]]
    inline uint8_t priority() const noexcept;

    /**
     * \brief   Sets the raw priority value in the EventHeader.
     *
     * \param   prio    Raw EventPriority value (uint8_t) to store.
     **/
    inline void set_priority(uint8_t prio) noexcept;

    /**
     * \brief   Returns the event specific class ID. This value should be set by the top
     *          level class, which created the event. For unknown classes, this should be set
     *          areg::CHECKSUM_IGNORE (0).
     **/
    [[nodiscard]]
    inline uint32_t event_id() const noexcept;

    /**
     * \brief   Sets the event specific class ID. This value should be set by the top
     *          level class, which created the event. For unknown classes, this should be set
     *          areg::CHECKSUM_IGNORE (0).
     * 
     * \param   eventId     The class ID of the event structure. Set areg::CHECKSUM_IGNORE if it is ignored.
     **/
    inline void set_event_id(uint32_t eventId) noexcept;

    /**
     * \brief   Returns the checksum stored in the EventHeader, or areg::CHECKSUM_INVALID.
     **/
    [[nodiscard]]
    inline uint32_t checksum() const noexcept;

    /**
     * \brief   Returns true if the stored checksum is a computed value (not areg::CHECKSUM_INVALID).
     **/
    [[nodiscard]]
    inline bool is_checksum_valid() const noexcept;

    /**
     * \brief   Returns true if the checksum is set to the ignore sentinel (areg::CHECKSUM_IGNORE).
     **/
    [[nodiscard]]
    inline bool is_checksum_ignore() const noexcept;

//////////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Updates the buffer header length and computes the checksum. Call once
     *          the payload has been fully serialised, before sending.
     **/
    void buffer_completion_fix() const;

    /**
     * \brief   Fast-path initialisation from an existing EventHeader. Reuses the current
     *          heap block when it is unshared and large enough; otherwise allocates a new one.
     *
     * \param   evtHeader   Header to copy into the buffer.
     * \param   reserve     Extra bytes to reserve for the payload (payload bytes are NOT written).
     * \return  Writable pointer to the start of the payload area, or nullptr on allocation failure.
     **/
    uint8_t* init_envelope(const areg::EventHeader& evtHeader, uint32_t reserve = 0);

    /**
     * \brief   Creates an independent copy of the envelope. Optionally overrides consumer
     *          or provider endpoints.
     *
     * \param   consumer    When non-null, written to EventHeader.consumer in the clone.
     * \param   provider    When non-null, written to EventHeader.provider in the clone.
     * \return  A new EventEnvelope with its own heap block.
     **/
    [[nodiscard]]
    EventEnvelope clone(const areg::Endpoint* consumer = nullptr, const areg::Endpoint* provider = nullptr) const;

    /**
     * \brief   Returns a mutable pointer to the payload area (bytes immediately after the
     *          120-byte EventHeader). Used by Event subclasses to placement-new a DataClass
     *          into the envelope tail. Returns nullptr if the buffer is not allocated.
     **/
    [[nodiscard]]
    inline uint8_t* payload_ptr() noexcept;

    /**
     * \brief   Returns a const pointer to the payload area after the 120-byte EventHeader.
     **/
    [[nodiscard]]
    inline const uint8_t* payload_ptr() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Initialises a newly allocated raw buffer as a RawEnvelope, optionally
     *          copying payload and header data from the existing buffer.
     *
     * \param[out] newBuffer    Pointer to the fresh heap block.
     * \param   bufLength       Total byte count of newBuffer.
     * \param   makeCopy        When true, copies data from the current buffer.
     * \return  Current used-byte count after initialisation, or INVALID_CURSOR_POSITION.
     **/
    uint32_t init_buffer(uint8_t* newBuffer, uint32_t bufLength, bool makeCopy) const noexcept override;

    /**
     * \brief   Returns the byte offset at which the payload begins.
     *          Equal to offsetof(areg::RawEnvelope, envData) = sizeof(areg::EventHeader) = 120.
     **/
    [[nodiscard]]
    inline uint32_t data_offset() const noexcept override;

    /**
     * \brief   Returns the size of the heap layout struct: sizeof(areg::RawEnvelope) = 124.
     **/
    [[nodiscard]]
    inline uint32_t header_size() const noexcept override;

//////////////////////////////////////////////////////////////////////////
// Hidden helpers
//////////////////////////////////////////////////////////////////////////
private:
    [[nodiscard]]
    inline const areg::EventHeader& _header() const;

    [[nodiscard]]
    inline areg::EventHeader& _header();

    [[nodiscard]]
    inline const areg::RawEnvelope& _raw_envelope() const noexcept;

    [[nodiscard]]
    inline areg::RawEnvelope& _raw_envelope() noexcept;

    [[nodiscard]]
    static uint32_t _checksum_calculate(const areg::RawEnvelope& rawEnvelope) noexcept;
};

//////////////////////////////////////////////////////////////////////////
// EventEnvelope inline implementations
//////////////////////////////////////////////////////////////////////////

inline const areg::EventHeader& EventEnvelope::_header() const
{
    return reinterpret_cast<const areg::EventHeader&>(*byte_buffer());
}

inline areg::EventHeader& EventEnvelope::_header()
{
    ASSERT(mByteBuffer.get() != nullptr);
    return reinterpret_cast<areg::EventHeader&>(*(mByteBuffer.get()));
}

inline const areg::RawEnvelope& EventEnvelope::_raw_envelope() const noexcept
{
    ASSERT(mByteBuffer.get() != nullptr);
    return reinterpret_cast<const areg::RawEnvelope&>(*mByteBuffer.get());
}

inline areg::RawEnvelope& EventEnvelope::_raw_envelope() noexcept
{
    ASSERT(mByteBuffer.get() != nullptr);
    return reinterpret_cast<areg::RawEnvelope&>(*mByteBuffer.get());
}

//////////////////////////////////////////////////////////////////////////
// Header bulk access
//////////////////////////////////////////////////////////////////////////

inline const areg::RawEnvelope* EventEnvelope::raw_envelope() const noexcept
{
    return reinterpret_cast<const areg::RawEnvelope*>(mByteBuffer.get());
}

inline areg::EventHeader* EventEnvelope::header() noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(mByteBuffer.get()) };
    return (env != nullptr ? &env->envHeader : nullptr);
}

inline const areg::EventHeader* EventEnvelope::header() const noexcept
{
    const areg::RawEnvelope* env{ reinterpret_cast<const areg::RawEnvelope*>(mByteBuffer.get()) };
    return (env != nullptr ? &env->envHeader : nullptr);
}

inline const areg::Endpoint& EventEnvelope::consumer() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.consumer : EventEnvelope::_INVALID_ENDPOINT);
}

inline void EventEnvelope::set_consumer(const areg::Endpoint& src) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.consumer = src;
}

inline const areg::Endpoint& EventEnvelope::provider() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.provider : EventEnvelope::_INVALID_ENDPOINT);
}

inline void EventEnvelope::set_provider(const areg::Endpoint& src) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.provider = src;
}

inline const areg::RawService& EventEnvelope::raw_service() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.rawService : EventEnvelope::_INVALID_SERVICE);
}

inline void EventEnvelope::set_raw_service(const areg::RawService& src) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.rawService = src;
}

//////////////////////////////////////////////////////////////////////////
// Consumer endpoint fields
//////////////////////////////////////////////////////////////////////////

inline uint32_t EventEnvelope::consumer_id() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.consumer.id : 0u);
}

inline void EventEnvelope::set_consumer_id(uint32_t id) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.consumer.id = id;
}

inline uint32_t EventEnvelope::consumer_number() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.consumer.number : 0u);
}

inline void EventEnvelope::set_consumer_number(uint32_t number) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.consumer.number = number;
}

inline uint32_t EventEnvelope::consumer_thread() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.consumer.thread : 0u);
}

inline void EventEnvelope::set_consumer_thread(uint32_t thread) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.consumer.thread = thread;
}

inline uint16_t EventEnvelope::consumer_version_major() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.consumer.major : 0u);
}

inline void EventEnvelope::set_consumer_version_major(uint16_t major) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.consumer.major = major;
}

inline uint16_t EventEnvelope::consumer_version_minor() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.consumer.minor : 0u);
}

inline void EventEnvelope::set_consumer_version_minor(uint16_t minor) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.consumer.minor = minor;
}

inline uint16_t EventEnvelope::consumer_version_patch() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.consumer.patch : 0u);
}

inline void EventEnvelope::set_consumer_version_patch(uint16_t patch) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.consumer.patch = patch;
}

inline uint16_t EventEnvelope::consumer_type() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.consumer.type : 0u);
}

inline void EventEnvelope::set_consumer_type(uint16_t type) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.consumer.type = type;
}

//////////////////////////////////////////////////////////////////////////
// Provider endpoint fields
//////////////////////////////////////////////////////////////////////////

inline uint32_t EventEnvelope::provider_id() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.provider.id : 0u);
}

inline void EventEnvelope::set_provider_id(uint32_t id) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.provider.id = id;
}

inline uint32_t EventEnvelope::provider_number() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.provider.number : 0u);
}

inline void EventEnvelope::set_provider_number(uint32_t number) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.provider.number = number;
}

inline uint32_t EventEnvelope::provider_thread() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.provider.thread : 0u);
}

inline void EventEnvelope::set_provider_thread(uint32_t thread) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.provider.thread = thread;
}

inline uint16_t EventEnvelope::provider_version_major() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.provider.major : 0u);
}

inline void EventEnvelope::set_provider_version_major(uint16_t major) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.provider.major = major;
}

inline uint16_t EventEnvelope::provider_version_minor() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.provider.minor : 0u);
}

inline void EventEnvelope::set_provider_version_minor(uint16_t minor) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.provider.minor = minor;
}

inline uint16_t EventEnvelope::provider_version_patch() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.provider.patch : 0u);
}

inline void EventEnvelope::set_provider_version_patch(uint16_t patch) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.provider.patch = patch;
}

inline uint16_t EventEnvelope::provider_type() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.provider.type : 0u);
}

inline void EventEnvelope::set_provider_type(uint16_t type) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.provider.type = type;
}

//////////////////////////////////////////////////////////////////////////
// Shared service interface fields
//////////////////////////////////////////////////////////////////////////

inline uint32_t EventEnvelope::service_role() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.rawService.role : 0u);
}

inline void EventEnvelope::set_service_role(uint32_t role) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.rawService.role = role;
}

inline uint32_t EventEnvelope::service_item() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.rawService.service : 0u);
}

inline void EventEnvelope::set_service_item(uint32_t item) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.rawService.service = item;
}

inline uint32_t EventEnvelope::target() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.target : 0u);
}

inline void EventEnvelope::set_target(uint32_t dst) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.target = dst;
}

inline uint32_t EventEnvelope::source() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.source : 0u);
}

inline void EventEnvelope::set_source(uint32_t src) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.source = src;
}

inline uint32_t EventEnvelope::channel() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.channel : 0u);
}

inline void EventEnvelope::set_channel(uint32_t ch) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.channel = ch;
}

//////////////////////////////////////////////////////////////////////////
// Event routing fields
//////////////////////////////////////////////////////////////////////////

inline uint32_t EventEnvelope::message_id() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.messageId : areg::INVALID_MESSAGE_ID);
}

inline void EventEnvelope::set_message_id(uint32_t msgId) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.messageId = msgId;
}

inline const SequenceNumber& EventEnvelope::sequence() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.sequenceNr : EventEnvelope::_INVALID_SEQUENCE);
}

inline void EventEnvelope::set_sequence(const SequenceNumber& seq) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.sequenceNr = seq;
}

inline uint32_t EventEnvelope::result() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.result : areg::INVALID_VALUE);
}

inline void EventEnvelope::set_result(uint32_t newResult) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.result = newResult;
}

inline uint16_t EventEnvelope::event_type() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.eventType : 0u);
}

inline void EventEnvelope::set_event_type(uint16_t evtType) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.eventType = evtType;
}

inline uint8_t EventEnvelope::call_type() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.callType : 0u);
}

inline void EventEnvelope::set_call_type(uint8_t callType) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.callType = callType;
}

inline uint8_t EventEnvelope::priority() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.priority : 0u);
}

inline void EventEnvelope::set_priority(uint8_t prio) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.priority = prio;
}

inline uint32_t EventEnvelope::event_id() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.eventId : areg::CHECKSUM_IGNORE);
}

inline void EventEnvelope::set_event_id(uint32_t eventId) noexcept
{
    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(byte_buffer()) };
    if (env != nullptr)
        env->envHeader.eventId = eventId;
}

inline uint32_t EventEnvelope::checksum() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? env->envHeader.checksum : areg::CHECKSUM_INVALID);
}

inline bool EventEnvelope::is_checksum_valid() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr) && (env->envHeader.checksum != areg::CHECKSUM_INVALID);
}

inline bool EventEnvelope::is_checksum_ignore() const noexcept
{
    return checksum() == areg::CHECKSUM_IGNORE;
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

inline uint8_t* EventEnvelope::payload_ptr() noexcept
{
    return buffer();
}

inline const uint8_t* EventEnvelope::payload_ptr() const noexcept
{
    return buffer();
}

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t EventEnvelope::data_offset() const noexcept
{
    return offsetof(areg::RawEnvelope, envData);
}

inline uint32_t EventEnvelope::header_size() const noexcept
{
    return sizeof(areg::RawEnvelope);
}

/************************************************************************/
// SharedBuffer interop operators
/************************************************************************/

inline EventEnvelope& operator << (EventEnvelope& env, const SharedBuffer& src) noexcept
{
    const uint32_t len{ src.size_used() };
    if (len > 0u)
    {
        env.write_data(src.buffer(), len);
    }

    return env;
}

inline const EventEnvelope& operator >> (const EventEnvelope& env, SharedBuffer& dst) noexcept
{
    const uint32_t len{ env.size_used() };
    if (len > 0u)
    {
        // viewStart = 0: SharedBuffer data_base already accounts for biOffset (= 120).
        // So view [0, len) within the data area covers exactly the payload bytes.
        env.share_as_view(dst, 0u, len);
    }

    return env;
}

/************************************************************************/
// Friend streaming operators
/************************************************************************/

inline const InStream& operator >> (const InStream& stream, EventEnvelope& input)
{
    if (static_cast<const InStream*>(&stream) == static_cast<const InStream*>(&input))
        return stream;

    areg::EventHeader evtHdr{ };
    if (stream.read(reinterpret_cast<uint8_t*>(&evtHdr), sizeof(areg::EventHeader)) != sizeof(areg::EventHeader))
        return stream;

    evtHdr.internal1 = 0u;
    evtHdr.internal2 = 0u;
    evtHdr.custom    = 0u;

    uint8_t* dst{ input.init_envelope(evtHdr) };
    if (dst != nullptr)
    {
        uint32_t read{ stream.read(dst, evtHdr.bufHeader.biUsed) };
        input.set_size_used(read);
        input.move_to_begin();
    }

    return stream;
}

inline OutStream& operator << (OutStream& stream, const EventEnvelope& output)
{
    if (static_cast<const OutStream*>(&stream) == static_cast<const OutStream*>(&output))
        return stream;

    const areg::RawEnvelope* env{ reinterpret_cast<const areg::RawEnvelope*>(output.mByteBuffer.get()) };
    if (env != nullptr)
    {
        areg::EventHeader wireHdr{ env->envHeader };
        wireHdr.internal1 = 0u;  // LOCAL-ONLY: never transmit DispatcherThread*
        wireHdr.internal2 = 0u;  // LOCAL-ONLY: never transmit EventConsumer*
        wireHdr.custom    = 0u;  // LOCAL-ONLY: never transmit DATA_CLASS cleanup hook
        stream.write(reinterpret_cast<const uint8_t*>(&wireHdr), sizeof(areg::EventHeader));
        stream.write(env->envData, env->envHeader.bufHeader.biUsed);
    }
    else
    {
        areg::EventHeader hdr{ };
        stream.write(reinterpret_cast<const uint8_t*>(&hdr), sizeof(areg::EventHeader));
    }

    return stream;
}

} // namespace areg

#endif  // AREG_BASE_EVENTENVELOPE_HPP
