#ifndef AREG_TRACE_PRIVATE_ELAYOUTS_HPP
#define AREG_TRACE_PRIVATE_ELAYOUTS_HPP
/************************************************************************
 * \file        areg/trace/private/ELayouts.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, The collection of layouts
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/private/NELogConfig.hpp"
#include "areg/base/String.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEOutStream;

/************************************************************************
 * \brief   This file contains declaration of layout objects used
 *          to format output logging message. The layouts are created
 *          during initialization of logging when read configuration file.
 *          The output message formats are specified in configuration file
 *          and the syntax indicates which layout object to create to
 *          output logging message.
 ************************************************************************/
/************************************************************************
 * Declarations and hierarchies.
 ************************************************************************/
class IELayout;
    class TickCountLayout;
    class DayTimeLaytout;
    class ModuleIdLayout;
    class MessageLayout;
    class EndOfLineLayout;
    class PriorityLayout;
    class ScopeIdLayout;
    class ThreadIdLayout;
    class ModuleNameLayout;
    class ThreadNameLayout;
    class ScopeNameLayout;
    class AnyTextLayout;

//////////////////////////////////////////////////////////////////////////
// IELayout interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The layout base interface extended by all layout objects.
 **/
class IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the layout type
     **/
    IELayout( NELogConfig::eLayouts layout );

public:
    /**
     * \brief   Destructor. Public
     **/
    virtual ~IELayout( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     * \note    Every layout object should overwrite this message to make layout specific outputs.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the layout type
     **/
    inline NELogConfig::eLayouts getLayoutType( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Layout type. Cannot be modified.
     **/
    const NELogConfig::eLayouts  mLayout;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    IELayout( void );
    IELayout( const IELayout & /* src */ );
    const IELayout & operator = ( const IELayout & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// TickCountLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the tick-count value in milliseconds since process has been started.
 **/
class TickCountLayout   : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    TickCountLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    TickCountLayout( const TickCountLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~TickCountLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const TickCountLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// DayTimeLaytout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout formats and outputs day-time data in streaming object
 **/
class DayTimeLaytout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    DayTimeLaytout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    DayTimeLaytout( const DayTimeLaytout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~DayTimeLaytout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const DayTimeLaytout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs formated information of running module data.
 **/
class ModuleIdLayout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    ModuleIdLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    ModuleIdLayout( const ModuleIdLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~ModuleIdLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const ModuleIdLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// MessageLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs message data to the stream.
 **/
class MessageLayout       : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    MessageLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    MessageLayout( const MessageLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~MessageLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const MessageLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout prints end-of-line data at the end of message.
 **/
class EndOfLineLayout   : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    EndOfLineLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    EndOfLineLayout( const EndOfLineLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~EndOfLineLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const EndOfLineLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout prints message priority information in the stream.
 **/
class PriorityLayout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    PriorityLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    PriorityLayout( const PriorityLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~PriorityLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const PriorityLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the information of scope ID in the message.
 **/
class ScopeIdLayout : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    ScopeIdLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    ScopeIdLayout( const ScopeIdLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~ScopeIdLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const ScopeIdLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// ThreadIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the information of thread ID, which generated logging message.
 **/
class ThreadIdLayout      : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    ThreadIdLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    ThreadIdLayout( const ThreadIdLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~ThreadIdLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const ThreadIdLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// ModuleNameLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the name of running module (process) in the message.
 **/
class ModuleNameLayout      : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    ModuleNameLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    ModuleNameLayout( const ModuleNameLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~ModuleNameLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const ModuleNameLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// ThreadNameLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout output the name of thread in output logging message.
 **/
class ThreadNameLayout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    ThreadNameLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    ThreadNameLayout( const ThreadNameLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~ThreadNameLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const ThreadNameLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout output information of scope name in the logging message.
 **/
class ScopeNameLayout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    ScopeNameLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    ScopeNameLayout( const ScopeNameLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~ScopeNameLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const ScopeNameLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// AnyTextLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs any text message as it is without formating.
 **/
class AnyTextLayout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value.
     **/
    AnyTextLayout( void );

    /**
     * \brief   Sets layout type value and the text to output in the stream.
     * \param   anyMessage  A text to write in stream.
     **/
    AnyTextLayout( const String & anyMessage );

    /**
     * \brief   Sets layout type value and the text to output in the stream.
     * \param   anyMessage  A text to write in stream.
     **/
    AnyTextLayout( const char * anyMessage );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    AnyTextLayout( const AnyTextLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~AnyTextLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const AnyTextLayout & src );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IELayout interface overrides
/************************************************************************/

    /**
     * \brief   Makes layout specific formated text output of give message to the streaming object.
     * \param   msgLog  The log message structure, which contains required information for message output
     * \param   stream  The streaming object, where the text message should be written.
     **/
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Member variable
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The text message to log
     **/
    String    mTextMessage;
};

//////////////////////////////////////////////////////////////////////////
// Inline methods
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IELayout interface inline methods
//////////////////////////////////////////////////////////////////////////

inline NELogConfig::eLayouts IELayout::getLayoutType( void ) const
{
    return mLayout;
}

//////////////////////////////////////////////////////////////////////////
// TickCountLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void TickCountLayout::operator = ( const TickCountLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// DayTimeLaytout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void DayTimeLaytout::operator = ( const DayTimeLaytout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void ModuleIdLayout::operator = ( const ModuleIdLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// MessageLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void MessageLayout::operator = ( const MessageLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void EndOfLineLayout::operator = ( const EndOfLineLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void PriorityLayout::operator = ( const PriorityLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void ScopeIdLayout::operator = ( const ScopeIdLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// ThreadIdLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void ThreadIdLayout::operator = ( const ThreadIdLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// ModuleNameLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void ModuleNameLayout::operator = ( const ModuleNameLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// ThreadNameLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void ThreadNameLayout::operator = ( const ThreadNameLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void ScopeNameLayout::operator = ( const ScopeNameLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// AnyTextLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void AnyTextLayout::operator = ( const AnyTextLayout & src )
{   mTextMessage = src.mTextMessage;        }

#endif  // AREG_TRACE_PRIVATE_ELAYOUTS_HPP
