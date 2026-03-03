#ifndef AREG_LOGGING_PRIVATE_LAYOUTMANAGER_HPP
#define AREG_LOGGING_PRIVATE_LAYOUTMANAGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LayoutManager.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Layout Manager to format output text messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/ArrayList.hpp"
#include "areg/logging/LoggingDefs.hpp"
#if AREG_LOGGING

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class LogLayout;
    class OutStream;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ClientService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Layout Manager keeps list of layout objects to format output message. The Layouts
 *          are created based on data in tracing configuration file. Currently, there are 3 types of
 *          layout manager used: Message layout to display output message, Enter scope layout to
 *          display enter scope message, and Exit scope layout to display exit scope message.
 **/
class LayoutManager
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants.
//////////////////////////////////////////////////////////////////////////
private:
    using ListLayouts   = ArrayList<LogLayout *>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty layout manager.
     **/
    LayoutManager() = default;
    /**
     * \brief   Destructor
     **/
    virtual ~LayoutManager() ;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates list of layout objects from passed formatting string.
     *
     * \param   layoutFormat    The formatting string to parse and create layout objects.
     * \return  Returns true if after parsing the layout manager contains at least one layout
     *          object.
     **/
    bool create_layouts( const char * layoutFormat );
    /**
     * \brief   Creates list of layout objects from passed formatting string.
     *
     * \param   layoutFormat    The formatting string to parse and create layout objects.
     * \return  Returns true if after parsing the layout manager contains at least one layout
     *          object.
     * \note    Overload with String parameter.
     **/
    bool create_layouts( const String & layoutFormat );

    /**
     * \brief   Releases and deletes list of layout objects.
     **/
    void delete_layouts();

    /**
     * \brief   Logs the message in the streaming object by using layout objects. It will go through
     *          all layouts to generate message and write in stream.
     *
     * \param   logMsg      The logging message to stream.
     * \param   stream      The streaming object to write output message.
     **/
    void log_message( const areg::LogEntry & logMsg, OutStream & stream ) const;

    /**
     * \brief   Returns true if layout manager is valid. The layout manager is valid if it has at
     *          least one layout object.
     **/
    inline bool is_valid() const;

private:

    /**
     * \brief   Instantiates and creates layout objects from layout format string.
     *
     * \param   layoutFormat    The layout string to parse and create objects.
     **/
    inline void _create_layouts(char* layoutFormat);

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
    AREG_NOCOPY_NOMOVE( LayoutManager );
};

//////////////////////////////////////////////////////////////////////////
// LayoutManager class inline methods
//////////////////////////////////////////////////////////////////////////
inline bool LayoutManager::is_valid() const
{
    return (mLayoutList.is_empty() == false);
}

} // namespace areg

#endif  // AREG_LOGGING
#endif  // AREG_LOGGING_PRIVATE_LAYOUTMANAGER_HPP
