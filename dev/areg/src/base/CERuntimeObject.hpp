#ifndef AREG_BASE_CERUNTIMEOBJECT_HPP
#define AREG_BASE_CERUNTIMEOBJECT_HPP
/************************************************************************
 * \file        areg/src/base/CERuntimeObject.hpp
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
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEObject.hpp"
#include "areg/src/base/private/CERuntimeBase.hpp"

#include "areg/src/base/CERuntimeClassID.hpp"
#include "areg/src/base/NEUtilities.hpp"

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
 *              class MyClass   : public CERuntimeObject
 *              {
 *                  DECLARE_RUNTIME(MyClass)
 *              public:
 *                  MyClass( void );
 *                  ~MyClass( void );
 *              };
 *              IMPLEMENT_RUNTIME(MyClass, CERuntimeObject)
 * 
 *              MyClass* convert(CERuntimeObject& runtimeObj)
 *              {
 *                  return RUNTIME_CAST(&runtimeObj, MyClass);
 *              }
 **/

/************************************************************************/
// Runtime object MACRO definition. Begin
/************************************************************************/

/**
 * \brief   Declare this MACRO in your class to make runtime compatible
 *          Your class should be derived from CERuntimeObject class.
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
    static const CERuntimeClassID _classId;                                                             \
    /** \brief   Returns CERuntimeClassID object                    **/                                 \
    static const CERuntimeClassID & _getClassId( void );                                                \
/*********************************************************************/                                 \
/** CERuntimeBase class overrides                                   **/                                 \
/*********************************************************************/                                 \
public:                                                                                                 \
    /** \brief   Returns the Runtime Class Identifier object        **/                                 \
    virtual const CERuntimeClassID & GetRuntimeClassId( void ) const;                                   \
    /** \brief   Returns the class name (Identifier name)           **/                                 \
    virtual const char* GetRuntimeClassName( void ) const;                                              \
    /** \brief   Checks class instance by Class Identifier          **/                                 \
    /**          Checking is done hiearchically and if any class    **/                                 \
    /**          in base hierarchi has same CERuntimeClassID,       **/                                 \
    /**          returns true. Otherwise, return false.             **/                                 \
    /** \param   classId    The Class Identifier to check.          **/                                 \
    virtual bool IsInstanceOfRuntimeClass(const CERuntimeClassID& classId) const;                       \
    /** \brief   Checks class instance by given name                **/                                 \
    /**          Checking is done hiearchically and if any class    **/                                 \
    /**          in base hierarchi has same name, returns true.     **/                                 \
    /**          Otherwise, return false.                           **/                                 \
    /** \param   className  The name of class to check.             **/                                 \
    virtual bool IsInstanceOfRuntimeClass(const char* className) const;                                 \


/**
 * \brief   Use this MACRO in source code and specify the base class of Runtime Object.
 * \param   ClassName       The name of Runtime Class. Should not be string.
 * \param   BaseClassName   The name of base / parent class. Should not be string.
 * \example IMPLEMENT_RUNTIME(MyClass, CERuntimeObject)
 **/
//////////////////////////////////////////////////////////////////////////
// IMPLEMENT_RUNTIME macro definition
//////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_RUNTIME(ClassName, BaseClassName)                                                     \
/** Initialize Runtime Class ID object      **/                                                         \
const CERuntimeClassID ClassName::_classId(#ClassName);                                                 \
/** Return class identifier object **/                                                                  \
inline const CERuntimeClassID& ClassName::_getClassId( void )                                           \
{   ASSERT(ClassName::_classId.IsEmpty() == false); return ClassName::_classId; }                       \
/** Return class identifier object **/                                                                  \
const CERuntimeClassID & ClassName::GetRuntimeClassId( void ) const                                     \
{   return _getClassId();           }                                                                   \
/** Return class name **/                                                                               \
const char* ClassName::GetRuntimeClassName( void ) const                                                \
{   return _getClassId().GetName(); }                                                                   \
/** Check class instance by Class Identifier **/                                                        \
bool ClassName::IsInstanceOfRuntimeClass( const CERuntimeClassID& classId ) const                       \
{   return ( _getClassId() == classId ? true : BaseClassName::IsInstanceOfRuntimeClass(classId) );  }   \
/** Check class instance by name**/                                                                     \
bool ClassName::IsInstanceOfRuntimeClass( const char* className ) const                                 \
{   return ( _getClassId() == className ? true : BaseClassName::IsInstanceOfRuntimeClass(className) );  }

/**
 * \brief   Use MACRO in header and specify the base class.
 *          Inline function implementation
 * \param   ClassName       The name of Runtime Class. Should not be string.
 * \param   BaseClassName   The name of base / parent class. Should not be string.
 * \example IMPLEMENT_RUNTIME_INLINE(MyClass, CERuntimeObject)
 **/
//////////////////////////////////////////////////////////////////////////
// IMPLEMENT_RUNTIME_INLINE macro definition
//////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_RUNTIME_INLINE(ClassName, BaseClassName)                                              \
const CERuntimeClassID ClassName::_classId(#ClassName);                                                 \
inline const CERuntimeClassID& ClassName::_getClassId( void )                                           \
{   /** Class Identifier **/                                                                            \
    return ClassName::_classId;                                                                         \
}                                                                                                       \
/** Return class identifier object **/                                                                  \
inline const CERuntimeClassID& ClassName::GetRuntimeClassId( void ) const                               \
{   return _getClassId();           }                                                                   \
/** Return class name **/                                                                               \
inline const char* ClassName::GetRuntimeClassName( void ) const                                         \
{   return _getClassId().GetName(); }                                                                   \
/** Check class instance by Class Identifier **/                                                        \
inline bool ClassName::IsInstanceOfRuntimeClass( const CERuntimeClassID& classId ) const                \
{   return ( _getClassId() == classId ? true : BaseClassName::IsInstanceOfRuntimeClass(classId) );  }   \
/** Check class instance by name**/                                                                     \
inline bool ClassName::IsInstanceOfRuntimeClass( const char* className ) const                          \
{   return ( _getClassId() == className ? true : BaseClassName::IsInstanceOfRuntimeClass(className) );  }

/**
 * \brief   Use MACRO in source code of class template and specify the base class. 
 *          class template function implementation.
 * \param   Template        The template type definition
 * \param   ClassName       The name of Runtime Class. Should not be string.
 * \param   BaseClassName   The name of base / parent class. Should not be string.
 * \param   ClassIdType     The template Runtime Class ID
 * \example IMPLEMENT_RUNTIME_TEMPLATE(template <class DATA_CLASS, class DATA_CLASS_TYPE>, MyClass<DATA_CLASS, DATA_CLASS_TYPE>, CERuntimeObject, MyClass);
 **/
//////////////////////////////////////////////////////////////////////////
// IMPLEMENT_RUNTIME_TEMPLATE macro definition
//////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_RUNTIME_TEMPLATE(Template, ClassName, BaseClassName, ClassIdType)                     \
/** Initialize Runtime Class ID    **/                                                                  \
Template const CERuntimeClassID ClassName::_classId(#ClassIdType);                                      \
/** Return class identifier object **/                                                                  \
Template inline const CERuntimeClassID& ClassName::_getClassId( void )                                  \
{   ASSERT(ClassName::_classId.IsEmpty() == false); return ClassName::_classId;    }                    \
/** Return class identifier object **/                                                                  \
Template const CERuntimeClassID& ClassName::GetRuntimeClassId( void ) const                             \
{   return _getClassId();           }                                                                   \
/** Return class name **/                                                                               \
Template const char* ClassName::GetRuntimeClassName( void ) const                                       \
{   return _getClassId().GetName(); }                                                                   \
/** Check class instance by Class Identifier **/                                                        \
Template bool ClassName::IsInstanceOfRuntimeClass( const CERuntimeClassID& classId ) const              \
{   return ( _getClassId() == classId ? true : BaseClassName::IsInstanceOfRuntimeClass(classId) );  }   \
/** Check class instance by name**/                                                                     \
Template bool ClassName::IsInstanceOfRuntimeClass( const char* className ) const                        \
{   return ( _getClassId() == className ? true : BaseClassName::IsInstanceOfRuntimeClass(className) );  }

/**
 * \brief   Use this MACRO to make casting of instance of constant object during runtime.
 *          It returns pointer of object if the Runtime Class ID is matching to given ClassName
 *          object. Otherwise, it will return NULL pointer.
 * \param   ptr         Pointer to object
 * \param   ClassName   The name of class to cast
 **/
#define RUNTIME_CONST_CAST(ptr, ClassName)  static_cast<const ClassName *>(::RuntimeCast(static_cast<const CERuntimeObject *>(ptr), #ClassName))

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
// CERuntimeObject class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CERuntimeObject  : private   CERuntimeBase   // Base Runtime class, declared as private
                                , public    CEObject        // Instance of CEObject class
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor
     **/
    CERuntimeObject( void );

    /**
     * \brief   Destructor
     **/
    virtual ~CERuntimeObject( void );

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
    DECLARE_RUNTIME(CERuntimeObject)

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
    inline const CERuntimeObject* RuntimeCast(const CERuntimeClassID & classId) const;

    /**
     * \brief	Makes casting of pointer of object during runtime 
     *          by given class name. If class is an instance
     *          of given class name, it returns valid pointer.
     *          Otherwise, it returns NULL.
     * \param	className   Class name to make casting
     * \return	Returns valid pointer, if class is an instance of passed
     *          class name. Otherwise return NULL.
     **/
    inline const CERuntimeObject* RuntimeCast(const char * className) const;

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
    friend inline const CERuntimeObject* RuntimeCast(const CERuntimeObject * ptr, const CERuntimeClassID & classId);

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
    friend inline const CERuntimeObject* RuntimeCast(const CERuntimeObject* ptr, const char* className);

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    CERuntimeObject( const CERuntimeObject & /*src*/ );
    const CERuntimeObject & operator = ( const CERuntimeObject & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CERuntimeObject class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const CERuntimeObject* CERuntimeObject::RuntimeCast( const CERuntimeClassID& classId ) const
{   return (IsInstanceOfRuntimeClass( classId ) ? this : NULL);   }

inline const CERuntimeObject* CERuntimeObject::RuntimeCast( const char* className ) const
{   return (IsInstanceOfRuntimeClass( className ) ? this : NULL);       }

inline const CERuntimeObject* RuntimeCast(const CERuntimeObject* ptr, const CERuntimeClassID& classId)
{   return (ptr != NULL ? ptr->RuntimeCast(classId) : NULL); }

inline const CERuntimeObject* RuntimeCast(const CERuntimeObject* ptr, const char* className)
{   return (ptr != NULL ? ptr->RuntimeCast(className) : NULL);      }

#endif  // AREG_BASE_CERUNTIMEOBJECT_HPP
