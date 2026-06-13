/************************************************************************
 * \file        common/common.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Latency benchmark service provider component.
 ************************************************************************/

#ifndef EXAMPLES_30_PUBLATENCY_COMMON_COMMON_HPP
#define EXAMPLES_30_PUBLATENCY_COMMON_COMMON_HPP

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include <chrono>

namespace Latency {

    //!< Sizes (in bytes) of the SharedBuffer "rest" payload that pads each large
    //!< structure up to its nominal size, after the fixed 64- and 128-byte blocks.
    constexpr uint32_t REST_1024{ 1024 - 128 - 64 };
    constexpr uint32_t REST_4096{ 4096 - 128 - 64 };
    constexpr uint32_t REST_65536{ 65536 - 128 - 64 };

    /**
     * \brief   Extra 8 bytes of payload for broadcast and request/response ping-pong calls.
     *          Latency::Latency8
     **/
    struct Latency8
    {
        uint64_t    data_1_8{ 14 };
    };

    /**
     * \brief   Extra 16 bytes of payload for broadcast and request/response ping-pong calls.
     *          Latency::Latency16
     **/
    struct Latency16
    {
        uint64_t    data_1_8{ 14 };
        uint64_t    data_9_16{ 912 };
    };

    /**
     * \brief   Extra 32 bytes of payload for broadcast and request/response ping-pong calls.
     *          Latency::Latency32
     **/
    struct Latency32
    {
        uint64_t    data_1_8{ 14 };
        uint64_t    data_9_16{ 912 };
        uint64_t    data_17_24{ 1724 };
        uint64_t    data_25_32{ 2532 };
    };

    /**
     * \brief   Extra 64 bytes of payload for broadcast and request/response ping-pong calls.
     *          Latency::Latency64
     **/
    struct Latency64
    {
        uint64_t    data_1_8{ 14 };
        uint64_t    data_9_16{ 912 };
        uint64_t    data_17_24{ 1724 };
        uint64_t    data_25_32{ 2532 };
        uint64_t    data_33_40{ 3340 };
        uint64_t    data_41_48{ 4148 };
        uint64_t    data_49_56{ 4956 };
        uint64_t    data_57_64{ 5764 };
    };

    /**
     * \brief   Extra 128 bytes of payload: a 64-byte block plus a 64-character text.
     *          Latency::Latency128
     **/
    struct Latency128
    {
        Latency::Latency64  data1;
        areg::String        data128{ "123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456" };
    };

    /**
     * \brief   Extra 256 bytes of payload for broadcast and request/response ping-pong calls.
     *          Latency::Latency256
     **/
    struct Latency256
    {
        Latency::Latency128 data1;
        Latency::Latency128 data2;
    };

    /**
     * \brief   Extra 512 bytes of payload for broadcast and request/response ping-pong calls.
     *          Latency::Latency512
     **/
    struct Latency512
    {
        Latency::Latency256 data1;
        Latency::Latency256 data2;
    };

    /**
     * \brief   Extra 1024 bytes of payload: fixed 64- and 128-byte blocks plus a sized SharedBuffer.
     *          Latency::Latency1024
     **/
    struct Latency1024
    {
        inline Latency1024();

        Latency::Latency64  data_64;
        Latency::Latency128 data_128;
        areg::SharedBuffer  data_rest;
    };

    /**
     * \brief   Extra 4096 bytes of payload: fixed 64- and 128-byte blocks plus a sized SharedBuffer.
     *          Latency::Latency4096
     **/
    struct Latency4096
    {
        inline Latency4096();

        Latency::Latency64  data_64;
        Latency::Latency128 data_128;
        areg::SharedBuffer  data_rest;
    };

    /**
     * \brief   Extra 65536 bytes of payload: an 8-byte field, a 128-byte block, plus a sized SharedBuffer.
     *          Latency::Latency65536
     **/
    struct Latency65536
    {
        inline Latency65536();

        uint64_t            data_64{ 0 };
        Latency::Latency128 data_128;
        areg::SharedBuffer  data_rest;
    };

    /**
     * The SharedBuffer-backed structures reserve their "rest" payload and immediately mark it
     * as used, so that streaming actually serialises the bytes instead of an empty buffer.
     **/
    inline Latency1024::Latency1024()
        : data_rest(REST_1024, areg::BLOCK_SIZE)
    {
        data_rest.set_size_used(REST_1024);
    }

    inline Latency4096::Latency4096()
        : data_rest(REST_4096, areg::BLOCK_SIZE)
    {
        data_rest.set_size_used(REST_4096);
    }

    inline Latency65536::Latency65536()
        : data_rest(REST_65536, areg::BLOCK_SIZE)
    {
        data_rest.set_size_used(REST_65536);
    }

}   // namespace Latency


/**
 * Encode and decode Latency::Latency8 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency8& input)
{
    stream >> input.data_1_8;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency8& output)
{
    stream << output.data_1_8;
    return stream;
}

/**
 * Encode and decode Latency::Latency16 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency16& input)
{
    stream >> input.data_1_8;
    stream >> input.data_9_16;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency16& output)
{
    stream << output.data_1_8;
    stream << output.data_9_16;
    return stream;
}

/**
 * Encode and decode Latency::Latency32 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency32& input)
{
    stream >> input.data_1_8;
    stream >> input.data_9_16;
    stream >> input.data_17_24;
    stream >> input.data_25_32;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency32& output)
{
    stream << output.data_1_8;
    stream << output.data_9_16;
    stream << output.data_17_24;
    stream << output.data_25_32;
    return stream;
}

/**
 * Encode and decode Latency::Latency64 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency64& input)
{
    stream >> input.data_1_8;
    stream >> input.data_9_16;
    stream >> input.data_17_24;
    stream >> input.data_25_32;
    stream >> input.data_33_40;
    stream >> input.data_41_48;
    stream >> input.data_49_56;
    stream >> input.data_57_64;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency64& output)
{
    stream << output.data_1_8;
    stream << output.data_9_16;
    stream << output.data_17_24;
    stream << output.data_25_32;
    stream << output.data_33_40;
    stream << output.data_41_48;
    stream << output.data_49_56;
    stream << output.data_57_64;
    return stream;
}

/**
 * Encode and decode Latency::Latency128 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency128& input)
{
    stream >> input.data1;
    stream >> input.data128;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency128& output)
{
    stream << output.data1;
    stream << output.data128;
    return stream;
}

/**
 * Encode and decode Latency::Latency256 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency256& input)
{
    stream >> input.data1;
    stream >> input.data2;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency256& output)
{
    stream << output.data1;
    stream << output.data2;
    return stream;
}

/**
 * Encode and decode Latency::Latency512 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency512& input)
{
    stream >> input.data1;
    stream >> input.data2;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency512& output)
{
    stream << output.data1;
    stream << output.data2;
    return stream;
}

/**
 * Encode and decode Latency::Latency1024 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency1024& input)
{
    stream >> input.data_64;
    stream >> input.data_128;
    stream >> input.data_rest;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency1024& output)
{
    stream << output.data_64;
    stream << output.data_128;
    stream << output.data_rest;
    return stream;
}

/**
 * Encode and decode Latency::Latency4096 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency4096& input)
{
    stream >> input.data_64;
    stream >> input.data_128;
    stream >> input.data_rest;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency4096& output)
{
    stream << output.data_64;
    stream << output.data_128;
    stream << output.data_rest;
    return stream;
}

/**
 * Encode and decode Latency::Latency65536 structure
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, Latency::Latency65536& input)
{
    stream >> input.data_64;
    stream >> input.data_128;
    stream >> input.data_rest;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const Latency::Latency65536& output)
{
    stream << output.data_64;
    stream << output.data_128;
    stream << output.data_rest;
    return stream;
}


/************************************************************************
 * Prebuilt singletons: each structure is constructed exactly once and
 * reused for every send, so the SharedBuffer-backed payloads are
 * allocated and sized a single time.
 ************************************************************************/

namespace Latency {

    static inline const Latency::Latency8& latency8()
    {
        static Latency::Latency8 _data;
        return _data;
    }

    static inline const Latency::Latency16& latency16()
    {
        static Latency::Latency16 _data;
        return _data;
    }

    static inline const Latency::Latency32& latency32()
    {
        static Latency::Latency32 _data;
        return _data;
    }

    static inline const Latency::Latency64& latency64()
    {
        static Latency::Latency64 _data;
        return _data;
    }

    static inline const Latency::Latency128& latency128()
    {
        static Latency::Latency128 _data;
        return _data;
    }

    static inline const Latency::Latency256& latency256()
    {
        static Latency::Latency256 _data;
        return _data;
    }

    static inline const Latency::Latency512& latency512()
    {
        static Latency::Latency512 _data;
        return _data;
    }

    static inline const Latency::Latency1024& latency1024()
    {
        static Latency::Latency1024 _data;
        return _data;
    }

    static inline const Latency::Latency4096& latency4096()
    {
        static Latency::Latency4096 _data;
        return _data;
    }

    static inline const Latency::Latency65536& latency65536()
    {
        static Latency::Latency65536 _data;
        return _data;
    }
}

#endif // EXAMPLES_30_PUBLATENCY_COMMON_COMMON_HPP
