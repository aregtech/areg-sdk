/************************************************************************
 * \file        common/latency_common.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Latency benchmark service provider component.
 ************************************************************************/

#ifndef EXAMPLES_30_PUBLATENCY_COMMON_LATENCY_COMMON_HPP
#define EXAMPLES_30_PUBLATENCY_COMMON_LATENCY_COMMON_HPP

#include "examples/30_publatency/services/Latency.hpp"
#include <chrono>

namespace Latency 
{
    //!< Role name for the latency service provider; used in component registration and proxy lookup.
    constexpr char LATENCY_SERVICE_ROLE[]{ "LatencyServiceProvider" };

    [[nodiscard]]
    inline uint64_t now_ns() noexcept;

    [[nodiscard]]
    inline constexpr const char* mode_as_str(Latency::LatencyMode m) noexcept;

    [[nodiscard]]
    inline Latency::LatencyMode string_as_mode(const areg::String& mode) noexcept;
}

inline uint64_t Latency::now_ns() noexcept
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count()
        );
}

constexpr const char* Latency::mode_as_str(Latency::LatencyMode m) noexcept
{
    switch (m)
    {
    case Latency::LatencyMode::Stop:            return "stop";
    case Latency::LatencyMode::Broadcast0:      return "bc0";
    case Latency::LatencyMode::Broadcast8:      return "bc8";
    case Latency::LatencyMode::Broadcast16:     return "bc16";
    case Latency::LatencyMode::Broadcast32:     return "bc32";
    case Latency::LatencyMode::Broadcast64:     return "bc64";
    case Latency::LatencyMode::Broadcast128:    return "bc128";
    case Latency::LatencyMode::Broadcast256:    return "bc256";
    case Latency::LatencyMode::Broadcast512:    return "bc512";
    case Latency::LatencyMode::Broadcast1024:   return "bc1024";
    case Latency::LatencyMode::Broadcast4096:   return "bc4096";
    case Latency::LatencyMode::Broadcast65536:  return "bc65536";
    case Latency::LatencyMode::Request0:        return "pp0";
    case Latency::LatencyMode::Request8:        return "pp8";
    case Latency::LatencyMode::Request16:       return "pp16";
    case Latency::LatencyMode::Request32:       return "pp32";
    case Latency::LatencyMode::Request64:       return "pp64";
    case Latency::LatencyMode::Request128:      return "pp128";
    case Latency::LatencyMode::Request256:      return "pp256";
    case Latency::LatencyMode::Request512:      return "pp512";
    case Latency::LatencyMode::Request1024:     return "pp1024";
    case Latency::LatencyMode::Request4096:     return "pp4096";
    case Latency::LatencyMode::Request65536:    return "pp65536";
    case Latency::LatencyMode::QuitApp:         return "quit";
    default:                                    return "?";
    }
}

inline Latency::LatencyMode Latency::string_as_mode(const areg::String& mode) noexcept
{
    if (mode == "pp0")  return Latency::LatencyMode::Request0;
    else if (mode == "pp8")     return Latency::LatencyMode::Request8;
    else if (mode == "pp16")    return Latency::LatencyMode::Request16;
    else if (mode == "pp32")    return Latency::LatencyMode::Request32;
    else if (mode == "pp64")    return Latency::LatencyMode::Request64;
    else if (mode == "pp128")   return Latency::LatencyMode::Request128;
    else if (mode == "pp256")   return Latency::LatencyMode::Request256;
    else if (mode == "pp512")   return Latency::LatencyMode::Request512;
    else if (mode == "pp1024")  return Latency::LatencyMode::Request1024;
    else if (mode == "pp4096")  return Latency::LatencyMode::Request4096;
    else if (mode == "pp65536") return Latency::LatencyMode::Request65536;
    else if (mode == "bc0")     return Latency::LatencyMode::Broadcast0;
    else if (mode == "bc8")     return Latency::LatencyMode::Broadcast8;
    else if (mode == "bc16")    return Latency::LatencyMode::Broadcast16;
    else if (mode == "bc32")    return Latency::LatencyMode::Broadcast32;
    else if (mode == "bc64")    return Latency::LatencyMode::Broadcast64;
    else if (mode == "bc128")   return Latency::LatencyMode::Broadcast128;
    else if (mode == "bc256")   return Latency::LatencyMode::Broadcast256;
    else if (mode == "bc512")   return Latency::LatencyMode::Broadcast512;
    else if (mode == "bc1024")  return Latency::LatencyMode::Broadcast1024;
    else if (mode == "bc4096")  return Latency::LatencyMode::Broadcast4096;
    else if (mode == "bc65536") return Latency::LatencyMode::Broadcast65536;
    else return Latency::LatencyMode::Undefined;
}

#endif // EXAMPLES_30_PUBLATENCY_COMMON_LATENCY_COMMON_HPP
