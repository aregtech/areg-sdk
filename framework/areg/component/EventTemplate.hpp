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
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
#define AREG_DEFINE_EVENT_STANDARD(DATA_CLASS, EventClass, ConsumerClass, EventKind)                                            \
/*****************************************************************************************************************************/ \
/** Declared classes                                                                                                        **/ \
/*****************************************************************************************************************************/ \
template <class DATA_CLASS> class AregImpl_##EventClass;                                                                        \
template <class DATA_CLASS> class AregImpl_##ConsumerClass;                                                                     \
template <class DATA_CLASS> class AregImpl_##ConsumerClass##Extended;                                                           \
/*****************************************************************************************************************************/ \
/** Declaration of ConsumerClass, derive from TEEventConsumer template.                                                     **/ \
/** Does nothing, needed for Runtime Object information and declare process_event() function                                **/ \
/*****************************************************************************************************************************/ \
template <class DATA_CLASS>                                                                                                     \
class AregImpl_##ConsumerClass : public    areg::EventConsumer                                                                  \
{                                                                                                                               \
protected:                                                                                                                      \
    /**                                                                                                                     **/ \
    /** Protected constructor, the object cannot be instantiated                                                            **/ \
    /**                                                                                                                     **/ \
    AregImpl_##ConsumerClass() = default;                                                                                       \
    /**                                                                                                                     **/ \
    /** Protected destructor.                                                                                               **/ \
    /**                                                                                                                     **/ \
    ~AregImpl_##ConsumerClass() = default;                                                                                      \
    /**                                                                                                                     **/ \
    /** Overrides.                                                                                                          **/ \
    /**                                                                                                                     **/ \
protected:                                                                                                                      \
    /**                                                                                                                     **/ \
    /** \brief  Override operation. Implement this function to receive events and make processing.                          **/ \
    /** \param  data    The data, which was passed as an event.                                                             **/ \
    /**                                                                                                                     **/ \
    virtual void process_event(const DATA_CLASS & data) = 0;                                                                    \
private:                                                                                                                        \
    /**                                                                                                                     **/ \
    /** \brief  The method is derived from EventConsumer to receive event, extract data and pass for further processing.    **/ \
    /** \param	eventElem	The Event object to process.                                                                    **/ \
    /**                                                                                                                     **/ \
    virtual void start_event_processing( areg::Event & eventElem ) final;                                                       \
    /*************************************************************************************************************************/ \
    /**       Forbidden calls.                                                                                              **/ \
    /*************************************************************************************************************************/ \
    AREG_NOCOPY_NOMOVE(AregImpl_##ConsumerClass);                                                                               \
};                                                                                                                              \
/*****************************************************************************************************************************/ \
/** Declaration of EventClass template class, derive from EventTemplate template                                            **/ \
/*****************************************************************************************************************************/ \
template <class DATA_CLASS>                                                                                                     \
class AregImpl_##EventClass final : public   areg::Event                                                                        \
{                                                                                                                               \
    /**                                                                                                                     **/ \
    /** Declare extended consumer friend class to be able to access Runtime Class ID private static function                **/ \
    /**                                                                                                                     **/ \
    friend class AregImpl_##ConsumerClass##Extended<DATA_CLASS>;                                                                \
    /**                                                                                                                     **/ \
    /** It is a runtime class. Declare  runtime functions to access runtime information.                                    **/ \
    /** The runtime information will be changed for every data class and on every event declaration.                        **/ \
    /** This ensures finding appropriate consumer for specific event. This is done by having class template.                **/ \
    /**                                                                                                                     **/ \
    AREG_DECLARE_RUNTIME(EventClass)                                                                                            \
protected:                                                                                                                      \
    AregImpl_##EventClass( areg::EventPriority eventPrio = areg::DefaultPriority );                                             \
    /**                                                                                                                     **/ \
    /** \brief  Constructor, gets data object and consumer. Declared for communication within same thread.                  **/ \
    /**         This constructor creates Internal event and should not be used for inter-thread communication.              **/ \
    /**         It assumes that event is created in dispatcher thread and will be dispatched by current thread only.        **/ \
    /**         The consumer object is set directly in event object.                                                        **/ \
    /**                                                                                                                     **/ \
    AregImpl_##EventClass( const DATA_CLASS& data                                                                               \
                         , AregImpl_##ConsumerClass<DATA_CLASS> & listener                                                      \
                         , areg::EventPriority eventPrio = areg::DefaultPriority );                                             \
    /**                                                                                                                     **/ \
    /** \brief  Constructor, gets event type and data object. Declared for special cases in inter-thread communication.     **/ \
    /**         This constructor is protected and the event type can be directly specified.                                 **/ \
    /**                                                                                                                     **/ \
    AregImpl_##EventClass( areg::EventType eventType                                                                            \
                         , const DATA_CLASS & data                                                                              \
                         , areg::EventPriority eventPrio = areg::DefaultPriority );                                             \
    /**                                                                                                                     **/ \
    /** \brief  Constructor, gets event type, data object and consumer.                                                     **/ \
    /**         This constructor is protected and the event type can be directly specified.                                 **/ \
    /**         The consumer object is set directly in event object.                                                        **/ \
    /**                                                                                                                     **/ \
    AregImpl_##EventClass( areg::EventType eventType                                                                            \
                         , const DATA_CLASS & data                                                                              \
                         , AregImpl_##ConsumerClass<DATA_CLASS> & listener                                                      \
                         , areg::EventPriority eventPrio = areg::DefaultPriority );                                             \
    /**                                                                                                                     **/ \
    /** \brief  Constructor, sets event data. DataClass can be any class, including Dummy / Empty class                     **/ \
    /**         This constructor is protected. To send an event, used send_event(const DATA_CLASS&) function instead.       **/ \
    /**                                                                                                                     **/ \
    AregImpl_##EventClass(const DATA_CLASS & data, areg::EventPriority eventPrio = areg::DefaultPriority );                     \
    /**                                                                                                                     **/ \
    /** \brief  Protected destructor. Should not be called directly, call Destroy() instead.                                **/ \
    /**                                                                                                                     **/ \
    virtual ~AregImpl_##EventClass();                                                                                           \
public:                                                                                                                         \
    /**                                                                                                                     **/ \
    /** \brief  Static function. By default, sends an Event specified in MACRO.                                             **/ \
    /**         Before sending, make sure that consumer is registered in thread.                                            **/ \
    /**         Returns true if could send event. Sending fails if for specified event there is no consumer registered.     **/ \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/ \
    /** \param  data        the data object to send as an event.                                                            **/ \
    /** \param  eventPrio   The priority of the event to set. By default, it is Normal.                                     **/ \
    /** \return Returns true if event successfully was sent.                                                                **/ \
    /**                                                                                                                     **/ \
    static inline bool send_event(const DATA_CLASS & data, areg::EventPriority eventPrio = areg::DefaultPriority);              \
    /**                                                                                                                     **/ \
    /** \brief  Sends an Event of specified type. If Internal, make sure that event is send in same thread context.         **/ \
    /**         Before sending, make sure that consumer is registered in thread.                                            **/ \
    /**         Returns true if could send event. Sending fails if for specified event there is no consumer registered.     **/ \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/ \
    /** \param  data        The data object to send as an event.                                                            **/ \
    /** \param  eventType   The type of event to generated and sent.                                                        **/ \
    /** \param  eventPrio   The priority of the event to set. By default, it is Normal.                                     **/ \
    /** \return Returns true if event successfully was sent.                                                                **/ \
    /**                                                                                                                     **/ \
    static inline bool send_event( const DATA_CLASS & data                                                                      \
                                 , areg::EventType eventType                                                                    \
                                 , areg::EventPriority eventPrio = areg::DefaultPriority);                                      \
    /**                                                                                                                     **/ \
    /** \brief  Sends event to specified consumer within same thread.                                                       **/ \
    /**         Call this method if sure that event should be processed within same thread scope.                           **/ \
    /**         Otherwise register consumer in required thread and use calling send_event(data) function.                   **/ \
    /**         Returns true if could send event. Sending fails if current thread is not a dispatching thread.              **/ \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/ \
    /** \param  data        The data object to send as an event.                                                            **/ \
    /** \param  listener    The consumer, which should process event object.                                                **/ \
    /** \param  eventPrio   The priority of the event to set. By default, it is Normal.                                     **/ \
    /** \return Returns true if event successfully was sent.                                                                **/ \
    /**                                                                                                                     **/ \
    static inline bool send_event( const DATA_CLASS & data                                                                      \
                                 , AregImpl_##ConsumerClass<DATA_CLASS> & listener                                              \
                                 , areg::EventPriority eventPrio = areg::DefaultPriority);                                      \
    /**                                                                                                                     **/ \
    /** \brief  Sends event of specified type to specified consumer within same thread.                                     **/ \
    /**         If Event is Internal, make sure that it is sent in the same thread context.                                 **/ \
    /**         Call this method if sure that event should be processed within same thread scope.                           **/ \
    /**         Otherwise register consumer in required thread and use calling send_event(data) function.                   **/ \
    /**         Returns true if could send event. Sending fails if current thread is not a dispatching thread.              **/ \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/ \
    /** \param  data        The data object to send as an event.                                                            **/ \
    /** \param  listener    The consumer, which should process event object.                                                **/ \
    /** \param  eventType   The consumer, which should process event object.                                                **/ \
    /** \param  eventPrio   The priority of the event to set. By default, it is Normal.                                     **/ \
    /** \return Returns true if event successfully was sent.                                                                **/ \
    /**                                                                                                                     **/ \
    static inline bool send_event( const DATA_CLASS & data                                                                      \
                                 , AregImpl_##ConsumerClass<DATA_CLASS> & listener                                              \
                                 , areg::EventType eventType                                                                    \
                                 , areg::EventPriority eventPrio = areg::DefaultPriority);                                      \
    /**                                                                                                                     **/ \
    /** \brief  Sends event to specified dispatcher thread, which should already have registered event consumer.            **/ \
    /**         Call this method if sure that event should be processed within specified thread scope and                   **/ \
    /**         the dispatcher thread has registered consumer for the event.                                                **/ \
    /**         Returns true if could send event.                                                                           **/ \
    /**         It creates event object and pushes it into thread event queue for further processing.                       **/ \
    /** \param  data        The data object to send as an event.                                                            **/ \
    /** \param  dispThread  The Dispatcher Thread, which should receive event.                                              **/ \
    /**                     The event will be dispatched in this thread and the caller should be sure                       **/ \
    /**                     that the consumer is registered in specified thread.                                            **/ \
    /** \param  eventPrio   The priority of the event to set. By default, it is Normal.                                     **/ \
    /** \return Returns true if event successfully was sent.                                                                **/ \
    /**                                                                                                                     **/ \
    static inline bool send_event( const DATA_CLASS & data                                                                      \
                                 , areg::DispatcherThread & dispThread                                                          \
                                 , areg::EventPriority eventPrio = areg::DefaultPriority);                                      \
    /**                                                                                                                     **/ \
    /** \brief  Sends event to specified consumer within specified thread.                                                  **/ \
    /**         Call this method if sure that event should be processed within specified thread scope.                      **/ \
    /**         Returns true if could send event.                                                                           **/ \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/ \
    /** \param  data        The data object to send as an event.                                                            **/ \
    /** \param  listener    The consumer, which should process event object.                                                **/ \
    /** \param  dispThread  The Dispatcher Thread, which should receive event.                                              **/ \
    /**                     The event will be dispatched in this thread and the caller should be sure                       **/ \
    /**                     that the consumer is registered in specified thread.                                            **/ \
    /** \param  eventPrio   The priority of the event to set. By default, it is Normal.                                     **/ \
    /** \return Returns true if event successfully was sent.                                                                **/ \
    /**                                                                                                                     **/ \
    static inline bool send_event( const DATA_CLASS & data                                                                      \
                                 , AregImpl_##ConsumerClass<DATA_CLASS> & listener                                              \
                                 , areg::DispatcherThread & dispThread                                                          \
                                 , areg::EventPriority eventPrio = areg::DefaultPriority);                                      \
    /**                                                                                                                     **/ \
    /** \brief  Delivers a pre-allocated event to the specified consumer in the specified thread.                           **/ \
    /**         Sets listener and priority on the event, then registers and delivers it.                                    **/ \
    /**         Takes ownership of the event pointer. Returns true if the event was delivered.                              **/ \
    /** \param  event       The pre-allocated event to deliver. If nullptr, returns false.                                  **/ \
    /** \param  listener    The consumer that will process the event.                                                       **/ \
    /** \param  dispThread  The dispatcher thread that will receive the event.                                              **/ \
    /** \param  eventPrio   The priority of the event. By default, it is Normal.                                            **/ \
    /**                                                                                                                     **/ \
    static inline bool send_event( AregImpl_##EventClass<DATA_CLASS>* event                                                     \
                                 , AregImpl_##ConsumerClass<DATA_CLASS> & listener                                              \
                                 , areg::DispatcherThread & dispThread                                                          \
                                 , areg::EventPriority eventPrio = areg::DefaultPriority);                                      \
    /**                                                                                                                     **/ \
    /** \brief  Delivers a pre-allocated event to the consumer registered for the event class.                              **/ \
    /**         Takes ownership of the event pointer. Returns true if the event was delivered.                              **/ \
    /** \param  event       The pre-allocated event to deliver. If nullptr, returns false.                                  **/ \
    /** \param  eventPrio   The priority of the event. By default, it is Normal.                                            **/ \
    /**                                                                                                                     **/ \
    static inline bool send_event( AregImpl_##EventClass<DATA_CLASS>* event                                                     \
                                 , areg::EventPriority eventPrio = areg::DefaultPriority);                                      \
    /**                                                                                                                     **/ \
    /** \brief  Registers Consumer in the worker / component thread                                                         **/ \
    /**         Returns true if successfully registered consumer                                                            **/ \
    /**         Returns false if failed to register consumer or consumer was already registered for specified event         **/ \
    /**         If 'whichThread' parameter is null, it will use current thread to register consumer.                        **/ \
    /** \param  listener    The consumer to register for current event runtime type.                                        **/ \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/ \
    /** \param  whichThread The name of thread to register consumer. If null, it will register in current thread.           **/ \
    /** \return Returns true if consumer registration succeeded.                                                            **/ \
    /**         Returns false if thread already has consumer registered or registration failed.                             **/ \
    /**                                                                                                                     **/ \
    static inline bool add_listener(AregImpl_##ConsumerClass<DATA_CLASS> & listener, const areg::String & whichThread);         \
    /**                                                                                                                     **/ \
    /** \brief  Registers Consumer in the worker / component thread                                                         **/ \
    /**         Returns true if successfully registered consumer                                                            **/ \
    /**         Returns false if failed to register consumer or consumer was already registered for specified event         **/ \
    /**         If 'whichThread' parameter is 0, it will use current thread to register consumer.                           **/ \
    /** \param  listener    The consumer to register for current event runtime type.                                        **/ \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/ \
    /** \param  whichThread The ID of thread to register consumer. If zero, it will register in current thread.             **/ \
    /** \return Returns true if consumer registration succeeded.                                                            **/ \
    /**         Returns false if thread already has consumer registered or registration failed.                             **/ \
    /**                                                                                                                     **/ \
    static inline bool add_listener(AregImpl_##ConsumerClass<DATA_CLASS> & listener, id_type whichThread = 0);                  \
    /**                                                                                                                     **/ \
    /** \brief  Registers Consumer in the worker / component thread                                                         **/ \
    /**         Returns true if successfully registered consumer                                                            **/ \
    /**         Returns false if failed to register consumer or consumer was already registered for specified event         **/ \
    /** \param  listener    The consumer to register for current event runtime type.                                        **/ \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/ \
    /** \param	dispThread  The dispatcher thread, which dispatches messages                                                **/ \
    /** \return Returns true if consumer registration succeeded.                                                            **/ \
    /**         Returns false if thread already has consumer registered or registration failed.                             **/ \
    /**                                                                                                                     **/ \
    static inline bool add_listener(AregImpl_##ConsumerClass<DATA_CLASS> & listener, areg::DispatcherThread & dispThread);      \
    /**                                                                                                                     **/ \
    /** \brief  Unregisters Consumer from worker / component thread.                                                        **/ \
    /**         Returns true if successfully removed registered consumer                                                    **/ \
    /**         If 'whichThread' parameter is null, it will use current thread to unregister consumer.                      **/ \
    /** \param  listener    The consumer to unregister for current event runtime type.                                      **/ \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/ \
    /** \param  whichThread The name of thread to unregister consumer. If null, it will unregister in current thread.       **/ \
    /** \return Returns true if consumer is unregistered with success.                                                      **/ \
    /**                                                                                                                     **/ \
    static inline bool remove_listener(AregImpl_##ConsumerClass<DATA_CLASS> & listener, const areg::String & whichThread);      \
    /**                                                                                                                     **/ \
    /** \brief  Unregisters Consumer from worker / component thread.                                                        **/ \
    /**         Returns true if successfully removed registered consumer                                                    **/ \
    /**         If 'whichThread' parameter is zero, it will use current thread to unregister consumer.                      **/ \
    /** \param  listener    The consumer to unregister for current event runtime type.                                      **/ \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/ \
    /** \param  whichThread The ID of thread to unregister consumer. If zero, it will unregister in current thread.         **/ \
    /** \return Returns true if consumer is unregistered with success.                                                      **/ \
    /**                                                                                                                     **/ \
    static inline bool remove_listener(AregImpl_##ConsumerClass<DATA_CLASS> & listener, id_type whichThread = 0);               \
    /**                                                                                                                     **/ \
    /** \brief  Unregisters Consumer from worker / component thread                                                         **/ \
    /**         Returns true if successfully removed registered consumer                                                    **/ \
    /** \param  listener    The consumer to unregister for current event runtime type.                                      **/ \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/ \
    /** \param	dispThread  The dispatcher thread, which dispatches messages                                                **/ \
    /** \return Returns true if consumer is unregistered with success.                                                      **/ \
    /**                                                                                                                     **/ \
    static inline bool remove_listener(AregImpl_##ConsumerClass<DATA_CLASS> & listener, areg::DispatcherThread & dispThread);   \
    /**                                                                                                                     **/ \
    /* \brief	Returns read-only event data.                                                                               **/ \
    /**                                                                                                                     **/ \
    [[nodiscard]]                                                                                                               \
    inline const DATA_CLASS & data() const noexcept;                                                                            \
    /**                                                                                                                     **/ \
    /** \brief  Returns mutable event data. Use after make_event() to fill the data before sending.                         **/ \
    /**                                                                                                                     **/ \
    [[nodiscard]]                                                                                                               \
    inline DATA_CLASS & data() noexcept;                                                                                        \
    /**                                                                                                                     **/ \
    /** \brief  Creates a heap-allocated event with default-constructed data and priority.                                  **/ \
    /**         The returned pointer must not be deleted by the caller; send_event() takes ownership.                       **/ \
    /**                                                                                                                     **/ \
    static AregImpl_##EventClass<DATA_CLASS>* make_event(areg::EventPriority eventPrio = areg::DefaultPriority);                \
    /**                                                                                                                     **/ \
    /** \brief  Creates a heap-allocated event with default-constructed data and priority.                                  **/ \
    /**         Fill the data via data(), then pass the pointer to send_event(event, listener, dispThread).                 **/ \
    /**         The returned pointer must not be deleted by the caller; send_event() takes ownership.                       **/ \
    /**                                                                                                                     **/ \
    static AregImpl_##EventClass<DATA_CLASS>* make_event( AregImpl_##ConsumerClass<DATA_CLASS> & listener                       \
                                                        , areg::EventPriority eventPrio = areg::DefaultPriority);               \
protected:                                                                                                                      \
    /**                                                                                                                     **/ \
    /** \brief  Event data. Class or simple object, which has copy constructor and assignment operator.                     **/ \
    /**                                                                                                                     **/ \
    DATA_CLASS  mData;                                                                                                          \
private:                                                                                                                        \
    /** Private functions. Registers event for specified worker thread and sends event                                      **/ \
    static bool _send(areg::DispatcherThread & dispThread, AregImpl_##EventClass<DATA_CLASS> * eventElem);                      \
private:                                                                                                                        \
    /*************************************************************************************************************************/ \
    /**       Forbidden calls.                                                                                              **/ \
    /*************************************************************************************************************************/ \
    AREG_NOCOPY_NOMOVE(AregImpl_##EventClass);                                                                                  \
};                                                                                                                              \
/*****************************************************************************************************************************/ \
/** Event class type definition.                                                                                            **/ \
/*****************************************************************************************************************************/ \
using EventClass    = AregImpl_##EventClass<DATA_CLASS>;                                                                        \
/*****************************************************************************************************************************/ \
/** Event class implementation                                                                                              **/ \
/*****************************************************************************************************************************/ \
/**                                                                                                                         **/ \
/** Implement Runtime Object overrides and get Runtime Class ID                                                             **/ \
/**                                                                                                                         **/ \
AREG_IMPLEMENT_RUNTIME_TEMPLATE(template <class DATA_CLASS>, AregImpl_##EventClass<DATA_CLASS>, areg::Event, EventClass)        \
/** Default constructor implementation.                                                                                     **/ \
template <class DATA_CLASS>                                                                                                     \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass( areg::EventPriority eventPrio)                                        \
    : areg::Event (EventKind), mData( )                                                                                         \
{ set_event_priority(eventPrio); }                                                                                              \
/** Constructor implementation, pass DataClass object and Event Consumer object. The event is internal.                     **/ \
template <class DATA_CLASS>                                                                                                     \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass( const DATA_CLASS & data                                               \
                                                        , AregImpl_##ConsumerClass<DATA_CLASS> & listener                       \
                                                        , areg::EventPriority eventPrio)                                        \
    : areg::Event (EventKind), mData(data)                                                                                      \
{                                                                                                                               \
    set_event_consumer(static_cast<EventConsumer *>(&listener));                                                                \
    set_event_priority(eventPrio);                                                                                              \
}                                                                                                                               \
/** Constructor implementation, pass DataClass object and event type. By default, the event is external                     **/ \
template <class DATA_CLASS>                                                                                                     \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass( areg::EventType eventType                                             \
                                                        , const DATA_CLASS & data                                               \
                                                        , areg::EventPriority eventPrio)                                        \
    : areg::Event (eventType), mData(data)                                                                                      \
{ set_event_priority(eventPrio); }                                                                                              \
/** Constructor implementation, pass event type, DataClass and consumer objects.                                            **/ \
template <class DATA_CLASS>                                                                                                     \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass( areg::EventType eventType                                             \
                                                        , const DATA_CLASS & data                                               \
                                                        , AregImpl_##ConsumerClass<DATA_CLASS> & listener                       \
                                                        , areg::EventPriority eventPrio)                                        \
    : areg::Event (eventType), mData(data)                                                                                      \
{                                                                                                                               \
    set_event_consumer(static_cast<EventConsumer *>(&listener));                                                                \
    set_event_priority(eventPrio);                                                                                              \
}                                                                                                                               \
/** Constructor implementation, pass DataClass object to Event                                                              **/ \
template <class DATA_CLASS>                                                                                                     \
AregImpl_##EventClass<DATA_CLASS>::AregImpl_##EventClass( const DATA_CLASS & data                                               \
                                          , areg::EventPriority eventPrio)                                                      \
    : areg::Event (EventKind), mData(data)                                                                                      \
{ set_event_priority(eventPrio); }                                                                                              \
/** Destructor implementation. Do nothing                                                                                   **/ \
template <class DATA_CLASS>                                                                                                     \
AregImpl_##EventClass<DATA_CLASS>::~AregImpl_##EventClass()                                                                     \
{ ; }                                                                                                                           \
/**                                                                                                                         **/ \
/** Static function, sends event and pass DataClass object. Before sending, it will search Consumer Thread to forward event **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event( const DATA_CLASS& data                                               \
                                                         , areg::EventPriority eventPrio)                                       \
{                                                                                                                               \
    return AregImpl_##EventClass<DATA_CLASS>::send_event(data, EventKind, eventPrio);                                           \
}                                                                                                                               \
/**                                                                                                                         **/ \
/** Static function, sends event of specified type and pass DataClass object.                                               **/ \
/** If the event type is Internal, make sure that event sent in same thread context to execute immediately.                 **/ \
/** Before sending an event, it will search Consumer Thread to forward the event.                                           **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event( const DATA_CLASS & data                                              \
                                                         , areg::EventType eventType                                            \
                                                         , areg::EventPriority eventPrio)                                       \
{                                                                                                                               \
    DispatcherThread* dispThread = DispatcherThread::find_consumer_thread(_class_id());                                         \
    return ( (dispThread != nullptr) &&                                                                                         \
            _send(*dispThread, DEBUG_NEW AregImpl_##EventClass<DATA_CLASS>(eventType, data, eventPrio)) );                      \
}                                                                                                                               \
/**                                                                                                                         **/ \
/** Static function. Sends a Event of specified type to specified consumer within same thread.                              **/ \
/** Call this method if sure that event should be processed within same thread scope.                                       **/ \
/** Otherwise register consumer in required thread and use calling send_event(data) function.                               **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event( const DATA_CLASS & data                                              \
                                                 , AregImpl_##ConsumerClass<DATA_CLASS> & listener                              \
                                                 , areg::EventPriority eventPrio)                                               \
{                                                                                                                               \
    return AregImpl_##EventClass<DATA_CLASS>::send_event(data, listener, EventKind, eventPrio);                                 \
}                                                                                                                               \
/**                                                                                                                         **/ \
/** Static function. Sends event to specified consumer within same thread.                                                  **/ \
/** Call this method if sure that event should be processed within same thread scope.                                       **/ \
/** Otherwise register consumer in required thread and use calling send_event(data) function.                               **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event( const DATA_CLASS & data                                              \
                                                 , AregImpl_##ConsumerClass<DATA_CLASS> & listener                              \
                                                 , areg::EventType eventType                                                    \
                                                 , areg::EventPriority eventPrio)                                               \
{                                                                                                                               \
    DispatcherThread& dispThread = DispatcherThread::current_dispatcher_thread( );                                              \
    return ( dispThread.is_valid() &&                                                                                           \
            _send(dispThread, DEBUG_NEW AregImpl_##EventClass<DATA_CLASS>(eventType, data, listener, eventPrio)) );             \
}                                                                                                                               \
/**                                                                                                                         **/ \
/** Static function. Sends event to specified dispatcher thread.                                                            **/ \
/** Call this method if sure that specified thread has registered consumer.                                                 **/ \
/** Returns true if could send event.                                                                                       **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event( const DATA_CLASS & data                                              \
                                                 , areg::DispatcherThread & dispThread                                          \
                                                 , areg::EventPriority eventPrio)                                               \
{                                                                                                                               \
    return ( dispThread.is_valid() &&                                                                                           \
            _send(dispThread, DEBUG_NEW AregImpl_##EventClass<DATA_CLASS>(EventKind, data, eventPrio)) );                       \
}                                                                                                                               \
/**                                                                                                                         **/ \
/** Static function. Sends event to specified consumer within specified thread.                                             **/ \
/** Call this method if sure that event should be processed within specified thread scope.                                  **/ \
/** Returns true if could send event. Returns false if the thread is not valid                                              **/ \
/** It will create event object and push into registered consumer thread event queue.                                       **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event( const DATA_CLASS & data                                              \
                                                         , AregImpl_##ConsumerClass<DATA_CLASS> & listener                      \
                                                         , areg::DispatcherThread & dispThread                                  \
                                                         , areg::EventPriority eventPrio)                                       \
{ return ( dispThread.is_valid() &&                                                                                             \
           _send(dispThread, DEBUG_NEW AregImpl_##EventClass<DATA_CLASS>(EventKind, data, listener, eventPrio)) ); }            \
/**                                                                                                                         **/ \
/** Delivers a pre-allocated event to the specified consumer in the specified dispatcher thread.                            **/ \
/** Sets listener and priority on the event, then registers and delivers it (takes ownership).                              **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event( AregImpl_##EventClass<DATA_CLASS>* event                             \
                                                         , AregImpl_##ConsumerClass<DATA_CLASS> & listener                      \
                                                         , DispatcherThread & dispThread                                        \
                                                         , areg::EventPriority eventPrio)                                       \
{                                                                                                                               \
    if (event == nullptr) { return false; }                                                                                     \
    event->set_event_consumer(static_cast<areg::EventConsumer*>(&listener));                                                    \
    event->set_event_priority(eventPrio);                                                                                       \
    return _send(dispThread, event);                                                                                            \
}                                                                                                                               \
/**                                                                                                                         **/ \
/** Delivers a pre-allocated event to the consumer registered for the event.                                                **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::send_event( AregImpl_##EventClass<DATA_CLASS>* event                             \
                                                         , areg::EventPriority eventPrio)                                       \
{                                                                                                                               \
    if (event == nullptr) { return false; }                                                                                     \
    areg::DispatcherThread* dispThread = areg::DispatcherThread::find_consumer_thread(event->class_id());                       \
    if (dispThread == nullptr) { event->destroy(); return false; }                                                              \
    event->set_event_priority(eventPrio);                                                                                       \
    return _send(*dispThread, event);                                                                                           \
}                                                                                                                               \
/**                                                                                                                         **/ \
/** Adds listener (registers consumer) for specified thread name. The thread should be already running.                     **/ \
/** Returns true if successfully registered consumer                                                                        **/ \
/** Returns false if failed to register consumer or consumer was already registered for specified event                     **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::add_listener( AregImpl_##ConsumerClass<DATA_CLASS> & listener                    \
                                                           , const areg::String & whichThread)                                  \
{   return areg::Event::add_listener(AregImpl_##EventClass<DATA_CLASS>::_class_id(), listener, whichThread); }                  \
/**                                                                                                                         **/ \
/** Adds listener (registers consumer) for specified thread ID. The thread should be already running.                       **/ \
/** Returns true if successfully registered consumer                                                                        **/ \
/** Returns false if failed to register consumer or consumer was already registered for specified event                     **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::add_listener( AregImpl_##ConsumerClass<DATA_CLASS> & listener                    \
                                                           , id_type whichThread)                                               \
{   return areg::Event::add_listener(AregImpl_##EventClass<DATA_CLASS>::_class_id(), listener, whichThread); }                  \
/**                                                                                                                         **/ \
/** Adds listener (registers consumer) for specified dispatcher thread. The thread should be already running.               **/ \
/** Returns true if successfully registered consumer                                                                        **/ \
/** Returns false if failed to register consumer or consumer was already registered for specified event                     **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::add_listener( AregImpl_##ConsumerClass<DATA_CLASS> & listener                    \
                                                           , areg::DispatcherThread & dispThread)                               \
{   return areg::Event::add_listener(AregImpl_##EventClass<DATA_CLASS>::_class_id(), listener, dispThread); }                   \
/**                                                                                                                         **/ \
/** Removes registered consumer. If succeed, returns true. Otherwise returns false.                                         **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::remove_listener( AregImpl_##ConsumerClass<DATA_CLASS> & listener                 \
                                                              , const areg::String & whichThread)                               \
{   return areg::Event::remove_listener(AregImpl_##EventClass<DATA_CLASS>::_class_id(), listener, whichThread); }               \
/**                                                                                                                         **/ \
/** Removes registered consumer. If succeed, returns true. Otherwise returns false.                                         **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::remove_listener( AregImpl_##ConsumerClass<DATA_CLASS> & listener                 \
                                                              , id_type whichThread)                                            \
{   return areg::Event::remove_listener(AregImpl_##EventClass<DATA_CLASS>::_class_id(), listener, whichThread); }               \
/**                                                                                                                         **/ \
/** Removes registered consumer. If succeed, returns true. Otherwise returns false.                                         **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline bool AregImpl_##EventClass<DATA_CLASS>::remove_listener( AregImpl_##ConsumerClass<DATA_CLASS> & listener                 \
                                                              , areg::DispatcherThread & dispThread)                            \
{   return areg::Event::remove_listener(AregImpl_##EventClass<DATA_CLASS>::_class_id(), listener, dispThread); }                \
/**                                                                                                                         **/ \
/* \brief	Returns read-only event data.                                                                                   **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline const DATA_CLASS & AregImpl_##EventClass<DATA_CLASS>::data() const noexcept                                              \
{ return mData; }                                                                                                               \
/**                                                                                                                         **/ \
/** Returns mutable event data for in-place filling after pre-allocation.                                                   **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
inline DATA_CLASS & AregImpl_##EventClass<DATA_CLASS>::data() noexcept                                                          \
{ return mData; }                                                                                                               \
/**                                                                                                                         **/ \
/** Factory: creates a heap-allocated event with default-constructed data.                                                  **/ \
/** Caller fills via data(), then passes the pointer to send_event(event, ...) which takes ownership.                       **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
AregImpl_##EventClass<DATA_CLASS>* AregImpl_##EventClass<DATA_CLASS>::make_event(areg::EventPriority eventPrio)                 \
{ return DEBUG_NEW AregImpl_##EventClass<DATA_CLASS>(eventPrio); }                                                              \
/**                                                                                                                         **/ \
/** \brief  Creates a heap-allocated event with default-constructed data and priority.                                      **/ \
/**         Fill the data via data(), then pass the pointer to send_event(event, listener, dispThread).                     **/ \
/**         The returned pointer must not be deleted by the caller; send_event() takes ownership.                           **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
AregImpl_##EventClass<DATA_CLASS>* AregImpl_##EventClass<DATA_CLASS>::make_event(AregImpl_##ConsumerClass<DATA_CLASS>& listener \
                                                                                , areg::EventPriority eventPrio)                \
{                                                                                                                               \
    AregImpl_##EventClass<DATA_CLASS>* event = DEBUG_NEW AregImpl_##EventClass<DATA_CLASS>(eventPrio);                          \
    if (event == nullptr) { return nullptr; }                                                                                   \
    event->set_event_consumer(static_cast<areg::EventConsumer*>(&listener));                                                    \
    return event;                                                                                                               \
}                                                                                                                               \
/** Private static function to forward event. First it will register event for thread (event consumer thread)               **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
bool AregImpl_##EventClass<DATA_CLASS>::_send(areg::DispatcherThread & dispThread, AregImpl_##EventClass<DATA_CLASS> * event)   \
{                                                                                                                               \
    if (event == nullptr) { return false; }                                                                                     \
    static_cast<Event*>(event)->register_for_thread(&dispThread);                                                               \
    static_cast<Event*>(event)->deliver_event();                                                                                \
    return true;                                                                                                                \
}                                                                                                                               \
/*****************************************************************************************************************************/ \
/** Event consumer class implementation                                                                                     **/ \
/*****************************************************************************************************************************/ \
/**                                                                                                                         **/ \
/** \brief  The method is derived from EventConsumer to receive event, extract data and pass for further processing.        **/ \
/**                                                                                                                         **/ \
template <class DATA_CLASS>                                                                                                     \
void AregImpl_##ConsumerClass<DATA_CLASS>::start_event_processing( areg::Event & eventElem )                                    \
{                                                                                                                               \
    EventClass * threadEvent = static_cast<EventClass *>( AREG_RUNTIME_CAST(&eventElem, EventClass) );                          \
    if (threadEvent != nullptr) { process_event(threadEvent->data()); }                                                         \
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
    VERIFY( dispThread.register_event_consumer(AregImpl_##EventClass<DATA_CLASS>::_class_id(), self()) );                       \
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
