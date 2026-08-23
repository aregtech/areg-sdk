/************************************************************************
 * \file        common/common.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Payload types of the local latency benchmark (example 31).
 ************************************************************************/

#ifndef EXAMPLES_31_LOCLATENCY_COMMON_COMMON_HPP
#define EXAMPLES_31_LOCLATENCY_COMMON_COMMON_HPP

/**
 * \brief   Example 31 measures the very same messages as example 30, but inside one
 *          process instead of between two processes. To be able to compare the numbers
 *          of the two examples one to one, both must send payloads of exactly the same
 *          type and of exactly the same size. Therefore this file does not define new
 *          payload types: it re-uses the types of example 30.
 *
 *          The included file gives the following, all in namespace `Latency`:
 *          - the payload structures `Latency8` ... `Latency65536`;
 *          - the stream operators that write them into and read them from a message;
 *          - the `areg::required_size<>` helpers that tell the framework how many bytes
 *            each payload needs;
 *          - the ready-made singletons `latency8()` ... `latency65536()`, so that the
 *            benchmark never allocates a payload while it is measuring.
 *
 *          Do not add benchmark logic here. Everything that belongs only to example 31
 *          lives in the `src` folder of this example. The path below is relative to this
 *          file, because the folder of example 30 is not on the include path of example 31.
 **/
#include "../../30_publatency/common/common.hpp"

#endif // EXAMPLES_31_LOCLATENCY_COMMON_COMMON_HPP
