#ifndef AREG_BASE_IOSTREAM_HPP
#define AREG_BASE_IOSTREAM_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/IOStream.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Input / Output stream interfaces
 *              Following interfaces are defined:
 *              InStream     - is an interface of stream to read data from.
 *              OutStream    - is an interface of stream to write data to.
 *              IOStream     - is an interface of reads and writes streaming.
 *              Implemented instances should provide read and write
 *              functionalities.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <cstring>
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/************************************************************************
 * Declared classes
 ************************************************************************/

/************************************************************************
 * \brief   Following classes are defined in following hierarchy.
 *          Description of every class see bellow.
 ************************************************************************/
namespace areg {
    class InStream;
    class OutStream;
    class IOStream;
} // namespace areg

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class String;
    class WideString;
    class SharedBuffer;
} // namespace areg

/************************************************************************
 * \brief   Predefined MACRO to support data read / write streaming
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Macros declaration: declare data as streamable, i.e. available for read and write
// Fine for object with primitives. Might be not proper for objects with multiple instance.
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Declares friend stream operators for a data type (use inside a class).
 **/
#define AREG_DECLARE_STREAMABLE(data_type)                                                                  \
    friend inline const areg::InStream& operator >> (const areg::InStream& stream, data_type& input);       \
    friend inline areg::OutStream& operator << (areg::OutStream& stream, const data_type& output)

/**
 * \brief   Declares global (non-friend) stream operators for a data type.
 **/
#define AREG_GLOBAL_DECLARE_STREAMABLE(data_type)                                                           \
    const areg::InStream& operator >> (const areg::InStream& stream, data_type& input);                     \
    areg::OutStream& operator << (areg::OutStream& stream, const data_type& output)

/**
 * \brief   Implements stream operators for trivially-copyable types via raw memory read/write.
 *          Includes a static_assert to catch misuse with non-trivial types.
 **/
#define AREG_IMPLEMENT_STREAMABLE(data_type)                                                                \
    inline const areg::InStream& operator >> (const areg::InStream& stream, data_type& input)               \
    {   stream.read( reinterpret_cast<uint8_t *>(&input), sizeof(data_type) ); return stream; }             \
    inline areg::OutStream& operator << (areg::OutStream& stream, const data_type& output)                  \
    {   stream.write( reinterpret_cast<const uint8_t *>(&output), sizeof(data_type) ); return stream; }

/**
 * \brief   Declares friend stream operators with DLL export/import specifier.
 **/
#define AREG_DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                                \
    friend ExportDef const areg::InStream& operator >> (const areg::InStream& stream, data_type& input);    \
    friend ExportDef areg::OutStream& operator << (areg::OutStream& stream, const data_type& output);

#define AREG_GLOBAL_DECLARE_STREAMABLE_EXPORT(data_type, ExportDef)                                         \
    ExportDef const areg::InStream& operator >> (const areg::InStream& stream, data_type& input);           \
    ExportDef areg::OutStream& operator << (areg::OutStream& stream, const data_type& output);

#define AREG_IMPLEMENT_READABLE_EXPORT(data_type, ExportDef)                                                \
    ExportDef const areg::InStream& operator >> (const areg::InStream& stream, data_type& input)

#define AREG_IMPLEMENT_WRITABLE_EXPORT(data_type, ExportDef)                                                \
    ExportDef areg::OutStream& operator << (areg::OutStream& stream, const data_type& output)

namespace areg {

//////////////////////////////////////////////////////////////////////////
// InStream class declaration: to read data from
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface and base implementation for reading data from input streams.
 **/
class AREG_API InStream
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    InStream() noexcept = default;

    virtual ~InStream() noexcept = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// InStream convenience read helpers (non-virtual, inline)
/************************************************************************/
    /**
     * \brief   Reads and returns an 8-bit value from the stream.
     **/
    inline uint8_t read8_bits() const noexcept;

    /**
     * \brief   Reads and returns a 16-bit value from the stream.
     **/
    inline uint16_t read16_bits() const noexcept;

    /**
     * \brief   Reads and returns a 32-bit value from the stream.
     **/
    inline uint32_t read32_bits() const noexcept;

    /**
     * \brief   Reads and returns a 64-bit value from the stream.
     **/
    inline uint64_t read64_bits() const noexcept;

    /**
     * \brief   Reads a 32-bit floating-point value from the stream.
     **/
    inline float read32_real() const noexcept;

    /**
     * \brief   Reads a 64-bit floating-point value from the stream.
     **/
    inline double read64_real() const noexcept;

    /**
     * \brief   Returns the element count, limited to the bytes left to read. A container
     *          extractor reads at most this many; an element serialized as zero bytes is
     *          not supported.
     *
     * \param   count   The element count read from the stream.
     * \return  \a count, or the number of bytes left if that is smaller.
     **/
    [[nodiscard]]
    inline uint32_t bounded_count(uint32_t count) const noexcept;

    /**
     * \brief   Reads a String written in the stream string format. Returns the number of bytes read.
     *
     * \param[out] ascii       String to receive the data.
     * \return  The number of bytes read.
     **/
    uint32_t read( String& ascii ) const;

    /**
     * \brief   Reads a WideString written in the stream string format. Returns the number of bytes read.
     *
     * \param[out] wide    WideString to receive the data.
     * \return  The number of bytes read.
     **/
    uint32_t read( WideString& wide ) const;

    /**
     * \brief   Reads a string in the stream string format: a uint32_t count of code units, then the
     *          units. Narrow text is read as bytes; wide text is UTF-16LE on the stream and is
     *          converted when the character type has 4 bytes. A count larger than the readable data
     *          consumes the rest of the stream and gives an empty string.
     *
     * \tparam  CharType    Character type of 1, 2 or 4 bytes.
     * \param[out] output   The string to receive the text.
     * \return  The number of bytes read.
     **/
    template<typename CharType>
    uint32_t read_text(std::basic_string<CharType>& output) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// InStream interface overrides
/************************************************************************/

    /**
     * \brief   Reads data from stream into buffer. Returns the number of bytes read.
     *
     * \param[out] buffer   A buffer to receive the data.
     * \param   size        Maximum number of bytes to read.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read( uint8_t * buffer, uint32_t size ) const noexcept = 0;

    /**
     * \brief   Reads data from stream into a SharedBuffer. Returns the number of bytes read.
     *
     * \param[out] buffer   A SharedBuffer object to receive the data.
     * \return  The number of bytes read.
     **/
    virtual uint32_t read(SharedBuffer& buffer ) const = 0;

    /**
     * \brief   Resets the cursor to the beginning of the stream.
     **/
    virtual void reset() const noexcept = 0;

protected:
    /**
     * \brief   Returns the number of bytes available to read.
     **/
    [[nodiscard]]
    virtual uint32_t size_readable() const noexcept = 0;

    /**
     * \brief   Returns the address of 'size' contiguous bytes at the read position and moves the
     *          position past them; nullptr if the stream cannot give them, then nothing moves.
     *
     * \param   size    The number of bytes to read.
     **/
    [[nodiscard]]
    virtual const uint8_t* read_view(uint32_t size) const noexcept = 0;

private:
    /**
     * \brief   Reads and drops up to 'size' bytes. Returns the number of bytes dropped.
     **/
    inline uint32_t _skip(uint32_t size) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( InStream );
}; 


//////////////////////////////////////////////////////////////////////////
// OutStream class declaration: to write data to
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface and base implementation for writing data to output streams.
 **/
class AREG_API OutStream
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    OutStream() noexcept = default;

    virtual ~OutStream() noexcept = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// OutStream convenience write helpers (non-virtual, inline)
/************************************************************************/
    /**
     * \brief   Writes an 8-bit value to the stream. Returns true if successful.
     **/
    inline bool write8_bits( uint8_t value );

    /**
     * \brief   Writes a 16-bit value to the stream. Returns true if successful.
     **/
    inline bool write16_bits( uint16_t value );

    /**
     * \brief   Writes a 32-bit value to the stream. Returns true if successful.
     **/
    inline bool write32_bits( uint32_t value );

    /**
     * \brief   Writes a 64-bit value to the stream. Returns true if successful.
     **/
    inline bool write64_bits( uint64_t value );

    /**
     * \brief   Writes a 32-bit floating-point value to the stream. Returns true if successful.
     **/
    inline bool write32_real( float value );

    /**
     * \brief   Writes a 64-bit floating-point value to the stream. Returns true if successful.
     **/
    inline bool write64_real( double value );

    /**
     * \brief   Writes a String in the stream string format. Returns the number of bytes written.
     *
     * \param   ascii       The string to write.
     * \return  The number of bytes written.
     **/
    uint32_t write( const String& ascii );

    /**
     * \brief   Writes a WideString in the stream string format. Returns the number of bytes written.
     *
     * \param   wide    The wide string to write.
     * \return  The number of bytes written.
     **/
    uint32_t write( const WideString& wide );

    /**
     * \brief   Writes a string in the stream string format: a uint32_t count of code units, then the
     *          units, no terminator. Narrow text is written as bytes; wide text is written as
     *          UTF-16LE, so a 4-byte character above U+FFFF becomes a surrogate pair and a value
     *          above U+10FFFF becomes U+FFFD.
     *
     * \tparam  CharType    Character type of 1, 2 or 4 bytes.
     * \param   text        The characters to write; nullptr writes an empty string.
     * \param   count       The number of characters in 'text'.
     * \return  The number of bytes written; 0 if nothing was written.
     **/
    template<typename CharType>
    uint32_t write_text(const CharType* text, uint32_t count);

    /**
     * \brief   Writes the characters as stream units without a count: narrow text as bytes, wide
     *          text as UTF-16LE, with the same conversion as write_text().
     *
     * \tparam  CharType    Character type of 1, 2 or 4 bytes.
     * \param   text        The characters to write.
     * \param   count       The number of characters in 'text'.
     * \return  The number of bytes written.
     **/
    template<typename CharType>
    uint32_t write_units(const CharType* text, uint32_t count);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// OutStream interface overrides
/************************************************************************/

    /**
     * \brief   Writes data from buffer to stream. Returns the number of bytes written.
     *
     * \param   buffer      Pointer to the data buffer.
     * \param   size        Number of bytes in the buffer.
     * \return  The number of bytes written.
     **/
    virtual uint32_t write( const uint8_t * buffer, uint32_t size ) = 0;

    /**
     * \brief   Writes data from a SharedBuffer to stream. Returns the number of bytes written.
     *
     * \param   buffer      The SharedBuffer to write.
     * \return  The number of bytes written.
     **/
    virtual uint32_t write( const SharedBuffer& buffer ) = 0;

    /**
     * \brief   Flushes cached data to the output stream.
     **/
    virtual void flush() noexcept = 0;

    /**
     * \brief   Reserve and ensure additional size to the existing. If the free space of the stream to write
     *          is enough, no changes should be done. If the size of the stream is not enough, it should allocate
     *          additional space and should not loose existing data.
     * \param   addSize     The size to add if required.
     * \return  Returns true if the stream has enough space to write the data.
     **/
    virtual bool ensure_size(uint32_t addSize) = 0;

protected:
    /**
     * \brief   Returns the available space in bytes remaining to write.
     **/
    [[nodiscard]]
    virtual uint32_t size_writable() const noexcept = 0;

    /**
     * \brief   Returns the address of 'size' contiguous bytes at the write position, which the caller
     *          fills, and moves the position past them; nullptr if the stream cannot give them, then
     *          nothing moves.
     *
     * \param   size    The number of bytes to write.
     **/
    [[nodiscard]]
    virtual uint8_t* write_view(uint32_t size) noexcept = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( OutStream );
};

//////////////////////////////////////////////////////////////////////////
// IOStream class declaration: to read / write data
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Input and output streaming interface.
 **/
class AREG_API IOStream : public InStream
                        , public OutStream
{
protected:
    IOStream() noexcept = default;

public:
    virtual ~IOStream() noexcept = default;

public:
    /**
     * \brief   Writes an ASCII string to the stream.
     **/
    friend inline OutStream& operator << (OutStream& stream, const char * output);

    /**
     * \brief   Writes a wide string to the stream.
     **/
    friend inline OutStream& operator << (OutStream& stream, const wchar_t * output);

    /**
     * \brief   Writes an STL string to the stream.
     **/
    template<typename CharType>
    friend inline OutStream& operator << (OutStream& stream, const std::basic_string<CharType>& output);
    /**
     * \brief   Writes an STL string_view to the stream.
     **/
    template<typename CharType>
    friend inline OutStream& operator << (OutStream& stream, const std::basic_string_view<CharType>& output);

    /**
     * \brief   Reads an STL string from the stream.
     **/
    template<typename CharType>
    friend inline const InStream& operator >> (const InStream& stream, std::basic_string<CharType>& input);

    /**
     * \brief   Writes an std::deque to the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline OutStream& operator << (OutStream& stream, const std::deque<ElemType>& output);
    /**
     * \brief   Reads an std::deque from the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline const InStream& operator >> (const InStream& stream, std::deque<ElemType>& input);

    /**
     * \brief   Writes an std::list to the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline OutStream& operator << (OutStream& stream, const std::list<ElemType>& output);
    /**
     * \brief   Reads an std::list from the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline const InStream& operator >> (const InStream& stream, std::list<ElemType>& input);

    /**
     * \brief   Writes an std::vector to the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline OutStream& operator << (OutStream& stream, const std::vector<ElemType>& output);
    /**
     * \brief   Reads an std::vector from the stream. Each element must be streamable.
     **/
    template<typename ElemType>
    friend inline const InStream& operator >> (const InStream& stream, std::vector<ElemType>& input);

    /**
     * \brief   Writes an std::pair to the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline OutStream& operator << (OutStream& stream, const std::pair<Key, Value>& output);
    /**
     * \brief   Reads an std::pair from the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline const InStream& operator >> (const InStream& stream, std::pair<Key, Value>& input);

    /**
     * \brief   Writes an std::map to the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline OutStream& operator << (OutStream& stream, const std::map<Key, Value>& output);
    /**
     * \brief   Reads an std::map from the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline const InStream& operator >> (const InStream& stream, std::map<Key, Value>& input);

    /**
     * \brief   Writes an std::unordered_map to the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline OutStream& operator << (OutStream& stream, const std::unordered_map<Key, Value>& output);
    /**
     * \brief   Reads an std::unordered_map from the stream. Each element must be streamable.
     **/
    template<typename Key, typename Value>
    friend inline const InStream& operator >> (const InStream& stream, std::unordered_map<Key, Value>& input);


//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( IOStream );
};

//////////////////////////////////////////////////////////////////////////
// Inline function implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * \brief   InStream convenience helpers -- inline implementations
 ************************************************************************/

namespace text_units
{
    //!< Number of UTF-16 units converted per step.
    constexpr uint32_t CHUNK{ 256u };

    /**
     * \brief   Returns the number of UTF-16 units of 4-byte characters and sets 'simple' to true
     *          if no character is above U+FFFF.
     **/
    template<typename CharType>
    inline uint32_t count_units(const CharType* text, uint32_t count, bool& simple) noexcept
    {
        uint32_t over{ 0u };
        for (uint32_t i = 0u; i < count; ++i)
        {
            over |= (static_cast<uint32_t>(text[i]) & 0xFFFF0000u);
        }

        simple = (over == 0u);
        uint32_t units{ count };
        if (simple == false)
        {
            for (uint32_t i = 0u; i < count; ++i)
            {
                units += ((static_cast<uint32_t>(text[i]) - 0x10000u) <= 0xFFFFFu) ? 1u : 0u;
            }
        }

        return units;
    }

    /**
     * \brief   Writes 4-byte characters as UTF-16LE to 'dst' of any alignment and returns the bytes
     *          written. A value above U+10FFFF is written as U+FFFD.
     **/
    template<typename CharType>
    inline uint32_t encode(const CharType* text, uint32_t count, uint8_t* dst, bool simple) noexcept
    {
        if (simple)
        {
            for (size_t i = 0u; i < count; ++i)
            {
                const uint16_t unit{ static_cast<uint16_t>(text[i]) };
                std::memcpy(dst + i * 2u, &unit, 2u);
            }

            return count * 2u;
        }

        uint8_t* out{ dst };
        for (uint32_t i = 0u; i < count; ++i)
        {
            const uint32_t code{ static_cast<uint32_t>(text[i]) };
            if (code < 0x10000u)
            {
                const uint16_t unit{ static_cast<uint16_t>(code) };
                std::memcpy(out, &unit, 2u);
                out += 2u;
            }
            else if (code <= 0x10FFFFu)
            {
                const uint16_t high{ static_cast<uint16_t>(0xD800u | ((code - 0x10000u) >> 10u)) };
                const uint16_t low{ static_cast<uint16_t>(0xDC00u | ((code - 0x10000u) & 0x3FFu)) };
                std::memcpy(out, &high, 2u);
                std::memcpy(out + 2u, &low, 2u);
                out += 4u;
            }
            else
            {
                const uint16_t unit{ static_cast<uint16_t>(0xFFFDu) };
                std::memcpy(out, &unit, 2u);
                out += 2u;
            }
        }

        return static_cast<uint32_t>(out - dst);
    }

    /**
     * \brief   Reads 'units' UTF-16LE units at 'src' of any alignment into 4-byte characters at 'dst'
     *          and returns the characters written. A surrogate pair becomes one character, a lone
     *          surrogate stays as it is; 'high' carries a high surrogate to the next call.
     **/
    template<typename CharType>
    inline uint32_t decode(const uint8_t* src, uint32_t units, CharType* dst, uint32_t& high) noexcept
    {
        if (high == 0u)
        {
            uint32_t surrogates{ 0u };
            for (size_t i = 0u; i < units; ++i)
            {
                uint16_t unit;
                std::memcpy(&unit, src + i * 2u, 2u);
                surrogates |= ((static_cast<uint32_t>(unit) - 0xD800u) < 0x800u) ? 1u : 0u;
                dst[i] = static_cast<CharType>(unit);
            }

            if (surrogates == 0u)
                return units;
        }

        const auto load = [src](size_t at) -> uint32_t
        {
            uint16_t unit;
            std::memcpy(&unit, src + at * 2u, 2u);
            return unit;
        };

        uint32_t result{ 0u };
        size_t i{ 0u };
        if (high != 0u)
        {
            const uint32_t low{ load(0u) };
            if ((low & 0xFC00u) == 0xDC00u)
            {
                dst[result ++] = static_cast<CharType>(0x10000u + ((high - 0xD800u) << 10u) + (low - 0xDC00u));
                i = 1u;
            }
            else
            {
                dst[result ++] = static_cast<CharType>(high);
            }

            high = 0u;
        }

        while (i < units)
        {
            const uint32_t unit{ load(i) };
            if ((unit & 0xFC00u) == 0xD800u)
            {
                if ((i + 1u) == units)
                {
                    high = unit;
                    break;
                }

                const uint32_t low{ load(i + 1u) };
                if ((low & 0xFC00u) == 0xDC00u)
                {
                    dst[result ++] = static_cast<CharType>(0x10000u + ((unit - 0xD800u) << 10u) + (low - 0xDC00u));
                    i += 2u;
                    continue;
                }
            }

            dst[result ++] = static_cast<CharType>(unit);
            ++ i;
        }

        return result;
    }
} // namespace text_units

inline uint8_t areg::InStream::read8_bits() const noexcept
{
    uint8_t result{ 0u };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(uint8_t));
    return result;
}

inline uint16_t areg::InStream::read16_bits() const noexcept
{
    uint16_t result{ 0u };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(uint16_t));
    return result;
}

inline uint32_t areg::InStream::read32_bits() const noexcept
{
    uint32_t result{ 0u };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(uint32_t));
    return result;
}

inline uint64_t areg::InStream::read64_bits() const noexcept
{
    uint64_t result{ 0u };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(uint64_t));
    return result;
}

inline float areg::InStream::read32_real() const noexcept
{
    float result{ 0.0f };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(float));
    return result;
}

inline double areg::InStream::read64_real() const noexcept
{
    double result{ 0.0 };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(double));
    return result;
}

inline uint32_t areg::InStream::bounded_count(uint32_t count) const noexcept
{
    const uint32_t readable{ size_readable() };
    return (count < readable ? count : readable);
}

inline uint32_t areg::InStream::_skip(uint32_t size) const noexcept
{
    uint8_t scratch[256];
    uint32_t result{ 0u };
    while (result < size)
    {
        const uint32_t part{ (size - result) < sizeof(scratch) ? (size - result) : static_cast<uint32_t>(sizeof(scratch)) };
        const uint32_t done{ read(scratch, part) };
        result += done;
        if (done != part)
            break;
    }

    return result;
}

template<typename CharType>
inline uint32_t areg::InStream::read_text(std::basic_string<CharType>& output) const
{
    static_assert((sizeof(CharType) == 1u) || (sizeof(CharType) == 2u) || (sizeof(CharType) == 4u), "Unsupported character type");
    constexpr uint32_t UNIT{ sizeof(CharType) == 1u ? 1u : 2u };
    constexpr uint32_t HEAD{ static_cast<uint32_t>(sizeof(uint32_t)) };

    output.clear();
    uint32_t count{ 0u };
    const uint8_t* const head{ read_view(HEAD) };
    if (head != nullptr)
    {
        std::memcpy(&count, head, HEAD);
        if (count == 0u)
            return HEAD;

        const uint8_t* const data{ count <= (0xFFFFFFFFu / UNIT) ? read_view(count * UNIT) : nullptr };
        if (data == nullptr)
            return HEAD + _skip(size_readable());

        if constexpr (sizeof(CharType) == 1u)
        {
            output.assign(reinterpret_cast<const CharType*>(data), static_cast<size_t>(count));
        }
        else if constexpr (sizeof(CharType) == 2u)
        {
            output.resize(count);
            std::memcpy(output.data(), data, static_cast<size_t>(count) * UNIT);
        }
        else
        {
            output.resize(count);
            uint32_t high{ 0u };
            uint32_t length{ text_units::decode(data, count, output.data(), high) };
            if (high != 0u)
            {
                output[length ++] = static_cast<CharType>(high);
            }

            output.resize(length);
        }

        return HEAD + count * UNIT;
    }

    const uint32_t done{ read(reinterpret_cast<uint8_t*>(&count), HEAD) };
    if ((done != HEAD) || (count == 0u))
        return done;

    const uint32_t readable{ size_readable() };
    if (count > (readable / UNIT))
        return HEAD + _skip(readable);

    output.resize(count);
    if constexpr (sizeof(CharType) != 4u)
    {
        const uint32_t size{ count * UNIT };
        const uint32_t got{ read(reinterpret_cast<uint8_t*>(output.data()), size) };
        output.resize(got / UNIT);
        return HEAD + got;
    }
    else
    {
        uint8_t bytes[text_units::CHUNK * 2u];
        uint32_t length{ 0u };
        uint32_t high{ 0u };
        uint32_t result{ HEAD };
        uint32_t left{ count };
        while (left != 0u)
        {
            const uint32_t part{ left < text_units::CHUNK ? left : text_units::CHUNK };
            const uint32_t got{ read(bytes, part * UNIT) };
            result += got;
            length += text_units::decode(bytes, got / UNIT, output.data() + length, high);
            if (got != (part * UNIT))
                break;

            left -= part;
        }

        if (high != 0u)
        {
            output[length ++] = static_cast<CharType>(high);
        }

        output.resize(length);
        return result;
    }
}

/************************************************************************
 * \brief   OutStream convenience helpers -- inline implementations
 ************************************************************************/

inline bool areg::OutStream::write8_bits( uint8_t value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(uint8_t)) == sizeof(uint8_t);
}

inline bool areg::OutStream::write16_bits( uint16_t value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(uint16_t)) == sizeof(uint16_t);
}

inline bool areg::OutStream::write32_bits( uint32_t value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(uint32_t)) == sizeof(uint32_t);
}

inline bool areg::OutStream::write64_bits( uint64_t value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(uint64_t)) == sizeof(uint64_t);
}

inline bool areg::OutStream::write32_real( float value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(float)) == sizeof(float);
}

inline bool areg::OutStream::write64_real( double value )
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(double)) == sizeof(double);
}

template<typename CharType>
inline uint32_t areg::OutStream::write_text(const CharType* text, uint32_t count)
{
    static_assert((sizeof(CharType) == 1u) || (sizeof(CharType) == 2u) || (sizeof(CharType) == 4u), "Unsupported character type");
    constexpr uint32_t UNIT{ sizeof(CharType) == 1u ? 1u : 2u };
    constexpr uint32_t HEAD{ static_cast<uint32_t>(sizeof(uint32_t)) };
    constexpr uint32_t MAX_UNITS{ (0xFFFFFFFFu - HEAD) / 4u };

    if (text == nullptr)
    {
        count = 0u;
    }

    if (count > MAX_UNITS)
        return 0u;

    bool simple{ true };
    uint32_t units{ count };
    if constexpr (sizeof(CharType) == 4u)
    {
        units = text_units::count_units(text, count, simple);
    }

    const uint32_t bytes{ units * UNIT };
    uint8_t* const dst{ write_view(HEAD + bytes) };
    if (dst != nullptr)
    {
        std::memcpy(dst, &units, HEAD);
        if constexpr (sizeof(CharType) != 4u)
        {
            if (bytes != 0u)
            {
                std::memcpy(dst + HEAD, text, bytes);
            }
        }
        else
        {
            text_units::encode(text, count, dst + HEAD, simple);
        }

        return HEAD + bytes;
    }

    if (write(reinterpret_cast<const uint8_t*>(&units), HEAD) != HEAD)
        return 0u;

    return HEAD + write_units(text, count);
}

template<typename CharType>
inline uint32_t areg::OutStream::write_units(const CharType* text, uint32_t count)
{
    static_assert((sizeof(CharType) == 1u) || (sizeof(CharType) == 2u) || (sizeof(CharType) == 4u), "Unsupported character type");
    if ((text == nullptr) || (count == 0u))
        return 0u;

    if constexpr (sizeof(CharType) != 4u)
    {
        return write(reinterpret_cast<const uint8_t*>(text), count * static_cast<uint32_t>(sizeof(CharType)));
    }
    else
    {
        constexpr uint32_t PART{ text_units::CHUNK / 2u };
        uint8_t bytes[text_units::CHUNK * 2u];
        uint32_t result{ 0u };
        while (count != 0u)
        {
            const uint32_t part{ count < PART ? count : PART };
            bool simple{ true };
            static_cast<void>(text_units::count_units(text, part, simple));
            result += write(bytes, text_units::encode(text, part, bytes, simple));
            text  += part;
            count -= part;
        }

        return result;
    }
}

/************************************************************************
 * \brief   Support streaming of primitives
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// MACRO make primitives streamable
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE(bool)
AREG_IMPLEMENT_STREAMABLE(char)
AREG_IMPLEMENT_STREAMABLE(uint8_t)
AREG_IMPLEMENT_STREAMABLE(int16_t)
AREG_IMPLEMENT_STREAMABLE(uint16_t)
AREG_IMPLEMENT_STREAMABLE(int32_t)
AREG_IMPLEMENT_STREAMABLE(uint32_t)
AREG_IMPLEMENT_STREAMABLE(int64_t)
AREG_IMPLEMENT_STREAMABLE(uint64_t)
AREG_IMPLEMENT_STREAMABLE(float)
AREG_IMPLEMENT_STREAMABLE(double)

/**
 * \brief   A single wchar_t is streamed as a uint32_t on every platform. A 2-byte wchar_t
 *          receives U+FFFD for a value above U+FFFF.
 **/
inline const areg::InStream& operator >> (const areg::InStream& stream, wchar_t& input)
{
    uint32_t value{ 0u };
    stream.read(reinterpret_cast<uint8_t*>(&value), static_cast<uint32_t>(sizeof(uint32_t)));
    if constexpr (sizeof(wchar_t) == 2u)
    {
        input = static_cast<wchar_t>(value <= 0xFFFFu ? value : 0xFFFDu);
    }
    else
    {
        input = static_cast<wchar_t>(value);
    }

    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const wchar_t& output)
{
    const uint32_t value{ static_cast<uint32_t>(output) };
    stream.write(reinterpret_cast<const uint8_t*>(&value), static_cast<uint32_t>(sizeof(uint32_t)));
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const char * output)
{
    stream.write_text(output, output != nullptr ? static_cast<uint32_t>(std::char_traits<char>::length(output)) : 0u);
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const wchar_t * output)
{
    stream.write_text(output, output != nullptr ? static_cast<uint32_t>(std::char_traits<wchar_t>::length(output)) : 0u);
    return stream;
}

template<typename CharType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::basic_string<CharType>& output)
{
    stream.write_text(output.data(), static_cast<uint32_t>(output.length()));
    return stream;
}

template<typename CharType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::basic_string_view<CharType>& output)
{
    stream.write_text(output.data(), static_cast<uint32_t>(output.length()));
    return stream;
}

template<typename CharType>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::basic_string<CharType>& input)
{
    stream.read_text(input);
    return stream;
}

template<typename ElemType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::deque<ElemType>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::deque<ElemType>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    input.resize(stream.bounded_count(size));
    for (auto& elem : input)
    {
        stream >> elem;
    }

    return stream;
}

template<typename ElemType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::list<ElemType>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::list<ElemType>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    input.resize(stream.bounded_count(size));
    for (auto& elem : input)
    {
        stream >> elem;
    }

    return stream;
}

template<typename ElemType>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::vector<ElemType>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const auto& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename ElemType>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::vector<ElemType>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    input.resize(stream.bounded_count(size));
    for (auto& elem : input)
    {
        stream >> elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::pair<Key, Value>& output)
{
    stream << output.first;
    stream << output.second;
    return stream;
}

template<typename Key, typename Value>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::pair<Key, Value>& input)
{
    stream >> input.first;
    stream >> input.second;
    return stream;
}

template<typename Key, typename Value>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::map<Key, Value>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const std::pair<const Key, Value>& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::map<Key, Value>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    size = stream.bounded_count(size);
    for (uint32_t i = 0; i < size; ++i)
    {
        std::pair<Key, Value> elem;
        stream >> elem;
        input[elem.first] = elem.second;
    }

    return stream;
}

template<typename Key, typename Value>
inline areg::OutStream& operator << (areg::OutStream& stream, const std::unordered_map<Key, Value>& output)
{
    stream << static_cast<uint32_t>(output.size());
    for (const std::pair<const Key, Value>& elem : output)
    {
        stream << elem;
    }

    return stream;
}

template<typename Key, typename Value>
inline const areg::InStream& operator >> (const areg::InStream& stream, std::unordered_map<Key, Value>& input)
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    size = stream.bounded_count(size);
    for (uint32_t i = 0; i < size; ++i)
    {
        std::pair<Key, Value> elem;
        stream >> elem;
        input[elem.first] = elem.second;
    }

    return stream;
}

} // namespace areg

#endif  // AREG_BASE_IOSTREAM_HPP
