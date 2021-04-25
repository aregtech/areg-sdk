#ifndef AREG_BASE_ETEMPLATEBASE_HPP
#define AREG_BASE_ETEMPLATEBASE_HPP
/************************************************************************
 * \file        areg/src/base/ETemplateBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, templates constants class.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/NEMath.hpp"
#include "areg/src/base/NEString.hpp"
#include "areg/src/base/IEIOStream.hpp"

/**
 * \brief   Class template to implement equality of 2 values.
 *          It is used in container objects.
 * \tparam  VALUE_TYPE  The type of value to compare.
 **/
template <typename VALUE_TYPE>
class TEEqualValueImpl
{
public:
    /**
     * \brief   Compares 2 values, returns true if they are equal.
     * \param   Value1  The right-side value to compare.
     * \param   Value2  The left-side value to compare.
     * \return  Returns true if 2 values are equal.
     **/
    inline bool ImplEqualValues(VALUE_TYPE Value1, VALUE_TYPE Value2) const
    {
        return (Value1 == Value2);
    }
};

/**
 * \brief   Class template to implement equality of 2 keys.
 *          It is used in container objects.
 * \tparam  KEY_TYPE    The type of keys to compare.
 **/
template <typename KEY_TYPE>
class TEEqualKeyImpl
{
public:
    /**
     * \brief   Compares 2 keys, returns true if they are equal.
     * \param   Value1  The key of right-side object to compare.
     * \param   Value2  The key of left-side object to compare.
     * \return  Returns true if 2 keys are equal.
     **/
    inline bool ImplEqualKeys(KEY_TYPE Key1, KEY_TYPE Key2) const
    {
        return (Key1 == Key2);
    }
};

/**
 * \brief   Class template to implement comparing functionality of 2 values.
 *          It is used in sorting algorithms.
 * /tparam  VALUE_TYPE  The type of values to compare.
 **/
template <typename VALUE_TYPE>
class TECompareImpl
{
public:
    /**
     * \brief   Compares 2 values and returns:
     *              - NEMath::CompSmaller   if Value1 is smaller than Value2;
     *              - NEMath::CompEqual     if Value1 and Value2 are equal;
     *              - NEMath::CompGreater   if Value1 is greater than Value2.
     *          Make own implementation if comparing has special functionality.
     * \param   Value1  The left-side value to compare.
     * \param   Value2  The right-side value to compare.
     * \return  Returns one of following values:
     *              - NEMath::CompSmaller   if Value1 is smaller than Value2;
     *              - NEMath::CompEqual     if Value1 and Value2 are equal;
     *              - NEMath::CompGreater   if Value1 is greater than Value2.
     **/
    inline NEMath::eCompare ImplCompareValues(VALUE_TYPE Value1, VALUE_TYPE Value2) const
    {
        if (Value1 < Value2)
            return NEMath::CompSmaller;
        if (Value1 == Value2)
            return NEMath::CompEqual;
        else
            return NEMath::CompGreater;
    }
};

/**
 * \brief   Class template to calculate 32-bit hash key value.
 *          Make own implementation if need to customize calculation.
 *          Normally used by hash map.
 * \tparam  KEY_TYPE    The type of value to calculate 32-bit hash key.
 **/
template <typename KEY_TYPE>
class TEHashKeyImpl
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    unsigned int ImplHashKey(KEY_TYPE Key) const
    {
#if _MSC_VER
    #pragma warning(disable: 4311)
    #pragma warning(disable: 4302)
#endif  // _MSC_VER

        return MACRO_MAKE_NUMBER32( Key );

#if _MSC_VER
    #pragma warning(default: 4302)
    #pragma warning(default: 4311)
#endif  // _MSC_VER
    }
};

/**
 * \brief   The base class template for lists such as
 *          linked list, array or fixed array.
 *          Make own implementation if it differs from default.
 * \tparam  The type of values saved in the lists.
 **/
template <typename VALUE_TYPE>
class TEListImpl        : public TEEqualValueImpl<VALUE_TYPE>
{

};

/**
 * \brief   Class template used to implement sorting algorithms.
 * \tparam  VALUE_TYPE  The type of values to compare.
 **/
template <typename VALUE_TYPE>
class TESortImpl    : public TEEqualValueImpl<VALUE_TYPE>
                    , public TECompareImpl<VALUE_TYPE>
{

};

/**
 * \brief   The base class template for key-value pair container.
 *          By default it contains methods to check equality of
 *          keys and values of 2 pair containers.
 * \tparam  KEY_TYPE    The type of key saved in the pair container.
 * \tparam  VALUE_TYPE  The type of value saved in the pair container.
 **/
template <typename KEY_TYPE, typename VALUE_TYPE>
class TEPairImpl        : public TEEqualKeyImpl     <KEY_TYPE>
                        , public TEEqualValueImpl   <VALUE_TYPE>
{

};

/**
 * \brief   The base class template for hash-map container, which 
 *          data are accessed by hash key.
 *          By default it contains methods to check equality of
 *          keys and values of 2 pair containers, and calculation
 *          of the hash-key.
 * \tparam  KEY_TYPE    The type of key saved in the hash-map container.
 * \tparam  VALUE_TYPE  The type of value associated with the key.
 **/
template <typename KEY_TYPE, typename VALUE_TYPE>
class TEHashMapImpl     : public TEPairImpl     <KEY_TYPE, VALUE_TYPE>
                        , public TEHashKeyImpl  <KEY_TYPE>
{

};

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT>
class TEResourceMapImpl
{
public:
    /**
     * \brief	Called when all resources are removed.
     * \param	Key	        The Key value of resource
     * \param	Resource	Pointer to resource object
     **/
    inline void ImplCleanResourceElement( RESOURCE_KEY & /*Key*/, RESOURCE_OBJECT * /*Resource*/ )
    {   }
};

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
    inline void ImplCleanResourceList( RESOURCE_KEY & Key, ResourceList & List )
    {   }

    /**
     * \brief	Called when need to add resource object to the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to add to the list.
     **/
    inline void ImplAddResourceToList( ResourceList & List, RESOURCE_OBJECT * Resource )
    {   }

    /**
     * \brief	Called when need to remove resource object from the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to remove from the list.
     **/
    inline bool ImplRemoveResourceFromList( ResourceList & List, RESOURCE_OBJECT * Resource )
    {   return false;   }
};

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
    inline CharType ImplMakeLower( CharType ch ) const
    {
        return static_cast<CharType>( NEString::toAsciiLower<CharType>( ch ) );
    }

    /**
     * \brief   Makes the given character upper case 
     *          Override method to convert non ASCII characters
     *          By default, the conversion is done based on ISO8859
     * \param   ch      The character to convert to upper case
     * \return  Returns upper case character
     **/
    inline CharType ImplMakeUpper( CharType ch ) const
    {
        return static_cast<CharType>(NEString::toAsciiUpper<CharType>( ch ));
    }

    /**
     * \brief   Returns true if given character is a letter.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool ImplIsLetter( CharType ch ) const
    {
        return NEString::isLetter<CharType>( ch );
    }

    /**
     * \brief   Returns true if given character is a number.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool ImplIsNumeric( CharType ch ) const
    {
        return NEString::isNumeric<CharType>( ch );
    }

    /**
     * \brief   Returns true if given character is a letter or number.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool ImplIsAlphanumeric( CharType ch ) const
    {
        return NEString::isAlphanumeric<CharType>( ch );
    }

    /**
     * \brief   Returns true if given character is a symbol.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool ImplIsSymbol( CharType ch ) const
    {
        return NEString::isSymbol<CharType>( ch );
    }

    /**
     * \brief   Returns true if given character is a white-space.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool ImplIsWhitespace( CharType ch ) const
    {
        return NEString::isWhitespace<CharType>(ch);
    }

    /**
     * \brief   Compares the existing string with given string and returns:
     *              NEMath::CompSmaller if string is less than given string
     *              NEMath::CompEqual   if strings have equal
     *              NEMath::CompGreater if string is more than given string
     *          The comparing starts from given position, compares given
     *          amount of character of until the end if 'charCount' is NEString::CountAll,
     *          and the comparing is either by exact match or ignores cases sensitive.
     * \param   strLeft         The left-side string buffer to compare.
     * \param   strRight        The right-side string buffer to compare.
     * \param   charCount       The amount of characters to compare.
     *                          If equal to NEString::CountAll compares until end of string.
     * \param   caseSensitive   If true, compares exact match of string. Otherwise, ignores lower and upper cases.
     * \return  Return:
     *              NEMath::CompSmaller if string is less than given string
     *              NEMath::CompEqual   if strings have equal
     *              NEMath::CompGreater if string is more than given string
     **/
    inline NEMath::eCompare ImplCompareString( const CharType * strLeft, const CharType * strRight, NEString::CharCount charCount = NEString::CountAll, bool caseSensitive = true ) const
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
    int ImplReadStream(const IEInStream & stream, CharType * buffer, int count)
    {
        int result = 0;

        if ((buffer != static_cast<CharType *>(NULL)) && (count > 0))
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
    int ImplWriteStream(IEOutStream & stream, const CharType * buffer) const
    {
        int result = 0;

        if (buffer != static_cast<CharType>(NULL))
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

//////////////////////////////////////////////////////////////////////////
// CETemplateConstants class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Template Constants class has no functions it is defining
 *          constants, used in template classes.
 *          The idea to use class instead of namespace is to be able
 *          to derive class privately.
 **/
class AREG_API CETemplateConstants
{
//////////////////////////////////////////////////////////////////////////
// Constants defined for template classes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   CETemplateConstants::MAP_DEFAULT_BLOCK_SIZE
     *          The number of blocks by default created when increase list in map. For hash map internal use
     **/
    static const int            MAP_DEFAULT_BLOCK_SIZE  /*= static_cast<int>(64)*/;             /*0x00000040*/
    /**
     * \brief   CETemplateConstants::MAP_MAX_BLOCK_SIZE
     *          The number of maximum blocks created at once when increase list in map. For hash map internal use
     **/
    static const int            MAP_MAX_BLOCK_SIZE      /*= static_cast<int>(2048)*/;           /*0x000000800*/
    /**
     * \brief   CETemplateConstants::MAP_DEFAULT_HASH_SIZE
     *          The size of hash table by default created in map. For hash map internal use
     **/
    static const int            MAP_DEFAULT_HASH_SIZE   /*= static_cast<int>(64)*/;             /*0x00000040*/
    /**
     * \brief   CETemplateConstants::MAP_DEFAULT_HASH_SIZE
     *          The maximum size of hash table created in map. For hash map internal use
     **/
    static const int            MAP_MAX_TABLE_SIZE      /*= static_cast<int>(2048)*/;           /*0x000000800*/
    /**
     * \brief   CETemplateConstants::MAP_SHIFT
     *          Bit-shift used to generate has key number
     **/
    static const int            MAP_SHIFT               /*= static_cast<int>(5)*/;              /*0x00000005*/
    /**
     * \brief   CETemplateConstants::MAP_INVALID_HASH
     *          Defines invalid Hash value
     **/
    static const unsigned int   MAP_INVALID_HASH        /*= static_cast<unsigned int>(~0)*/;    /*0xFFFFFFFF*/
    /**
     * \brief   CETemplateConstants::START_POSITION
     *          Defines starting position in hash-map
     **/
    static void * const         START_POSITION          /*= static_cast<void *>(~0)*/;          /*0xFFFFFFFF*/

    /**
     * \brief   CETemplateConstants::ARRAY_DEFAULT_MIN_GROW
     *          The minimum new created entries when grow array
     **/
    static const int            ARRAY_DEFAULT_MIN_GROW  /*= static_cast<int>(4)*/;              /*0x00000004*/
    /**
     * \brief   CETemplateConstants::ARRAY_DEFAULT_MAX_GROW
     *          The maximum new created entries when grow array
     **/
    static const int            ARRAY_DEFAULT_MAX_GROW  /*= static_cast<int>(2048)*/;           /*0x000000800*/
    /**
     * \brief   CETemplateConstants::ARRAY_DEFAULT_INCREASE
     *          Defines default increase. When used, default values should be used.
     **/
    static const int            ARRAY_DEFAULT_INCREASE  /*= static_cast<int>(~0)*/;             /*0xFFFFFFFF*/

    /**
     * \brief   CETemplateConstants::INVALID_POSITION
     *          Defines invalid position.
     **/
    static void * const         INVALID_POSITION        /*= static_cast<void *>(NULL)*/;        /*0x00000000*/

    /**
     * \brief   CETemplateConstants::START_INDEX
     *          Defines starting index in the lists.
     **/
    static const int            START_INDEX             /*= static_cast<int>(MIN_INT_32)*/;     /*0x80000000*/

    /**
     * \brief   CETemplateConstants::INVALID_INDEX
     *          Defines invalid index in the lists.
     **/
    static const int            INVALID_INDEX           /*= static_cast<int>(-1)*/;             /*-1*/
};

#endif // AREG_BASE_ETEMPLATEBASE_HPP
