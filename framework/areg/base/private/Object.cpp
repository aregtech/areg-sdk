/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Object.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform Generic Object class
 *
 ************************************************************************/

#include "areg/base/Object.hpp"
#include <new>

//////////////////////////////////////////////////////////////////////////
// Object class implementation
//////////////////////////////////////////////////////////////////////////
inline const Object& Object::self( void ) const
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Constructor to store pointer to void object
 **/
Object::Object( void* objData )
    : IEGenericObject( objData )
{
}

/**
 * \brief   Copy constructor.
 **/
Object::Object( const Object& src )
    : IEGenericObject( src.mObjData )
{
}

/**
 * \brief   Move constructor.
 **/
Object::Object( Object && src ) noexcept
    : IEGenericObject( src.mObjData )
{
    src.mObjData = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Call to clone object
 **/
IEGenericObject* Object::clone( void ) const
{
    return static_cast<IEGenericObject *>(DEBUG_NEW Object( self() ));
}

/**
 * \brief   Destroys created (cloned) object
 **/
void Object::destroy( void )
{
    delete this;
}

/**
 * \brief   Checks if object data is similar.
 **/
bool Object::isEqual( const IEGenericObject* object ) const
{
    return (object != nullptr ? static_cast<const IEGenericObject *>(this) == object || this->mObjData == object->mObjData : false);
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

Object & Object::operator = ( const Object& src )
{
    if (this != &src)
    {
        this->mObjData = src.mObjData;
    }

    return (*this);
}

Object & Object::operator = ( Object && src ) noexcept
{
    if ( this != &src )
    {
        this->mObjData = src.mObjData;
        src.mObjData = nullptr;
    }

    return (*this);
}

/**
 * \brief   Checks is 2 objects are equal
 **/
bool Object::operator == ( const Object& other ) const
{
    return isEqual( static_cast<const IEGenericObject *>(&other) );
}

/**
 * \brief   Checks whether 2 objects are not equal
 **/
bool Object::operator != ( const Object & other ) const
{
    return (isEqual( static_cast<const IEGenericObject *>(&other) ) == false);
}

/**
 * \brief   Operator to get integer value of object, mainly used in map
 **/
Object::operator unsigned int ( void ) const
{
    return MACRO_PTR2INT32(this);
}

/**
 * \brief   Overloaded new() operator
 **/
void * Object::operator new( size_t size )
{
    return ::operator new(size);
}

/**
 * \brief   Overloaded array new operator
 **/
void * Object::operator new [] ( size_t size )
{
    return ::operator new [] (size);
}

/**
 * \brief   Overloaded placement new
 **/
void * Object::operator new( size_t /*size*/, void * ptr )
{
    return ptr;
}

/**
 * \brief   Overloaded placement new
 **/
void * Object::operator new [ ]( size_t /*size*/, void * ptr )
{
    return ptr;
}

/**
 * \brief   Overloaded placement new. Stores block type, file name and line number information
 *          Used in debugging version. In other versions, only allocates memory without
 *          containing other information.
 **/
#if defined(_DEBUG) && defined(_MSC_VER)

void * Object::operator new( size_t size, int /*block*/, const char * file, int line )
{
    return ::operator new(size, 1, file, line);
}

#else   // _DEBUG

void * Object::operator new( size_t size, int /*block*/, const char * /*file*/, int /*line*/ )
{
    return ::operator new (size);
}

#endif  // _DEBUG

/**
 * \brief   Overloaded placement new. Stores block type, file name and line number information
 *          Used in debugging version. In other versions, only allocates memory without
 *          containing other information.
 **/
#if defined(_DEBUG) && defined(_MSC_VER)

void * Object::operator new [ ]( size_t size, int /*block*/, const char *file, int line )
{
    return ::operator new(size, 1, file, line);
}

#else   // _DEBUG

void * Object::operator new [ ]( size_t size, int /*block*/, const char * /*file*/, int /*line*/ )
{
    return ::operator new [] (size);
}

#endif  // _DEBUG

/**
 * \brief   Overloaded delete() operator
 **/
void Object::operator delete( void * ptr )
{
    ::operator delete(ptr);
}

/**
 * \brief   Overloaded delete() operator
 **/
void Object::operator delete( void * ptr, size_t /*size*/ )
{
    ::operator delete(ptr);
}

/**
 * \brief   Overloaded delete() operator
 **/
void Object::operator delete( void * ptr, int, const char *, int )
{
    ::operator delete (ptr);
}

/**
 * \brief   Overloaded array delete operator
 **/
void Object::operator delete [ ] ( void * ptr )
{
    ::operator delete [] (ptr);
}

/**
 * \brief   Overloaded array delete operator
 **/
void Object::operator delete [ ] ( void * ptr, size_t /*size*/ )
{
    ::operator delete [] (ptr);
}

/**
 * \brief   Overloaded delete [] operator
 **/
void Object::operator delete [ ]( void * ptr, int, const char *, int )
{
    ::operator delete [] (ptr);
}
