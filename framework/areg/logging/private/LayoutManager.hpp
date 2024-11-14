#ifndef AREG_LOGGING_PRIVATE_LAYOUTMANAGER_HPP
#define AREG_LOGGING_PRIVATE_LAYOUTMANAGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LayoutManager.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Layout Manager to format output text messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEArrayList.hpp"
#include "areg/logging/NELogging.hpp"

#if AREG_LOGS

/************************************************************************
 * Dependencies
 ************************************************************************/
class IELayout;
class IEOutStream;

//////////////////////////////////////////////////////////////////////////
// ClientService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Layout Manager keeps list of layout objects to format output message
 *          The Layouts are created based on data in tracing configuration file.
 *          Currently, there are 3 types of layout manager used:
 *              - Message layout, format to display output message
 *              - Enter scope layout, format to display enter scope message
 *              - Exit scope layout, format to display exit scope message
 **/
class LayoutManager
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants.
//////////////////////////////////////////////////////////////////////////
private:
    using ListLayouts   = TEArrayList<IELayout *>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    LayoutManager( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~LayoutManager( void ) ;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates list of layout objects outside of passed formatting string.
     * \param   layoutFormat    The formatting string to parse and crate layout objects.
     * \return  Returns true if after parsing the layout manager contains at least one layout object.
     **/
    bool createLayouts( const char * layoutFormat );
    bool createLayouts( const String & layoutFormat );

    /**
     * \brief   Release and delete list of layout objects.
     **/
    void deleteLayouts( void );

    /**
     * \brief   Logs the message in the streaming object by using layout objects.
     *          It will go through all layouts to generate message and write in stream.
     * \param   logMsg  The logging message to stream.
     * \param   stream  The streaming object to write output message.
     **/
    void logMessage( const NELogging::sLogMessage & logMsg, IEOutStream & stream ) const;

    /**
     * \brief   Returns true if layout manager is valid.
     *          The layout manager is valid if it has at least one layout object.
     **/
    inline bool isValid( void ) const;

private:

    /**
     * \brief   Instantiates and creates layout objects out of layout format string.
     * \param   layoutFormat    The layout string to parse and create objects.
     **/
    inline void _createLayouts(char* layoutFormat);

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
    DECLARE_NOCOPY_NOMOVE( LayoutManager );
};

//////////////////////////////////////////////////////////////////////////
// LayoutManager class inline methods
//////////////////////////////////////////////////////////////////////////
inline bool LayoutManager::isValid( void ) const
{
    return (mLayoutList.isEmpty() == false);
}

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_LAYOUTMANAGER_HPP
