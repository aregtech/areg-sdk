/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Containers.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Containers
 *
 ************************************************************************/

#include "areg/base/Containers.hpp"

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // Tokenizer class implementation
    //////////////////////////////////////////////////////////////////////////

    areg::Tokenizer::Tokenizer( const areg::String & str, const areg::String & delimiters, bool keepEmpty/*=true*/)
        : mTokens   ()
    {
    tokenize(str, delimiters, keepEmpty);
    }

    areg::Tokenizer::Tokenizer( const areg::Tokenizer & src )
        : mTokens( src.mTokens )
    {
    }

    areg::Tokenizer::Tokenizer( areg::Tokenizer && src ) noexcept
        : mTokens( std::move(src.mTokens) )
    {
    }

    const areg::StringArray& areg::Tokenizer::tokenize( const areg::String & str, const areg::String & delimiters, bool keepEmpty/*=true*/)
    {
        areg::CharPos lastPos   = 0;
        areg::CharCount length  = str.getLength();
        // empty self
        mTokens.clear();
        while (lastPos <= length)
        {
            areg::CharPos pos = str.findOneOf(delimiters, lastPos);
            if (pos < 0)
            pos = length;

            if (pos != lastPos || keepEmpty)
            {
                areg::String temp;
                str.substring(temp, lastPos, pos - lastPos);
                mTokens.add(temp);
            }
            
            lastPos = pos + 1;
        }

        return mTokens;
    }

} // namespace areg