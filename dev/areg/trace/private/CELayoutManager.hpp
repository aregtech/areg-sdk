#ifndef AREG_TRACE_PRIVATE_CELAYOUTMANAGER_HPP
#define AREG_TRACE_PRIVATE_CELAYOUTMANAGER_HPP
/************************************************************************
 * \file        areg/trace/private/CELayoutManager.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Layout Manager to format output text messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEStack.hpp"
#include "areg/trace/NETrace.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IELayout;
class IEOutStream;

//////////////////////////////////////////////////////////////////////////
// CEClientService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Layout Manager keeps list of layout objects to format output message
 *          The Layouts are created based on data in tracing configuration file.
 *          Currently, there are 3 types of layout manager used:
 *              - Message layout, format to display output message
 *              - Enter scope layout, format to display enter scope message
 *              - Exit scope layout, format to display exit scope message
 **/
class CELayoutManager
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants.
//////////////////////////////////////////////////////////////////////////
private:
    typedef TENolockStack<IELayout *, const IELayout *>     ListLayouts;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    CELayoutManager( void );
    /**
     * \brief   Destructor
     **/
    virtual ~CELayoutManager( void ) ;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates list of layout objects outside of passed formatting string.
     * \param   layoutFormat    The formatting string to parse and crate layout objects.
     * \return  Returns true if after parsing the layout manager contains at least one layout object.
     **/
    bool CreateLayouts( const char * layoutFormat );

    /**
     * \brief   Release and delete list of layout objects.
     **/
    void DeleteLayouts( void );

    /**
     * \brief   Logs the message in the streaming object by using layout objects.
     *          It will go through all layouts to generate message and write in stream.
     * \param   logMsg  The logging message to stream.
     * \param   stream  The streaming object to write output message.
     * \return  Returns true if operation succeeded.
     **/
    bool LogMessage( const NETrace::sLogMessage & logMsg, IEOutStream & stream ) const;

    /**
     * \brief   Returns true if layout manager is valid.
     *          The layout manager is valid if it has at least one layout object.
     **/
    inline bool IsValid( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The list of layouts
     **/
    ListLayouts     mLayoutList;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CELayoutManager( const CELayoutManager & /*src*/ );
    const CELayoutManager & operator = ( const CELayoutManager & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CELayoutManager class inline methods
//////////////////////////////////////////////////////////////////////////
inline bool CELayoutManager::IsValid( void ) const
{   return (mLayoutList.IsEmpty() == false);    }

#endif  // AREG_TRACE_PRIVATE_CELAYOUTMANAGER_HPP
