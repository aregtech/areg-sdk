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
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// InStream class implementation: to read data from
//////////////////////////////////////////////////////////////////////////

uint8_t InStream::read8Bits() const
{
    uint8_t result{ 0u };
    read( reinterpret_cast<unsigned char *>(&result), 1 );
    return result;
}

uint16_t InStream::read16Bits() const
{
    uint16_t result{ 0u };
    read( reinterpret_cast<unsigned char *>(&result), 2 );
    return result;
}

uint32_t InStream::read32Bits() const
{
    uint32_t result{ 0u };
    read( reinterpret_cast<unsigned char *>(&result), 4 );
    return result;
}

uint64_t InStream::read64Bits() const
{
    uint64_t result{ 0u };
    read( reinterpret_cast<unsigned char *>(&result), 8 );
    return result;
}

//////////////////////////////////////////////////////////////////////////
// OutStream class implementation: to write data to
//////////////////////////////////////////////////////////////////////////

bool OutStream::write8Bits( uint8_t value8Bit )
{
    return write( reinterpret_cast<const unsigned char *>(&value8Bit), 1) == 1;
}

bool OutStream::write16Bits( uint16_t value16Bit )
{
    return write( reinterpret_cast<const unsigned char *>(&value16Bit), 2) == 2;
}

bool OutStream::write32Bits( uint32_t value32Bit )
{
    return write( reinterpret_cast<const unsigned char *>(&value32Bit), 4) == 4;
}

bool OutStream::write64Bits( uint64_t value64Bit )
{
    return write( reinterpret_cast<const unsigned char *>(&value64Bit), 8) == 8;
}

//////////////////////////////////////////////////////////////////////////
// IOStream class declaration: to read / write data
//////////////////////////////////////////////////////////////////////////
