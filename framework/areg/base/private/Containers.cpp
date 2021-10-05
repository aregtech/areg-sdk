/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Containers.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Containers
 *
 ************************************************************************/

#include "areg/base/Containers.hpp"

//////////////////////////////////////////////////////////////////////////
// Tokenizer class implementation
//////////////////////////////////////////////////////////////////////////

Tokenizer::Tokenizer( const String & str, const String & delimiters, bool keepEmpty/*=true*/)
{
    Tokenize(str, delimiters, keepEmpty);
}

Tokenizer::Tokenizer( const Tokenizer & src )
    : StringArray( static_cast<const StringArray &>(src) )
{
}

Tokenizer::Tokenizer( Tokenizer && src ) noexcept
    : StringArray( static_cast<StringArray &&>(src) )
{
}

void Tokenizer::Tokenize( const String & str, const String & delimiters, bool keepEmpty/*=true*/)
{
    NEString::CharPos lastPos   = 0;
    NEString::CharCount length  = str.getLength();
    // empty self
    removeAll();
    while (lastPos <= length)
    {
        NEString::CharPos pos = str.findOneOf(delimiters, lastPos);
        if (pos == NEString::INVALID_POS)
           pos = length;

        if (pos != lastPos || keepEmpty)
            add(str.substring(lastPos, pos - lastPos));
        
        lastPos = pos + 1;
    }
}
