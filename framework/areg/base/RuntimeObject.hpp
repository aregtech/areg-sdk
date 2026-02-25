#ifndef AREG_BASE_RUNTIMEOBJECT_HPP
#define AREG_BASE_RUNTIMEOBJECT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/RuntimeObject.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform Runtime Object class.
 *              All instances of Runtime Object may have individual
 *              class ID. To define class ID, use macro:
 *              AREG_DECLARE_RUNTIME and AREG_IMPLEMENT_RUNTIME
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/private/RuntimeBase.hpp"

#include "areg/base/UtilityDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/RuntimeClassID.hpp"

/**
 * \brief       MACRO to declare Runtime Class ID in runtime object.
 *
 * \example     Runtime Class ID declaration via MACRO
 *
 *              In this small example the class MyClass is declared as Runtime Object
 *              predefined MACRO define Runtime Class ID, which is used in 
 *              function convert(). Function convert() returns valid pointer to 
 *              MyClass object if Runtime Object is an instance of MyClass.
 *              Otherwise it will return nullptr.
 *
 *              class MyClass   : public RuntimeObject
 *              {
 *                  AREG_DECLARE_RUNTIME(MyClass)
 *              public:
 *                  MyClass();
 *                  ~MyClass();
 *              };
 *              AREG_IMPLEMENT_RUNTIME(MyClass, RuntimeObject)
 * 
 *              MyClass* convert(RuntimeObject& runtimeObj)
 *              {
 *                  return AREG_RUNTIME_CAST(&runtimeObj, MyClass);
 *              }
 **/

/************************************************************************/
// Runtime object MACRO definition. Begin
/************************************************************************/

/**
 * \brief   Declare this MACRO in your class to make runtime compatible
 *          Your class should be derived from RuntimeObject class.
 * \param   ClassName   The name of Runtime Class. Should not be string.
 * \example AREG_DECLARE_RUNTIME(MyClass)
 **/
//////////////////////////////////////////////////////////////////////////
// AREG_DECLARE_RUNTIME macro definition
//////////////////////////////////////////////////////////////////////////
#define AREG_DECLARE_RUNTIME(ClassName)                                                                 \
public:                                                                                                 \
    /** \brief   Returns RuntimeClassID object                      **/                                 \
    static const RuntimeClassID & _getClassId();                                                        \
    /** \brief   Returns the Runtime Class Identifier object        **/                                 \
    virtual const RuntimeClassID & getRuntimeClassId() const override;                                  \
    /** \brief   Returns the class name (Identifier name)           **/                                 \
    virtual const areg::String& getRuntimeClassName() const override;                                         \
    /** \brief   Returns the calculated number of runtime class.    **/                                 \
    virtual uint32_t getRuntimeClassNumber() const override;                                        \
    /** \brief   Checks class instance by Class Identifier.         **/                                 \
    /**          Checking is done hierarchically.                   **/                                 \
    virtual bool isInstanceOfRuntimeClass(const RuntimeClassID & classId) const override;               \
    /** \brief   Checks class instance by given name.               **/                                 \
    virtual bool isInstanceOfRuntimeClass(const char * className) const override;                       \
    virtual bool isInstanceOfRuntimeClass(const areg::String & className) const override;                     \
    /** \brief   Checks class instance by magic number.             **/                                 \
    virtual bool isInstanceOfRuntimeClass( uint32_t classMagic ) const override;                    \


/**
 * \brief   Use this MACRO in source code and specify the base class of Runtime Object.
 * \param   ClassName       The name of Runtime Class. Should not be string.
 * \param   BaseClassName   The name of base / parent class. Should not be string.
 * \example AREG_IMPLEMENT_RUNTIME(MyClass, RuntimeObject)
 **/
//////////////////////////////////////////////////////////////////////////
// AREG_IMPLEMENT_RUNTIME macro definition
//////////////////////////////////////////////////////////////////////////
#define AREG_IMPLEMENT_RUNTIME(ClassName, BaseClassName)                                                        \
const RuntimeClassID & ClassName::_getClassId()                                                                 \
{   static const RuntimeClassID _classId(#ClassName); return _classId;                                      }   \
const RuntimeClassID & ClassName::getRuntimeClassId() const                                                     \
{   return ClassName::_getClassId();                                                                        }   \
const areg::String& ClassName::getRuntimeClassName() const                                                            \
{   return ClassName::_getClassId().getName();                                                              }   \
uint32_t ClassName::getRuntimeClassNumber() const                                                           \
{   return ClassName::_getClassId().getMagic();                                                             }   \
/* All 4 isInstanceOfRuntimeClass overloads: check own ID, then delegate to base. */                            \
bool ClassName::isInstanceOfRuntimeClass( const RuntimeClassID & classId ) const                                \
{   return ((ClassName::_getClassId() == classId) || BaseClassName::isInstanceOfRuntimeClass(classId));     }   \
bool ClassName::isInstanceOfRuntimeClass( const char * className ) const                                        \
{   return ((className == ClassName::_getClassId()) || BaseClassName::isInstanceOfRuntimeClass(className)); }   \
bool ClassName::isInstanceOfRuntimeClass( const areg::String & className ) const                                      \
{   return ((className == ClassName::_getClassId()) || BaseClassName::isInstanceOfRuntimeClass(className)); }   \
bool ClassName::isInstanceOfRuntimeClass( uint32_t classMagic ) const                                       \
{   return ((classMagic == ClassName::_getClassId()) || BaseClassName::isInstanceOfRuntimeClass(classMagic));   }

/**
 * \brief   Use MACRO in source code of class template and specify the base class. 
 *          class template function implementation.
 * \param   Template        The template type definition
 * \param   ClassName       The name of Runtime Class. Should not be string.
 * \param   BaseClassName   The name of base / parent class. Should not be string.
 * \param   ClassIdType     The template Runtime Class ID
 * \example AREG_IMPLEMENT_RUNTIME_TEMPLATE(template <class DATA_CLASS, class DATA_CLASS_TYPE>, MyClass<DATA_CLASS, DATA_CLASS_TYPE>, RuntimeObject, MyClass);
 **/
//////////////////////////////////////////////////////////////////////////
// AREG_IMPLEMENT_RUNTIME_TEMPLATE macro definition
//////////////////////////////////////////////////////////////////////////
#define AREG_IMPLEMENT_RUNTIME_TEMPLATE(Template, ClassName, BaseClassName, ClassIdType)                        \
/** Return class identifier object **/                                                                          \
Template const RuntimeClassID & ClassName::_getClassId()                                                        \
{   static const RuntimeClassID _classId(#ClassName); return _classId;                                      }   \
/** Return class identifier object **/                                                                          \
Template const RuntimeClassID& ClassName::getRuntimeClassId() const                                             \
{   return ClassName::_getClassId();                                                                        }   \
/** Return class name **/                                                                                       \
Template const areg::String & ClassName::getRuntimeClassName() const                                                  \
{   return ClassName::_getClassId().getName();                                                              }   \
/** Return class number **/                                                                                     \
Template uint32_t ClassName::getRuntimeClassNumber() const                                                  \
{   return ClassName::_getClassId().getMagic();                                                             }   \
/** Check class instance by Class Identifier **/                                                                \
Template bool ClassName::isInstanceOfRuntimeClass( const RuntimeClassID & classId ) const                       \
{   return ((ClassName::_getClassId() == classId) || BaseClassName::isInstanceOfRuntimeClass(classId));     }   \
/** Check class instance by name**/                                                                             \
Template bool ClassName::isInstanceOfRuntimeClass( const char * className ) const                               \
{   return ((className == ClassName::_getClassId()) || BaseClassName::isInstanceOfRuntimeClass(className)); }   \
Template bool ClassName::isInstanceOfRuntimeClass( const areg::String & className ) const                             \
{   return ((className == ClassName::_getClassId()) || BaseClassName::isInstanceOfRuntimeClass(className)); }   \
/** Check class instance by number **/                                                                          \
Template bool ClassName::isInstanceOfRuntimeClass( uint32_t classMagic ) const                              \
{   return ((classMagic == ClassName::_getClassId()) || BaseClassName::isInstanceOfRuntimeClass(classMagic));   }

/**
 * \brief   Use this MACRO to make exact object casting of instance of constant object during runtime.
 *          It returns pointer of object if the Runtime Class ID is matching to given ClassName
 *          object. Otherwise, it will return nullptr pointer.
 * \param   ptr         Pointer to object
 * \param   ClassName   The name of class to cast
 **/
/**
 * \brief   Runtime const cast using ClassID lookup (fast path, used in Release and Debug).
 **/
#define AREG_RUNTIME_CONST_CAST(ptr, ClassName)     static_cast<const ClassName *>(areg::RuntimeCast(static_cast<const areg::RuntimeObject *>(ptr), ClassName::_getClassId()))

/**
 * \brief   Runtime cast. Returns non-const pointer if the object is an instance of ClassName.
 **/
#define AREG_RUNTIME_CAST(ptr, ClassName)            const_cast<ClassName *>(AREG_RUNTIME_CONST_CAST(ptr, ClassName))

// Kept for reference: string-based exact cast (slower, uses class name string lookup).
// #define AREG_RUNTIME_CONST_EXACT_CAST(ptr, ClassName)  static_cast<const ClassName *>(areg::RuntimeCast(static_cast<const areg::RuntimeObject *>(ptr), #ClassName))

/************************************************************************/
// Runtime object MACRO definition. End
/************************************************************************/

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // RuntimeObject class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Runtime class object is a base class for all Runtime classes,
     *          which contain class name used in Runtime operation.
     **/
    class AREG_API RuntimeObject    : private   RuntimeBase   // Base Runtime class, declared as private
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Constructor
         **/
        RuntimeObject() = default;

        /**
         * \brief   Destructor
         **/
        virtual ~RuntimeObject() = default;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // Declare Runtime standard functions and variables
    /************************************************************************/
        /**
         * \brief   The Runtime Object should contain runtime information.
         **/
        AREG_DECLARE_RUNTIME(RuntimeObject)

    /************************************************************************/
    // RuntimeObject interface overrides
    /************************************************************************/
        /**
         * \brief   Destroys created (cloned) object
         **/
        virtual void destroy();

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Operator to get integer value of object, mainly used in map
         * \return  Integer value of object.
         **/
        explicit operator uint32_t () const;

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
         * \param   ptr     Pointer to the memory block where the object is located
         * \return  Pointer to the memory block where the object is located, same as 'ptr'
         **/
        void * operator new( size_t /*size*/, void * ptr );

        /**
         * \brief   Overloaded placement new
         * \param   ptr     Pointer to the memory block where the object is located
         * \return  Pointer to the memory block where the object is located, same as 'ptr'
         **/
        void * operator new [ ] ( size_t /*size*/, void *ptr );

        /**
         * \brief   Overloaded placement new. Stores block type, file name and line number information
         *          Used in debugging version. In other versions, only allocates memory without
         *          containing other information.
         * \param   size    The size of the memory block to allocate
         * \param   file    Ignored in non-debug version. Source code file name, normally __FILE__
         * \param   line    Ignored in non-debug version. Source code line number, normally __LINE__
         * \return  Pointer to a memory block of size 'size' or nullptr in case of error.
         **/
        void * operator new( size_t size, int32_t /*block*/, const char * file, int32_t line );

        /**
         * \brief   Overloaded placement new. Stores block type, file name and line number information
         *          Used in debugging version. In other versions, only allocates memory without
         *          containing other information.
         * \param   size    The size of the memory block to allocate
         * \param   file    Ignored in non-debug version. Source code file name, normally __FILE__
         * \param   line    Ignored in non-debug version. Source code line number, normally __LINE__
         * \return  Pointer to a memory block of size 'size' or nullptr in case of error.
         **/
        void * operator new [ ] ( size_t size, int32_t /*block*/, const char * file, int32_t line );

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
        void operator delete( void * ptr, int32_t, const char *, int32_t );

        /**
         * \brief   Overloaded array delete operator
         * \param   ptr     Pointer to the memory block to delete
         **/
        void operator delete [ ] ( void* ptr );

        /**
         * \brief   Overloaded array delete operator
         * \param   ptr     Pointer to the memory block to delete
         **/
        void operator delete [ ] (void* ptr, size_t /*size*/);

        /**
         * \brief	Overloaded delete [] operator
         * \param	ptr	Pointer to the memory block to delete
         **/
        void operator delete [ ] ( void * ptr, int32_t, const char *, int32_t );

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief	Makes casting of pointer of object during runtime 
         *          by given class identifier. If class is an instance
         *          of given class identifier, it returns valid pointer.
         *          Otherwise, it returns nullptr.
         * \param	classId     Class identifier to make casting
         * \return	Returns valid pointer, if class is an instance of passed
         *          class identifier. Otherwise return nullptr.
         **/
        inline const RuntimeObject* runtimeCast(const RuntimeClassID & classId) const;

        /**
         * \brief	Makes casting of pointer of object during runtime 
         *          by given class name. If class is an instance
         *          of given class name, it returns valid pointer.
         *          Otherwise, it returns nullptr.
         * \param	className   Class name to make casting
         * \return	Returns valid pointer, if class is an instance of passed
         *          class name. Otherwise return nullptr.
         **/
        inline const RuntimeObject* runtimeCast(const char * className) const;
        inline const RuntimeObject* runtimeCast(const areg::String & className) const;

        /**
         * \brief	Makes casting of pointer of object during runtime 
         *          by given class number. If class numbers have same 
         *          magic numbers, it returns valid pointer.
         *          Otherwise, it returns nullptr.
         * \param	classNumber The magic number of the class to compare
         * \return	Returns valid pointer, if class is an instance of passed
         *          magic number of the class. Otherwise return nullptr.
         **/
        inline const RuntimeObject* runtimeCast(uint32_t classNumber) const;

    /************************************************************************/
    // friend global operations
    /************************************************************************/

        /**
         * \brief	Makes casting of constant pointer of object during runtime 
         *          by given class identifier. If class is an instance
         *          of given class identifier, it returns valid pointer.
         *          Otherwise, it returns nullptr.
         * \param   ptr         Constant Pointer of object to cast
         * \param	classId     Class identifier to make casting
         * \return	Returns valid pointer, if class is an instance of passed
         *          class identifier. Otherwise return nullptr.
         **/
        friend inline const RuntimeObject* RuntimeCast(const RuntimeObject * ptr, const RuntimeClassID & classId);

        /**
         * \brief	Makes casting of constant pointer of object during runtime 
         *          by given class name. If class is an instance
         *          of given class name, it returns valid pointer.
         *          Otherwise, it returns nullptr.
         * \param   ptr         Constant Pointer of object to cast
         * \param	className   Class name to make casting
         * \return	Returns valid pointer, if class is an instance of passed
         *          class name. Otherwise return nullptr.
         **/
        friend inline const RuntimeObject* RuntimeCast(const RuntimeObject* ptr, const char* className);
        friend inline const RuntimeObject* RuntimeCast(const RuntimeObject* ptr, const areg::String & className);

        /**
         * \brief	Makes casting of constant pointer of object during runtime
         *          by given class number (magic number). If class has same magic,
         *          number, it returns valid pointer.
         *          Otherwise, it returns nullptr.
         * \param   ptr         Constant Pointer of object to cast
         * \param	classNumber Class number (magic number) to compare.
         * \return	Returns valid pointer, if class has same magic number. 
         *          Otherwise return nullptr.
         **/
        friend const RuntimeObject* RuntimeCast( const RuntimeObject* ptr, uint32_t classNumber );

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE( RuntimeObject );
    };

    //////////////////////////////////////////////////////////////////////////
    // RuntimeObject class inline function implementation
    //////////////////////////////////////////////////////////////////////////

    inline const RuntimeObject* RuntimeObject::runtimeCast( const RuntimeClassID & classId ) const
    {
        return (isInstanceOfRuntimeClass( classId ) ? this : nullptr);
    }

    inline const RuntimeObject* RuntimeObject::runtimeCast( const char* className ) const
    {
        return (isInstanceOfRuntimeClass( className ) ? this : nullptr);
    }

    inline const RuntimeObject* RuntimeObject::runtimeCast(const areg::String & className) const
    {
        return (isInstanceOfRuntimeClass(className) ? this : nullptr);
    }

    inline const RuntimeObject* RuntimeObject::runtimeCast( uint32_t classNumber ) const
    {
        return (isInstanceOfRuntimeClass( classNumber ) ? this : nullptr);
    }

    inline const RuntimeObject* RuntimeCast(const RuntimeObject * ptr, const RuntimeClassID & classId)
    {
        return (ptr != nullptr ? ptr->runtimeCast(classId) : nullptr);
    }

    inline const RuntimeObject* RuntimeCast(const RuntimeObject * ptr, const char * className)
    {
        return (ptr != nullptr ? ptr->runtimeCast(className) : nullptr);
    }

    inline const RuntimeObject* RuntimeCast(const RuntimeObject* ptr, const areg::String & className)
    {
        return (ptr != nullptr ? ptr->runtimeCast(className) : nullptr);
    }

    inline const RuntimeObject* RuntimeCast(const RuntimeObject* ptr, uint32_t classNumber)
    {
        return (ptr != nullptr ? ptr->runtimeCast(classNumber) : nullptr);
    }

} // namespace areg
#endif  // AREG_BASE_RUNTIMEOBJECT_HPP
