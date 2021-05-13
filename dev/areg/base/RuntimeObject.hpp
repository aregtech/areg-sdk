#ifndef AREG_BASE_RUNTIMEOBJECT_HPP
#define AREG_BASE_RUNTIMEOBJECT_HPP
/************************************************************************
 * \file        areg/base/RuntimeObject.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Runtime Object class.
 *              All instances of Runtime Object may have individual
 *              class ID. To define class ID, use macro:
 *              DECLARE_RUNTIME and IMPLEMENT_RUNTIME
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/Object.hpp"
#include "areg/base/private/RuntimeBase.hpp"

#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/NEUtilities.hpp"

/**
 * \brief       MACRO to declare Runtime Class ID in runtime object.
 *
 * \example     Runtime Class ID declaration via MACRO
 *
 *              In this small example the class MyClass is declared as Runtime Object
 *              predefined MACRO define Runtime Class ID, which is used in 
 *              function convert(). Function convert() returns valid pointer to 
 *              MyClass object if Runtime Object is an instance of MyClass.
 *              Otherwise it will return NULL.
 *
 *              class MyClass   : public RuntimeObject
 *              {
 *                  DECLARE_RUNTIME(MyClass)
 *              public:
 *                  MyClass( void );
 *                  ~MyClass( void );
 *              };
 *              IMPLEMENT_RUNTIME(MyClass, RuntimeObject)
 * 
 *              MyClass* convert(RuntimeObject& runtimeObj)
 *              {
 *                  return RUNTIME_CAST(&runtimeObj, MyClass);
 *              }
 **/

/************************************************************************/
// Runtime object MACRO definition. Begin
/************************************************************************/

/**
 * \brief   Declare this MACRO in your class to make runtime compatible
 *          Your class should be derived from RuntimeObject class.
 * \param   ClassName   The name of Runtime Class. Should not be string.
 * \example DECLARE_RUNTIME(MyClass)
 **/
//////////////////////////////////////////////////////////////////////////
// DECLARE_RUNTIME macro definition
//////////////////////////////////////////////////////////////////////////
#define DECLARE_RUNTIME(ClassName)                                                                      \
/*********************************************************************/                                 \
/** Static members and constants                                    **/                                 \
/*********************************************************************/                                 \
private:                                                                                                \
    /** \brief   Runtime Class Identifier                           **/                                 \
    static const RuntimeClassID _classId;                                                               \
    /** \brief   Returns RuntimeClassID object                    **/                                   \
    static const RuntimeClassID & _getClassId( void );                                                  \
/*********************************************************************/                                 \
/** RuntimeBase class overrides                                   **/                                   \
/*********************************************************************/                                 \
public:                                                                                                 \
    /** \brief   Returns the Runtime Class Identifier object        **/                                 \
    virtual const RuntimeClassID & getRuntimeClassId( void ) const;                                     \
    /** \brief   Returns the class name (Identifier name)           **/                                 \
    virtual const char* getRuntimeClassName( void ) const;                                              \
    /** \brief   Checks class instance by Class Identifier          **/                                 \
    /**          Checking is done hiearchically and if any class    **/                                 \
    /**          in base hierarchi has same RuntimeClassID,       **/                                   \
    /**          returns true. Otherwise, return false.             **/                                 \
    /** \param   classId    The Class Identifier to check.          **/                                 \
    virtual bool isInstanceOfRuntimeClass(const RuntimeClassID& classId) const;                         \
    /** \brief   Checks class instance by given name                **/                                 \
    /**          Checking is done hiearchically and if any class    **/                                 \
    /**          in base hierarchi has same name, returns true.     **/                                 \
    /**          Otherwise, return false.                           **/                                 \
    /** \param   className  The name of class to check.             **/                                 \
    virtual bool isInstanceOfRuntimeClass(const char* className) const;                                 \


/**
 * \brief   Use this MACRO in source code and specify the base class of Runtime Object.
 * \param   ClassName       The name of Runtime Class. Should not be string.
 * \param   BaseClassName   The name of base / parent class. Should not be string.
 * \example IMPLEMENT_RUNTIME(MyClass, RuntimeObject)
 **/
//////////////////////////////////////////////////////////////////////////
// IMPLEMENT_RUNTIME macro definition
//////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_RUNTIME(ClassName, BaseClassName)                                                     \
/** Initialize Runtime Class ID object      **/                                                         \
const RuntimeClassID ClassName::_classId(#ClassName);                                                   \
/** Return class identifier object **/                                                                  \
inline const RuntimeClassID& ClassName::_getClassId( void )                                             \
{   ASSERT(ClassName::_classId.isEmpty() == false); return ClassName::_classId; }                       \
/** Return class identifier object **/                                                                  \
const RuntimeClassID & ClassName::getRuntimeClassId( void ) const                                       \
{   return _getClassId();           }                                                                   \
/** Return class name **/                                                                               \
const char* ClassName::getRuntimeClassName( void ) const                                                \
{   return _getClassId().getName(); }                                                                   \
/** Check class instance by Class Identifier **/                                                        \
bool ClassName::isInstanceOfRuntimeClass( const RuntimeClassID& classId ) const                         \
{   return ( (_getClassId() == classId) || BaseClassName::isInstanceOfRuntimeClass(classId) );      }   \
/** Check class instance by name**/                                                                     \
bool ClassName::isInstanceOfRuntimeClass( const char* className ) const                                 \
{   return ( (_getClassId() == className) || BaseClassName::isInstanceOfRuntimeClass(className) );  }

/**
 * \brief   Use MACRO in header and specify the base class.
 *          Inline function implementation
 * \param   ClassName       The name of Runtime Class. Should not be string.
 * \param   BaseClassName   The name of base / parent class. Should not be string.
 * \example IMPLEMENT_RUNTIME_INLINE(MyClass, RuntimeObject)
 **/
//////////////////////////////////////////////////////////////////////////
// IMPLEMENT_RUNTIME_INLINE macro definition
//////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_RUNTIME_INLINE(ClassName, BaseClassName)                                              \
const RuntimeClassID ClassName::_classId(#ClassName);                                                   \
inline const RuntimeClassID& ClassName::_getClassId( void )                                             \
{   /** Class Identifier **/                                                                            \
    return ClassName::_classId;                                                                         \
}                                                                                                       \
/** Return class identifier object **/                                                                  \
inline const RuntimeClassID& ClassName::getRuntimeClassId( void ) const                                 \
{   return _getClassId();           }                                                                   \
/** Return class name **/                                                                               \
inline const char* ClassName::getRuntimeClassName( void ) const                                         \
{   return _getClassId().getName(); }                                                                   \
/** Check class instance by Class Identifier **/                                                        \
inline bool ClassName::isInstanceOfRuntimeClass( const RuntimeClassID& classId ) const                  \
{   return ( (_getClassId() == classId) || BaseClassName::isInstanceOfRuntimeClass(classId) );      }   \
/** Check class instance by name**/                                                                     \
inline bool ClassName::isInstanceOfRuntimeClass( const char* className ) const                          \
{   return ( (_getClassId() == className) || BaseClassName::isInstanceOfRuntimeClass(className) );  }

/**
 * \brief   Use MACRO in source code of class template and specify the base class. 
 *          class template function implementation.
 * \param   Template        The template type definition
 * \param   ClassName       The name of Runtime Class. Should not be string.
 * \param   BaseClassName   The name of base / parent class. Should not be string.
 * \param   ClassIdType     The template Runtime Class ID
 * \example IMPLEMENT_RUNTIME_TEMPLATE(template <class DATA_CLASS, class DATA_CLASS_TYPE>, MyClass<DATA_CLASS, DATA_CLASS_TYPE>, RuntimeObject, MyClass);
 **/
//////////////////////////////////////////////////////////////////////////
// IMPLEMENT_RUNTIME_TEMPLATE macro definition
//////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_RUNTIME_TEMPLATE(Template, ClassName, BaseClassName, ClassIdType)                     \
/** Initialize Runtime Class ID    **/                                                                  \
Template const RuntimeClassID ClassName::_classId(#ClassIdType);                                        \
/** Return class identifier object **/                                                                  \
Template inline const RuntimeClassID& ClassName::_getClassId( void )                                    \
{   ASSERT(ClassName::_classId.isEmpty() == false); return ClassName::_classId;    }                    \
/** Return class identifier object **/                                                                  \
Template const RuntimeClassID& ClassName::getRuntimeClassId( void ) const                               \
{   return _getClassId();           }                                                                   \
/** Return class name **/                                                                               \
Template const char* ClassName::getRuntimeClassName( void ) const                                       \
{   return _getClassId().getName(); }                                                                   \
/** Check class instance by Class Identifier **/                                                        \
Template bool ClassName::isInstanceOfRuntimeClass( const RuntimeClassID& classId ) const                \
{   return ( (_getClassId() == classId) || BaseClassName::isInstanceOfRuntimeClass(classId) );      }   \
/** Check class instance by name**/                                                                     \
Template bool ClassName::isInstanceOfRuntimeClass( const char* className ) const                        \
{   return ( (_getClassId() == className) || BaseClassName::isInstanceOfRuntimeClass(className) );  }

/**
 * \brief   Use this MACRO to make casting of instance of constant object during runtime.
 *          It returns pointer of object if the Runtime Class ID is matching to given ClassName
 *          object. Otherwise, it will return NULL pointer.
 * \param   ptr         Pointer to object
 * \param   ClassName   The name of class to cast
 **/
#define RUNTIME_CONST_CAST(ptr, ClassName)  static_cast<const ClassName *>(::RuntimeCast(static_cast<const RuntimeObject *>(ptr), #ClassName))

/**
 * \brief   Use this MACRO to make casting of instance of object during runtime.
 *          It returns pointer of object if the Runtime Class ID is matching to given ClassName
 *          object. Otherwise, it will return NULL pointer.
 * \param   ptr         Pointer to object
 * \param   ClassName   The name of class to cast
 **/
#define RUNTIME_CAST(ptr, ClassName)        const_cast<ClassName *>(RUNTIME_CONST_CAST(ptr, ClassName))

/************************************************************************/
// Runtime object MACRO definition. End
/************************************************************************/


/**
 * \brief   Runtime class object is a base class for all Runtime classes,
 *          which contain class name used in Runtime operation.
 **/
//////////////////////////////////////////////////////////////////////////
// RuntimeObject class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API RuntimeObject    : private   RuntimeBase   // Base Runtime class, declared as private
                                , public    Object        // Instance of Object class
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor
     **/
    RuntimeObject( void );

    /**
     * \brief   Destructor
     **/
    virtual ~RuntimeObject( void );

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
    DECLARE_RUNTIME(RuntimeObject)

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Makes casting of pointer of object during runtime 
     *          by given class identifier. If class is an instance
     *          of given class identifier, it returns valid pointer.
     *          Otherwise, it returns NULL.
     * \param	classId     Class identifier to make casting
     * \return	Returns valid pointer, if class is an instance of passed
     *          class identifier. Otherwise return NULL.
     **/
    inline const RuntimeObject* runtimeCast(const RuntimeClassID & classId) const;

    /**
     * \brief	Makes casting of pointer of object during runtime 
     *          by given class name. If class is an instance
     *          of given class name, it returns valid pointer.
     *          Otherwise, it returns NULL.
     * \param	className   Class name to make casting
     * \return	Returns valid pointer, if class is an instance of passed
     *          class name. Otherwise return NULL.
     **/
    inline const RuntimeObject* runtimeCast(const char * className) const;

/************************************************************************/
// friend global operations
/************************************************************************/

    /**
     * \brief	Makes casting of constant pointer of object during runtime 
     *          by given class identifier. If class is an instance
     *          of given class identifier, it returns valid pointer.
     *          Otherwise, it returns NULL.
     * \param   ptr         Constant Pointer of object to cast
     * \param	classId     Class identifier to make casting
     * \return	Returns valid pointer, if class is an instance of passed
     *          class identifier. Otherwise return NULL.
     **/
    friend inline const RuntimeObject* runtimeCast(const RuntimeObject * ptr, const RuntimeClassID & classId);

    /**
     * \brief	Makes casting of constant pointer of object during runtime 
     *          by given class name. If class is an instance
     *          of given class name, it returns valid pointer.
     *          Otherwise, it returns NULL.
     * \param   ptr         Constant Pointer of object to cast
     * \param	className   Class name to make casting
     * \return	Returns valid pointer, if class is an instance of passed
     *          class name. Otherwise return NULL.
     **/
    friend inline const RuntimeObject* runtimeCast(const RuntimeObject* ptr, const char* className);

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    RuntimeObject( const RuntimeObject & /*src*/ );
    const RuntimeObject & operator = ( const RuntimeObject & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// RuntimeObject class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const RuntimeObject* RuntimeObject::runtimeCast( const RuntimeClassID& classId ) const
{
    return (isInstanceOfRuntimeClass( classId ) ? this : NULL);
}

inline const RuntimeObject* RuntimeObject::runtimeCast( const char* className ) const
{
    return (isInstanceOfRuntimeClass( className ) ? this : NULL);
}

inline const RuntimeObject* RuntimeCast(const RuntimeObject* ptr, const RuntimeClassID& classId)
{
    return (ptr != NULL ? ptr->runtimeCast(classId) : NULL);
}

inline const RuntimeObject* RuntimeCast(const RuntimeObject* ptr, const char* className)
{
    return (ptr != NULL ? ptr->runtimeCast(className) : NULL);
}

#endif  // AREG_BASE_RUNTIMEOBJECT_HPP
