#ifndef AREG_BASE_IEGENERICOBJECT_HPP
#define AREG_BASE_IEGENERICOBJECT_HPP
/************************************************************************
 * \file        areg/src/base/IEGenericObject.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Object Interface and 
 *              Generic Object instance factory.
 *              Following classes are declared here:
 *              IEGenericObject    -- base interface for generic object instances
 *              TEObjectFactory    -- class template to support object factoring.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

/************************************************************************
 * Declared classes
 ************************************************************************/
class IEGenericObject;
template <typename ClassName> class TEObjectFactory;

//////////////////////////////////////////////////////////////////////////
// IEGenericObject class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Generic objects are supporting such functionalities
 *              as clone and destroy, as well as comparing function.
 * 
 * \details     Generic objects should support cloning, destroying and
 *              comparing functionalities. To be able to instantiate them
 *              by Object Factory, they should have of public access
 *              at least default and initialization constructor getting 
 *              void data. 
 *              For more information see declarations of Generic
 *              Object constructors.
 *
 **/
class AREG_API IEGenericObject
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor. Protected.
     **/
    IEGenericObject( void );
    /**
     * \brief   Initialization Constructor. Protected.
     **/
    IEGenericObject( void * objData );

public:
    /**
     * \brief   Destructor.
     **/
    virtual ~IEGenericObject( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEGenericObject overrides
/************************************************************************/
    /**
     * \brief	Clones Instance and returns pointer to cloned instance.
     *          The cloned instance should call Destroy() function
     *          to delete instance.
     *          Implement function in child classes.
     * \return	Pointer to cloned instance.
     **/
    virtual IEGenericObject* Clone( void ) const = 0;

    /**
     * \brief	Call to destroy instance of class.
     *          If an instance was created by calling Clone()
     *          this function should be called for cleaning up.
     **/
    virtual void Destroy( void ) = 0;

    /**
     * \brief	Compares existing generic object instance with
     *          passed as parameter. Returns true if 2 instances
     *          are equal. 
     *          Override this method to provide comparing functionality
     * \param	object	The object to compare
     * \return	Returns true if 2 instances are equal.
     **/
    virtual bool IsEqual(const IEGenericObject* object) const = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Pointer to undefined data.
     *          The meaning of content may vary in different classes.
     **/
    void *  mObjData;
};

//////////////////////////////////////////////////////////////////////////
// IEGenericObject template class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Object factory template class. It creates and destroys
 *              instances of generic object. The ClassName type
 *              should be instance of IEGenericObject
 * 
 * \details     Inherit and overwrite methods if the method to
 *              instantiate and delete generic object instance
 *              should be changed. By default, they are instantiated
 *              by calling either default or initialization constructor,
 *              and deleted by calling Destroy() function of generic
 *              object instance.
 *
 * \tparam  ClassName   The class, which should be instantiated.
 **/
template <typename ClassName>
class TEObjectFactory
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    TEObjectFactory( void );
    /**
     * \brief   Destructor.
     **/
    virtual ~TEObjectFactory( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates Instance of Generic object.
     *          By default it calls either default constructor, or
     *          initialization constructor, depending of object data
     *          value passed in function call.
     *          Overwrite method if need to change instantiation procedure.
     * \param	objData	The object data to set in generic object.
     *                  If NULL, default constructor will be called.
     * \return	Returns pointer to new created generic object.
     **/
    virtual IEGenericObject * CreateObject( void * objData );

    /**
     * \brief	Destroys instance of generic object.
     *          By default it calls Destroy() function of generic object.
     *          Overwrite method to change behavior.
     * \param	obj	    Should be valid pointer to generic object instance.
     **/
    virtual void DestroyObject( IEGenericObject * obj );
};

//////////////////////////////////////////////////////////////////////////
// TEObjectFactory template class function implementation
//////////////////////////////////////////////////////////////////////////

template <typename ClassName>
TEObjectFactory<ClassName>::TEObjectFactory()
{
    ; // do nothing
}

template <typename ClassName>
TEObjectFactory<ClassName>::~TEObjectFactory()
{
    ; // do nothing
}

template <typename ClassName>
IEGenericObject* TEObjectFactory<ClassName>::CreateObject( void * objData )
{
    return static_cast<IEGenericObject *>(objData != static_cast<void *>(NULL) ? DEBUG_NEW ClassName(objData) : DEBUG_NEW ClassName());
}

template <typename ClassName>
void TEObjectFactory<ClassName>::DestroyObject( IEGenericObject * obj )
{
    if (obj != static_cast<IEGenericObject *>(NULL))
        obj->Destroy();
}

#endif  // AREG_BASE_IEGENERICOBJECT_HPP
