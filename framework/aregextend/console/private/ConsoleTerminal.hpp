#ifndef AREG_AREGEXTEND_CONSOLE_PRIVATE_CONSOLETERMINAL_HPP
#define AREG_AREGEXTEND_CONSOLE_PRIVATE_CONSOLETERMINAL_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/private/ConsoleTerminal.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the OS specific part of the console: the standard
 *              streams, the terminal size and the reading of keys.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/MathDefs.hpp"

namespace areg::ext::terminal {

    //!< The result of reading one byte of input.
    enum class KeyRead
    {
          Byte          //!< A byte was read.
        , Timeout       //!< No byte arrived in time.
        , Closed        //!< The input reached its end or failed.
        , Interrupted   //!< The read was interrupted by interrupt().
    };

    /**
     * \brief   Prepares the standard streams: switches a terminal input to key by key mode without
     *          echo until close(), restored also when a signal or a console event ends the process.
     *          On POSIX only for a terminal of the foreground process, and creates the interrupt pipe.
     *
     * \param   pipe    The interrupt pipe to create, both ends are -1 if there is none.
     * \return  Returns true if the output is a terminal that draws at coordinates.
     **/
    bool open(int (&pipe)[2]) noexcept;

    /**
     * \brief   Restores the standard streams and closes the interrupt pipe.
     **/
    void close(int (&pipe)[2]) noexcept;

    /**
     * \brief   Returns the visible size of the terminal, columns in posX and rows in posY,
     *          { 0, 0 } if unknown.
     **/
    areg::Coord screen_size() noexcept;

    /**
     * \brief   Returns true if the input is read key by key without echo.
     **/
    bool is_key_mode() noexcept;

    /**
     * \brief   Reads one byte of the standard input.
     *
     * \param   pipeFd      The read end of the interrupt pipe, -1 if none.
     * \param   timeoutMs   The time to wait in milliseconds. A negative value waits for ever.
     * \param   ch          On output, the byte read.
     **/
    KeyRead read_byte(int pipeFd, int timeoutMs, char& ch) noexcept;

    /**
     * \brief   Wakes the thread waiting in read_byte(). The line it reads is dropped.
     **/
    void interrupt(const int (&pipe)[2]) noexcept;

} // namespace areg::ext::terminal

#endif  // AREG_AREGEXTEND_CONSOLE_PRIVATE_CONSOLETERMINAL_HPP
