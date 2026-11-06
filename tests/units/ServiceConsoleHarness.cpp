/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 ************************************************************************/

// Declares the pseudo terminal functions: g++ on Cygwin does not predefine it, unlike on Linux.
#if defined(__CYGWIN__) && !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#endif

#include "units/ServiceConsoleHarness.hpp"

#if !defined(_WIN32)
    #include <sys/ioctl.h>
#endif

namespace service_harness
{
    void make_terminal(int (&fds)[2])
    {
#if defined(_WIN32)
        make_pipe(fds);
#else
    #if defined(__CYGWIN__)
        // A new session stops Cygwin from making a duplicated slave the controlling terminal, whose master reads end of file.
        AREG_CHILD_CHECK(::getsid(0) == ::getpid() || ::setsid() != -1);
    #endif
        fds[0] = ::posix_openpt(O_RDWR | O_NOCTTY);
        AREG_CHILD_CHECK(fds[0] >= 0);
        AREG_CHILD_CHECK(::grantpt(fds[0]) == 0 && ::unlockpt(fds[0]) == 0);
        const char* name{ ::ptsname(fds[0]) };
        AREG_CHILD_CHECK(name != nullptr);
        fds[1] = ::open(name, O_RDWR | O_NOCTTY);
        AREG_CHILD_CHECK(fds[1] >= 0);
        winsize size{};
        size.ws_row = 40;
        size.ws_col = 120;
        AREG_CHILD_CHECK(::ioctl(fds[1], TIOCSWINSZ, &size) == 0);
#endif
    }

    void report_threads()
    {
#if defined(__APPLE__)
        const std::string file{ testing::TempDir() + "areg-threads-" + std::to_string(::getpid()) + ".txt" };
        const std::string sample{ "/usr/bin/sample " + std::to_string(::getpid()) + " 1 -mayDie -file '" + file + "' >/dev/null 2>&1" };
        const std::string print{ "cat '" + file + "' >&2; rm -f '" + file + "'" };
        std::fflush(stderr);
        if (std::system(sample.c_str()) == 0)
            std::system(print.c_str());
        else
            std::fprintf(stderr, "the stacks of the threads are not available\n");
#endif
    }
}
