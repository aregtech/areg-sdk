#ifndef AREG_PERSIST_PRIVATE_NEPERSISTENCE_HPP
#define AREG_PERSIST_PRIVATE_NEPERSISTENCE_HPP
/************************************************************************
 * \file        areg/src/persist/private/NEPersistence.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       File data persist syntax namespace
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// NEPersistence namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Constants used when parsing persistence file
 **/
namespace NEPersistence
{
//////////////////////////////////////////////////////////////////////////
// Syntax to parse configuration file
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Symbol of object separator in configuration file
     **/
    const char              SYNTAX_OBJECT_SEPARATOR             = '.';

    /**
     * \brief   Symbol of data separator in configuration file
     **/
    const char              SYNTAX_DATA_SEPARATOR               = ':';

    /**
     * \brief   White space delimiter
     **/
    const char              SYNTAX_WHITESPACE_DELIMITER         = ' ';

    /**
     * \brief   Tab delimiter
     **/
    const char              SYNTAX_TAB_DELIMITER                = '\t';

    /**
     * \brief   End of command delimiter, optional. Might be skipped
     **/
    const char              SYNTAX_END_COMMAND                  = ';';

    /**
     * \brief   Logical OR, used only when setting values as identifiers.
     *          Do not use in all other cases, since it will have not effect.
     **/
    const char              SYNTAX_LOGICAL_OR                   = '|';

    /**
     * \brief   End of line symbol in configuration file
     **/
    const char * const      SYNTAX_LINEEND                      = "\n";

    /**
     * \brief   Symbol of equal operation in configuration file
     **/
    const char * const      SYNTAX_EQUAL                        = "=";

    /**
     * \brief   Symbol of comment in configuration file
     **/
    const char * const      SYNTAX_COMMENT                      = "# ";

    /**
     * \brief   List of valid delimiters
     **/
    const char * const      SYNTAX_DELIMITERS                   = " \t";

    /**
     * \brief   List of valid whitespace symbols
     **/
    const char * const      SYNTAX_WHITESPACES                  = "\t\r\n";

    /**
     * \brief   The length of comment string
     **/
    extern const int        SYNTAX_COMMENT_LEN;

}

#endif  // AREG_PERSIST_PRIVATE_NEPERSISTENCE_HPP
