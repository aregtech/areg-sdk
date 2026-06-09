#ifndef AREG_BASE_TABLEDEFS_HPP
#define AREG_BASE_TABLEDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/TableDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, compile-time lookup tables for CRC and other
 *              hash utilities. Include via MathDefs.hpp; do not include directly.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <array>
#include <cstdint>

namespace areg {

/************************************************************************/
// CRC32C (Castagnoli) table generation
/************************************************************************/

/**
 * \brief   Computes one entry of the CRC32C (Castagnoli) lookup table for the
 *          given input byte. Uses the reflected (LSB-first) polynomial
 *          representation 0x82F63B78, which is the bit-reversal of the
 *          canonical Castagnoli polynomial 0x1EDC6F41.
 *
 *          CRC polynomial:
 *          X^32+X^28+X^27+X^26+X^25+X^23+X^22+X^20+X^19+X^18+X^14+X^13+X^11+X^10+X^9+X^8+X^6+1
 *
 *          Hardware CRC32 instruction -- _mm_crc32_u*. Choosing this polynomial
 *          keeps the software and hardware paths bit-for-bit identical.
 *
 * \param   byte    Input byte value (0-255) for which to compute the entry.
 * \return  32-bit CRC32C table entry corresponding to the given byte.
 **/
[[nodiscard]]
constexpr uint32_t crc32_table_entry(uint8_t byte) noexcept
{
    uint32_t crc { static_cast<uint32_t>(byte) };
    for (int bit = 0; bit < 8; ++bit)
    {
        // Branchless: -(crc & 1u) is 0x00000000 when LSB=0, 0xFFFFFFFF when LSB=1.
        // Equivalent to: if (crc & 1) crc = (crc >> 1) ^ 0x82F63B78; else crc >>= 1;
        crc = (crc >> 1) ^ (0x82F63B78u & static_cast<uint32_t>(0u - (crc & 1u)));
    }

    return crc;
}

/**
 * \brief   Generates the full 256-entry CRC32C (Castagnoli) lookup table at compile time.
 *          Called once by the CRC32_TABLE initializer; not intended for direct use.
 *
 * \return  std::array of 256 uint32_t entries forming the complete CRC32C lookup table.
 **/
[[nodiscard]]
constexpr std::array<uint32_t, 256> make_crc32_table() noexcept
{
    std::array<uint32_t, 256> table { };
    for (uint32_t i = 0u; i < 256u; ++i)
    {
        table[i] = crc32_table_entry(static_cast<uint8_t>(i));
    }

    return table;
}

/**
 * \brief   CRC32C (Castagnoli) lookup table, fully evaluated at compile time.
 *          256 entries × 4 bytes = 1 024 bytes in read-only data.
 *
 *          Declared inline constexpr so that exactly one copy exists in the
 *          final binary regardless of how many translation units include this
 *          header.  All crc32_calculate() and crc32_start() functions in
 *          MathDefs.hpp read from this table.  Matches the polynomial used by
 *          the SSE4.2 _mm_crc32_u* hardware instructions.
 **/
inline constexpr std::array<uint32_t, 256> CRC32_TABLE { make_crc32_table() };

/**
 * \brief   Character types. Used bits are defining special meaning of characters.
 *          areg::CharCategory
 **/
enum class CharCategory : uint16_t
{
      Undefined =   0   //!< bin = 0000 0000 0000 0000, Undefined character type
    , EOfS      =   1   //!< bin = 0000 0000 0000 0001, End-of-string character
    , EndOfLine =   2   //!< bin = 0000 0000 0000 0010, End-of-line character
    , CarReturn =   4   //!< bin = 0000 0000 0000 0100, Carriage return character
    , WhiteSpace=   8   //!< bin = 0000 0000 0000 1000, Whitespace character
    , Delimiter =  16   //!< bin = 0000 0000 0001 0000, Syntax Delimiter
    , Control   =  32   //!< bin = 0000 0000 0010 0000, Control key / value
    , Printable =  64   //!< bin = 0000 0000 0100 0000, Printable character or character, which change text layout like space or tab
    , Number    = 128   //!< bin = 0000 0000 1000 0000, Numeric character
    , Symbol    = 256   //!< bin = 0000 0001 0000 0000, Symbol
    , Letter    = 512   //!< bin = 0000 0010 0000 0000, Letter
    , LetterUp  = 1536  //!< bin = 0000 0110 0000 0000, Upper case letter
    , LetterLo  = 2560  //!< bin = 0000 1010 0000 0000, Lower case letter
};

//!< Bit-flag aliases matching areg::CharCategory values (mirrored here to avoid circular inclusion).

//!< Unclassified or reserved code point.
inline constexpr uint16_t TBL_DUMMY{ static_cast<uint16_t>(areg::CharCategory::Undefined) };

//!< Null byte -- end of string marker.
inline constexpr uint16_t TBL_EOFS{ static_cast<uint16_t>(areg::CharCategory::EOfS) };

//!< End-of-line character (e.g. '\n'); also acts as whitespace.
inline constexpr uint16_t TBL_EOFL{ static_cast<uint16_t>(areg::CharCategory::EndOfLine) | static_cast<uint16_t>(areg::CharCategory::WhiteSpace) };

//!< Carriage-return character ('\r'); also treated as end-of-line.
inline constexpr uint16_t TBL_DEOL{ static_cast<uint16_t>(areg::CharCategory::CarReturn) | static_cast<uint16_t>(areg::CharCategory::EndOfLine) };

//!< Space character (0x20); acts as both whitespace and token delimiter.
inline constexpr uint16_t TBL_SPACE{ static_cast<uint16_t>(areg::CharCategory::WhiteSpace) | static_cast<uint16_t>(areg::CharCategory::Delimiter) };

//!< Non-printable control code that is also treated as whitespace (e.g. '\t').
inline constexpr uint16_t TBL_CTRL{ static_cast<uint16_t>(areg::CharCategory::Control) | static_cast<uint16_t>(areg::CharCategory::WhiteSpace) };

//!< Any printable character (letters, digits, symbols, space, tab).
inline constexpr uint16_t TBL_PRINT{ static_cast<uint16_t>(areg::CharCategory::Printable) };

//!< Decimal digit (0-9); also printable.
inline constexpr uint16_t TBL_NUMBER{ static_cast<uint16_t>(areg::CharCategory::Number) | static_cast<uint16_t>(areg::CharCategory::Printable) };

//!< Printable punctuation or special symbol.
inline constexpr uint16_t TBL_SYMBOL{ static_cast<uint16_t>(areg::CharCategory::Symbol) | static_cast<uint16_t>(areg::CharCategory::Printable) };

//!< Uppercase letter; also printable.
inline constexpr uint16_t TBL_LET_UP{ static_cast<uint16_t>(areg::CharCategory::LetterUp) | static_cast<uint16_t>(areg::CharCategory::Printable) };

//!< Lowercase letter; also printable.
inline constexpr uint16_t TBL_LET_LO{ static_cast<uint16_t>(areg::CharCategory::LetterLo) | static_cast<uint16_t>(areg::CharCategory::Printable) };

/**
 * \brief   Character category bit-flags for the first 256 UTF-8 code page symbols.
 *          Reference: https://www.charset.org/utf-8
 **/
inline constexpr uint16_t UTF8_256_Table[256] =
{
    /*   0 -   0 */   TBL_EOFS  | TBL_EOFL | TBL_CTRL                                                                              //   '\0', Null, end of string
    /*   1 -   4 */ , TBL_EOFL  | TBL_CTRL              , TBL_EOFL | TBL_CTRL               , TBL_EOFL | TBL_CTRL   , TBL_EOFL | TBL_CTRL
    /*   5 -   8 */ , TBL_EOFL  | TBL_CTRL              , TBL_EOFL | TBL_CTRL               , TBL_EOFL | TBL_CTRL   , TBL_EOFL | TBL_CTRL
    /*   9 -  12 */ , TBL_SPACE | TBL_CTRL | TBL_PRINT  , TBL_EOFL | TBL_CTRL | TBL_PRINT   , TBL_PRINT | TBL_CTRL  , TBL_EOFL | TBL_CTRL
    /*  13 -  16 */ , TBL_DEOL  | TBL_CTRL | TBL_PRINT  , TBL_EOFL | TBL_CTRL               , TBL_EOFL | TBL_CTRL   , TBL_EOFL | TBL_CTRL
    /*  17 -  20 */ , TBL_EOFL  | TBL_CTRL              , TBL_EOFL | TBL_CTRL               , TBL_EOFL | TBL_CTRL   , TBL_EOFL | TBL_CTRL
    /*  21 -  24 */ , TBL_EOFL  | TBL_CTRL              , TBL_EOFL | TBL_CTRL               , TBL_EOFL | TBL_CTRL   , TBL_EOFL | TBL_CTRL
    /*  25 -  28 */ , TBL_EOFL  | TBL_CTRL              , TBL_EOFL | TBL_CTRL               , TBL_EOFL | TBL_CTRL   , TBL_EOFL | TBL_CTRL
    /*  29 -  32 */ , TBL_EOFL  | TBL_CTRL              , TBL_EOFL | TBL_CTRL               , TBL_EOFL | TBL_CTRL   , TBL_SPACE| TBL_PRINT
    /*  33 -  36 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /*  37 -  40 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /*  41 -  44 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /*  45 -  48 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_NUMBER
    /*  49 -  52 */ , TBL_NUMBER                        , TBL_NUMBER                        , TBL_NUMBER            , TBL_NUMBER
    /*  53 -  56 */ , TBL_NUMBER                        , TBL_NUMBER                        , TBL_NUMBER            , TBL_NUMBER
    /*  57 -  60 */ , TBL_NUMBER                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /*  61 -  64 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /*  65 -  68 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /*  69 -  72 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /*  73 -  76 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /*  77 -  80 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /*  81 -  84 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /*  85 -  88 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /*  89 -  92 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_SYMBOL            , TBL_SYMBOL
    /*  93 -  96 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /*  97 - 100 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 101 - 104 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 105 - 108 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 109 - 112 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 113 - 116 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 117 - 120 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 121 - 124 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_SYMBOL            , TBL_SYMBOL
    /* 125 - 128 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_EOFL   | TBL_CTRL , TBL_SYMBOL | TBL_CTRL
    /* 129 - 132 */ , TBL_DUMMY  | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL , TBL_SYMBOL | TBL_CTRL
    /* 133 - 136 */ , TBL_SYMBOL | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL , TBL_SYMBOL | TBL_CTRL
    /* 137 - 140 */ , TBL_SYMBOL | TBL_CTRL             , TBL_LET_UP | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL , TBL_LET_UP | TBL_CTRL
    /* 141 - 144 */ , TBL_EOFL   | TBL_CTRL             , TBL_LET_UP | TBL_CTRL             , TBL_EOFL   | TBL_CTRL , TBL_EOFL   | TBL_CTRL
    /* 145 - 148 */ , TBL_SYMBOL | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL , TBL_SYMBOL | TBL_CTRL
    /* 149 - 152 */ , TBL_SYMBOL | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL , TBL_SYMBOL | TBL_CTRL
    /* 153 - 156 */ , TBL_SYMBOL | TBL_CTRL             , TBL_LET_LO | TBL_CTRL             , TBL_SYMBOL | TBL_CTRL , TBL_LET_LO | TBL_CTRL
    /* 157 - 160 */ , TBL_EOFL   | TBL_CTRL             , TBL_LET_LO | TBL_CTRL             , TBL_LET_UP | TBL_CTRL , TBL_SYMBOL
    /* 161 - 164 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /* 165 - 168 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /* 169 - 172 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /* 173 - 176 */ , TBL_CTRL                          , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /* 177 - 180 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /* 181 - 184 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /* 185 - 188 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_SYMBOL
    /* 189 - 192 */ , TBL_SYMBOL                        , TBL_SYMBOL                        , TBL_SYMBOL            , TBL_LET_UP
    /* 193 - 196 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /* 197 - 200 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /* 201 - 204 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /* 205 - 208 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /* 209 - 212 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /* 213 - 216 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_SYMBOL            , TBL_LET_UP
    /* 217 - 220 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_UP            , TBL_LET_UP
    /* 221 - 224 */ , TBL_LET_UP                        , TBL_LET_UP                        , TBL_LET_LO            , TBL_LET_LO
    /* 225 - 228 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 229 - 232 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 233 - 236 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 237 - 240 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 241 - 244 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 245 - 248 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_SYMBOL            , TBL_LET_LO
    /* 249 - 252 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO            , TBL_LET_LO
    /* 253 - 255 */ , TBL_LET_LO                        , TBL_LET_LO                        , TBL_LET_LO
};

/**
 * \brief   Lower-case mapping table for the first 256 UTF-8 code page symbols.
 *          Non-letter values map to themselves.
 *          Reference: https://www.charset.org/utf-8
 **/
inline constexpr uint8_t UTF8_256_Table_lower[256] =
{
    /*   0 -   0 */     0
    /*   1 -   4 */ ,   1,   2,   3,   4
    /*   5 -   8 */ ,   5,   6,   7,   8
    /*   9 -  12 */ ,   9,  10,  11,  12
    /*  13 -  16 */ ,  13,  14,  15,  16
    /*  17 -  20 */ ,  17,  18,  19,  20
    /*  21 -  24 */ ,  21,  22,  23,  24
    /*  25 -  28 */ ,  25,  26,  27,  28
    /*  29 -  32 */ ,  29,  30,  31,  32
    /*  33 -  36 */ ,  33,  34,  35,  36
    /*  37 -  40 */ ,  37,  38,  39,  40
    /*  41 -  44 */ ,  41,  42,  43,  44
    /*  45 -  48 */ ,  45,  46,  47,  48
    /*  49 -  52 */ ,  49,  50,  51,  52
    /*  53 -  56 */ ,  53,  54,  55,  56
    /*  57 -  60 */ ,  57,  58,  59,  60
    /*  61 -  64 */ ,  61,  62,  63,  64
    /*  65 -  68 */ ,  97 /* 'a' */,  98 /* 'b' */,  99 /* 'c' */, 100 /* 'd' */
    /*  69 -  72 */ , 101 /* 'e' */, 102 /* 'f' */, 103 /* 'g' */, 104 /* 'h' */
    /*  73 -  76 */ , 105 /* 'i' */, 106 /* 'j' */, 107 /* 'k' */, 108 /* 'l' */
    /*  77 -  80 */ , 109 /* 'm' */, 110 /* 'n' */, 111 /* 'o' */, 112 /* 'p' */
    /*  81 -  84 */ , 113 /* 'q' */, 114 /* 'r' */, 115 /* 's' */, 116 /* 't' */
    /*  85 -  88 */ , 117 /* 'u' */, 118 /* 'v' */, 119 /* 'w' */, 120 /* 'x' */
    /*  89 -  92 */ , 121 /* 'y' */, 122 /* 'z' */,  91,  92
    /*  93 -  96 */ ,  93,  94,  95,  96
    /*  97 - 100 */ ,  97 /* 'a' */,  98 /* 'b' */,  99 /* 'c' */, 100 /* 'd' */
    /* 101 - 104 */ , 101 /* 'e' */, 102 /* 'f' */, 103 /* 'g' */, 104 /* 'h' */
    /* 105 - 108 */ , 105 /* 'i' */, 106 /* 'j' */, 107 /* 'k' */, 108 /* 'l' */
    /* 109 - 112 */ , 109 /* 'm' */, 110 /* 'n' */, 111 /* 'o' */, 112 /* 'p' */
    /* 113 - 116 */ , 113 /* 'q' */, 114 /* 'r' */, 115 /* 's' */, 116 /* 't' */
    /* 117 - 120 */ , 117 /* 'u' */, 118 /* 'v' */, 119 /* 'w' */, 120 /* 'x' */
    /* 121 - 124 */ , 121 /* 'y' */, 122 /* 'z' */, 123, 124
    /* 125 - 128 */ , 125, 126, 127, 128
    /* 129 - 132 */ , 129, 130, 131, 132
    /* 133 - 136 */ , 133, 134, 135, 136
    /* 137 - 140 */ , 137, 154 /* 'š' */, 139, 156 /* 'œ' */
    /* 141 - 144 */ , 141, 158 /* 'ž' */, 143, 144
    /* 145 - 148 */ , 145, 146, 147, 148
    /* 149 - 152 */ , 149, 150, 151, 152
    /* 153 - 156 */ , 153, 154 /* 'š' */, 155, 156 /* 'œ' */
    /* 157 - 160 */ , 157, 158 /* 'ž' */, 255 /* 'ÿ' */, 160
    /* 161 - 164 */ , 161, 162, 163, 164
    /* 165 - 168 */ , 165, 166, 167, 168
    /* 169 - 172 */ , 169, 170, 171, 172
    /* 173 - 176 */ , 173, 174, 175, 176
    /* 177 - 180 */ , 177, 178, 179, 180
    /* 181 - 184 */ , 181, 182, 183, 184
    /* 185 - 188 */ , 185, 186, 187, 188
    /* 189 - 192 */ , 189, 190, 191, 224 /* 'à' */
    /* 193 - 196 */ , 225 /* 'á' */, 226 /* 'â' */, 227 /* 'ã' */, 228 /* 'ä' */
    /* 197 - 200 */ , 229 /* 'å' */, 230 /* 'æ' */, 231 /* 'ç' */, 232 /* 'è' */
    /* 201 - 204 */ , 233 /* 'é' */, 234 /* 'ê' */, 235 /* 'ë' */, 236 /* 'ì' */
    /* 205 - 208 */ , 237 /* 'í' */, 238 /* 'î' */, 239 /* 'ï' */, 240 /* 'ð' */
    /* 209 - 212 */ , 241 /* 'ñ' */, 242 /* 'ò' */, 243 /* 'ó' */, 244 /* 'ô' */
    /* 213 - 216 */ , 245 /* 'õ' */, 246 /* 'ö' */, 215, 248 /* 'ø' */
    /* 217 - 220 */ , 249 /* 'ù' */, 250 /* 'ú' */, 251 /* 'û' */, 252 /* 'ü' */
    /* 221 - 224 */ , 253 /* 'ý' */, 254 /* 'þ' */, 223 /* 'ß' */, 224 /* 'à' */
    /* 225 - 228 */ , 225 /* 'á' */, 226 /* 'â' */, 227 /* 'ã' */, 228 /* 'ä' */
    /* 229 - 232 */ , 229 /* 'å' */, 230 /* 'æ' */, 231 /* 'ç' */, 232 /* 'è' */
    /* 233 - 236 */ , 233 /* 'é' */, 234 /* 'ê' */, 235 /* 'ë' */, 236 /* 'ì' */
    /* 237 - 240 */ , 237 /* 'í' */, 238 /* 'î' */, 239 /* 'ï' */, 240 /* 'ð' */
    /* 241 - 244 */ , 241 /* 'ñ' */, 242 /* 'ò' */, 243 /* 'ó' */, 244 /* 'ô' */
    /* 245 - 248 */ , 245 /* 'õ' */, 246 /* 'ö' */, 247, 248 /* 'ø' */
    /* 249 - 252 */ , 249 /* 'ù' */, 250 /* 'ú' */, 251 /* 'û' */, 252 /* 'ü' */
    /* 253 - 255 */ , 253 /* 'ý' */, 254 /* 'þ' */, 255 /* 'ÿ' */
};

/**
 * \brief   Upper-case mapping table for the first 256 UTF-8 code page symbols.
 *          Non-letter values map to themselves.
 *          Reference: https://www.charset.org/utf-8
 **/
inline constexpr uint8_t UTF8_256_Table_upper[256] =
{
    /*   0 -   0 */     0
    /*   1 -   4 */ ,   1,   2,   3,   4
    /*   5 -   8 */ ,   5,   6,   7,   8
    /*   9 -  12 */ ,   9,  10,  11,  12
    /*  13 -  16 */ ,  13,  14,  15,  16
    /*  17 -  20 */ ,  17,  18,  19,  20
    /*  21 -  24 */ ,  21,  22,  23,  24
    /*  25 -  28 */ ,  25,  26,  27,  28
    /*  29 -  32 */ ,  29,  30,  31,  32
    /*  33 -  36 */ ,  33,  34,  35,  36
    /*  37 -  40 */ ,  37,  38,  39,  40
    /*  41 -  44 */ ,  41,  42,  43,  44
    /*  45 -  48 */ ,  45,  46,  47,  48
    /*  49 -  52 */ ,  49,  50,  51,  52
    /*  53 -  56 */ ,  53,  54,  55,  56
    /*  57 -  60 */ ,  57,  58,  59,  60
    /*  61 -  64 */ ,  61,  62,  63,  64
    /*  65 -  68 */ ,  65 /* 'A' */,  66 /* 'B' */,  67 /* 'C' */,  68 /* 'D' */
    /*  69 -  72 */ ,  69 /* 'E' */,  70 /* 'F' */,  71 /* 'G' */,  72 /* 'H' */
    /*  73 -  76 */ ,  73 /* 'I' */,  74 /* 'J' */,  75 /* 'K' */,  76 /* 'L' */
    /*  77 -  80 */ ,  77 /* 'M' */,  78 /* 'N' */,  79 /* 'O' */,  80 /* 'P' */
    /*  81 -  84 */ ,  81 /* 'Q' */,  82 /* 'R' */,  83 /* 'S' */,  84 /* 'T' */
    /*  85 -  88 */ ,  85 /* 'U' */,  86 /* 'V' */,  87 /* 'W' */,  88 /* 'X' */
    /*  89 -  92 */ ,  89 /* 'Y' */,  90 /* 'Z' */,  91,  92
    /*  93 -  96 */ ,  93,  94,  95,  96
    /*  97 - 100 */ ,  65 /* 'A' */,  66 /* 'B' */,  67 /* 'C' */,  68 /* 'D' */
    /* 101 - 104 */ ,  69 /* 'E' */,  70 /* 'F' */,  71 /* 'G' */,  72 /* 'H' */
    /* 105 - 108 */ ,  73 /* 'I' */,  74 /* 'J' */,  75 /* 'K' */,  76 /* 'L' */
    /* 109 - 112 */ ,  77 /* 'M' */,  78 /* 'N' */,  79 /* 'O' */,  80 /* 'P' */
    /* 113 - 116 */ ,  81 /* 'Q' */,  82 /* 'R' */,  83 /* 'S' */,  84 /* 'T' */
    /* 117 - 120 */ ,  85 /* 'U' */,  86 /* 'V' */,  87 /* 'W' */,  88 /* 'X' */
    /* 121 - 124 */ ,  89 /* 'Y' */,  90 /* 'Z' */, 123, 124
    /* 125 - 128 */ , 125, 126, 127, 128
    /* 129 - 132 */ , 129, 130, 131, 132
    /* 133 - 136 */ , 133, 134, 135, 136
    /* 137 - 140 */ , 137, 138 /* 'Š' */, 139, 140 /* 'Œ' */
    /* 141 - 144 */ , 141, 142 /* 'Ž' */, 143, 144
    /* 145 - 148 */ , 145, 146, 147, 148
    /* 149 - 152 */ , 149, 150, 151, 152
    /* 153 - 156 */ , 153, 138 /* 'Š' */, 155, 140 /* 'Œ' */
    /* 157 - 160 */ , 157, 142 /* 'Ž' */, 159 /* 'Ÿ' */, 160
    /* 161 - 164 */ , 161, 162, 163, 164
    /* 165 - 168 */ , 165, 166, 167, 168
    /* 169 - 172 */ , 169, 170, 171, 172
    /* 173 - 176 */ , 173, 174, 175, 176
    /* 177 - 180 */ , 177, 178, 179, 180
    /* 181 - 184 */ , 181, 182, 183, 184
    /* 185 - 188 */ , 185, 186, 187, 188
    /* 189 - 192 */ , 189, 190, 191, 192 /* 'À' */
    /* 193 - 196 */ , 193 /* 'Á' */, 194 /* 'Â' */, 195 /* 'Ã' */, 196 /* 'Ä' */
    /* 197 - 200 */ , 197 /* 'Å' */, 198 /* 'Æ' */, 199 /* 'Ç' */, 200 /* 'È' */
    /* 201 - 204 */ , 201 /* 'É' */, 202 /* 'Ê' */, 203 /* 'Ë' */, 204 /* 'Ì' */
    /* 205 - 208 */ , 205 /* 'Í' */, 206 /* 'Î' */, 207 /* 'Ï' */, 208 /* 'Ð' */
    /* 209 - 212 */ , 209 /* 'Ñ' */, 210 /* 'Ò' */, 211 /* 'Ó' */, 212 /* 'Ô' */
    /* 213 - 216 */ , 213 /* 'Õ' */, 214 /* 'Ö' */, 215, 216 /* 'Ø' */
    /* 217 - 220 */ , 217 /* 'Ù' */, 218 /* 'Ú' */, 219 /* 'Û' */, 220 /* 'Ü' */
    /* 221 - 224 */ , 221 /* 'Ý' */, 222 /* 'Þ' */, 223 /* 'ß' */, 192 /* 'À' */
    /* 225 - 228 */ , 193 /* 'Á' */, 194 /* 'Â' */, 195 /* 'Ã' */, 196 /* 'Ä' */
    /* 229 - 232 */ , 197 /* 'Å' */, 198 /* 'Æ' */, 199 /* 'Ç' */, 200 /* 'È' */
    /* 233 - 236 */ , 201 /* 'É' */, 202 /* 'Ê' */, 203 /* 'Ë' */, 204 /* 'Ì' */
    /* 237 - 240 */ , 205 /* 'Í' */, 206 /* 'Î' */, 207 /* 'Ï' */, 208 /* 'Ð' */
    /* 241 - 244 */ , 209 /* 'Ñ' */, 210 /* 'Ò' */, 211 /* 'Ó' */, 212 /* 'Ô' */
    /* 245 - 248 */ , 213 /* 'Õ' */, 214 /* 'Ö' */, 247, 216 /* 'Ø' */
    /* 249 - 252 */ , 217 /* 'Ù' */, 218 /* 'Ú' */, 219 /* 'Û' */, 220 /* 'Ü' */
    /* 253 - 255 */ , 221 /* 'Ý' */, 222 /* 'Þ' */, 159 /* 'Ÿ' */
};
} // namespace areg

#endif  // AREG_BASE_TABLEDEFS_HPP
