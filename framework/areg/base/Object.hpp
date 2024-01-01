#ifndef AREG_BASE_OBJECT_HPP
#define AREG_BASE_OBJECT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/Object.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Generic Object class
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEGenericObject.hpp"
#include <functional>

//////////////////////////////////////////////////////////////////////////
// Object class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This class inherits IEGenericObject interface.
 *          Used to have common object class to be able to save
 *          pointers in typed template classes. As well as it has
 *          overwritten new and delete operators, and other basic
 *          operators that are used.
 **/
class AREG_API Object  : public IEGenericObject
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class TEObjectFactory<Object>;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    Object( void ) = default;

    /**
     * \brief   Store pointer as object data.
     * \param   objData     The pointer to store in object.
     **/
    explicit Object( void* objData );

    /**
     * \brief   Copy data from given source.
     * \param   src     The source to copy data.
     **/
    Object( const Object & src );

    /**
     * \brief   Moves data from given source.
     * \param   src     The source to move data.
     **/
    Object( Object && src ) noexcept;

    /**
     * \brief   Destructor.
     **/
    virtual ~Object( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes / Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEGenericObject interface overrides
/************************************************************************/
    /**
     * \brief   Call to clone object
     * \return  Pointer to cloned object
     **/
    virtual IEGenericObject* clone( void ) const override;

    /**
     * \brief   Destroys created (cloned) object
     **/
    virtual void destroy( void ) override;

    /**
     * \brief   Checks if object data is similar.
     *          In this class it checks address of pointer to void object.
     *          The method is used in map and vector objects.
     *          Override this method if it needs to have another logic of comparing 2 objects
     *
     * \param   other   Pointer to an object to compare
     * \return  Returns 'true' if 2 objects are equal
     **/
    virtual bool isEqual( const IEGenericObject * other ) const override;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators ( = , ==,
/************************************************************************/
    /**
     * \brief   Assignment operator.
     * \param   src     Source to copy data
     **/
    Object & operator = ( const Object & src );

    /**
     * \brief   Move operator.
     * \param   src     Source to move data
     **/
    Object & operator = ( Object && src ) noexcept;

    /**
     * \brief   Checks whether 2 objects are equal.
     * \param   other   Reference to an object to compare.
     * \return  Returns 'true' if 2 objects are equal.
     **/
    bool operator == ( const Object& other ) const;

    /**
     * \brief   Checks whether 2 objects are not equal.
     * \param   other   Reference to an object to compare.
     * \return  Returns 'true' if 2 objects are not equal.
     **/
    bool operator != ( const Object & other ) const;

    /**
     * \brief   Operator to get integer value of object, mainly used in map
     * \return  Integer value of object.
     **/
    explicit operator unsigned int ( void ) const;

/************************************************************************
 * new operator
 ************************************************************************/
    /**
     * \brief   Overloaded new() operator
     * \param   size    The size of the memory block to allocate
     * \return  Valid pointer to a memory block of size 'size' or nullptr in case of error.
     **/
    void * operator new( size_t size );

    /**
     * \brief   Overloaded array new operator
     * \param   size    The size of the memory block to allocate
     * \return  Pointer to a memory block of size 'size' or nullptr in case of error.
     **/
    void * operator new [ ] ( size_t size );

    /**
     * \brief   Overloaded placement new
     * \param   size    Not used. Size of the memory block to allocate
     * \param   ptr     Pointer to the memory block where the object is located
     * \return  Pointer to the memory block where the object is located, same as 'ptr'
     **/
    void * operator new( size_t /*size*/, void * ptr );

    /**
     * \brief   Overloaded placement new
     * \param   size    Not used. Size of the memory block to allocate
     * \param   ptr     Pointer to the memory block where the object is located
     * \return  Pointer to the memory block where the object is located, same as 'ptr'
     **/
    void * operator new [ ] ( size_t /*size*/, void *ptr );

    /**
     * \brief   Overloaded placement new. Stores block type, file name and line number information
     *          Used in debugging version. In other versions, only allocates memory without
     *          containing other information.
     * \param   size    The size of the memory block to allocate
     * \param   block   Not used. Block type. Always passed 1 as a normal block.
     * \param   file    Ignored in non-debug version. Source code file name, normally __FILE__
     * \param   line    Ignored in non-debug version. Source code line number, normally __LINE__
     * \return  Pointer to a memory block of size 'size' or nullptr in case of error.
     **/
    void * operator new( size_t size, int /*block*/, const char * file, int line );

    /**
     * \brief   Overloaded placement new. Stores block type, file name and line number information
     *          Used in debugging version. In other versions, only allocates memory without
     *          containing other information.
     * \param   size    The size of the memory block to allocate
     * \param   block   Not used. Block type. Always passed 1 as a normal block.
     * \param   file    Ignored in non-debug version. Source code file name, normally __FILE__
     * \param   line    Ignored in non-debug version. Source code line number, normally __LINE__
     * \return  Pointer to a memory block of size 'size' or nullptr in case of error.
     **/
    void * operator new [ ] ( size_t size, int /*block*/, const char * file, int line );

/************************************************************************
 * delete operators
 ************************************************************************/
    /**
     * \brief   Overloaded delete() operator
     * \param   ptr   pointer to the memory block to delete
     **/
    void operator delete( void * ptr );

    /**
     * \brief   Overloaded delete() operator
     * \param   ptr     Pointer to the memory block to delete
     * \param   size    Not used.
     **/
    void operator delete( void * ptr, size_t size );

    /**
     * \brief	Overloaded delete() operator
     * \param	ptr	Pointer to the memory block to delete
     **/
    void operator delete( void * ptr, int, const char *, int );

    /**
     * \brief   Overloaded array delete operator
     * \param   ptr     Pointer to the memory block to delete
     **/
    void operator delete [ ] ( void* ptr );

    /**
     * \brief   Overloaded array delete operator
     * \param   ptr     Pointer to the memory block to delete
     * \param   size    Not used.
     **/
    void operator delete [ ] (void* ptr, size_t /*size*/);

    /**
     * \brief	Overloaded delete [] operator
     * \param	ptr	Pointer to the memory block to delete
     **/
    void operator delete [ ] ( void * ptr, int, const char *, int );

//////////////////////////////////////////////////////////////////////////
// Hidden functions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the reference to the object.
     **/
   inline const Object& self( void ) const;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of Object class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the Object.
 */
namespace std
{
    template<>
    struct hash<Object>
    {
        //! A function to convert Object to unsigned int.
        inline unsigned int operator()(const Object& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// Object class inline functions
//////////////////////////////////////////////////////////////////////////

#endif  // AREG_BASE_OBJECT_HPP
