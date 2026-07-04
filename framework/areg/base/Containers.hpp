#ifndef AREG_BASE_CONTAINERS_HPP
#define AREG_BASE_CONTAINERS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/Containers.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Collections of containers.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/ArrayList.hpp"
#include "areg/base/HashMap.hpp"
#include "areg/base/LinkedList.hpp"
#include "areg/base/OrderedMap.hpp"
#include "areg/base/RingStack.hpp"
#include "areg/base/Stack.hpp"
#include "areg/base/SortedLinkedList.hpp"

#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/MathDefs.hpp"

namespace areg {

/************************************************************************
 * \brief   Collection of container such as hash maps, arrays, linked 
 *          list and sorted list. For name of existing classes and
 *          hierarchy see details bellow.
 ************************************************************************/

/**
 * \brief   Hash map template with integer keys.
 **/
template <typename VALUE>
using IntegerHashMap = HashMap<uint32_t, VALUE>;

/**
 * \brief   Hash map template with uint32_t ID keys for resource tracking and management.
 **/
template <typename VALUE>
using IdHashMap = HashMap<id_type, VALUE>;

/**
 * \brief   Hash map template with string keys.
 **/
template <typename VALUE>
using StringHashMap = HashMap<String, VALUE>;

/**
 * \brief   Hash map template with pointer keys.
 **/
template <typename VALUE>
using PtrHashMap = HashMap<void *, VALUE>;

/**
 * \brief   Sorted map template with integer keys.
 **/
template <typename VALUE>
using IntegerMap = OrderedMap<uint32_t, VALUE>;

/**
 * \brief   Sorted map template for storing values associated with Item IDs, used for resource
 *          management.
 **/
template <typename VALUE>
using IdMap = OrderedMap<id_type, VALUE>;

/**
 * \brief   Sorted map template with string keys.
 **/
template <typename VALUE>
using StringMap = OrderedMap<String, VALUE>;

/**
 * \brief   Sorted map template with pointer keys.
 **/
template <typename VALUE>
using PtrMap = OrderedMap<void *, VALUE>;

/**
  * \brief   Array of integer elements
 **/
using IntegerArray  = ArrayList<uint32_t>;

/**
 * \brief   Array of string elements
 **/
using StringArray   = ArrayList<String>;

/**
 * \brief   Array of pointer elements
 **/
using PointerArray  = ArrayList<void *>;

/**
 * \brief   Hash Map where keys are values are integers.
 **/
using IntegerToIntegerHashMap   = IntegerHashMap<uint32_t>;

/**
 * \brief   Hash Map where keys are integers and values are strings
 **/
using IntegerToStringHashMap    = IntegerHashMap<String>;

/**
 * \brief   Hash Map where keys are integers and values are pointers.
 **/
using IntegerToPointergHashMap  = IntegerHashMap<void *>;

/**
 * \brief   Hash Map where keys are strings and values are integers.
 **/
using StringToIntegerHashMap    = StringHashMap<uint32_t>;

/**
 * \brief   Hash Map where keys are strings and values are strings.
 **/
using StringToStringHashMap     = StringHashMap<String>;

/**
 * \brief   Hash Map where keys are strings and values are pointers.
 **/
using StringToPointergHashMap   = StringHashMap<void *>;

/**
 * \brief   Hash Map where keys are pointers and values are integers.
 **/
using PointerToIntegerHashMap   = PtrHashMap<uint32_t>;

/**
 * \brief   Hash Map where keys are pointers and values are strings.
 **/
using PointerToStringHashMap    = PtrHashMap<String>;

/**
 * \brief   Sorted Map where keys are values are integers.
 **/
using IntegerToIntegerMap = IntegerMap<uint32_t>;

/**
 * \brief   Sorted Map where keys are integers and values are strings
 **/
using IntegerToStringMap = IntegerMap<String>;

/**
 * \brief   Sorted Map where keys are integers and values are pointers.
 **/
using IntegerToPointergMap = IntegerMap<void*>;

/**
 * \brief   Sorted Map where keys are strings and values are integers.
 **/
using StringToIntegerMap = StringMap<uint32_t>;

/**
 * \brief   Sorted Map where keys are strings and values are strings.
 **/
using StringToStringMap = StringMap<String>;

/**
 * \brief   Sorted Map where keys are strings and values are pointers.
 **/
using StringToPointergMap = StringMap<void*>;

/**
 * \brief   Sorted Map where keys are pointers and values are integers.
 **/
using PointerToIntegerMap = PtrMap<uint32_t>;

/**
 * \brief   Sorted Map where keys are pointers and values are strings.
 **/
using PointerToStringMap = PtrMap<String>;

/**
 * \brief   Linked List where values are integers.
 **/
using IntegerList   = LinkedList<uint32_t>;

/**
 * \brief   Linked List where values are strings.
 **/
using StringList    = LinkedList<String>;

/**
 * \brief   Linked List where values are pointers
 **/
using PointerList   = LinkedList<void *>;

/**
* \brief   Sorted List class where values are integers.
 **/
using SortedIntegerList = SortedLinkedList<uint32_t>;

/**
 * \brief   Sorted List class where values are strings.
 **/
using SortedStringList  = SortedLinkedList<String>;

//////////////////////////////////////////////////////////////////////////
// Tokenizer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Tokenizes a string by splitting it on delimiter characters.
 **/
class AREG_API Tokenizer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    Tokenizer() = default;
    
    /**
     * \brief   Tokenizes the string using the specified delimiters.
     *
     * \param   str             The string to tokenize.
     * \param   delimiters      One or more delimiter characters that define token boundaries.
     * \param   keepEmpty       If true, consecutive delimiters create empty tokens; if false, only
     *                          non-empty tokens are included.
     **/
    inline Tokenizer( const String & str, const String & delimiters, bool keepEmpty = true);

    inline Tokenizer( const Tokenizer & src );

    inline Tokenizer( Tokenizer && src ) noexcept;

    ~Tokenizer() = default;
//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Tokenizes the string and populates the array with the resulting tokens.
     *
     * \param   str             The string to tokenize.
     * \param   delimiters      One or more delimiter characters.
     * \param   keepEmpty       If true, consecutive delimiters create empty tokens; if false, only
     *                          non-empty tokens are included.
     * \return  Returns the array of resulting tokens.
     **/
    inline const StringArray & tokenize( const String & str, const String & delimiters, bool keepEmpty = true);

    /**
     * \brief   Returns the array of tokens from the most recent tokenization.
     **/
    [[nodiscard]]
    inline const StringArray& list() const noexcept;

private:

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The list of tokenized string.
     */
    StringArray mTokens;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// Tokenizer class inline implementation
//////////////////////////////////////////////////////////////////////////

inline Tokenizer::Tokenizer(const String& str, const String& delimiters, bool keepEmpty/*=true*/)
    : mTokens()
{
    tokenize(str, delimiters, keepEmpty);
}

inline Tokenizer::Tokenizer(const Tokenizer& src)
    : mTokens(src.mTokens)
{
}

inline Tokenizer::Tokenizer(Tokenizer&& src) noexcept
    : mTokens(std::move(src.mTokens))
{
}

inline const StringArray& Tokenizer::list() const noexcept
{
    return mTokens;
}

inline const StringArray& Tokenizer::tokenize(const String& str, const String& delimiters, bool keepEmpty/*=true*/)
{
    areg::CharPos lastPos = 0;
    areg::CharCount length = str.length();
    // empty self
    mTokens.clear();
    while (lastPos <= length)
    {
        areg::CharPos pos = str.find_one_of(delimiters, lastPos);
        if (pos < 0)
            pos = length;

        if (pos != lastPos || keepEmpty)
        {
            String temp;
            str.substring(temp, lastPos, pos - lastPos);
            mTokens.add(temp);
        }

        lastPos = pos + 1;
    }

    return mTokens;
}

} // namespace areg
#endif  // AREG_BASE_CONTAINERS_HPP
