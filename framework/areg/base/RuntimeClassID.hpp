#ifndef AREG_BASE_RUNTIMECLASSID_HPP
#define AREG_BASE_RUNTIMECLASSID_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/RuntimeClassID.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Runtime Class ID
 *              This class contains information of Runtime Class ID.
 *              Every Runtime Class contains class ID value to identify and
 *              cast instances of Runtime Object.
 *              As an ID of Runtime class used class name.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/String.hpp"
#include <utility>

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // RuntimeClassID class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Runtime Class ID is used in all objects derived from 
     *          Runtime Objects. It contains the class ID, so that during
     *          runtime can check and cast the instance of Runtime Object.
     *          In particular, Runtime Object and Runtime Class ID are used 
     *          in Threads,  Events and other objects to check the instance 
     *          of object during runtime.
     *
     * \see RuntimeObject
     **/
    class AREG_API RuntimeClassID
    {
    /************************************************************************
     * friend classes to access default constructor.
     ************************************************************************/
        /**
         * \brief   Declare friend classes to access private default constructor
         *          required to initialize runtime class ID in hash map blocks.
         **/
        template < typename KEY, typename VALUE >
        friend class HashMap;
        template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Implement>
        friend class ResourceMapBase;

    //////////////////////////////////////////////////////////////////////////
    // Static members
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Static function to create empty Runtime Class ID object.
         *          By default, the class ID value is BAD_CLASS_ID.
         *          Change the value after creating class ID object.
         **/
        static inline RuntimeClassID createEmptyClassID();

    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Initialization constructor.
         *          This constructor is initializing the name Runtime Class ID
         * \param   className   The name of Runtime Class ID
         **/
        explicit RuntimeClassID( const char * className );
        explicit RuntimeClassID( const areg::String& className );

        /**
         * \brief   Copy constructor.
         * \param   src     The source to copy data.
         **/
        RuntimeClassID( const RuntimeClassID & src );

        /**
         * \brief   Move constructor.
         * \param   src     The source to move data.
         **/
        RuntimeClassID( RuntimeClassID && src ) noexcept;

        /**
         * \brief   Destructor
         **/
        ~RuntimeClassID() = default;

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // friend global operators
    /************************************************************************/

        /**
         * \brief   Compares null-terminated string with Runtime Class ID name.
         * \param   lhs     Left-Hand Operand, string to compare.
         * \param   rhs     Right-Hand Operand, Runtime Class ID to compare the name.
         * \return  Returns true if string is equal to Runtime Class ID.
         **/
        friend inline bool operator == ( const char * lhs, const RuntimeClassID & rhs );
        friend inline bool operator == ( const areg::String& lhs, const RuntimeClassID & rhs );

        /**
         * \brief   Compare null-terminated string with Runtime Class ID name.
         * \param   lhs     Left-Hand Operand, string to compare.
         * \param   rhs     Right-Hand Operand, Runtime Class to compare the name.
         * \return  Returns true if string is not equal to Runtime Class ID.
         **/
        friend inline bool operator != ( const char * lhs, const RuntimeClassID & rhs );
        friend inline bool operator != ( const areg::String& lhs, const RuntimeClassID & rhs );

        /**
         * \brief   Compares number with Runtime Class ID calculated number.
         * \param   lhs     Left-Hand Operand, number to compare.
         * \param   rhs     Right-Hand Operand, Runtime Class ID to compare the calculated number.
         * \return  Returns true if number is equal to Runtime Class ID.
         **/
        friend inline bool operator == ( uint32_t lhs, const RuntimeClassID & rhs );

        /**
         * \brief   Compares number with Runtime Class ID calculated number.
         * \param   lhs     Left-Hand Operand, number to compare.
         * \param   rhs     Right-Hand Operand, Runtime Class ID to compare the calculated number.
         * \return  Returns true if number is not equal to Runtime Class ID.
         **/
        friend inline bool operator != ( uint32_t lhs, const RuntimeClassID & rhs );

    /************************************************************************/
    // class members operators
    /************************************************************************/

        /**
         * \brief   Assigning operator. Copies Runtime Class ID name from given Runtime Class ID source
         * \param   src     The source of Runtime Class ID to copy.
         * \return  Returns Runtime Class ID object.
         **/
        inline RuntimeClassID & operator = ( const RuntimeClassID & src );

        /**
         * \brief   Move operator. Moves Runtime Class ID name from given Runtime Class ID source.
         * \param   src     The source of Runtime Class ID to move.
         * \return  Returns Runtime Class ID object.
         **/
        inline RuntimeClassID & operator = ( RuntimeClassID && src ) noexcept;

        /**
         * \brief   Assigning operator. Copies Runtime Class ID name from given string buffer source
         * \param   src     The source of string buffer to copy.
         * \return  Returns Runtime Class ID object.
         **/
        inline RuntimeClassID & operator = ( const char * src );
        inline RuntimeClassID & operator = ( const areg::String& src );

        /**
         * \brief   Comparing operator. Compares 2 Runtime Class ID objects.
         * \param   other   The Runtime Class ID object to compare
         * \return  Returns true if 2 instance of Runtime Class ID have same Class ID value.
         **/
        inline bool operator == ( const RuntimeClassID & other ) const;
        /**
         * \brief   Comparing operator. Compares Runtime Class ID and string.
         * \param   other   The null-terminated string to compare
         * \return  Returns true if Runtime Class ID value is equal to null-terminated string.
         **/
        inline bool operator == ( const char * other ) const;
        inline bool operator == ( const areg::String& other ) const;
        /**
         * \brief   Comparing operator. Compares 2 Runtime Class ID objects.
         * \param   other   The Runtime Class ID object to compare
         * \return  Returns true if 2 instance of Runtime Class ID have different Class ID values.
         **/
        inline bool operator != ( const RuntimeClassID & other ) const;
        /**
         * \brief   Comparing operator. Compares Runtime Class ID and string.
         * \param   other   The null-terminated string to compare
         * \return  Returns true if Runtime Class ID value is not equal to given null-terminated string.
         **/
        inline bool operator != (const char * other) const;
        inline bool operator != (const areg::String& other) const;

        /**
         * \brief   Operator to convert the value or Runtime Class ID to unsigned integer value.
         *          Used to calculate hash value in hash map
         **/
        inline explicit operator uint32_t () const;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
        /**
         * \brief   Returns true if the value of Runtime Class ID is not equal to RuntimeClassID::BAD_CLASS_ID.
         **/
        inline bool isValid() const;

        /**
         * \brief   Returns the name of Runtime Class ID.
         **/
        inline const areg::String& getName() const;

        /**
         * \brief   Sets the name of Runtime Class ID.
         **/
        void setName( const char* className );
        void setName( const areg::String& className );

        /**
         * \brief   Returns calculated number of runtime class.
         **/
        inline unsigned getMagic() const;

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Default constructor. Private. Will create BAD_CLASS_ID
         *          object.
         **/
        RuntimeClassID();

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Runtime Class ID value.
         **/
        areg::String          mClassName;
        /**
         * \brief   The calculated number of runtime class.
         **/
        uint32_t    mMagicNum;
    };

    //////////////////////////////////////////////////////////////////////////
    // RuntimeClassID class inline function implementation
    //////////////////////////////////////////////////////////////////////////

    inline RuntimeClassID RuntimeClassID::createEmptyClassID()
    {
        return RuntimeClassID();
    }

    inline RuntimeClassID & RuntimeClassID::operator = ( const RuntimeClassID & src )
    {
        this->mClassName= src.mClassName;
        this->mMagicNum = src.mMagicNum;

        return (*this);
    }

    inline RuntimeClassID & RuntimeClassID::operator = ( RuntimeClassID && src ) noexcept
    {
        this->mClassName= std::move(src.mClassName);
        this->mMagicNum = src.mMagicNum;

        return (*this);
    }

    inline RuntimeClassID & RuntimeClassID::operator = ( const char * src )
    {
        setName(src);
        return (*this);
    }

    inline RuntimeClassID & RuntimeClassID::operator = ( const areg::String & src )
    {
        setName(src);
        return (*this);
    }

    inline bool RuntimeClassID::operator == ( const RuntimeClassID & other ) const
    {
        return (mMagicNum == other.mMagicNum);
    }

    inline bool RuntimeClassID::operator == ( const char * other ) const
    {
        return mClassName == other;
    }

    inline bool RuntimeClassID::operator == (const areg::String& other) const
    {
        return mClassName == other;
    }

    inline bool RuntimeClassID::operator != ( const RuntimeClassID  & other ) const
    {
        return (mMagicNum != other.mMagicNum);
    }

    inline bool RuntimeClassID::operator != ( const char* other ) const
    {
        return mClassName != other;
    }

    inline bool RuntimeClassID::operator != (const areg::String & other) const
    {
        return mClassName != other;
    }

    inline RuntimeClassID::operator uint32_t () const
    {
        return mMagicNum;
    }

    inline bool RuntimeClassID::isValid() const
    {
        return (mMagicNum != areg::CHECKSUM_IGNORE);
    }

    inline const areg::String & RuntimeClassID::getName() const
    {
        return mClassName;
    }

    inline unsigned RuntimeClassID::getMagic() const
    {
        return mMagicNum;
    }

    inline bool operator == ( const char * lhs, const RuntimeClassID & rhs )
    {
        return rhs.mClassName   == lhs;
    }

    inline bool operator == ( const areg::String & lhs, const RuntimeClassID & rhs )
    {
        return rhs.mClassName   == lhs;
    }

    inline bool operator != ( const char* lhs, const RuntimeClassID & rhs )
    {
        return rhs.mClassName != lhs;
    }

    inline bool operator != ( const areg::String & lhs, const RuntimeClassID & rhs )
    {
        return rhs.mClassName != lhs;
    }

    inline bool operator == ( uint32_t lhs, const RuntimeClassID & rhs )
    {
        return rhs.mMagicNum  == lhs;
    }

    inline bool operator != ( uint32_t lhs, const RuntimeClassID & rhs )
    {
        return rhs.mMagicNum != lhs;
    }

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of RuntimeClassID class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the RuntimeClassID.
 */
namespace std
{
    template<>
    struct hash<areg::RuntimeClassID>
    {
        //! A function to convert String object to uint32_t.
        inline uint32_t operator()(const areg::RuntimeClassID& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
}

#endif  // AREG_BASE_RUNTIMECLASSID_HPP
