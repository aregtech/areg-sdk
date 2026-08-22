/************************************************************************
 * \file        common/headless.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Start-up command line of the latency benchmark (example 30).
 ************************************************************************/

#ifndef EXAMPLES_30_PUBLATENCY_COMMON_HEADLESS_HPP
#define EXAMPLES_30_PUBLATENCY_COMMON_HEADLESS_HPP

#include "areg/base/String.hpp"

namespace Latency
{

/**
 * \brief   The two applications of example 30 normally draw a full screen console and wait
 *          for the user to type commands. That is good to look at, but it cannot be used by
 *          a script: the screen is redrawn hundreds of times and the program never ends by
 *          itself.
 *
 *          Both applications therefore have a second way of running, called HEADLESS. In
 *          headless mode:
 *          - no console is drawn and no key is read;
 *          - the consumer runs the command it was given on the command line;
 *          - the consumer prints one plain line of text per finished run;
 *          - when the last run is finished the consumer quits, and it also asks the provider
 *            to quit, so nothing is left behind.
 *
 *          The functions below carry the command line from `main()` to the components. The
 *          components are created by the framework, not by `main()`, so they cannot be given
 *          the text through a constructor. It is written once, before the model is loaded,
 *          and only read afterwards, so no locking is needed.
 **/

/**
 * \brief   Returns the one and only start-up command of this process.
 *
 *          `main()` fills it with the command line before it loads the model. An empty text
 *          means: no command line was given, so run as before, with the console.
 **/
[[nodiscard]]
inline areg::String & startup_command();

/**
 * \brief   Tells whether this process must run without the console.
 *          It is true exactly when startup_command() is not empty.
 **/
[[nodiscard]]
inline bool is_headless();

/**
 * \brief   Joins the command line arguments into one line, separated by single spaces.
 *
 * \param   argc    Number of arguments, as given to main().
 * \param   argv    The arguments, as given to main().
 * \return  Returns the joined text. Returns an empty text when there is no argument.
 **/
[[nodiscard]]
inline areg::String join_arguments(int argc, char * argv[]);

//////////////////////////////////////////////////////////////////////////
// Latency inline methods
//////////////////////////////////////////////////////////////////////////

inline areg::String & startup_command()
{
    static areg::String _theCommand;
    return _theCommand;
}

inline bool is_headless()
{
    return startup_command().is_empty() == false;
}

inline areg::String join_arguments(int argc, char * argv[])
{
    areg::String result;
    for (int i = 1; i < argc; ++i)
    {
        if (result.is_empty() == false)
            result += " ";

        result += argv[i];
    }

    return result;
}

}   // namespace Latency

#endif // EXAMPLES_30_PUBLATENCY_COMMON_HEADLESS_HPP
