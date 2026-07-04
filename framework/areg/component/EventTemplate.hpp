#ifndef AREG_COMPONENT_EVENTTEMPLATE_HPP
#define AREG_COMPONENT_EVENTTEMPLATE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
* \file     areg/component/EventTemplate.hpp
* \ingroup  Areg SDK, Automated Real-time Event Grid Software Development Kit 
* \author   Artak Avetyan
* \brief    Areg Platform, Event class template.
*           Base classes for multi-threading communication by
*           processing event by event data. This is possible by defining 
*           different data type in template classes, i.e. in fact instantiate
*           different type of classes and having different runtime information.
*           Use Marco to have proper definition. 
*
************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Event.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/WorkerThread.hpp"

#include <cstring>
// namespace areg {

/************************************************************************
 * \brief   In this file following class MACRO are declared:
 *              1. event object
 *              2. event consumer object
 *          Both classes are templates and used to define custom Event
 *          and custom Event Consumer, which contain and process certain
 *          Event Data object. The Event is dispatched in the thread that
 *          is registered for Event Consumer. To define custom Event, 
 *          declare Data class and define Event with Event Consumer, using
 *          one of predefined MACRO. The Event Data class should have at least
 *          Copy constructor and assigning operator.
 *          For more details, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Custom event object and consumer MACRO.
//////////////////////////////////////////////////////////////////////////

/**
 * \details     Declared template class, having passed DataClass, EventClass
 *              and ConsumerClass names. The extended version is also getting
 *              already running thread name. If thread is not running yet,
 *              instantiation of extended consumer class will not register
 *              in the consumer thread.
 *
 *              Used template approach because of DLL import / export issue.
 *              The MACRO is declaring 2 classes. These classes might be used
 *              in different libraries. Using classes instead of templates
 *              would require import / export definition. Otherwise, the 
 *              consumer class implementation will raise warnings.
 *
 * \example     Custom Event Declaration
 *
 *              In the example bellow, no warnings should be raised
 * \code [.cpp]
 *
 *  class ClassData1
 *  {
 *  public:
 *      uint32_t    mData1;
 *  };
 *
 *  class ClassData2
 *  {
 *   public:
 *      String mData2;
 *  };
 *
 *  // <==  Declare Data1Event and Data1Consumer
 *  AREG_DECLARE_EVENT(Data1, Data1Event, Data1Consumer)
 *
 *  // <==  Declare Data1Event and Data1Consumer, when event consumer is instantiated, 
 *  //      it is registered in "Data2ConsumerWorkerThreadName" worker / component thread
 *  AREG_DECLARE_EVENT_EX(Data2, Data2Event, Data2Consumer, "Data2ConsumerWorkerThreadName");
 *
 *  class __declspec(dllimport) AllConsumers   : public Data1Consumer
 *                                             , public Data2Consumer
 *  {
 *  public:
 *      AllConsumers()          {   ; }
 *      virtual ~AllConsumers() {   ; }
 *  protected:
 *      virtual void process_event(const Data1& data);
 *      virtual void process_event(const Data2& data);
 *      virtual void process_event(const Data3& data);
 *  };
 *
 *  void AllConsumers::process_event( const Data1& data )
 *  {  ;   }
 *
 *  void AllConsumers::process_event( const Data2& data )
 *  {  ;   }
 *
 * \endcode
 **/

//////////////////////////////////////////////////////////////////////////
// Custom event class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Declares Event and Consumer classes. Standard version, without registering Consumer in the thread.
 *          Do not call. Use AREG_DECLARE_EVENT or AREG_DECLARE_EVENT_EX instead.
 * \param   DATA_CLASS      The name of data object type. Can be any class and should have assigning operator available.
 * \param   EventClass      The event class name.
 * \param   ConsumerClass   The event consumer class name.
 * \param   EventKind       The type of event to create.
 **/
#define AREG_DEFINE_EVENT_STANDARD(DATA_CLASS, EventClass, ConsumerClass, EventKind)                                  \
template <class DATA_CLASS> class AregImpl_##EventClass;                                                                \
template <class DATA_CLASS> class AregImpl_##ConsumerClass;                                                             \
template <class DATA_CLASS> class AregImpl_##ConsumerClass##Extended;                                                   \
template <class DATA_CLASS>                                                                                             \
class AregImpl_##ConsumerClass : public areg::EventConsumer                                                             \
{                                                                                                                       \
protected:                                                                                                              \
    AregImpl_##ConsumerClass()  = default;                                                                              \
    ~AregImpl_##ConsumerClass() = default;                                                                              \
protected:                                                                                                              \
    /** \brief  Override to receive and process a dispatched event.                            **/                      \
    virtual void process_event(const DATA_CLASS& data) = 0;                                                             \
private:                                                                                                                \
    void start_event_processing(areg::Event& eventElem) final;                                                          \
    AREG_NOCOPY_NOMOVE(AregImpl_##ConsumerClass);                                                                       \
};                                                                                                                      \
template <class DATA_CLASS>                                                                                             \
class AregImpl_##EventClass final : public areg::Event                                                                  \
{                                                                                                                       \
    friend class AregImpl_##ConsumerClass##Extended<DATA_CLASS>;                                                        \
    AREG_DECLARE_RUNTIME_EVENT(EventClass)                                                                              \
public:                                                                                                                 \
    /** \brief  Constructs a default event (DATA_CLASS value-initialized).                     **/                      \
    explicit AregImpl_##EventClass(areg::EventPriority prio = areg::DefaultPriority);                                   \
    /** \brief  Constructs with data and routes to a specific consumer in the current thread.  **/                      \
    AregImpl_##EventClass(const DATA_CLASS& data,                                                                       \
                          AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                               \
                          areg::EventPriority prio = areg::DefaultPriority);                                            \
    AregImpl_##EventClass(DATA_CLASS&& data,                                                                            \
                          AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                               \
                          areg::EventPriority prio = areg::DefaultPriority) noexcept;                                   \
    /** \brief  Constructs with an explicit event type (for inter-thread dispatch).            **/                      \
    AregImpl_##EventClass(areg::EventType eventType, const DATA_CLASS& data,                                            \
                          areg::EventPriority prio = areg::DefaultPriority);                                            \
    AregImpl_##EventClass(areg::EventType eventType, DATA_CLASS&& data,                                                 \
                          areg::EventPriority prio = areg::DefaultPriority) noexcept;                                   \
    AregImpl_##EventClass(areg::EventType eventType, const DATA_CLASS& data,                                            \
                          AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                               \
                          areg::EventPriority prio = areg::DefaultPriority);                                            \
    AregImpl_##EventClass(areg::EventType eventType, DATA_CLASS&& data,                                                 \
                          AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                               \
                          areg::EventPriority prio = areg::DefaultPriority) noexcept;                                   \
    /** \brief  Constructs with data using the default event kind (EventKind).                 **/                      \
    explicit AregImpl_##EventClass(const DATA_CLASS& data,                                                              \
                                   areg::EventPriority prio = areg::DefaultPriority);                                   \
    explicit AregImpl_##EventClass(DATA_CLASS&& data,                                                                   \
                                   areg::EventPriority prio = areg::DefaultPriority) noexcept;                          \
    ~AregImpl_##EventClass() override;                                                                                  \
public:                                                                                                                 \
    /** \brief  Sends to the registered consumer's thread (CLASS_ID lookup).                   **/                      \
    static inline bool send_event(const DATA_CLASS& data,                                                               \
                                  areg::EventPriority prio = areg::DefaultPriority);                                    \
    static inline bool send_event(DATA_CLASS&& data,                                                                    \
                                  areg::EventPriority prio = areg::DefaultPriority) noexcept;                           \
    /** \brief  Sends with an explicit event type (CLASS_ID lookup).                           **/                      \
    static inline bool send_event(const DATA_CLASS& data, areg::EventType eventType,                                    \
                                  areg::EventPriority prio = areg::DefaultPriority);                                    \
    static inline bool send_event(DATA_CLASS&& data, areg::EventType eventType,                                         \
                                  areg::EventPriority prio = areg::DefaultPriority) noexcept;                           \
    /** \brief  Sends to a specific consumer in the current dispatcher thread.                 **/                      \
    static inline bool send_event(const DATA_CLASS& data,                                                               \
                                  AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                       \
                                  areg::EventPriority prio = areg::DefaultPriority);                                    \
    static inline bool send_event(DATA_CLASS&& data,                                                                    \
                                  AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                       \
                                  areg::EventPriority prio = areg::DefaultPriority) noexcept;                           \
    static inline bool send_event(const DATA_CLASS& data,                                                               \
                                  AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                       \
                                  areg::EventType eventType,                                                            \
                                  areg::EventPriority prio = areg::DefaultPriority);                                    \
    static inline bool send_event(DATA_CLASS&& data,                                                                    \
                                  AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                       \
                                  areg::EventType eventType,                                                            \
                                  areg::EventPriority prio = areg::DefaultPriority) noexcept;                           \
    /** \brief  Sends to a specific dispatcher thread (consumer registered there).             **/                      \
    static inline bool send_event(const DATA_CLASS& data, areg::DispatcherThread& dispThread,                           \
                                  areg::EventPriority prio = areg::DefaultPriority);                                    \
    static inline bool send_event(DATA_CLASS&& data, areg::DispatcherThread& dispThread,                                \
                                  areg::EventPriority prio = areg::DefaultPriority) noexcept;                           \
    /** \brief  Sends to a specific consumer in a specific dispatcher thread.                  **/                      \
    static inline bool send_event(const DATA_CLASS& data,                                                               \
                                  AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                       \
                                  areg::DispatcherThread& dispThread,                                                   \
                                  areg::EventPriority prio = areg::DefaultPriority);                                    \
    static inline bool send_event(DATA_CLASS&& data,                                                                    \
                                  AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                       \
                                  areg::DispatcherThread& dispThread,                                                   \
                                  areg::EventPriority prio = areg::DefaultPriority) noexcept;                           \
    /** \brief  Delivers a pre-constructed event to a specific consumer in a specific thread.  **/                      \
    static inline bool send_event(AregImpl_##EventClass<DATA_CLASS>& event,                                             \
                                  AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                       \
                                  areg::DispatcherThread& dispThread,                                                   \
                                  areg::EventPriority prio = areg::DefaultPriority);                                    \
    /** \brief  Delivers a pre-constructed event using the CLASS_ID consumer lookup.           **/                      \
    static inline bool send_event(AregImpl_##EventClass<DATA_CLASS>& event,                                             \
                                  areg::EventPriority prio = areg::DefaultPriority);                                    \
    /** \brief  Registers the consumer for this event type in the specified thread.            **/                      \
    static inline bool add_listener(AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                     \
                                    const areg::String& whichThread);                                                   \
    static inline bool add_listener(AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                     \
                                    areg::DispatcherThread& dispThread);                                                 \
    /** \brief  Unregisters the consumer for this event type from the specified thread.        **/                      \
    static inline bool remove_listener(AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                  \
                                       const areg::String& whichThread);                                                \
    static inline bool remove_listener(AregImpl_##ConsumerClass<DATA_CLASS>& listener,                                  \
                                       areg::DispatcherThread& dispThread);                                             \
    /** \brief  Read-only access to the event data stored in the payload.                      **/                      \
    [[nodiscard]] inline const DATA_CLASS& data() const noexcept;                                                       \
    /** \brief  Mutable access to the event data stored in the payload.                        **/                      \
    [[nodiscard]] inline DATA_CLASS& data() noexcept;                                                                   \
private:                                                                                                                \
    /** Destructor trampoline stored in EventHeader::custom; called by Event::~Event() when    **/                      \
    /** this buffer's refcount drops to zero from a non-typed owner (e.g. event queue).        **/                      \
    static void _data_destroy(void* ptr) noexcept;                                                                      \
    /** Stores &_data_destroy in EventHeader::custom after each placement-new.                 **/                      \
    inline void _init_hook() noexcept;                                                                                  \
    /** Sets internal1 (DispatcherThread*) then delivers the event. Named lvalue required.       **/                      \
    static bool _send(areg::DispatcherThread& dispThread, AregImpl_##EventClass<DATA_CLASS>& event);                    \
};                                                                                                                      \
using EventClass = AregImpl_##EventClass<DATA_CLASS>;                                                                   \
AREG_IMPLEMENT_EVENT_REGISTRATION_TEMPLATE(template <class DATA_CLASS>, AregImpl_##EventClass<DATA_CLASS>)              \
template <class DATA_CLASS>                                                                                             \
void AregImpl_##ConsumerClass<DATA_CLASS>::start_event_processing(areg::Event& eventElem)                               \
{                                                                                                                       \
    if (eventElem.event_id() == EventClass::CLASS_ID)                                                                   \
    {   process_event(static_cast<EventClass&>(eventElem).data()); }                                                    \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass(areg::EventPriority prio)                                      \
    : areg::Event(EventKind, static_cast<uint32_t>(sizeof(DATA_CLASS)), prio)                                           \
{ new (payload_ptr()) DATA_CLASS(); _init_hook(); set_event_id(AregImpl_##EventClass::CLASS_ID); }                      \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass(const DATA_CLASS& data,                                        \
                                                          AregImpl_##ConsumerClass<DATA_CLASS>& listener,               \
                                                          areg::EventPriority prio)                                     \
    : areg::Event(EventKind, static_cast<uint32_t>(sizeof(DATA_CLASS)), prio)                                           \
{                                                                                                                       \
    new (payload_ptr()) DATA_CLASS(data);                                                                               \
    set_event_consumer(static_cast<areg::EventConsumer*>(&listener));                                                   \
    set_event_id(AregImpl_##EventClass::CLASS_ID);                                                                      \
    _init_hook();                                                                                                       \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass(DATA_CLASS&& data,                                             \
                                                          AregImpl_##ConsumerClass<DATA_CLASS>& listener,               \
                                                          areg::EventPriority prio) noexcept                            \
    : areg::Event(EventKind, static_cast<uint32_t>(sizeof(DATA_CLASS)), prio)                                           \
{                                                                                                                       \
    new (payload_ptr()) DATA_CLASS(std::move(data));                                                                    \
    set_event_consumer(static_cast<areg::EventConsumer*>(&listener));                                                   \
    set_event_id(AregImpl_##EventClass::CLASS_ID);                                                                      \
    _init_hook();                                                                                                       \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass(areg::EventType eventType,                                     \
                                                          const DATA_CLASS& data,                                       \
                                                          areg::EventPriority prio)                                     \
    : areg::Event(eventType, static_cast<uint32_t>(sizeof(DATA_CLASS)), prio)                                           \
{ new (payload_ptr()) DATA_CLASS(data); set_event_id(AregImpl_##EventClass::CLASS_ID); _init_hook(); }                  \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass(areg::EventType eventType,                                     \
                                                          DATA_CLASS&& data,                                            \
                                                          areg::EventPriority prio) noexcept                            \
    : areg::Event(eventType, static_cast<uint32_t>(sizeof(DATA_CLASS)), prio)                                           \
{ new (payload_ptr()) DATA_CLASS(std::move(data)); set_event_id(AregImpl_##EventClass::CLASS_ID); _init_hook(); }       \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass(areg::EventType eventType,                                     \
                                                          const DATA_CLASS& data,                                       \
                                                          AregImpl_##ConsumerClass<DATA_CLASS>& listener,               \
                                                          areg::EventPriority prio)                                     \
    : areg::Event(eventType, static_cast<uint32_t>(sizeof(DATA_CLASS)), prio)                                           \
{                                                                                                                       \
    new (payload_ptr()) DATA_CLASS(data);                                                                               \
    set_event_consumer(static_cast<areg::EventConsumer*>(&listener));                                                   \
    set_event_id(AregImpl_##EventClass::CLASS_ID);                                                                      \
    _init_hook();                                                                                                       \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass(areg::EventType eventType,                                     \
                                                          DATA_CLASS&& data,                                            \
                                                          AregImpl_##ConsumerClass<DATA_CLASS>& listener,               \
                                                          areg::EventPriority prio) noexcept                            \
    : areg::Event(eventType, static_cast<uint32_t>(sizeof(DATA_CLASS)), prio)                                           \
{                                                                                                                       \
    new (payload_ptr()) DATA_CLASS(std::move(data));                                                                    \
    set_event_id(AregImpl_##EventClass::CLASS_ID);                                                                      \
    set_event_consumer(static_cast<areg::EventConsumer*>(&listener));                                                   \
    _init_hook();                                                                                                       \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass(const DATA_CLASS& data,                                        \
                                                          areg::EventPriority prio)                                     \
    : areg::Event(EventKind, static_cast<uint32_t>(sizeof(DATA_CLASS)), prio)                                           \
{ new (payload_ptr()) DATA_CLASS(data); set_event_id(AregImpl_##EventClass::CLASS_ID); _init_hook(); }                  \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass(DATA_CLASS&& data,                                             \
                                                          areg::EventPriority prio) noexcept                            \
    : areg::Event(EventKind, static_cast<uint32_t>(sizeof(DATA_CLASS)), prio)                                           \
{ new (payload_ptr()) DATA_CLASS(std::move(data)); set_event_id(AregImpl_##EventClass::CLASS_ID); _init_hook(); }       \
template <class DATA_CLASS>                                                                                             \
AregImpl_##EventClass<DATA_CLASS>::~AregImpl_##EventClass()                                                             \
{                                                                                                                       \
    areg::EventHeader* _hdr{ header() };                                                                                \
    if ((_hdr != nullptr) && is_unique())                                                                               \
    {                                                                                                                   \
        reinterpret_cast<DATA_CLASS*>(payload_ptr())->~DATA_CLASS();                                                    \
        _hdr->custom = 0;   /* prevent Event::~Event() from calling _data_destroy again */                              \
    }                                                                                                                   \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
void AregImpl_##EventClass<DATA_CLASS>::_data_destroy(void* ptr) noexcept                                               \
{                                                                                                                       \
    reinterpret_cast<DATA_CLASS*>(ptr)->~DATA_CLASS();                                                                  \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline void AregImpl_##EventClass<DATA_CLASS>::_init_hook() noexcept                                                    \
{                                                                                                                       \
    areg::EventHeader* _hdr{ header() };                                                                                \
    if (_hdr != nullptr)                                                                                                \
    {                                                                                                                   \
        using _CleanupFn = void(*)(void*) noexcept;                                                                     \
        static_assert(sizeof(_CleanupFn) <= sizeof(uint64_t),                                                           \
                      "cleanup fn ptr does not fit in EventHeader::custom");                                            \
        _CleanupFn _fn{ &AregImpl_##EventClass<DATA_CLASS>::_data_destroy };                                            \
        std::memcpy(&_hdr->custom, &_fn, sizeof(_fn));                                                                  \
    }                                                                                                                   \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(const DATA_CLASS& data, areg::EventPriority prio)             \
{   return AregImpl_##EventClass<DATA_CLASS>::send_event(data, EventKind, prio); }                                      \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(DATA_CLASS&& data, areg::EventPriority prio) noexcept         \
{   return AregImpl_##EventClass<DATA_CLASS>::send_event(std::move(data), EventKind, prio); }                           \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(const DATA_CLASS& data,                                       \
                                                           areg::EventType eventType,                                   \
                                                           areg::EventPriority prio)                                    \
{                                                                                                                       \
    areg::DispatcherThread* dt{ areg::DispatcherThread::find_consumer_thread(CLASS_ID) };                               \
    if (dt == nullptr) return false;                                                                                    \
    AregImpl_##EventClass<DATA_CLASS> _evt(eventType, data, prio);                                                      \
    return _send(*dt, _evt);                                                                                            \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(DATA_CLASS&& data,                                            \
                                                           areg::EventType eventType,                                   \
                                                           areg::EventPriority prio) noexcept                           \
{                                                                                                                       \
    areg::DispatcherThread* dt{ areg::DispatcherThread::find_consumer_thread(CLASS_ID) };                               \
    if (dt == nullptr) return false;                                                                                    \
    AregImpl_##EventClass<DATA_CLASS> _evt(eventType, std::move(data), prio);                                           \
    return _send(*dt, _evt);                                                                                            \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(const DATA_CLASS& data,                                       \
                                                           AregImpl_##ConsumerClass<DATA_CLASS>& listener,              \
                                                           areg::EventPriority prio)                                    \
{   return AregImpl_##EventClass<DATA_CLASS>::send_event(data, listener, EventKind, prio); }                            \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(DATA_CLASS&& data,                                            \
                                                           AregImpl_##ConsumerClass<DATA_CLASS>& listener,              \
                                                           areg::EventPriority prio) noexcept                           \
{   return AregImpl_##EventClass<DATA_CLASS>::send_event(std::move(data), listener, EventKind, prio); }                 \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(const DATA_CLASS& data,                                       \
                                                           AregImpl_##ConsumerClass<DATA_CLASS>& listener,              \
                                                           areg::EventType eventType,                                   \
                                                           areg::EventPriority prio)                                    \
{                                                                                                                       \
    areg::DispatcherThread& dt{ areg::DispatcherThread::current_dispatcher_thread() };                                  \
    if (!dt.is_valid()) return false;                                                                                   \
    AregImpl_##EventClass<DATA_CLASS> _evt(eventType, data, listener, prio);                                            \
    return _send(dt, _evt);                                                                                             \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(DATA_CLASS&& data,                                            \
                                                           AregImpl_##ConsumerClass<DATA_CLASS>& listener,              \
                                                           areg::EventType eventType,                                   \
                                                           areg::EventPriority prio) noexcept                           \
{                                                                                                                       \
    areg::DispatcherThread& dt{ areg::DispatcherThread::current_dispatcher_thread() };                                  \
    if (!dt.is_valid()) return false;                                                                                   \
    AregImpl_##EventClass<DATA_CLASS> _evt(eventType, std::move(data), listener, prio);                                 \
    return _send(dt, _evt);                                                                                             \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(const DATA_CLASS& data,                                       \
                                                           areg::DispatcherThread& dispThread,                          \
                                                           areg::EventPriority prio)                                    \
{                                                                                                                       \
    if (!dispThread.is_valid()) return false;                                                                           \
    AregImpl_##EventClass<DATA_CLASS> _evt(EventKind, data, prio);                                                      \
    return _send(dispThread, _evt);                                                                                     \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(DATA_CLASS&& data,                                            \
                                                           areg::DispatcherThread& dispThread,                          \
                                                           areg::EventPriority prio) noexcept                           \
{                                                                                                                       \
    if (!dispThread.is_valid()) return false;                                                                           \
    AregImpl_##EventClass<DATA_CLASS> _evt(EventKind, std::move(data), prio);                                           \
    return _send(dispThread, _evt);                                                                                     \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(const DATA_CLASS& data,                                       \
                                                           AregImpl_##ConsumerClass<DATA_CLASS>& listener,              \
                                                           areg::DispatcherThread& dispThread,                          \
                                                           areg::EventPriority prio)                                    \
{                                                                                                                       \
    if (!dispThread.is_valid()) return false;                                                                           \
    AregImpl_##EventClass<DATA_CLASS> _evt(EventKind, data, listener, prio);                                            \
    return _send(dispThread, _evt);                                                                                     \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(DATA_CLASS&& data,                                            \
                                                           AregImpl_##ConsumerClass<DATA_CLASS>& listener,              \
                                                           areg::DispatcherThread& dispThread,                          \
                                                           areg::EventPriority prio) noexcept                           \
{                                                                                                                       \
    if (!dispThread.is_valid()) return false;                                                                           \
    AregImpl_##EventClass<DATA_CLASS> _evt(EventKind, std::move(data), listener, prio);                                 \
    return _send(dispThread, _evt);                                                                                     \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(AregImpl_##EventClass<DATA_CLASS>& event,                     \
                                                           AregImpl_##ConsumerClass<DATA_CLASS>& listener,              \
                                                           areg::DispatcherThread& dispThread,                          \
                                                           areg::EventPriority prio)                                    \
{                                                                                                                       \
    event.set_event_consumer(static_cast<areg::EventConsumer*>(&listener));                                             \
    event.set_event_priority(prio);                                                                                     \
    return _send(dispThread, event);                                                                                    \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event(AregImpl_##EventClass<DATA_CLASS>& event,                     \
                                                           areg::EventPriority prio)                                    \
{                                                                                                                       \
    areg::DispatcherThread* dt{ areg::DispatcherThread::find_consumer_thread(event.event_id()) };                       \
    if (dt == nullptr) { return false; }                                                                                \
    event.set_event_priority(prio);                                                                                     \
    return _send(*dt, event);                                                                                           \
}                                                                                                                       \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::add_listener(AregImpl_##ConsumerClass<DATA_CLASS>& listener,             \
                                                             const areg::String& whichThread)                           \
{   return areg::Event::add_listener(AregImpl_##EventClass<DATA_CLASS>::CLASS_ID, listener, whichThread); }             \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::add_listener(AregImpl_##ConsumerClass<DATA_CLASS>& listener,             \
                                                             areg::DispatcherThread& dispThread)                        \
{   return areg::Event::add_listener(AregImpl_##EventClass<DATA_CLASS>::CLASS_ID, listener, dispThread); }              \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::remove_listener(AregImpl_##ConsumerClass<DATA_CLASS>& listener,          \
                                                                const areg::String& whichThread)                        \
{   return areg::Event::remove_listener(AregImpl_##EventClass<DATA_CLASS>::CLASS_ID, listener, whichThread); }          \
template <class DATA_CLASS>                                                                                             \
inline bool AregImpl_##EventClass<DATA_CLASS>::remove_listener(AregImpl_##ConsumerClass<DATA_CLASS>& listener,          \
                                                                areg::DispatcherThread& dispThread)                     \
{   return areg::Event::remove_listener(AregImpl_##EventClass<DATA_CLASS>::CLASS_ID, listener, dispThread); }           \
template <class DATA_CLASS>                                                                                             \
inline const DATA_CLASS& AregImpl_##EventClass<DATA_CLASS>::data() const noexcept                                       \
{   return *reinterpret_cast<const DATA_CLASS*>(payload_ptr()); }                                                       \
template <class DATA_CLASS>                                                                                             \
inline DATA_CLASS& AregImpl_##EventClass<DATA_CLASS>::data() noexcept                                                   \
{   return *reinterpret_cast<DATA_CLASS*>(payload_ptr()); }                                                             \
template <class DATA_CLASS>                                                                                             \
bool AregImpl_##EventClass<DATA_CLASS>::_send(areg::DispatcherThread& dispThread,                                       \
                                               AregImpl_##EventClass<DATA_CLASS>& event)                                \
{                                                                                                                       \
    event.register_for_thread(&dispThread);                                                                             \
    event.deliver_event();                                                                                              \
    return true;                                                                                                        \
}

/**
 * \brief   Declares Event and Consumer classes. Extended version, with registering Consumer in the thread.
 *          Do not call. Use AREG_DECLARE_EVENT or AREG_DECLARE_EVENT_EX instead.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   ConsumerThreadName  The name of thread where Event is dispatched and Consumer is registered.
 * \param   EventKind           The type of Event. Should be areg::EventType type. Either Internal or External.
 **/
#define AREG_DEFINE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, EventKind)                         \
/**                                                                                                                         **/ \
/** First, declare all in standard macro                                                                                    **/ \
/**                                                                                                                         **/ \
        AREG_DEFINE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, EventKind)                                             \
/**                                                                                                                         **/ \
/** ---- Extended Part ----                                                                                                 **/ \
/** Declaration of Extended version of ConsumerClass, derive from ConsumerClass template                                    **/ \
/** The Extended consumer class is registering consumer int32_t the Worker / Component thread when it is initialized.       **/ \
/** The worker / component thread should already run that we are able to find in resource map                               **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
class AregImpl_##ConsumerClass##Extended: public AregImpl_##ConsumerClass<DATA_CLASS>                                           \
{                                                                                                                               \
protected:                                                                                                                      \
    /**                                                                                                                     **/ \
    /** Constructor. When instantiated, it will register consumer in the worker thread passed by ConsumerThreadName         **/ \
    /** By this, the worker / component thread should already run that we are able to find in resource map                  **/ \
    /**                                                                                                                     **/ \
    AregImpl_##ConsumerClass##Extended();                                                                                       \
    /**                                                                                                                     **/ \
    /** Protected destructor.                                                                                               **/ \
    /**                                                                                                                     **/ \
    ~AregImpl_##ConsumerClass##Extended() = default;                                                                            \
private:                                                                                                                        \
    inline AregImpl_##ConsumerClass##Extended& self()     { return (*this); }                                                   \
private:                                                                                                                        \
    /** Forbidden calls.                                                                                                    **/ \
    DECLARE_NOCOPY_NOMOVE_TEMPLATE(AregImpl_##ConsumerClass##Extended, <DATA_CLASS>);                                           \
};                                                                                                                              \
/**                                                                                                                         **/ \
/** Lookup for thread by name and register consumer having EventClass Runtime Object ID                                     **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
AregImpl_##ConsumerClass##Extended<DATA_CLASS>::AregImpl_##ConsumerClass##Extended()                                            \
    : AregImpl_##ConsumerClass<DATA_CLASS> ( )                                                                                  \
{                                                                                                                               \
    areg::DispatcherThread& dispThread = areg::DispatcherThread::dispatcher_thread(ConsumerThreadName);                         \
    VERIFY( dispThread.register_event_consumer(AregImpl_##EventClass<DATA_CLASS>::CLASS_ID, self()) );                          \
}                                                                                                                               \

/************************************************************************
 * Predefine MACRO.
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// MACRO declaration. Use one of, to declare Event Class and Event Consumer
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   This MACRO is used to declare standard Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   EventKind           The type of Event. Should be areg::EventType type. Either Internal or External.
 **/
#define AREG_DECLARE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, EventKind)                                            \
    /**                                                                                             **/                         \
    /** First declare standard event and event consumer classes                                     **/                         \
    /**                                                                                             **/                         \
    AREG_DEFINE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, EventKind)                                                 \
    /**                                                                                             **/                         \
    /** Since event and event consumer classes are templates, define new types                      **/                         \
    /** Use 'EventClass' to send event and using event function.                                    **/                         \
    /** Use 'ConsumerClass' to implement process_event() function.                                  **/                         \
    /**                                                                                             **/                         \
    using ConsumerClass = AregImpl_##ConsumerClass<DataClass>;                                                                  \


/**
 * \brief   This MACRO is used to declare extended Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *          It differs by passing the Consumer Thread name, if it is predefined.
 *          When this MACRO is called, then Event Consumer is automatically
 *          registered in specified Event Consumer Thread.
 *          The Consumer Thread should be started and running before Consumer
 *          is instantiated.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   ConsumerThreadName  The Thread name of Event Consumer.
 * \param   EventKind           The type of Event. Should be areg::EventType type. Either Internal or External.
 **/
#define AREG_DECLARE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, EventKind)                        \
    /**                                                                                             **/                         \
    /** First declare event and extended event consumer classes                                     **/                         \
    /**                                                                                             **/                         \
    AREG_DEFINE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, EventKind)                             \
    /**                                                                                             **/                         \
    /** Since event and event consumer classes are templates, define new types                      **/                         \
    /** Use 'EventClass' to send event and using event function.                                    **/                         \
    /** Use 'ConsumerClass' to implement process_event() function.                                  **/                         \
    /**                                                                                             **/                         \
    using ConsumerClass = AregImpl_##ConsumerClass##Extended<DataClass>;                                                        \

//////////////////////////////////////////////////////////////////////////
// AREG_DECLARE_INTERNAL_EVENT declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This MACRO is used to declare internal standard Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *          The Event Type by default will be areg::EventType::EventCustomInternal
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 **/
#define AREG_DECLARE_INTERNAL_EVENT(DataClass, EventClass, ConsumerClass)                                                       \
    AREG_DECLARE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, areg::EventType::EventCustomInternal)

/**
 * \brief   This MACRO is used to declare internal extended Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *          The Event Type by default will be areg::EventType::EventCustomInternal.
 *          The Event Consumer will be automatically registered in
 *          specified Thread.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   ConsumerThreadName  The Thread name of Event Consumer.
 **/
#define AREG_DECLARE_INTERNAL_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName)                          \
    AREG_DECLARE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, areg::EventType::EventCustomInternal)

//////////////////////////////////////////////////////////////////////////
// AREG_DECLARE_EXTERNAL_EVENT declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This MACRO is used to declare external standard Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *          The Event Type by default will be areg::EventType::EventCustomExternal
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 **/
#define AREG_DECLARE_EXTERNAL_EVENT(DataClass, EventClass, ConsumerClass)                                                       \
    AREG_DECLARE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, areg::EventType::EventCustomExternal)

/**
 * \brief   This MACRO is used to declare external extended Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *          The Event Type by default will be areg::EventType::EventCustomExternal.
 *          The Event Consumer will be automatically registered in
 *          specified Thread.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   ConsumerThreadName  The Thread name of Event Consumer.
 **/
#define AREG_DECLARE_EXTERNAL_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName)                          \
    AREG_DECLARE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, areg::EventType::EventCustomExternal)

//////////////////////////////////////////////////////////////////////////
// AREG_DECLARE_EVENT declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declares standard Event and Event Consumer classes.
 *          The Event Type is set to areg::EventType::EventCustomExternal.
 *          The DataClass should be already declared before using this MACRO.
 *          Additionally, it will declare and implement 2 classes:
 *          EventClass and ConsumerClass.
 *          EventClass is and event object to communicate
 *          ConsumerClass should be derived and the pure virtual function
 *          process_event(const DataClass & data) should be overwritten.
 *          Use EventClass::send_event(const DataClass& data) function
 *          to send events. But before the consumer should be registered
 *          for this event.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 **/
#define AREG_DECLARE_EVENT(DataClass, EventClass, ConsumerClass)                                                                \
    AREG_DECLARE_EXTERNAL_EVENT(DataClass, EventClass, ConsumerClass)


//////////////////////////////////////////////////////////////////////////
// AREG_DECLARE_EVENT_EX declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declares standard Event and Event Consumer classes, and the
 *          Event Consumer is automatically registered in specified Thread.
 *          The Event Type is set to areg::EventType::EventCustomExternal.
 *          In deference to AREG_DECLARE_EVENT, this MACRO is getting Consumer
 *          Thread name. When consumer is instantiated, it will register
 *          automatically in mentioned thread by searching in resource map.
 *          The thread should be already started, before consumer is instantiated.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   ConsumerThreadName  The Thread name of Event Consumer.
 **/
#define AREG_DECLARE_EVENT_EX(DataClass, EventClass, ConsumerClass, ConsumerThreadName)                                         \
    AREG_DECLARE_EXTERNAL_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName)

// } // namespace areg
#endif  // AREG_COMPONENT_EVENTTEMPLATE_HPP
