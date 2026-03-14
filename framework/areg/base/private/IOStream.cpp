/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/IOStream.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Input / Output stream interfaces
 *
 ************************************************************************/
#include "areg/base/IOStream.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/UtilityDefs.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// InStream class implementation: to read data from
//////////////////////////////////////////////////////////////////////////

uint8_t InStream::read8_bits() const noexcept
{
    uint8_t result{ 0u };
    read( reinterpret_cast<uint8_t *>(&result), sizeof(uint8_t));
    return result;
}

uint16_t InStream::read16_bits() const noexcept
{
    uint16_t result{ 0u };
    read( reinterpret_cast<uint8_t *>(&result), sizeof(uint16_t));
    return result;
}

uint32_t InStream::read32_bits() const noexcept
{
    uint32_t result{ 0u };
    read( reinterpret_cast<uint8_t *>(&result), sizeof(uint32_t));
    return result;
}

uint64_t InStream::read64_bits() const noexcept
{
    uint64_t result{ 0u };
    read( reinterpret_cast<uint8_t *>(&result), sizeof(uint64_t) );
    return result;
}

float InStream::read32_real() const noexcept
{
    float result{ 0.0f };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(float));
    return result;
}

double InStream::read64_real() const noexcept
{
    double result{ 0.0 };
    read(reinterpret_cast<uint8_t*>(&result), sizeof(double));
    return result;
}

//////////////////////////////////////////////////////////////////////////
// OutStream class implementation: to write data to
//////////////////////////////////////////////////////////////////////////

bool OutStream::write8_bits( uint8_t value )
{
    return write( reinterpret_cast<const uint8_t *>(&value), sizeof(uint8_t)) == sizeof(uint8_t);
}

bool OutStream::write16_bits( uint16_t value )
{
    return write( reinterpret_cast<const uint8_t *>(&value), sizeof(uint16_t)) == sizeof(uint16_t);
}

bool OutStream::write32_bits( uint32_t value )
{
    return write( reinterpret_cast<const uint8_t *>(&value), sizeof(uint32_t)) == sizeof(uint32_t);
}

bool OutStream::write64_bits( uint64_t value )
{
    return write( reinterpret_cast<const uint8_t *>(&value), sizeof(uint64_t)) == sizeof(uint64_t);
}

bool OutStream::write32_real(float value)
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(float)) == sizeof(float);
}

bool OutStream::write64_real(double value)
{
    return write(reinterpret_cast<const uint8_t*>(&value), sizeof(double)) == sizeof(double);
}

//////////////////////////////////////////////////////////////////////////
// IOStream class declaration: to read / write data
//////////////////////////////////////////////////////////////////////////

} // namespace areg
