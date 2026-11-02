/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/String.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, String Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
#include "areg/base/String.hpp"
#include "areg/base/IOStream.hpp"

namespace areg {

const String & String::empty_string()
{
    static const String _emptyString{ "" };
    return _emptyString;
}

String::String( const InStream & stream )
    : Base( )
{
    from_stream(stream);
}

uint32_t String::from_stream(const InStream& stream)
{
    return stream.read_text(mData);
}

uint32_t String::to_stream(OutStream& stream) const
{
    return stream.write_text(mData.data(), static_cast<uint32_t>(mData.length()));
}

uint32_t InStream::read(String& ascii) const
{
    return ascii.from_stream(*this);
}

uint32_t OutStream::write(const String& ascii)
{
    return ascii.to_stream(*this);
}

} // namespace areg
