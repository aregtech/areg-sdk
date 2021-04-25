#ifndef AREG_COMPONENT_PRIVATE_CETIMERMANAGINGEVENT_HPP
#define AREG_COMPONENT_PRIVATE_CETIMERMANAGINGEVENT_HPP
/************************************************************************
 * \file        areg/src/component/private/CETimerManagingEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform. Event Class to manage Timer
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/TEEvent.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class CETimerManagingEventData;
// class CETimerManagingEvent
// class IETimerManagingEventConsumer

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. CETimerManagingEventData
 *              2. CETimerManagingEvent
 *              3. IETimerManagingEventConsumer
 *          The Timer Managing Event is used to send events to Timer Manager.
 *          The Timer Manager derives IETimerManagingEventConsumer and
 *          processes Event Data.
 *          For mode details, see descriptions bellow and see 
 *          class CETimerManager
 *
 * \see CETimerManager
 ************************************************************************/

/************************************************************************
 * Dependencies
 ************************************************************************/
class CETimer;

//////////////////////////////////////////////////////////////////////////
// CETimerManagingEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Timer Managing Event Data is a container object.
 *          It contains Timer object and requested action.
 *          The Data object is delivered to Timer Manager and
 *          depending on requested actions, the Timer Manager
 *          is performing action.
 *          For more details, see CETimerManager class
 **/
class CETimerManagingEventData
{
//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   CETimerManagingEventData::eTimerAction
     *          The actions to perform on Timer Manager side
     **/
    typedef enum E_TimerAction
    {
          TimerIgnore       //!< Ignore event, do nothing
        , TimerStart        //!< Start specified timer
        , TimerStop         //!< Stop specified timer
        , TimerCancel       //!< Cancel specified timer

    } eTimerAction;

    /**
     * \brief   CETimerManagingEventData::GetString()
     *          Retunrs the string value of CETimerManagingEventData::eTimerAction type
     **/
    static inline const char * GetString( CETimerManagingEventData::eTimerAction timerAction );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     *          Creates empty data with ignore action.
     **/
    inline CETimerManagingEventData( void );

    /**
     * \brief   Initialization constructor.
     *          Creates data object, sets timer object and defines action.
     *          The action should not be CETimerManagingEventData::TimerIgnore
     * \param   timer           The Timer object to set in Event Data.
     * \param   actionPerfom    One of defined actions to perform. It should not be CETimerManagingEventData::TimerIgnore
     **/
    inline CETimerManagingEventData( CETimer & timer, CETimerManagingEventData::eTimerAction actionPerfom );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    inline CETimerManagingEventData( const CETimerManagingEventData & src );

    /**
     * \brief   Destructor
     **/
    inline ~CETimerManagingEventData( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies Event Data from given source.
     * \param   src     The source of Event Data to copy
     **/
    const CETimerManagingEventData & operator = ( const CETimerManagingEventData & src );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the Timer object.
     **/
    inline CETimer * GetTimer( void ) const;

    /**
     * \brief   Returns the action value to perform.
     **/
    inline CETimerManagingEventData::eTimerAction GetAction( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Timer object of Event Data
     **/
    CETimer *       mTimer;
    /**
     * \brief   The Action value to perform.
     **/
    eTimerAction    mAction;
};

//////////////////////////////////////////////////////////////////////////
// CETimerManagingEvent and IETimerManagingEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declaration of CETimerManagingEvent and IETimerManagingEventConsumer.
 *          The CETimerManagingEvent is used to send Event Data
 *          The IETimerManagingEventConsumer is used to process data.
 **/
DECLARE_EVENT(CETimerManagingEventData, CETimerManagingEvent, IETimerManagingEventConsumer)

/************************************************************************
 * Inline functions
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// CETimerManagingEventData class inline functions declaration
//////////////////////////////////////////////////////////////////////////
inline CETimerManagingEventData::CETimerManagingEventData( void )
    : mTimer(NULL) , mAction(CETimerManagingEventData::TimerIgnore)
{   ;   }

inline CETimerManagingEventData::CETimerManagingEventData( CETimer &timer, eTimerAction actionPerfom )
    : mTimer(&timer), mAction(actionPerfom)
{   ASSERT(actionPerfom != CETimerManagingEventData::TimerIgnore);  }

inline CETimerManagingEventData::CETimerManagingEventData( const CETimerManagingEventData &src )
    : mTimer(src.mTimer), mAction(src.mAction)
{   ;   }

inline CETimerManagingEventData::~CETimerManagingEventData( void )
{   ;   }

inline CETimer* CETimerManagingEventData::GetTimer( void ) const
{   return mTimer;  }

inline CETimerManagingEventData::eTimerAction CETimerManagingEventData::GetAction( void ) const
{   return mAction;     }

inline const char * CETimerManagingEventData::GetString( CETimerManagingEventData::eTimerAction timerAction )
{
    switch ( timerAction )
    {
    CASE_MAKE_STRING(CETimerManagingEventData::TimerIgnore);
    CASE_MAKE_STRING(CETimerManagingEventData::TimerStart);
    CASE_MAKE_STRING(CETimerManagingEventData::TimerStop);
    CASE_MAKE_STRING(CETimerManagingEventData::TimerCancel);
    CASE_DEFAULT("ERR: Undefined CETimerManagingEventData::eTimerAction value!");
    }
}

#endif  // AREG_COMPONENT_PRIVATE_CETIMERMANAGINGEVENT_HPP
