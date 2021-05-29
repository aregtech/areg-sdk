#ifndef AREG_COMPONENT_TEEVENT_HPP
#define AREG_COMPONENT_TEEVENT_HPP
/************************************************************************
* \file     areg/component/TEEvent.hpp
* \ingroup  AREG SDK, Asynchronous Event Generator Software Development Kit 
* \author   Artak Avetyan (mailto:artak@aregtech.com)
* \brief    AREG Platform, Event class template.
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
#include "areg/base/GEGlobal.h"
#include "areg/component/private/ThreadEventBase.hpp"

#include "areg/component/WorkerThread.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
template <class DATA_CLASS, typename DATA_CLASS_TYPE> class TEEvent;
template <class DATA_CLASS, typename DATA_CLASS_TYPE> class TEEventConsumer;

/************************************************************************
 * \brief   In this file following class templates are declared:
 *              1. TEEvent
 *              2. TEEventConsumer
 *          Both classes are templates and used to define custom Event
 *          and custom Event Consumer, which is contain and process certain
 *          Event Data object. The Event is dispatched in the thread, which
 *          contains registered Event Consumer. To define custom Event, 
 *          declare Data class and define Event with Event Consumer, using
 *          one of predefined MACRO. The Event Data class should have at least
 *          Copy constructor and assigning operator.
 *          For more details, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TEEvent<DATA_CLASS, DATA_CLASS_TYPE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event class template. Declares Runtime Event object, which
 *          is used for multi-threading communication. The type DATA_CLASS
 *          is a custom Event Data class, which contained in Event template.
 *          The DATA_CLASS object should have copy constructor and assigning operator
 *          available for public access. Otherwise error will raise during compilation.
 *          Before sending an event, the consumer object should be registered in appropriate
 *          Thread (WorkerThread or ComponentThread) or event will be sent to current thread.
 *          The Internal Event can be used only for communication between component thread
 *          and its worker thread, as well between worker threads belonging to same component.
 *          Any other communication is impossible.
 * 
 * \tparam  DATA_CLASS          The Event Data class. Should have public available
 *                              default constructor, copy constructor and assigning operator.
 * \tparam  DATA_CLASS_TYPE     The type of DATA_CLASS to get and set values.
 *                              By default, it is constant of reference
 **/
template <class DATA_CLASS, typename DATA_CLASS_TYPE = const DATA_CLASS &>
class TEEvent  : public ThreadEventBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected, not for public assess.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor, can be called only be child class.
     *          By default, it creates custom external type event (Event::EventCustomExternal)
     **/
    TEEvent( void );
    /**
     * \brief   Copies DATA_CLASS object as event data. DATA_CLASS should have available assigning operator.
     *          By default, it is creating external type event (Event::EventCustomExternal)
     **/
    TEEvent( DATA_CLASS_TYPE data );
    /**
     * \brief	Creates event of specified type
     * \param	eventType	Event type to set.
     **/
    TEEvent( Event::eEventType eventType );
    /**
     * \brief	Creates event of specified type and sets the event data.
     * \param	eventType	Event type to set.
     * \param   data        The Event Data object to store in event.
     **/
    TEEvent( Event::eEventType eventType, DATA_CLASS_TYPE data );
    /**
     * \brief   Destructor
     **/
    ~TEEvent( void );

//////////////////////////////////////////////////////////////////////////
// Operations to set and get data
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Sets even data.
     * \param	newData	    The data to set. Data should have assignment
     *                      operator to set type DATA_CLASS_TYPE of data to DATA_CLASS
     **/
    void setData( DATA_CLASS_TYPE newData );
    /**
     * \brief	Returns read-only event data.
     **/
    const DATA_CLASS & getData( void ) const;
    /**
     * \brief	Returns event data, which can be modified.
     **/
    DATA_CLASS & getData( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Event data. Can be class or any other simple object.
     *          DATA_CLASS should have copy constructor, should have
     *          assignment operator available and should be able to
     *          assign DATA_CLASS_TYPE to DATA_CLASS.
     **/
    DATA_CLASS  mData;
};

//////////////////////////////////////////////////////////////////////////
// TEEventConsumer<DATA_CLASS, DATA_CLASS_TYPE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event Consumer template class.
 *          It is pure virtual class and the function processEvent() should be
 *          overwritten for every new event declaration.
 *          The function processEvent() is triggered in the thread for which
 *          the consumer is registered.
 *          To register consumer for certain thread either used appropriate
 *          addListener() function available in Event class or use
 *          INITIALIZE_EVENT_WITH_THREAD() MACRO, declared bellow. If Consumer
 *          is not registered in any thread (or group of worker threads within
 *          one component thread), the event will not be delivered and
 *          no processEvent() function will be triggered.
 *          The consumer should be instantiated after worker thread startup.
 *          Do not declare it as a static variable.
 * 
 * \tparam  DATA_CLASS  The Event Data class. Should have public available
 *                      default constructor, copy constructor and assigning operator.
 **/
template <class DATA_CLASS, typename DATA_CLASS_TYPE = const DATA_CLASS &>
class TEEventConsumer  : public ThreadEventConsumerBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected. Is called only from child class
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    TEEventConsumer( void );
    /**
     * \brief   Destructor
     **/
    ~TEEventConsumer( void );

//////////////////////////////////////////////////////////////////////////
// Override operations. Implement in derived class.
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// TEEventConsumer<DATA_CLASS, DATA_CLASS_TYPE> interface overrides.
/************************************************************************/
    /**
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    virtual void processEvent( DATA_CLASS_TYPE data ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden function call.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief	This method is derived from IEEventConsumer.
     *          It is hidden. The function is receiving event
     *          extracts data and triggers processEvent(DATA_CLASS_TYPE)
     * \param	eventElem	The Event object to process.
     **/
    virtual void startEventProcessing( Event & eventElem );
};

//////////////////////////////////////////////////////////////////////////
// Implementations
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEEvent<DATA_CLASS, DATA_CLASS_TYPE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
TEEvent<DATA_CLASS, DATA_CLASS_TYPE>::TEEvent( void )
    : ThreadEventBase (Event::EventCustomExternal)
    , mData ( )
{   ;   }

template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
TEEvent<DATA_CLASS, DATA_CLASS_TYPE>::TEEvent( DATA_CLASS_TYPE data )
    : ThreadEventBase (Event::EventCustomExternal)
    , mData (data)
{   ;   }

template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
TEEvent<DATA_CLASS, DATA_CLASS_TYPE>::TEEvent( Event::eEventType eventType )
    : ThreadEventBase (eventType)
    , mData ( )
{   ;   }

template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
TEEvent<DATA_CLASS, DATA_CLASS_TYPE>::TEEvent( Event::eEventType eventType, DATA_CLASS_TYPE data )
    : ThreadEventBase (eventType)
    , mData (data)
{   ;   }

template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
TEEvent<DATA_CLASS, DATA_CLASS_TYPE>::~TEEvent( void )
{   ;   }

template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
inline void TEEvent<DATA_CLASS, DATA_CLASS_TYPE>::setData( DATA_CLASS_TYPE newData )
{   mData   = newData;  }

template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
inline const DATA_CLASS & TEEvent<DATA_CLASS, DATA_CLASS_TYPE>::getData( void ) const
{   return mData;       }

template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
inline DATA_CLASS & TEEvent<DATA_CLASS, DATA_CLASS_TYPE>::getData( void )
{   return mData;       }

//////////////////////////////////////////////////////////////////////////
// TEWorkerConsumer<DATA_CLASS, DATA_CLASS_TYPE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
TEEventConsumer<DATA_CLASS, DATA_CLASS_TYPE>::TEEventConsumer( void )
    : ThreadEventConsumerBase ( )
{   ;   }

template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
TEEventConsumer<DATA_CLASS, DATA_CLASS_TYPE>::~TEEventConsumer( void )
{   ;   }

template <class DATA_CLASS, typename DATA_CLASS_TYPE /*= const DATA_CLASS &*/>
void TEEventConsumer<DATA_CLASS, DATA_CLASS_TYPE>::startEventProcessing( Event & eventElem )
{
    TEEvent<DATA_CLASS, DATA_CLASS_TYPE>* threadEvent = static_cast<TEEvent<DATA_CLASS, DATA_CLASS_TYPE> *>( RUNTIME_CAST(&eventElem, ThreadEventBase) );
    if (threadEvent != NULL)
    {
        processEvent(threadEvent->getData());
    }
}


//////////////////////////////////////////////////////////////////////////
// Main predefined MACRO.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Internal MACRO. Do not use
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
 *      unsigned int    mData1;
 *  };
 *
 *  class ClassData2
 *  {
 *   public:
 *      String mData2;
 *  };
 *
 *  // <==  Declare Data1Event and Data1Consumer
 *  DECLARE_EVENT(Data1, Data1Event, Data1Consumer)
 *
 *  // <==  Declare Data1Event and Data1Consumer, when event consumer is instantiated, 
 *  //      it is registered in "Data2ConsumerWorkerThreadName" worker / component thread
 *  DECLARE_EVENT_EX(Data2, Data2Event, Data2Consumer, "Data2ConsumerWorkerThreadName");
 *
 *  class __declspec(dllimport) AllConsumers   : public Data1Consumer
 *                                             , public Data2Consumer
 *  {
 *  public:
 *      AllConsumers( void )          {   ; }
 *      virtual ~AllConsumers( void ) {   ; }
 *  protected:
 *      virtual void processEvent(const Data1& data);
 *      virtual void processEvent(const Data2& data);
 *      virtual void processEvent(const Data3& data);
 *  };
 *
 *  void AllConsumers::processEvent( const Data1& data )
 *  {  ;   }
 *
 *  void AllConsumers::processEvent( const Data2& data )
 *  {  ;   }
 *
 * \endcode
 **/

//////////////////////////////////////////////////////////////////////////
// Custom event class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Declares Event and Consumer classes. Standard version, without registering Consumer in the thread.
 *          Do not call. Use DECLARE_EVENT or DECLARE_EVENT_EX instead.
 * \param   DATA_CLASS      The name of data object type. Can be any class and should have assigning operator available.
 * \param   EventClass      The event class name.
 * \param   ConsumerClass   The event consumer class name.
 * \param   EventType       The type of event to create.
 **/
#define DEFINE_DECLARE_EVENT_STANDARD(DATA_CLASS, EventClass, ConsumerClass, EventType)                                                             \
template <class DATA_CLASS> class __##EventClass;                                                                                                   \
template <class DATA_CLASS> class __##ConsumerClass;                                                                                                \
template <class DATA_CLASS> class __##ConsumerClass##Extended;                                                                                      \
/**                                                                                                                         **/                     \
/** Declaration of EventClass template class, derive from TEEvent template                                                  **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
class __##EventClass: public TEEvent<DATA_CLASS, const DATA_CLASS &>                                                                                \
{                                                                                                                                                   \
    /**                                                                                                                     **/                     \
    /** Declare extended consumer friend class to be able to access Runtime Class ID private static function                **/                     \
    /**                                                                                                                     **/                     \
    friend class __##ConsumerClass##Extended<DATA_CLASS>;                                                                                           \
    /**                                                                                                                     **/                     \
    /** It is a runtime class. Declare  runtime functions to access runtime information.                                    **/                     \
    /** The runtime information will be changed for every data class and on every event declaration.                        **/                     \
    /** This ensures finding appropriate consumer for specific event. This is done by having class template.                **/                     \
    /**                                                                                                                     **/                     \
    DECLARE_RUNTIME(__##EventClass)                                                                                                                 \
protected:                                                                                                                                          \
    /**                                                                                                                     **/                     \
    /** \brief  Constructor, gets data object and consumer. Declared for communication within same thread.                  **/                     \
    /**         This constructor creates Internal event and should not be used for inter-thread communication.              **/                     \
    /**         It assumes that event is created in dispatcher thread and will be dispatched by current thread only.        **/                     \
    /**         The consumer object is set directly in event object.                                                        **/                     \
    /**                                                                                                                     **/                     \
    __##EventClass(const DATA_CLASS& data, __##ConsumerClass<DATA_CLASS>& listener);                                                                \
    /**                                                                                                                     **/                     \
    /** \brief  Constructor, gets event type and data object. Declared for special cases in inter-thread communication.     **/                     \
    /**         This constructor is protected and the event type can be directly specified.                                 **/                     \
    /**                                                                                                                     **/                     \
    __##EventClass(Event::eEventType eventType, const DATA_CLASS & data);                                                                           \
    /**                                                                                                                     **/                     \
    /** \brief  Constructor, gets event type, data object and consumer.                                                     **/                     \
    /**         This constructor is protected and the event type can be directly specified.                                 **/                     \
    /**         The consumer object is set directly in event object.                                                        **/                     \
    /**                                                                                                                     **/                     \
    __##EventClass(Event::eEventType eventType, const DATA_CLASS & data, __##ConsumerClass<DATA_CLASS> & listener);                                 \
    /**                                                                                                                     **/                     \
    /** \brief  Constructor, sets event data. DataClass can be any class, including Dummy / Empty class                     **/                     \
    /**         This constructor is protected. To send an event, used sendEvent(const DATA_CLASS&) function instead.        **/                     \
    /**                                                                                                                     **/                     \
    __##EventClass(const DATA_CLASS & data);                                                                                                        \
    /**                                                                                                                     **/                     \
    /** \brief  Protected destructor. Should not be called directly, call Destroy() instead.                                **/                     \
    /**                                                                                                                     **/                     \
    virtual ~__##EventClass( void );                                                                                                                \
public:                                                                                                                                             \
    /**                                                                                                                     **/                     \
    /** \brief  Static function. By default, sends an Event specified in MACRO.                                             **/                     \
    /**         Before sending, make sure that consumer is registered in thread.                                            **/                     \
    /**         Returns true if could send event. Sending fails if for specified event there is no consumer registered.     **/                     \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/                     \
    /** \param  data    the data object to send as an event.                                                                **/                     \
    /** \return Returns true if event successfully was sent.                                                                **/                     \
    /**                                                                                                                     **/                     \
    static inline bool sendEvent(const DATA_CLASS & data);                                                                                          \
    /**                                                                                                                     **/                     \
    /** \brief  Sends an Event of specified type. If Internal, make sure that event is send in same thread context.         **/                     \
    /**         Before sending, make sure that consumer is registered in thread.                                            **/                     \
    /**         Returns true if could send event. Sending fails if for specified event there is no consumer registered.     **/                     \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/                     \
    /** \param  data        The data object to send as an event.                                                            **/                     \
    /** \param  eventType   The type of event to generated and sent.                                                        **/                     \
    /** \return Returns true if event successfully was sent.                                                                **/                     \
    /**                                                                                                                     **/                     \
    static inline bool sendEvent(const DATA_CLASS & data, Event::eEventType eventType);                                                             \
    /**                                                                                                                     **/                     \
    /** \brief  Sends event to specified consumer within same thread.                                                       **/                     \
    /**         Call this method if sure that event should be processed within same thread scope.                           **/                     \
    /**         Otherwise register consumer in required thread and use calling sendEvent(data) function.                    **/                     \
    /**         Returns true if could send event. Sending fails if current thread is not a dispatching thread.              **/                     \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/                     \
    /** \param  data        The data object to send as an event.                                                            **/                     \
    /** \param  listener    The consumer, which should process event object.                                                **/                     \
    /** \return Returns true if event successfully was sent.                                                                **/                     \
    /**                                                                                                                     **/                     \
    static inline bool sendEvent(const DATA_CLASS & data, __##ConsumerClass<DATA_CLASS> & listener);                                                \
    /**                                                                                                                     **/                     \
    /** \brief  Sends event of specified type to specified consumer within same thread.                                     **/                     \
    /**         If Event is Internal, make sure that it is sent in the same thread context.                                 **/                     \
    /**         Call this method if sure that event should be processed within same thread scope.                           **/                     \
    /**         Otherwise register consumer in required thread and use calling sendEvent(data) function.                    **/                     \
    /**         Returns true if could send event. Sending fails if current thread is not a dispatching thread.              **/                     \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/                     \
    /** \param  data        The data object to send as an event.                                                            **/                     \
    /** \param  listener    The consumer, which should process event object.                                                **/                     \
    /** \param  eventType   The consumer, which should process event object.                                                **/                     \
    /** \return Returns true if event successfully was sent.                                                                **/                     \
    /**                                                                                                                     **/                     \
    static inline bool sendEvent(const DATA_CLASS & data, __##ConsumerClass<DATA_CLASS> & listener, Event::eEventType eventType);                   \
    /**                                                                                                                     **/                     \
    /** \brief  Sends event to specified consumer within specified thread.                                                  **/                     \
    /**         Call this method if sure that event should be processed within specified thread scope.                      **/                     \
    /**         Returns true if could send event.                                                                           **/                     \
    /**         It creates event object and pushes it into registered consumer thread event queue.                          **/                     \
    /** \param  data        The data object to send as an event.                                                            **/                     \
    /** \param  listener    The consumer, which should process event object.                                                **/                     \
    /** \param  dispThread  The Dispatcher Thread, which should receive event.                                              **/                     \
    /**                     The event will be dispatched in this thread and the caller should be sure                       **/                     \
    /**                     that the consumer is registered in specified thread.                                            **/                     \
    /** \return Returns true if event successfully was sent.                                                                **/                     \
    /**                                                                                                                     **/                     \
    static inline bool sendEvent(const DATA_CLASS & data, __##ConsumerClass<DATA_CLASS> & listener, DispatcherThread & dispThread);                 \
    /**                                                                                                                     **/                     \
    /** \brief  Registers Consumer in the worker / component thread                                                         **/                     \
    /**         Returns true if successfully registered consumer                                                            **/                     \
    /**         Returns false if failed to register consumer or consumer was already registered for specified event         **/                     \
    /**         If 'whichThread' parameter is null, it will use current thread to register consumer.                        **/                     \
    /** \param  listener    The consumer to register for current event runtime type.                                        **/                     \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/                     \
    /** \param  whichThread The name of thread to register consumer. If null, it will register in current thread.           **/                     \
    /** \return Returns true if consumer registration succeeded.                                                            **/                     \
    /**         Returns false if thread already has consumer registered or registration failed.                             **/                     \
    /**                                                                                                                     **/                     \
    static inline bool addListener(__##ConsumerClass<DATA_CLASS> & listener, const char* whichThread = NULL);                                       \
    /**                                                                                                                     **/                     \
    /** \brief  Registers Consumer in the worker / component thread                                                         **/                     \
    /**         Returns true if successfully registered consumer                                                            **/                     \
    /**         Returns false if failed to register consumer or consumer was already registered for specified event         **/                     \
    /** \param  listener    The consumer to register for current event runtime type.                                        **/                     \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/                     \
    /** \param	dispThread  The dispatcher thread, which dispatches messages                                                **/                     \
    /** \return Returns true if consumer registration succeeded.                                                            **/                     \
    /**         Returns false if thread already has consumer registered or registration failed.                             **/                     \
    /**                                                                                                                     **/                     \
    static inline bool addListener(__##ConsumerClass<DATA_CLASS>& listener, DispatcherThread & dispThread);                                         \
    /**                                                                                                                     **/                     \
    /** \brief  Unregisters Consumer from worker / component thread                                                         **/                     \
    /**         Returns true if successfully removed registered consumer                                                    **/                     \
    /**         If 'whichThread' parameter is null, it will use current thread to unregister consumer.                      **/                     \
    /** \param  listener    The consumer to unregister for current event runtime type.                                      **/                     \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/                     \
    /** \param  whichThread The name of thread to unregister consumer. If null, it will unregister in current thread.       **/                     \
    /** \return Returns true if consumer is unregistered with success.                                                      **/                     \
    /**                                                                                                                     **/                     \
    static inline bool removeListener(__##ConsumerClass<DATA_CLASS> & listener, const char* whichThread = NULL);                                    \
    /**                                                                                                                     **/                     \
    /** \brief  Unregisters Consumer from worker / component thread                                                         **/                     \
    /**         Returns true if successfully removed registered consumer                                                    **/                     \
    /** \param  listener    The consumer to unregister for current event runtime type.                                      **/                     \
    /**                     The Event Runtime information will be extracted from private static runtime function.           **/                     \
    /** \param	dispThread  The dispatcher thread, which dispatches messages                                                **/                     \
    /** \return Returns true if consumer is unregistered with success.                                                      **/                     \
    /**                                                                                                                     **/                     \
    static inline bool removeListener(__##ConsumerClass<DATA_CLASS>& listener, DispatcherThread & dispThread);                                      \
private:                                                                                                                                            \
    /** Private functions. Registers event for specified worker thread and sends event                                      **/                     \
    static bool _send(DispatcherThread& dispThread, __##EventClass<DATA_CLASS> *eventElem);                                                         \
private:                                                                                                                                            \
    /**                                                                                                                     **/                     \
    /** Forbidden calls. Do not use them.                                                                                   **/                     \
    /**                                                                                                                     **/                     \
    __##EventClass( void );                                                                                                                         \
    __##EventClass(const __##EventClass<DATA_CLASS> &);                                                                                             \
    const __##EventClass<DATA_CLASS>& operator = (const __##EventClass<DATA_CLASS> &);                                                              \
};                                                                                                                                                  \
/**                                                                                                                         **/                     \
/** Implement Runtime Object overrides and get Runtime Class ID                                                             **/                     \
/**                                                                                                                         **/                     \
IMPLEMENT_RUNTIME_TEMPLATE(template <class DATA_CLASS>, __##EventClass<DATA_CLASS>, ThreadEventBase, EventClass)                                    \
/** Constructor implementation, pass DataClass object and Event Consumer object. The event is internal.                     **/                     \
template <class DATA_CLASS>                                                                                                                         \
__##EventClass<DATA_CLASS>::__##EventClass(const DATA_CLASS & data, __##ConsumerClass<DATA_CLASS> & listener)                                       \
    : TEEvent<DATA_CLASS, const DATA_CLASS &> (EventType, data)                                                                                     \
{   this->setEventConsumer(static_cast<IEEventConsumer *>(&listener));   }                                                                          \
/** Constructor implementation, pass DataClass object and event type. By default, the event is external                     **/                     \
template <class DATA_CLASS>                                                                                                                         \
__##EventClass<DATA_CLASS>::__##EventClass(Event::eEventType eventType, const DATA_CLASS & data)                                                    \
    : TEEvent<DATA_CLASS, const DATA_CLASS &> (eventType, data)                                                                                     \
{ ; }                                                                                                                                               \
/** Constructor implementation, pass event type, DataClass and consumer objects.                                            **/                     \
template <class DATA_CLASS>                                                                                                                         \
__##EventClass<DATA_CLASS>::__##EventClass( Event::eEventType eventType,                                                                            \
                                            const DATA_CLASS & data,                                                                                \
                                            __##ConsumerClass<DATA_CLASS> & listener)                                                               \
    : TEEvent<DATA_CLASS, const DATA_CLASS &> (eventType, data)                                                                                     \
{   this->setEventConsumer(static_cast<IEEventConsumer *>(&listener));   }                                                                          \
/** Constructor implementation, pass DataClass object to Event                                                              **/                     \
template <class DATA_CLASS>                                                                                                                         \
__##EventClass<DATA_CLASS>::__##EventClass(const DATA_CLASS& data)                                                                                  \
    : TEEvent<DATA_CLASS, const DATA_CLASS &> (EventType, data)                                                                                     \
{ ; }                                                                                                                                               \
/** Destructor implementation. Do nothing                                                                                   **/                     \
template <class DATA_CLASS>                                                                                                                         \
__##EventClass<DATA_CLASS>::~__##EventClass( void )                                                                                                 \
{ ; }                                                                                                                                               \
/**                                                                                                                         **/                     \
/** Static function, sends event and pass DataClass object. Before sending, it will search Consumer Thread to forward event **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
inline                                                                                                                                              \
bool __##EventClass<DATA_CLASS>::sendEvent(const DATA_CLASS& data)                                                                                  \
{   return __##EventClass<DATA_CLASS>::sendEvent(data, EventType);                                                                  }               \
/**                                                                                                                         **/                     \
/** Static function, sends event of specified type and pass DataClass object.                                               **/                     \
/** If the event type is Internal, make sure that event sent in same thread context to execute immediately.                 **/                     \
/** Before sending an event, it will search Consumer Thread to forward the event.                                           **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
inline                                                                                                                                              \
bool __##EventClass<DATA_CLASS>::sendEvent(const DATA_CLASS & data, Event::eEventType eventType)                                                    \
{                                                                                                                                                   \
    DispatcherThread* dispThread = DispatcherThread::findEventConsumerThread(_getClassId());                                                        \
    return (dispThread != NULL ? _send(*dispThread, DEBUG_NEW __##EventClass<DATA_CLASS>(eventType, data)) : false);                                \
}                                                                                                                                                   \
/**                                                                                                                         **/                     \
/** Static function. Sends a Event of specified type to specified consumer within same thread.                              **/                     \
/** Call this method if sure that event should be processed within same thread scope.                                       **/                     \
/** Otherwise register consumer in required thread and use calling sendEvent(data) function.                                **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
inline                                                                                                                                              \
bool __##EventClass<DATA_CLASS>::sendEvent(const DATA_CLASS & data, __##ConsumerClass<DATA_CLASS> & listener)                                       \
{   return __##EventClass<DATA_CLASS>::sendEvent(data, listener, EventType);                                                        }               \
/**                                                                                                                         **/                     \
/** Static function. Sends event to specified consumer within same thread.                                                  **/                     \
/** Call this method if sure that event should be processed within same thread scope.                                       **/                     \
/** Otherwise register consumer in required thread and use calling sendEvent(data) function.                                **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
inline                                                                                                                                              \
bool __##EventClass<DATA_CLASS>::sendEvent(const DATA_CLASS & data, __##ConsumerClass<DATA_CLASS> & listener, Event::eEventType eventType)          \
{                                                                                                                                                   \
    DispatcherThread& dispThread = DispatcherThread::getCurrentDispatcherThread( );                                                                 \
    return (dispThread.isValid() ? _send(dispThread, DEBUG_NEW __##EventClass<DATA_CLASS>(eventType, data, listener)) : false);                     \
}                                                                                                                                                   \
/**                                                                                                                         **/                     \
/** Static function. Sends event to specified consumer within specified thread.                                             **/                     \
/** Call this method if sure that event should be processed within specified thread scope.                                  **/                     \
/** Returns true if could send event. Returns false if the thread is not valid                                              **/                     \
/** It will create event object and push into registered consumer thread event queue.                                       **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
inline                                                                                                                                              \
bool __##EventClass<DATA_CLASS>::sendEvent(const DATA_CLASS& data, __##ConsumerClass<DATA_CLASS>& listener, DispatcherThread& dispThread)           \
{   return (dispThread.isValid() ? _send(dispThread, DEBUG_NEW __##EventClass<DATA_CLASS>(EventType, data, listener)) : false);     }               \
/**                                                                                                                         **/                     \
/** Adds listener (registers consumer) for specified thread name. The thread should be already running                      **/                     \
/** Returns true if successfully registered consumer                                                                        **/                     \
/** Returns false if failed to register consumer or consumer was already registered for specified event                     **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
inline                                                                                                                                              \
bool __##EventClass<DATA_CLASS>::addListener(__##ConsumerClass<DATA_CLASS>& listener, const char* whichThread)                                      \
{   return Event::addListener(__##EventClass<DATA_CLASS>::_getClassId(), listener, whichThread);                                    }               \
/**                                                                                                                         **/                     \
/** Adds listener (registers consumer) for specified dispatcher thread. The thread should be already running                **/                     \
/** Returns true if successfully registered consumer                                                                        **/                     \
/** Returns false if failed to register consumer or consumer was already registered for specified event                     **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
inline                                                                                                                                              \
bool __##EventClass<DATA_CLASS>::addListener(__##ConsumerClass<DATA_CLASS>& listener, DispatcherThread & dispThread)                                \
{   return Event::addListener(__##EventClass<DATA_CLASS>::_getClassId(), listener, dispThread);                                     }               \
/**                                                                                                                         **/                     \
/** Removes registered consumer. If succeed, returns true. Otherwise returns false.                                         **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
inline                                                                                                                                              \
bool __##EventClass<DATA_CLASS>::removeListener(__##ConsumerClass<DATA_CLASS>& listener, const char* whichThread)                                   \
{   return Event::removeListener(__##EventClass<DATA_CLASS>::_getClassId(), listener, whichThread);                                 }               \
/**                                                                                                                         **/                     \
/** Removes registered consumer. If succeed, returns true. Otherwise returns false.                                         **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
inline                                                                                                                                              \
bool __##EventClass<DATA_CLASS>::removeListener(__##ConsumerClass<DATA_CLASS>& listener, DispatcherThread & dispThread)                             \
{   return Event::removeListener(__##EventClass<DATA_CLASS>::_getClassId(), listener, dispThread);                                  }               \
/**                                                                                                                         **/                     \
/** Private static function to forward event. First it will register event for thread (event consumer thread)               **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
bool __##EventClass<DATA_CLASS>::_send(DispatcherThread& dispThread, __##EventClass<DATA_CLASS> *eventElem)                                         \
{                                                                                                                                                   \
    bool result = false;                                                                                                                            \
    if (eventElem != NULL)                                                                                                                          \
    {                                                                                                                                               \
        static_cast<Event *>(eventElem)->registerForThread(&dispThread);                                                                            \
        static_cast<Event *>(eventElem)->sendEvent();                                                                                               \
        result = true;                                                                                                                              \
    }                                                                                                                                               \
    return result;                                                                                                                                  \
}                                                                                                                                                   \
                                                                                                                                                    \
/**                                                                                                                         **/                     \
/** Declaration of ConsumerClass, derive from TEEventConsumer template.                                                     **/                     \
/** Does nothing, needed for Runtime Object information and declare processEvent() function                                 **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
class __##ConsumerClass: public TEEventConsumer<DATA_CLASS, const DATA_CLASS &>                                                                     \
{                                                                                                                                                   \
protected:                                                                                                                                          \
    /**                                                                                                                     **/                     \
    /** Protected constructor, the object cannot be instantiated                                                            **/                     \
    /**                                                                                                                     **/                     \
    __##ConsumerClass( void );                                                                                                                      \
    /**                                                                                                                     **/                     \
    /** Overrides.                                                                                                          **/                     \
    /**                                                                                                                     **/                     \
protected:                                                                                                                                          \
    /**                                                                                                                     **/                     \
    /** \brief  Override operation. Implement this function to receive events and make processing                           **/                     \
    /** \param  data    The data, which was passed as an event.                                                             **/                     \
    /**                                                                                                                     **/                     \
    virtual void processEvent(const DATA_CLASS& data) = 0;                                                                                          \
private:                                                                                                                                            \
    /**                                                                                                                     **/                     \
    /** Forbidden calls.                                                                                                    **/                     \
    /**                                                                                                                     **/                     \
    __##ConsumerClass(const __##ConsumerClass<DATA_CLASS> &);                                                                                       \
    const __##ConsumerClass<DATA_CLASS>& operator = (const __##ConsumerClass<DATA_CLASS> &);                                                        \
};                                                                                                                                                  \
/**                                                                                                                         **/                     \
/** Constructor                                                                                                             **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
__##ConsumerClass<DATA_CLASS>::__##ConsumerClass( void )                                                                                            \
    : TEEventConsumer<DATA_CLASS, const DATA_CLASS &> ( )                                                                                           \
{ ; }                                                                                                                                               \



/**
 * \brief   Declares Event and Consumer classes. Extended version, with registering Consumer in the thread.
 *          Do not call. Use DECLARE_EVENT or DECLARE_EVENT_EX instead.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   ConsumerThreadName  The name of thread where Event is dispatched and Consumer is registered.
 * \param   EventType           The type of Event. Should be Event::eEventType type. Either Internal or External.
 **/
#define DEFINE_DECLARE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, EventType)                                          \
/**                                                                                                                         **/                     \
/** First, declare all in standard macro                                                                                    **/                     \
/**                                                                                                                         **/                     \
        DEFINE_DECLARE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, EventType)                                                              \
/**                                                                                                                         **/                     \
/** ---- Extended Part ----                                                                                                 **/                     \
/** Declaration of Extended version of ConsumerClass, derive from ConsumerClass template                                    **/                     \
/** The Extended consumer class is registering consumer int the Worker / Component thread when it is initialized.           **/                     \
/** The worker / component thread should already run that we are able to find in resource map                               **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
class __##ConsumerClass##Extended: public __##ConsumerClass<DATA_CLASS>                                                                             \
{                                                                                                                                                   \
protected:                                                                                                                                          \
    /**                                                                                                                     **/                     \
    /** Constructor. When instantiated, it will register consumer in the worker thread passed by ConsumerThreadName         **/                     \
    /** By this, the worker / component thread should already run that we are able to find in resource map                  **/                     \
    /**                                                                                                                     **/                     \
    __##ConsumerClass##Extended( void );                                                                                                            \
private:                                                                                                                                            \
    inline __##ConsumerClass##Extended& self( void )     { return (*this); }                                                                        \
private:                                                                                                                                            \
    /** Forbidden calls.                                                                                                    **/                     \
    __##ConsumerClass##Extended(const __##ConsumerClass##Extended<DATA_CLASS> &);                                                                   \
    const __##ConsumerClass##Extended<DATA_CLASS>& operator = (const __##ConsumerClass##Extended<DATA_CLASS> &);                                    \
};                                                                                                                                                  \
/**                                                                                                                         **/                     \
/** Lookup for thread by name and register consumer having EventClass Runtime Object ID                                     **/                     \
/**                                                                                                                         **/                     \
template <class DATA_CLASS>                                                                                                                         \
__##ConsumerClass##Extended<DATA_CLASS>::__##ConsumerClass##Extended( void )                                                                        \
    : __##ConsumerClass<DATA_CLASS> ( )                                                                                                             \
{                                                                                                                                                   \
    DispatcherThread& dispThread = DispatcherThread::getDispatcherThread(ConsumerThreadName);                                                   \
    VERIFY( dispThread.registerEventConsumer(__##EventClass<DATA_CLASS>::_getClassId(), self()) );                                                  \
}                                                                                                                                                   \

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
 * \param   EventType           The type of Event. Should be Event::eEventType type. Either Internal or External.
 **/
#define DECLARE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, EventType)                                                                     \
    /**                                                                                             **/                                             \
    /** First declare standard event and event consumer classes                                     **/                                             \
    /**                                                                                             **/                                             \
    DEFINE_DECLARE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, EventType)                                                                  \
    /**                                                                                             **/                                             \
    /** Since event and event consumer classes are templates, define new types                      **/                                             \
    /** Use 'EventClass' to send event and using event function.                                    **/                                             \
    /** Use 'ConsumerClass' to implement processEvent() function.                                   **/                                             \
    /**                                                                                             **/                                             \
    typedef __##EventClass<DataClass>       EventClass;                                                                                             \
    typedef __##ConsumerClass<DataClass>    ConsumerClass;                                                                                          \


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
 * \param   EventType           The type of Event. Should be Event::eEventType type. Either Internal or External.
 **/
#define DECLARE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, EventType)                                                 \
    /**                                                                                             **/                                             \
    /** First declare event and extended event consumer classes                                     **/                                             \
    /**                                                                                             **/                                             \
    DEFINE_DECLARE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, EventType)                                              \
    /**                                                                                             **/                                             \
    /** Since event and event consumer classes are templates, define new types                      **/                                             \
    /** Use 'EventClass' to send event and using event function.                                    **/                                             \
    /** Use 'ConsumerClass' to implement processEvent() function.                                   **/                                             \
    /**                                                                                             **/                                             \
    typedef __##EventClass<DataClass>               EventClass;                                                                                     \
    typedef __##ConsumerClass##Extended<DataClass>  ConsumerClass;                                                                                  \

//////////////////////////////////////////////////////////////////////////
// DECLARE_INTERNAL_EVENT declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This MACRO is used to declare internal standard Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *          The Event Type by default will be Event::EventCustomInternal
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 **/
#define DECLARE_INTERNAL_EVENT(DataClass, EventClass, ConsumerClass)                                                                                \
    DECLARE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, Event::EventCustomInternal)

/**
 * \brief   This MACRO is used to declare internal extended Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *          The Event Type by default will be Event::EventCustomInternal.
 *          The Event Consumer will be automatically registered in
 *          specified Thread.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   ConsumerThreadName  The Thread name of Event Consumer.
 **/
#define DECLARE_INTERNAL_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName)                                                   \
    DECLARE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, Event::EventCustomInternal)

//////////////////////////////////////////////////////////////////////////
// DECLARE_EXTERNAL_EVENT declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This MACRO is used to declare external standard Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *          The Event Type by default will be Event::EventCustomExternal
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 **/
#define DECLARE_EXTERNAL_EVENT(DataClass, EventClass, ConsumerClass)                                                                                \
    DECLARE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass, Event::EventCustomExternal)

/**
 * \brief   This MACRO is used to declare external extended Event and Event Consumer.
 *          The Event Data class should be declared before using this MACRO.
 *          The Event Type by default will be Event::EventCustomExternal.
 *          The Event Consumer will be automatically registered in
 *          specified Thread.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   ConsumerThreadName  The Thread name of Event Consumer.
 **/
#define DECLARE_EXTERNAL_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName)                                                   \
    DECLARE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName, Event::EventCustomExternal)

//////////////////////////////////////////////////////////////////////////
// DECLARE_EVENT declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declares standard Event and Event Consumer classes.
 *          The Event Type is set to Event::EventCustomExternal.
 *          The DataClass should be already declared before using this MACRO.
 *          Additionally, it will declare and implement 2 classes:
 *          EventClass and ConsumerClass.
 *          EventClass is and event object to communicate
 *          ConsumerClass should be derived and the pure virtual function
 *          processEvent(const DataClass & data) should be overwritten.
 *          Use EventClass::sendEvent(const DataClass& data) function
 *          to send events. But before the consumer should be registered
 *          for this event.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 **/
#define DECLARE_EVENT(DataClass, EventClass, ConsumerClass)                                                                                         \
    DECLARE_EXTERNAL_EVENT(DataClass, EventClass, ConsumerClass)


//////////////////////////////////////////////////////////////////////////
// DECLARE_EVENT_EX declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declares standard Event and Event Consumer classes, and the
 *          Event Consumer is automatically registered in specified Thread.
 *          The Event Type is set to Event::EventCustomExternal.
 *          In deference to DECLARE_EVENT, this MACRO is getting Consumer
 *          Thread name. When consumer is instantiated, it will register
 *          automatically in mentioned thread by searching in resource map.
 *          By this, the thread should be already started, before consumer
 *          is instantiated.
 *
 * \param   DataClass           The Event Data class. Should be declared before MACRO.
 * \param   EventClass          The Event class, which will be declared after calling this MACRO.
 * \param   ConsumerClass       The Event Consumer class, which will be declared after calling this MACRO.
 * \param   ConsumerThreadName  The Thread name of Event Consumer.
 **/
#define DECLARE_EVENT_EX(DataClass, EventClass, ConsumerClass, ConsumerThreadName)                                                                  \
    DECLARE_EXTERNAL_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass, ConsumerThreadName)

#endif // AREG_COMPONENT_TEEVENT_HPP
