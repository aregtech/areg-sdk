#ifndef AREG_TRACE_PRIVATE_LAYOUTS_HPP
#define AREG_TRACE_PRIVATE_LAYOUTS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/Layouts.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
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
    virtual ~IELayout( void ) = default;

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
    IELayout( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( IELayout );
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
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    TickCountLayout( TickCountLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~TickCountLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline TickCountLayout & operator = ( const TickCountLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline TickCountLayout & operator = ( TickCountLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    DayTimeLaytout( DayTimeLaytout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~DayTimeLaytout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline DayTimeLaytout & operator = ( const DayTimeLaytout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline DayTimeLaytout & operator = ( DayTimeLaytout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    ModuleIdLayout( ModuleIdLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~ModuleIdLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline ModuleIdLayout & operator = ( const ModuleIdLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline ModuleIdLayout & operator = ( ModuleIdLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    MessageLayout( MessageLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~MessageLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline MessageLayout & operator = ( const MessageLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline MessageLayout & operator = ( MessageLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    EndOfLineLayout( EndOfLineLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~EndOfLineLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline EndOfLineLayout & operator = ( const EndOfLineLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline EndOfLineLayout & operator = ( EndOfLineLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    PriorityLayout( PriorityLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~PriorityLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline PriorityLayout & operator = ( const PriorityLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline PriorityLayout  & operator = ( PriorityLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    ScopeIdLayout( ScopeIdLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~ScopeIdLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline ScopeIdLayout & operator = ( const ScopeIdLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline ScopeIdLayout & operator = ( ScopeIdLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    ThreadIdLayout( ThreadIdLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~ThreadIdLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline ThreadIdLayout & operator = ( const ThreadIdLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline ThreadIdLayout & operator = ( ThreadIdLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    ModuleNameLayout( ModuleNameLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~ModuleNameLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline ModuleNameLayout & operator = ( const ModuleNameLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline ModuleNameLayout & operator = ( ModuleNameLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    ThreadNameLayout( ThreadNameLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~ThreadNameLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline ThreadNameLayout & operator = ( const ThreadNameLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline ThreadNameLayout & operator = ( ThreadNameLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    ScopeNameLayout( ScopeNameLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~ScopeNameLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline ScopeNameLayout& operator = ( const ScopeNameLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline ScopeNameLayout & operator = ( ScopeNameLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;
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
    explicit AnyTextLayout( const String & anyMessage );

    /**
     * \brief   Sets layout type value and the text to output in the stream.
     * \param   anyMessage  A text to write in stream.
     **/
    explicit AnyTextLayout( const char * anyMessage );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    AnyTextLayout( const AnyTextLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    AnyTextLayout( AnyTextLayout && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~AnyTextLayout( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline AnyTextLayout & operator = ( const AnyTextLayout & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source of data to move.
     **/
    inline AnyTextLayout & operator = ( AnyTextLayout && src ) noexcept;

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
    virtual void logMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const override;

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

inline TickCountLayout & TickCountLayout::operator = ( const TickCountLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline TickCountLayout & TickCountLayout::operator = ( TickCountLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// DayTimeLaytout class inline methods
//////////////////////////////////////////////////////////////////////////

inline DayTimeLaytout & DayTimeLaytout::operator = ( const DayTimeLaytout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline DayTimeLaytout & DayTimeLaytout::operator = ( DayTimeLaytout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ModuleIdLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline ModuleIdLayout & ModuleIdLayout::operator = ( const ModuleIdLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline ModuleIdLayout & ModuleIdLayout::operator = ( ModuleIdLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// MessageLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline MessageLayout & MessageLayout::operator = ( const MessageLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline MessageLayout & MessageLayout::operator = ( MessageLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// EndOfLineLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline EndOfLineLayout & EndOfLineLayout::operator = ( const EndOfLineLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline EndOfLineLayout & EndOfLineLayout::operator = ( EndOfLineLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// PriorityLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline PriorityLayout & PriorityLayout::operator = ( const PriorityLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline PriorityLayout &PriorityLayout::operator = ( PriorityLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ScopeIdLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline ScopeIdLayout & ScopeIdLayout::operator = ( const ScopeIdLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline ScopeIdLayout & ScopeIdLayout::operator = ( ScopeIdLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ThreadIdLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline ThreadIdLayout & ThreadIdLayout::operator = ( const ThreadIdLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline ThreadIdLayout & ThreadIdLayout::operator = ( ThreadIdLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ModuleNameLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline ModuleNameLayout & ModuleNameLayout::operator = ( const ModuleNameLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline ModuleNameLayout & ModuleNameLayout::operator = ( ModuleNameLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ThreadNameLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline ThreadNameLayout & ThreadNameLayout::operator = ( const ThreadNameLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline ThreadNameLayout & ThreadNameLayout::operator = ( ThreadNameLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ScopeNameLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline ScopeNameLayout & ScopeNameLayout::operator = ( const ScopeNameLayout & /*src*/ )
{
    /* Fixed layout type, do nothing */
    return (*this);
}

inline ScopeNameLayout & ScopeNameLayout::operator = ( ScopeNameLayout && /*src*/ ) noexcept
{
    /* Fixed layout type, do nothing */
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// AnyTextLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline AnyTextLayout & AnyTextLayout::operator = ( const AnyTextLayout & src )
{
    mTextMessage = src.mTextMessage;
    return (*this);
}

inline AnyTextLayout & AnyTextLayout::operator = ( AnyTextLayout && src ) noexcept
{
    mTextMessage = static_cast<String &&>(src.mTextMessage);
    return (*this);
}

#endif  // AREG_TRACE_PRIVATE_LAYOUTS_HPP
