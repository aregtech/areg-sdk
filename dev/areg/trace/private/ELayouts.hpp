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
#include "areg/base/CEString.hpp"

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
    class CETickCountLayout;
    class CEDayTimeLaytout;
    class CEModuleIdLayout;
    class CEMessageLayout;
    class CEEndOfLineLayout;
    class CEPriorityLayout;
    class CEScopeIdLayout;
    class CEThreadIdLayout;
    class CEModuleNameLayout;
    class CEThreadNameLayout;
    class CEScopeNameLayout;
    class CEAnyTextLayout;

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
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the layout type
     **/
    inline NELogConfig::eLayouts GetLayoutType( void ) const;

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
// CETickCountLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the tick-count value in milliseconds since process has been started.
 **/
class CETickCountLayout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CETickCountLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CETickCountLayout( const CETickCountLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CETickCountLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CETickCountLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEDayTimeLaytout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout formats and outputs day-time data in streaming object
 **/
class CEDayTimeLaytout      : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEDayTimeLaytout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEDayTimeLaytout( const CEDayTimeLaytout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEDayTimeLaytout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEDayTimeLaytout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEModuleIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs formated information of running module data.
 **/
class CEModuleIdLayout      : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEModuleIdLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEModuleIdLayout( const CEModuleIdLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEModuleIdLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEModuleIdLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEMessageLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs message data to the stream.
 **/
class CEMessageLayout       : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEMessageLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEMessageLayout( const CEMessageLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEMessageLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEMessageLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEEndOfLineLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout prints end-of-line data at the end of message.
 **/
class CEEndOfLineLayout     : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEEndOfLineLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEEndOfLineLayout( const CEEndOfLineLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEEndOfLineLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEEndOfLineLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEPriorityLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout prints message priority information in the stream.
 **/
class CEPriorityLayout      : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEPriorityLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEPriorityLayout( const CEPriorityLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEPriorityLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEPriorityLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEScopeIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the information of scope ID in the message.
 **/
class CEScopeIdLayout       : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEScopeIdLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEScopeIdLayout( const CEScopeIdLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEScopeIdLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEScopeIdLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEThreadIdLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the information of thread ID, which generated logging message.
 **/
class CEThreadIdLayout      : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEThreadIdLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEThreadIdLayout( const CEThreadIdLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEThreadIdLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEThreadIdLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEModuleNameLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs the name of running module (process) in the message.
 **/
class CEModuleNameLayout      : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEModuleNameLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEModuleNameLayout( const CEModuleNameLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEModuleNameLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEModuleNameLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEThreadNameLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout output the name of thread in output logging message.
 **/
class CEThreadNameLayout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEThreadNameLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEThreadNameLayout( const CEThreadNameLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEThreadNameLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEThreadNameLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEScopeNameLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout output information of scope name in the logging message.
 **/
class CEScopeNameLayout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value
     **/
    CEScopeNameLayout( void );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEScopeNameLayout( const CEScopeNameLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEScopeNameLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEScopeNameLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;
};

//////////////////////////////////////////////////////////////////////////
// CEAnyTextLayout class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This layout outputs any text message as it is without formating.
 **/
class CEAnyTextLayout    : public    IELayout
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets layout type value.
     **/
    CEAnyTextLayout( void );

    /**
     * \brief   Sets layout type value and the text to output in the stream.
     * \param   anyMessage  A text to write in stream.
     **/
    CEAnyTextLayout( const CEString & anyMessage );

    /**
     * \brief   Sets layout type value and the text to output in the stream.
     * \param   anyMessage  A text to write in stream.
     **/
    CEAnyTextLayout( const char * anyMessage );

    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    CEAnyTextLayout( const CEAnyTextLayout & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CEAnyTextLayout( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source.
     * \param   src     The source of data to copy.
     **/
    inline void operator = ( const CEAnyTextLayout & src );

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
     **/
    virtual void LogMessage( const NETrace::sLogMessage & msgLog, IEOutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Member variable
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The text message to log
     **/
    CEString    mTextMessage;
};

//////////////////////////////////////////////////////////////////////////
// Inline methods
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IELayout interface inline methods
//////////////////////////////////////////////////////////////////////////

inline NELogConfig::eLayouts IELayout::GetLayoutType( void ) const
{   return mLayout;                     }

//////////////////////////////////////////////////////////////////////////
// CETickCountLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CETickCountLayout::operator = ( const CETickCountLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEDayTimeLaytout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEDayTimeLaytout::operator = ( const CEDayTimeLaytout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEModuleIdLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEModuleIdLayout::operator = ( const CEModuleIdLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEMessageLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEMessageLayout::operator = ( const CEMessageLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEEndOfLineLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEEndOfLineLayout::operator = ( const CEEndOfLineLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEPriorityLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEPriorityLayout::operator = ( const CEPriorityLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEScopeIdLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEScopeIdLayout::operator = ( const CEScopeIdLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEThreadIdLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEThreadIdLayout::operator = ( const CEThreadIdLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEModuleNameLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEModuleNameLayout::operator = ( const CEModuleNameLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEThreadNameLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEThreadNameLayout::operator = ( const CEThreadNameLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEScopeNameLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEScopeNameLayout::operator = ( const CEScopeNameLayout & /*src*/ )
{   ; /* Fixed layout type, do nothing */   }

//////////////////////////////////////////////////////////////////////////
// CEAnyTextLayout class inline methods
//////////////////////////////////////////////////////////////////////////

inline void CEAnyTextLayout::operator = ( const CEAnyTextLayout & src )
{   mTextMessage = src.mTextMessage;        }

#endif  // AREG_TRACE_PRIVATE_ELAYOUTS_HPP
