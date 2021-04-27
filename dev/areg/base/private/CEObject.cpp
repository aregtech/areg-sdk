/************************************************************************
 * \file        areg/base/private/CEObject.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Generic Object class
 *
 ************************************************************************/

#include "areg/base/CEObject.hpp"
#include <new>

//////////////////////////////////////////////////////////////////////////
// CEObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Default constructor
 **/
CEObject::CEObject( void )
    : IEGenericObject( )
{
    ; // do nothing
}

/**
 * \brief   Constructor to store pointer to void object
 * \param objData   The pointer to void object
 **/
CEObject::CEObject( void* objData )
    : IEGenericObject( objData )
{
    ; // do nothing
}

/**
 * \brief   Copy constructor.
 * \param   src     The source of data to copy.
 **/
CEObject::CEObject( const CEObject& src )
    : IEGenericObject( src.mObjData )
{
    ; // do nothing
}

/**
* \brief   Destructor
**/
CEObject::~CEObject( void )
{
    mObjData = static_cast<void *>(NULL);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Call to clone object
 * \return  Pointer to cloned object
 **/
IEGenericObject* CEObject::Clone( void ) const
{
    return static_cast<IEGenericObject *>(DEBUG_NEW CEObject( self() ));
}

/**
 * \brief   Destroys created (cloned) object
 **/
void CEObject::Destroy( void )
{
    delete this;
}

/**
 * \brief   Checks if object data is similar.
 *          In this class it checks address of pointer to void object.
 *          The method is used in map and vector objects.
 *          Override this method if it needs to have another logic of comparing 2 objects
 *
 * \param   object      Pointer to an object to compare
 * \return  Returns 'true' if 2 objects are equal
 **/
bool CEObject::IsEqual( const IEGenericObject* object ) const
{
    return (object != NULL ? static_cast<const IEGenericObject *>(this) == object || this->mObjData == object->mObjData : false);
}

/**
 * \brief   Gets the hash key of the object. Mainly used in map to access data in hash table
 * \return  Hash key value, used in map
 **/
unsigned int CEObject::GetHashKey( void ) const
{
    // sizeof(this) might be 8 on some system -> reinterprert cast to unsigned int loses precision
    return MACRO_64_LO_BYTE32(reinterpret_cast<size_t>(this));
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Assignment operator.
 * \param   src     Reference to a source object
 * \return  Reference to the object
 **/
const CEObject& CEObject::operator = ( const CEObject& src )
{
    if (this != &src)
        this->mObjData = src.mObjData;
    return (*this);
}

/**
 * \brief   Checks is 2 objects are equal
 * \param   other   Reference to an object to compare
 * \return  Returns 'true' if 2 objects are equal
 **/
bool CEObject::operator == ( const CEObject& other ) const
{
    return IsEqual( static_cast<const IEGenericObject *>(&other) );
}

/**
 * \brief   Checks whether 2 objects are equal
 * \param   other   Reference to an object to compare
 * \return  Returns 'true' if 2 objects are not equal
 **/
bool CEObject::operator != ( const CEObject & other ) const
{
    return (IsEqual( static_cast<const IEGenericObject *>(&other) ) == false);
}

/**
 * \brief   Operator to get integer value of object, mainly used in map
 * \return  Integer value of object.
 **/
CEObject::operator unsigned int ( void ) const
{
    return GetHashKey();
}

/**
 * \brief   Overloaded new() operator
 * \param   size    Size of the memory block to allocate
 * \return  pointer to a memory block of size 'size' or NULL if an allocation error occurred
 **/
void * CEObject::operator new( size_t size )
{
    return ::operator new(size);
}

/**
 * \brief   Overloaded array new operator
 * \param   size    Size of the memory block to allocate
 * \return  Pointer to a memory block of size 'size' or NULL if an allocation error occurred
 **/
void * CEObject::operator new [] ( size_t size )
{
    return ::operator new [] (size);
}

/**
 * \brief   Overloaded placement new
 * \param   size    Not used. Size of the memory block to allocate
 * \param   ptr     Pointer to the memory block where the object is located
 * \return  Pointer to the memory block where the object is located, same as 'ptr'
 **/
void * CEObject::operator new( size_t /*size*/, void * ptr )
{
    return ptr;
}

/**
 * \brief   Overloaded placement new
 * \param   size    Not used. Size of the memory block to allocate
 * \param   ptr     Pointer to the memory block where the object is located
 * \return  Pointer to the memory block where the object is located, same as 'ptr'
 **/
void * CEObject::operator new [ ]( size_t /*size*/, void * ptr )
{
    return ptr;
}

/**
 * \brief   Overloaded placement new. Stores block type, file name and line number information
 *          Used in debugging version. In other versions, only allocates memory without
 *          containing other information.
 * \param   size    Size of the memory block to allocate
 * \param   block   Not used. Block type. Always passed 1 as a normal block.
 * \param   file    Ignored in non-debug version. Source code file name, normally __FILE__
 * \param   line    Ignored in non-debug version. Source code line number, normally __LINE__
 * \return  Pointer to a memory block of size 'size' or NULL if an allocation error occurred
 **/
#if defined(_DEBUG) && defined(_WINDOWS)

void * CEObject::operator new( size_t size, int /*block*/, const char * file, int line )
{
    return ::operator new(size, 1, file, line);
}

#else   // _DEBUG

void * CEObject::operator new( size_t size, int /*block*/, const char * /*file*/, int /*line*/ )
{
    return ::operator new (size);
}

#endif  // _DEBUG

/**
 * \brief   Overloaded placement new. Stores block type, file name and line number information
 *          Used in debugging version. In other versions, only allocates memory without
 *          containing other information.
 * \param   size    Size of the memory block to allocate
 * \param   block   Not used. Block type. Always passed 1 as a normal block.
 * \param   file    Ignored in non-debug version. Source code file name, normally __FILE__
 * \param   line    Ignored in non-debug version. Source code line number, normally __LINE__
 * \return  Pointer to a memory block of size 'size' or NULL if an allocation error occurred
 **/
#if defined(_DEBUG) && defined(_WINDOWS)

void * CEObject::operator new [ ]( size_t size, int /*block*/, const char *file, int line )
{
    return ::operator new(size, 1, file, line);
}

#else   // _DEBUG

void * CEObject::operator new [ ]( size_t size, int /*block*/, const char * /*file*/, int /*line*/ )
{
    return ::operator new [] (size);
}

#endif  // _DEBUG

/**
 * \brief   Overloaded delete() operator
 * \param   ptr   pointer to the memory block to delete
 **/
void CEObject::operator delete( void * ptr )
{
    ::operator delete(ptr);
}

/**
 * \brief   Overloaded delete() operator
 * \param   ptr     Pointer to the memory block to delete
 * \param   size    Size of the memory block to delete.
 *                  This parameter is provided by the virtual destructor
 **/
void CEObject::operator delete( void * ptr, size_t /*size*/ )
{
    ::operator delete(ptr);
}

/**
 * \brief	Overloaded delete() operator
 * \param	ptr	Pointer to the memory block to delete
 **/
void CEObject::operator delete( void * ptr, int, const char *, int )
{
    ::operator delete (ptr);
}

/**
 * \brief   Overloaded array delete operator
 * \param   ptr     Pointer to the memory block to delete
 **/
void CEObject::operator delete [ ] ( void * ptr )
{
    ::operator delete [] (ptr);
}

/**
 * \brief   Overloaded array delete operator
 * \param   ptr     Pointer to the memory block to delete
 * \param   size    Size of the memory block to delete. 
 *                  This parameter is provided by the virtual destructor
 **/
void CEObject::operator delete [ ] ( void * ptr, size_t /*size*/ )
{
    ::operator delete [] (ptr);
}

/**
 * \brief	Overloaded delete[] operator
 * \param	ptr	Pointer to the memory block to delete
 **/
void CEObject::operator delete [ ]( void * ptr, int, const char *, int )
{
    ::operator delete [] (ptr);
}
