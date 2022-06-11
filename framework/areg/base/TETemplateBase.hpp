#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/ETemplateBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, common constants class for templates.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NECommon.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/base/NEString.hpp"
#include "areg/base/IEIOStream.hpp"

/**
 * \brief   The class template to implement equality of 2 values used in containers.
 * \tparam  VALUE_TYPE  The type of value to compare.
 **/
template <typename VALUE>
class TEEqualValueImpl
{
public:
    /**
     * \brief   Compares 2 values, returns true if they are equal.
     * \param   Value1  The right-side value to compare.
     * \param   Value2  The left-side value to compare.
     * \return  Returns true if 2 values are equal.
     **/
    inline bool implEqualValues(const VALUE & Value1, const VALUE & Value2) const
    {
        return (Value1 == Value2);
    }
};

/**
 * \brief   The class template to implement equality of 2 keys used in containers.
 * \tparam  KEY_TYPE    The type of keys to compare.
 **/
template <typename KEY>
class TEEqualKeyImpl
{
public:
    /**
     * \brief   Compares 2 keys, returns true if they are equal.
     * \param   Value1  The key of right-side object to compare.
     * \param   Value2  The key of left-side object to compare.
     * \return  Returns true if 2 keys are equal.
     **/
    inline bool implEqualKeys(const KEY & Key1, const KEY& Key2) const
    {
        return (Key1 == Key2);
    }
};

/**
 * \brief   The class template to implement comparing functionality of 2 values used in sorting algorithms.
 * /tparam  VALUE_TYPE  The type of values to compare.
 **/
template <typename VALUE>
class TECompareImpl
{
public:
    /**
     * \brief   Compares 2 values and returns:
     *              - NEMath::Smaller   if Value1 is smaller than Value2;
     *              - NEMath::Equal     if Value1 and Value2 are equal;
     *              - NEMath::Bigger   if Value1 is greater than Value2.
     *          Make own implementation if comparing has special functionality.
     * \param   Value1  The left-side value to compare.
     * \param   Value2  The right-side value to compare.
     * \return  Returns one of following values:
     *              - NEMath::Smaller   if Value1 is smaller than Value2;
     *              - NEMath::Equal     if Value1 and Value2 are equal;
     *              - NEMath::Bigger   if Value1 is greater than Value2.
     **/
    inline NEMath::eCompare implCompareValues(const VALUE & Value1, const VALUE& Value2) const
    {
        if (Value1 < Value2)
            return NEMath::eCompare::Smaller;
        else if (Value1 == Value2)
            return NEMath::eCompare::Equal;
        else
            return NEMath::eCompare::Bigger;
    }
};

/**
 * \brief   The class template to calculate 32-bit hash key value used in hash map.
 *          Make own implementation if need to customize calculation.
 * \tparam  KEY_TYPE    The type of value to calculate 32-bit hash key.
 **/
template <typename KEY>
class TEHashKeyImpl
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    unsigned int implHashKey(const KEY & Key) const
    {
        return static_cast<unsigned int>( Key );
    }
};

/**
 * \brief   The class template for listing containers such as linked list, array or fixed array.
 *          Make own implementation if it differs from default implementation of value comparison.
 * \tparam  The type of values saved in the lists.
 **/
template <typename VALUE>
class TEListImpl        : public TEEqualValueImpl<VALUE>
{

};

/**
 * \brief   The class template used in the list containers that keep values sorted.
 *          Make own implementation if it differs from default implementation.
 * \tparam  VALUE_TYPE  The type of values to compare.
 **/
template <typename VALUE>
class TESortImpl    : public TEEqualValueImpl<VALUE>
                    , public TECompareImpl<VALUE>
{

};

/**
 * \brief   The class template for key-value pairs.  By default it contains methods 
 *          to check equality of keys and values.
 *          Make own implementation if it differs from default implementation.
 * \tparam  KEY_TYPE    The type of key saved in the pair container.
 * \tparam  VALUE_TYPE  The type of value saved in the pair container.
 **/
template <typename KEY, typename VALUE>
class TEPairImpl        : public TEEqualKeyImpl     <KEY>
                        , public TEEqualValueImpl   <VALUE>
{
};

/**
 * \brief   The class template for pointer hash-map container to compare keys and values and
 *          access data by hash key. The keys of this hash-map are pointers.
 *          Make own implementation if it differs from default implementation.
 * \tparam  KEY_TYPE    The type of key, which should be pointer, saved in the hash-map container.
 * \tparam  VALUE_TYPE  The type of value associated with the key.
 **/
template <typename KEY, typename VALUE>
class TEPointerHashMapImpl	: public TEPairImpl<KEY, VALUE>
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    unsigned int implHashKey(const KEY & Key) const
    {
    	return MACRO_PTR2INT32( Key );
    }
};

/**
 * \brief   The class template for hash-map container to compare keys and values and 
 *          access data by hash key.
 *          Make own implementation if it differs from default implementation.
 * \tparam  KEY_TYPE    The type of key saved in the hash-map container.
 * \tparam  VALUE_TYPE  The type of value associated with the key.
 **/
template <typename KEY, typename VALUE>
class TEHashMapImpl     : public TEPairImpl     <KEY, VALUE>
                        , public TEHashKeyImpl  <KEY>
{
};

/**
 * \brief   The class template for resource containers that save objects associated with the key.
 *          The methods of class are called when clean-up resource element.
 *          Make own implementation if it differs from default implementation.
 * \tparam  RESOURCE_KEY    The type of key to access resource object.
 * \tparam  RESOURCE_OBJECT The type of resource object stored in the map.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT>
class TEResourceMapImpl
{
public:
    /**
     * \brief	Called when all resources are removed.
     * \param	Key	        The Key value of resource
     * \param	Resource	Pointer to resource object
     **/
    inline void implCleanResource( RESOURCE_KEY & /*Key*/, RESOURCE_OBJECT * /*Resource*/ )
    {   }
};

/**
 * \brief   The class template for resource containers that save list of resources associated with the key.
 *          The methods of class are called when add or remove list, or when remove resource from the list to 
 *          make additional cleanup operation such as free memory.
 *          Make own implementation if it differs from default implementation.
 * \tparam  RESOURCE_KEY    The type of key to access the list of resource objects.
 * \tparam  RESOURCE_OBJECT The type of resource object stored in the resource list.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class ResourceList>
class TEResourceListMapImpl
{
public:
    /**
     * \brief	Called when all resources are removed.
     *          This function is called from RemoveAllResources() for every single
     *          resource being unregistered.
     * \param	Key	    The Key value of resource.
     * \param	List    The list of resource objects.
     **/
    inline void implCleanResourceList( RESOURCE_KEY & Key, ResourceList & List )
    {   }

    /**
     * \brief	Called when need to add resource object to the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to add to the list.
     **/
    inline void implAddResource( ResourceList & List, RESOURCE_OBJECT * Resource )
    {   }

    /**
     * \brief	Called when need to remove resource object from the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to remove from the list.
     **/
    inline bool implRemoveResource( ResourceList & List, RESOURCE_OBJECT * Resource )
    {
        return false;
    }
};

/**
 * \brief   Class template used in string objects to make basic converting and check-up operations
 *          such as make up- or lower-case, check whether the symbol is a letter or number, etc.
 *          By default, converting and check-up are done according ASCII table.
 *          Make own implementation if it differs from default implementation.
 * \tparam  CharType    The type of character used in the string. Normally, it is either single byte 'char'
 *                      or 2 byte 'wchar_t'.
 **/
template <typename CharType>
class TEStringImpl
{
public:
    /**
     * \brief   Makes the given character lower case.
     *          Override method to convert non ASCII characters
     *          By default, the conversion is done based on ISO8859
     * \param   ch      The character to convert to lower case
     * \return  Returns lower case character
     **/
    inline CharType implMakeLower( CharType ch ) const
    {
        return static_cast<CharType>( NEString::makeAsciiLower<CharType>( ch ) );
    }

    /**
     * \brief   Makes the given character upper case 
     *          Override method to convert non ASCII characters
     *          By default, the conversion is done based on ISO8859
     * \param   ch      The character to convert to upper case
     * \return  Returns upper case character
     **/
    inline CharType implMakeUpper( CharType ch ) const
    {
        return static_cast<CharType>(NEString::makeAsciiUpper<CharType>( ch ));
    }

    /**
     * \brief   Returns true if given character is a letter.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool implIsLetter( CharType ch ) const
    {
        return NEString::isLetter<CharType>( ch );
    }

    /**
     * \brief   Returns true if given character is a number.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool implIsNumeric( CharType ch ) const
    {
        return NEString::isNumeric<CharType>( ch );
    }

    /**
     * \brief   Returns true if given character is a letter or number.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool implIsAlphanumeric( CharType ch ) const
    {
        return NEString::isAlphanumeric<CharType>( ch );
    }

    /**
     * \brief   Returns true if given character is a symbol.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool implIsSymbol( CharType ch ) const
    {
        return NEString::isSymbol<CharType>( ch );
    }

    /**
     * \brief   Returns true if given character is a white-space.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool implIsWhitespace( CharType ch ) const
    {
        return NEString::isWhitespace<CharType>(ch);
    }

    /**
     * \brief   Compares the existing string with given string and returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     *          The comparing starts from given position, compares given
     *          amount of character of until the end if 'charCount' is NEString::COUNT_ALL,
     *          and the comparing is either by exact match or ignores cases sensitive.
     * \param   strLeft         The left-side string buffer to compare.
     * \param   strRight        The right-side string buffer to compare.
     * \param   charCount       The amount of characters to compare.
     *                          If equal to NEString::COUNT_ALL compares until end of string.
     * \param   caseSensitive   If true, compares exact match of string. Otherwise, ignores lower and upper cases.
     * \return  Return:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger  if string is more than given string
     **/
    inline NEMath::eCompare implCompareString( const CharType * strLeft, const CharType * strRight, NEString::CharCount charCount = NEString::COUNT_ALL, bool caseSensitive = true ) const
    {
        return static_cast<NEMath::eCompare>( NEString::compareStrings<CharType, CharType>(strLeft, strRight, charCount, caseSensitive));
    }

    /**
     * \brief   Reads string data from streaming object.
     * \param   stream  The streaming object, which contains string source data.
     * \param   buffer  The buffer to write the data.
     * \param   count   The number characters that can be written into the buffer.
     * \return  Returns the number of characters that actually read from the stream.
     **/
    int implReadStream(const IEInStream & stream, CharType * buffer, int count)
    {
        int result = 0;

        if ((buffer != nullptr) && (count > 0))
        {
            CharType * dst = buffer;
            do
            {
                stream >> *dst;
                if (*dst == NEString::EndOfString)
                    break;

                * (++dst) = NEString::EndOfString;

            } while ( -- count > 1 );

            result = static_cast<int>(dst - buffer);
        }

        return result;
    }

    /**
     * \brief   Writes string data to streaming object until match end of string.
     * \param   stream  The streaming object to write string data.
     * \param   buffer  The buffer to read data.
     * \return  Returns number of characters that have written to the stream.
     **/
    int implWriteStream(IEOutStream & stream, const CharType * buffer) const
    {
        int result = 0;

        if (buffer != nullptr)
        {
            const CharType * src = buffer;
            for ( ; *src != NEString::EndOfString; ++ src)
                stream << *src;

            result = static_cast<int>(src - buffer);
        }

        stream << static_cast<CharType>(NEString::EndOfString);

        return result;
    }
};

/**
 * \brief   Converts the const_iterator type random access iterator into normal iterator.
 * \tparam  Container   The container object type to convert.
 */
template <typename Container>
class Constless
{
public:
    /**
     * \brief   Converts the given const_iterator type into normal iterator type during run-time without casting.
     * \param   cont    The container object, which const_iterator should be converted.
     * \param   cit     The const_iterator object to convert
     * \return  Returns converted iterator type object.
     */
    inline typename const Container::iterator iter(typename const Container& cont, typename Container::const_iterator& cit) const
    {
        return const_cast<Container &>(cont).erase(cit, cit);
    }

    inline typename Container::iterator iter(typename Container& c, typename Container::const_iterator& cit)
    {
        return c.erase(cit, cit);
    }
};
