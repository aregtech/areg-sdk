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
 *              All instances of Runtime Object may have individual class ID.
 *              To define class ID, use macros AREG_DECLARE_RUNTIME and
 *              AREG_IMPLEMENT_RUNTIME.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/private/RuntimeBase.hpp"

#include "areg/base/UtilityDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/RuntimeClassID.hpp"

#include <new>

/************************************************************************/
// Runtime object MACRO definition. Begin
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// AREG_DECLARE_RUNTIME
//
// Insert once inside the class body of every class that participates in
// runtime type identification.  The class must ultimately derive from
// RuntimeObject.
//
// What this macro declares:
//   CLASS_ID      -- static constexpr RuntimeClassID: the single source of truth.
//                   Computed at compile time.  Zero runtime initialization cost.
//   class_id()    -- virtual override returning CLASS_ID.
//   class_name()  -- virtual override returning CLASS_ID.name().
//   class_number()-- virtual override returning CLASS_ID.magic() directly.
//   is_runtime()  -- five virtual overloads; magic-based paths use CLASS_ID.magic(), name-based
//                   paths use CLASS_ID.name(); each delegates to the base class on miss.
//
// \param   ClassName   The unqualified class name (not a string).
// \example AREG_DECLARE_RUNTIME(MyClass)
//////////////////////////////////////////////////////////////////////////
#define AREG_DECLARE_RUNTIME(ClassName)                                                                     \
public:                                                                                                     \
    /** \brief Full compile-time runtime class identifier. Single source of truth.           **/            \
    static constexpr areg::RuntimeClassID CLASS_ID { #ClassName };                                          \
    /** \brief Returns the Runtime Class Identifier object. **/                                             \
    [[nodiscard]]                                                                                           \
    const areg::RuntimeClassID & class_id() const noexcept override;                                        \
    /** \brief Returns the class name. **/                                                                  \
    [[nodiscard]]                                                                                           \
    std::string_view class_name() const noexcept override;                                                  \
    /** \brief   Returns the class name as a C-string. **/                                                  \
    [[nodiscard]]                                                                                           \
    const char* class_string() const noexcept override;                                                     \
    /** \brief Returns a magic number (compile-time constant). **/                                          \
    [[nodiscard]]                                                                                           \
    uint32_t class_number() const noexcept override;                                                        \
    /** \brief Checks class instance by RuntimeClassID. **/                                                 \
    [[nodiscard]]                                                                                           \
    bool is_runtime( const areg::RuntimeClassID & classId ) const noexcept override;                        \
    /** \brief Checks class instance by null-terminated class name. **/                                     \
    [[nodiscard]]                                                                                           \
    bool is_runtime( const char * className ) const noexcept override;                                      \
    /** \brief Checks class instance by string_view class name. **/                                         \
    [[nodiscard]]                                                                                           \
    bool is_runtime( std::string_view className ) const noexcept override;                                  \
    /** \brief Checks class instance by String class name. **/                                              \
    [[nodiscard]]                                                                                           \
    bool is_runtime( const areg::String & className ) const noexcept override;                              \
    /** \brief Checks class instance by magic number (fastest path). **/                                    \
    [[nodiscard]]                                                                                           \
    bool is_runtime( uint32_t classMagic ) const noexcept override;


//////////////////////////////////////////////////////////////////////////
// AREG_IMPLEMENT_RUNTIME
//
// Place once in the .cpp file of every class that uses AREG_DECLARE_RUNTIME.
//
// Notes:
//   class_id()     -- returns CLASS_ID directly (no function-call indirection).
//   class_number() -- returns CLASS_ID.magic() directly; compiler folds it to a literal.
//   is_runtime(uint32_t/RuntimeClassID) -- use CLASS_ID.magic(): single integer comparison.
//   is_runtime(char*/string_view/String) -- use CLASS_ID.name(): no function call.
//
// \param   ClassName       The class name (not a string).
// \param   BaseClassName   The direct base class that also uses AREG_DECLARE_RUNTIME.
// \example AREG_IMPLEMENT_RUNTIME(MyClass, RuntimeObject)
//////////////////////////////////////////////////////////////////////////
#define AREG_IMPLEMENT_RUNTIME(ClassName, BaseClassName)                                                    \
const areg::RuntimeClassID & ClassName::class_id() const noexcept                                           \
{   return ClassName::CLASS_ID;  }                                                                          \
uint32_t ClassName::class_number() const noexcept                                                           \
{   return ClassName::CLASS_ID.magic();  }                                                                  \
std::string_view ClassName::class_name() const noexcept                                                     \
{   return ClassName::CLASS_ID.name();   }                                                                  \
const char* ClassName::class_string() const noexcept                                                        \
{   return ClassName::CLASS_ID.name().data();   }                                                           \
bool ClassName::is_runtime( const areg::RuntimeClassID & classId ) const noexcept                           \
{   return ((ClassName::CLASS_ID.magic() == classId.magic()) || BaseClassName::is_runtime(classId)); }      \
bool ClassName::is_runtime( const char * className ) const noexcept                                         \
{   return ((ClassName::CLASS_ID.name() == className) || BaseClassName::is_runtime(className));  }          \
bool ClassName::is_runtime( std::string_view className ) const noexcept                                     \
{   return ((ClassName::CLASS_ID.name() == className) || BaseClassName::is_runtime(className));  }          \
bool ClassName::is_runtime( const areg::String & className ) const noexcept                                 \
{   return ((className == ClassName::CLASS_ID.name()) || BaseClassName::is_runtime(className));  }          \
bool ClassName::is_runtime( uint32_t classMagic ) const noexcept                                            \
{   return ((ClassName::CLASS_ID.magic() == classMagic) || BaseClassName::is_runtime(classMagic)); }


//////////////////////////////////////////////////////////////////////////
// AREG_IMPLEMENT_RUNTIME_TEMPLATE
//
// Variant of AREG_IMPLEMENT_RUNTIME for class templates.
// CLASS_ID is declared by AREG_DECLARE_RUNTIME(ClassName).
//
// \param   Template        Full template parameter list, e.g. template<class T>
// \param   ClassName       Fully-qualified template class, e.g. MyClass<T>
// \param   BaseClassName   Direct base class, e.g. RuntimeObject
// \param   ClassIdType     Unqualified name matching AREG_DECLARE_RUNTIME arg, e.g. MyClass
// \example
//   AREG_IMPLEMENT_RUNTIME_TEMPLATE(
//       template <class T>,
//       MyClass<T>, RuntimeObject, MyClass)
//////////////////////////////////////////////////////////////////////////
#define AREG_IMPLEMENT_RUNTIME_TEMPLATE(Template, ClassName, BaseClassName, ClassIdType)                    \
Template const areg::RuntimeClassID & ClassName::class_id() const noexcept                                  \
{   return ClassName::CLASS_ID;          }                                                                  \
Template uint32_t ClassName::class_number() const noexcept                                                  \
{   return ClassName::CLASS_ID.magic();       }                                                             \
Template std::string_view ClassName::class_name() const noexcept                                            \
{   return ClassName::CLASS_ID.name();   }                                                                  \
Template const char* ClassName::class_string() const noexcept                                               \
{   return ClassName::CLASS_ID.name().data();   }                                                           \
Template bool ClassName::is_runtime( const areg::RuntimeClassID & classId ) const noexcept                  \
{   return ((ClassName::CLASS_ID.magic() == classId.magic()) || BaseClassName::is_runtime(classId)); }      \
Template bool ClassName::is_runtime( const char * className ) const noexcept                                \
{   return ((ClassName::CLASS_ID.name() == className) || BaseClassName::is_runtime(className));      }      \
Template bool ClassName::is_runtime( std::string_view className ) const noexcept                            \
{   return ((ClassName::CLASS_ID.name() == className) || BaseClassName::is_runtime(className));      }      \
Template bool ClassName::is_runtime( const areg::String & className ) const noexcept                        \
{   return ((className == ClassName::CLASS_ID.name()) || BaseClassName::is_runtime(className));      }      \
Template bool ClassName::is_runtime( uint32_t classMagic ) const noexcept                                   \
{   return ((ClassName::CLASS_ID.magic() == classMagic) || BaseClassName::is_runtime(classMagic));   }


//////////////////////////////////////////////////////////////////////////
// AREG_RUNTIME_CONST_CAST / AREG_RUNTIME_CAST
//
// Type-safe runtime cast, equivalent to dynamic_cast but using the
// CRC32-based magic number instead of RTTI.
//
// \param   ptr         Pointer (const or non-const) to a RuntimeObject.
// \param   ClassName   Target class name (not a string).
// \return  Pointer to ClassName if the object IS-A ClassName; nullptr otherwise.
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Runtime const cast using CLASS_ID (compile-time constant).
 *          Returns a const pointer to ClassName if ptr IS-A ClassName; nullptr otherwise.
 *          CLASS_ID is a static constexpr RuntimeClassID.
 **/
#define AREG_RUNTIME_CONST_CAST(ptr, ClassName)                                                             \
    static_cast<const ClassName *>(                                                                         \
        areg::runtime_cast(static_cast<const areg::RuntimeObject *>(ptr), ClassName::CLASS_ID.magic()))

/**
 * \brief   Runtime cast returning a non-const pointer.
 *          Returns a pointer to ClassName if ptr IS-A ClassName; nullptr otherwise.
 **/
#define AREG_RUNTIME_CAST(ptr, ClassName)                                                                   \
    const_cast<ClassName *>(AREG_RUNTIME_CONST_CAST(ptr, ClassName))

/************************************************************************/
// Runtime object MACRO definition. End
/************************************************************************/

namespace areg {

//////////////////////////////////////////////////////////////////////////
// RuntimeObject class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all user-defined runtime classes.
 *
 *          Derive from this class and use AREG_DECLARE_RUNTIME in the class
 *          body and AREG_IMPLEMENT_RUNTIME in the .cpp file to participate
 *          in the runtime type system.
 **/
class AREG_API RuntimeObject : private RuntimeBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    RuntimeObject() = default;

    virtual ~RuntimeObject() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes -- own runtime identity
//////////////////////////////////////////////////////////////////////////
public:

    AREG_DECLARE_RUNTIME( RuntimeObject )

//////////////////////////////////////////////////////////////////////////
// Operations -- lifecycle
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Destroys the object.  Override to release resources before deletion.
     **/
    virtual void destroy();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts object to its magic (CRC32) number for use in maps.
     **/
    [[nodiscard]]
    inline explicit operator uint32_t () const;

/************************************************************************
 * new operators
 ************************************************************************/
    void * operator new( size_t size );

    void * operator new []( size_t size );

    /**
     * \brief   Over-aligned allocation. Required so types with extended alignment
     *          (e.g. cache-line-aligned dispatcher objects) are actually allocated
     *          aligned; without it the class operator new(size) would hide the global
     *          aligned operator and hand back under-aligned memory.
     **/
    void * operator new( size_t size, std::align_val_t align );

    void * operator new []( size_t size, std::align_val_t align );

    void * operator new( size_t /*size*/, void * ptr );

    void * operator new []( size_t /*size*/, void * ptr );

    /**
     * \brief   Debug placement new: records file and line for MSVC leak detection.
     *          In non-debug builds, falls through to the standard operator new.
     **/
    void * operator new( size_t size, int32_t /*block*/, const char * file, int32_t line );

    /**
     * \brief   Debug array placement new.
     **/
    void * operator new []( size_t size, int32_t /*block*/, const char * file, int32_t line );

/************************************************************************
 * delete operators
 ************************************************************************/
    void operator delete( void * ptr );

    void operator delete( void * ptr, size_t size );

    /**
     * \brief   Over-aligned deallocation, matching the aligned operator new above.
     **/
    void operator delete( void * ptr, std::align_val_t align );

    void operator delete( void * ptr, size_t size, std::align_val_t align );

    /**
     * \brief   Debug delete operator matching the debug new signature.
     **/
    void operator delete( void * ptr, int32_t, const char *, int32_t );

    void operator delete []( void * ptr );

    void operator delete []( void * ptr, size_t /*size*/ );

    void operator delete []( void * ptr, std::align_val_t align );

    void operator delete []( void * ptr, size_t size, std::align_val_t align );

    /**
     * \brief   Debug array delete operator matching the debug array new signature.
     **/
    void operator delete []( void * ptr, int32_t, const char *, int32_t );

//////////////////////////////////////////////////////////////////////////
// Operations -- runtime cast helpers
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns this if the object IS-A the class identified by classId; nullptr otherwise.
     **/
    [[nodiscard]]
    inline const RuntimeObject * runtime_cast( const RuntimeClassID & classId ) const noexcept;

    /**
     * \brief   Returns this if the object IS-A the class with the given name; nullptr otherwise.
     **/
    [[nodiscard]]
    inline const RuntimeObject * runtime_cast( const char * className ) const noexcept;

    /**
     * \brief   Returns this if the object IS-A the class with the given name; nullptr otherwise.
     **/
    [[nodiscard]]
    inline const RuntimeObject * runtime_cast( const String & className ) const noexcept;

    /**
     * \brief   Returns this if the object's class_number() equals classNumber; nullptr otherwise.
     *          Fastest overload: single integer comparison, no string operations.
     **/
    [[nodiscard]]
    inline const RuntimeObject * runtime_cast( uint32_t classNumber ) const noexcept;

/************************************************************************
 * friend global runtime_cast helpers (used by AREG_RUNTIME_CAST)
 ************************************************************************/

    friend inline const RuntimeObject * runtime_cast( const RuntimeObject * ptr, const RuntimeClassID & classId ) noexcept;

    friend inline const RuntimeObject * runtime_cast( const RuntimeObject * ptr, const char * className ) noexcept;

    friend inline const RuntimeObject * runtime_cast( const RuntimeObject * ptr, const String & className ) noexcept;

    friend const RuntimeObject * runtime_cast( const RuntimeObject * ptr, uint32_t classNumber ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RuntimeObject );
};

//////////////////////////////////////////////////////////////////////////
// RuntimeObject inline implementations
//////////////////////////////////////////////////////////////////////////

inline RuntimeObject::operator uint32_t() const
{
    return class_number();
}

inline const RuntimeObject * RuntimeObject::runtime_cast( const RuntimeClassID & classId ) const noexcept
{
    return (is_runtime(classId) ? this : nullptr);
}

inline const RuntimeObject * RuntimeObject::runtime_cast( const char * className ) const noexcept
{
    return (is_runtime(className) ? this : nullptr);
}

inline const RuntimeObject * RuntimeObject::runtime_cast( const String & className ) const noexcept
{
    return (is_runtime(className) ? this : nullptr);
}

inline const RuntimeObject * RuntimeObject::runtime_cast( uint32_t classNumber ) const noexcept
{
    return (is_runtime(classNumber) ? this : nullptr);
}

[[nodiscard]]
inline const RuntimeObject * runtime_cast( const RuntimeObject * ptr, const RuntimeClassID & classId ) noexcept
{
    return (ptr != nullptr ? ptr->runtime_cast(classId) : nullptr);
}

[[nodiscard]]
inline const RuntimeObject * runtime_cast( const RuntimeObject * ptr, const char * className ) noexcept
{
    return (ptr != nullptr ? ptr->runtime_cast(className) : nullptr);
}

[[nodiscard]]
inline const RuntimeObject * runtime_cast( const RuntimeObject * ptr, const String & className ) noexcept
{
    return (ptr != nullptr ? ptr->runtime_cast(className) : nullptr);
}

[[nodiscard]]
inline const RuntimeObject * runtime_cast( const RuntimeObject * ptr, uint32_t classNumber ) noexcept
{
    return (ptr != nullptr ? ptr->runtime_cast(classNumber) : nullptr);
}

template<>
struct required_size<areg::RuntimeObject>
{
    [[nodiscard]]
    inline constexpr uint32_t operator()() const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t));
    }

    [[nodiscard]]
    inline constexpr uint32_t operator()(const areg::RuntimeObject& /*rto*/) const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t));
    }

    [[nodiscard]]
    inline constexpr operator uint32_t () const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t));
    }
};

} // namespace areg

#endif  // AREG_BASE_RUNTIMEOBJECT_HPP
