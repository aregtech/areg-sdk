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
    static const RuntimeClassID & _class_id();                                                        \
    /** \brief   Returns the Runtime Class Identifier object        **/                                 \
    virtual const RuntimeClassID & runtime_class_id() const override;                                  \
    /** \brief   Returns the class name (Identifier name)           **/                                 \
    virtual const String& runtime_class_name() const override;                                         \
    /** \brief   Returns the calculated number of runtime class.    **/                                 \
    virtual uint32_t runtime_class_number() const override;                                        \
    /** \brief   Checks class instance by Class Identifier.         **/                                 \
    /**          Checking is done hierarchically.                   **/                                 \
    virtual bool is_runtime_instance(const RuntimeClassID & classId) const override;               \
    /** \brief   Checks class instance by given name.               **/                                 \
    virtual bool is_runtime_instance(const char * className) const override;                       \
    virtual bool is_runtime_instance(const String & className) const override;                     \
    /** \brief   Checks class instance by magic number.             **/                                 \
    virtual bool is_runtime_instance( uint32_t classMagic ) const override;                    \


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
const RuntimeClassID & ClassName::_class_id()                                                                 \
{   static const RuntimeClassID _classId(#ClassName); return _classId;                                      }   \
const RuntimeClassID & ClassName::runtime_class_id() const                                                     \
{   return ClassName::_class_id();                                                                        }   \
const String& ClassName::runtime_class_name() const                                                            \
{   return ClassName::_class_id().name();                                                              }   \
uint32_t ClassName::runtime_class_number() const                                                           \
{   return ClassName::_class_id().magic();                                                             }   \
/* All 4 is_runtime_instance overloads: check own ID, then delegate to base. */                            \
bool ClassName::is_runtime_instance( const RuntimeClassID & classId ) const                                \
{   return ((ClassName::_class_id() == classId) || BaseClassName::is_runtime_instance(classId));     }   \
bool ClassName::is_runtime_instance( const char * className ) const                                        \
{   return ((className == ClassName::_class_id()) || BaseClassName::is_runtime_instance(className)); }   \
bool ClassName::is_runtime_instance( const String & className ) const                                      \
{   return ((className == ClassName::_class_id()) || BaseClassName::is_runtime_instance(className)); }   \
bool ClassName::is_runtime_instance( uint32_t classMagic ) const                                       \
{   return ((classMagic == ClassName::_class_id()) || BaseClassName::is_runtime_instance(classMagic));   }

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
Template const RuntimeClassID & ClassName::_class_id()                                                        \
{   static const RuntimeClassID _classId(#ClassName); return _classId;                                      }   \
/** Return class identifier object **/                                                                          \
Template const RuntimeClassID& ClassName::runtime_class_id() const                                             \
{   return ClassName::_class_id();                                                                        }   \
/** Return class name **/                                                                                       \
Template const String & ClassName::runtime_class_name() const                                                  \
{   return ClassName::_class_id().name();                                                              }   \
/** Return class number **/                                                                                     \
Template uint32_t ClassName::runtime_class_number() const                                                  \
{   return ClassName::_class_id().magic();                                                             }   \
/** Check class instance by Class Identifier **/                                                                \
Template bool ClassName::is_runtime_instance( const RuntimeClassID & classId ) const                       \
{   return ((ClassName::_class_id() == classId) || BaseClassName::is_runtime_instance(classId));     }   \
/** Check class instance by name**/                                                                             \
Template bool ClassName::is_runtime_instance( const char * className ) const                               \
{   return ((className == ClassName::_class_id()) || BaseClassName::is_runtime_instance(className)); }   \
Template bool ClassName::is_runtime_instance( const String & className ) const                             \
{   return ((className == ClassName::_class_id()) || BaseClassName::is_runtime_instance(className)); }   \
/** Check class instance by number **/                                                                          \
Template bool ClassName::is_runtime_instance( uint32_t classMagic ) const                              \
{   return ((classMagic == ClassName::_class_id()) || BaseClassName::is_runtime_instance(classMagic));   }

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
#define AREG_RUNTIME_CONST_CAST(ptr, ClassName)     static_cast<const ClassName *>(::runtime_cast(static_cast<const RuntimeObject *>(ptr), ClassName::_class_id()))

/**
 * \brief   Runtime cast. Returns non-const pointer if the object is an instance of ClassName.
 **/
#define AREG_RUNTIME_CAST(ptr, ClassName)            const_cast<ClassName *>(AREG_RUNTIME_CONST_CAST(ptr, ClassName))

// Kept for reference: string-based exact cast (slower, uses class name string lookup).
// #define AREG_RUNTIME_CONST_EXACT_CAST(ptr, ClassName)  static_cast<const ClassName *>(::runtime_cast(static_cast<const RuntimeObject *>(ptr), #ClassName))

/************************************************************************/
// Runtime object MACRO definition. End
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// RuntimeObject class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all runtime classes. Contains class name information used in runtime
 *          operations and provides runtime type casting capabilities.
 **/
class AREG_API RuntimeObject    : private   RuntimeBase   // Base Runtime class, declared as private
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
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
     * \brief   Destroys created (cloned) object.
     **/
    virtual void destroy();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts object to integer value for use in maps.
     *
     * \return  Integer value of object.
     **/
    explicit operator uint32_t () const;

/************************************************************************
 * new operator
 ************************************************************************/
    /**
     * \brief   Overloaded memory allocation operator.
     *
     * \param   size    The size of the memory block to allocate
     * \return  Valid pointer to memory block or nullptr on error.
     **/
    void * operator new( size_t size );

    /**
     * \brief   Overloaded array memory allocation operator.
     *
     * \param   size    The size of the memory block to allocate
     * \return  Pointer to memory block or nullptr on error.
     **/
    void * operator new [ ] ( size_t size );

    /**
     * \brief   Placement new operator. Constructs object at given memory location.
     *
     * \param   ptr     Pointer to the memory block where the object is located
     * \return  Same pointer as input.
     **/
    void * operator new( size_t /*size*/, void * ptr );

    /**
     * \brief   Placement array new operator. Constructs array at given memory location.
     *
     * \param   ptr     Pointer to the memory block where the array is located
     * \return  Same pointer as input.
     **/
    void * operator new [ ] ( size_t /*size*/, void *ptr );

    /**
     * \brief   Debug placement new. Stores file name and line number information (ignored in
     *          non-debug builds).
     *
     * \param   size    The size of the memory block to allocate
     * \param   file    Source code file name, normally __FILE__. Ignored in non-debug build.
     * \param   line    Source code line number, normally __LINE__. Ignored in non-debug build.
     * \return  Pointer to memory block or nullptr on error.
     **/
    void * operator new( size_t size, int32_t /*block*/, const char * file, int32_t line );

    /**
     * \brief   Debug placement array new. Stores file name and line number information (ignored in
     *          non-debug builds).
     *
     * \param   size    The size of the memory block to allocate
     * \param   file    Source code file name, normally __FILE__. Ignored in non-debug build.
     * \param   line    Source code line number, normally __LINE__. Ignored in non-debug build.
     * \return  Pointer to memory block or nullptr on error.
     **/
    void * operator new [ ] ( size_t size, int32_t /*block*/, const char * file, int32_t line );

/************************************************************************
 * delete operators
 ************************************************************************/
    /**
     * \brief   Overloaded memory deallocation operator.
     *
     * \param   ptr     Pointer to the memory block to delete
     **/
    void operator delete( void * ptr );

    /**
     * \brief   Overloaded memory deallocation operator with size hint.
     *
     * \param   ptr     Pointer to the memory block to delete
     * \param   size    Not used; included for standard compatibility.
     **/
    void operator delete( void * ptr, size_t size );

    /**
     * \brief   Debug delete operator matching debug new signature.
     *
     * \param   ptr     Pointer to the memory block to delete
     **/
    void operator delete( void * ptr, int32_t, const char *, int32_t );

    /**
     * \brief   Overloaded array memory deallocation operator.
     *
     * \param   ptr     Pointer to the memory block to delete
     **/
    void operator delete [ ] ( void* ptr );

    /**
     * \brief   Overloaded array memory deallocation operator with size hint.
     *
     * \param   ptr     Pointer to the memory block to delete
     **/
    void operator delete [ ] (void* ptr, size_t /*size*/);

    /**
     * \brief   Debug array delete operator matching debug array new signature.
     *
     * \param   ptr     Pointer to the memory block to delete
     **/
    void operator delete [ ] ( void * ptr, int32_t, const char *, int32_t );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Casts pointer to given class identifier at runtime. Returns valid pointer if object
     *          is instance of class; nullptr otherwise.
     *
     * \param   classId     Class identifier to cast to
     * \return  Returns valid pointer if cast succeeds; nullptr otherwise.
     **/
    inline const RuntimeObject* runtime_cast(const RuntimeClassID & classId) const;

    /**
     * \brief   Casts pointer to given class name at runtime. Returns valid pointer if object is
     *          instance of class; nullptr otherwise.
     *
     * \param   className       Class name to cast to
     * \return  Returns valid pointer if cast succeeds; nullptr otherwise.
     **/
    inline const RuntimeObject* runtime_cast(const char * className) const;
    /**
     * \brief   Casts pointer to given class name at runtime. Returns valid pointer if object is
     *          instance of class; nullptr otherwise.
     *
     * \param   className       Class name to cast to
     * \return  Returns valid pointer if cast succeeds; nullptr otherwise.
     **/
    inline const RuntimeObject* runtime_cast(const String & className) const;

    /**
     * \brief   Casts pointer using class magic number at runtime. Returns valid pointer if class
     *          numbers match; nullptr otherwise.
     *
     * \param   classNumber     Magic number of the class to compare
     * \return  Returns valid pointer if class magic numbers match; nullptr otherwise.
     **/
    inline const RuntimeObject* runtime_cast(uint32_t classNumber) const;

/************************************************************************/
// friend global operations
/************************************************************************/

    /**
     * \brief   Casts constant pointer to given class identifier at runtime. Returns valid pointer
     *          if object is instance of class; nullptr otherwise.
     *
     * \param   ptr         Constant pointer of object to cast
     * \param   classId     Class identifier to cast to
     * \return  Returns valid pointer if cast succeeds; nullptr otherwise.
     **/
    friend inline const RuntimeObject* runtime_cast(const RuntimeObject * ptr, const RuntimeClassID & classId);

    /**
     * \brief   Casts constant pointer to given class name at runtime. Returns valid pointer if
     *          object is instance of class; nullptr otherwise.
     *
     * \param   ptr             Constant pointer of object to cast
     * \param   className       Class name to cast to
     * \return  Returns valid pointer if cast succeeds; nullptr otherwise.
     **/
    friend inline const RuntimeObject* runtime_cast(const RuntimeObject* ptr, const char* className);
    /**
     * \brief   Casts constant pointer to given class name at runtime. Returns valid pointer if
     *          object is instance of class; nullptr otherwise.
     *
     * \param   ptr             Constant pointer of object to cast
     * \param   className       Class name to cast to
     * \return  Returns valid pointer if cast succeeds; nullptr otherwise.
     **/
    friend inline const RuntimeObject* runtime_cast(const RuntimeObject* ptr, const String & className);

    /**
     * \brief   Casts constant pointer using class magic number at runtime. Returns valid pointer if
     *          class numbers match; nullptr otherwise.
     *
     * \param   ptr             Constant pointer of object to cast
     * \param   classNumber     Magic number of the class to compare
     * \return  Returns valid pointer if class magic numbers match; nullptr otherwise.
     **/
    friend const RuntimeObject* runtime_cast( const RuntimeObject* ptr, uint32_t classNumber );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RuntimeObject );
};

//////////////////////////////////////////////////////////////////////////
// RuntimeObject class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const RuntimeObject* RuntimeObject::runtime_cast( const RuntimeClassID & classId ) const
{
    return (is_runtime_instance( classId ) ? this : nullptr);
}

inline const RuntimeObject* RuntimeObject::runtime_cast( const char* className ) const
{
    return (is_runtime_instance( className ) ? this : nullptr);
}

inline const RuntimeObject* RuntimeObject::runtime_cast(const String & className) const
{
    return (is_runtime_instance(className) ? this : nullptr);
}

inline const RuntimeObject* RuntimeObject::runtime_cast( uint32_t classNumber ) const
{
    return (is_runtime_instance( classNumber ) ? this : nullptr);
}

inline const RuntimeObject* runtime_cast(const RuntimeObject * ptr, const RuntimeClassID & classId)
{
    return (ptr != nullptr ? ptr->runtime_cast(classId) : nullptr);
}

inline const RuntimeObject* runtime_cast(const RuntimeObject * ptr, const char * className)
{
    return (ptr != nullptr ? ptr->runtime_cast(className) : nullptr);
}

inline const RuntimeObject* runtime_cast(const RuntimeObject* ptr, const String & className)
{
    return (ptr != nullptr ? ptr->runtime_cast(className) : nullptr);
}

inline const RuntimeObject* runtime_cast(const RuntimeObject* ptr, uint32_t classNumber)
{
    return (ptr != nullptr ? ptr->runtime_cast(classNumber) : nullptr);
}

#endif  // AREG_BASE_RUNTIMEOBJECT_HPP
