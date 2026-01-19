#ifndef AREG_BASE_IEGENERICOBJECT_HPP
#define AREG_BASE_IEGENERICOBJECT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/IEGenericObject.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
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
#include "areg/base/GEGlobal.h"

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
    explicit IEGenericObject( void * objData );

public:
    /**
     * \brief   Destructor.
     **/
    virtual ~IEGenericObject( void ) = default;

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
    virtual IEGenericObject* clone( void ) const = 0;

    /**
     * \brief	Call to destroy instance of class.
     *          If an instance was created by calling Clone()
     *          this function should be called for cleaning up.
     **/
    virtual void destroy( void ) = 0;

    /**
     * \brief	Compares existing generic object instance with
     *          passed as parameter. Returns true if 2 instances
     *          are equal.
     *          Override this method to provide comparing functionality
     * \param	object	The object to compare
     * \return	Returns true if 2 instances are equal.
     **/
    virtual bool isEqual(const IEGenericObject* object) const = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Pointer to undefined data.
     *          The meaning of content may vary in different classes.
     **/
    void *  mObjData;

private:
    IEGenericObject(const IEGenericObject & /* src */) = delete;
    IEGenericObject & operator = (const IEGenericObject & /* src */) = delete;
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
    ~TEObjectFactory( void );

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
     *                  If nullptr, default constructor will be called.
     * \return	Returns pointer to new created generic object.
     **/
    IEGenericObject * createObject( void * objData );

    /**
     * \brief	Destroys instance of generic object.
     *          By default it calls Destroy() function of generic object.
     *          Overwrite method to change behavior.
     * \param	obj	    Should be valid pointer to generic object instance.
     **/
    void destroyObject( IEGenericObject * obj );
};

//////////////////////////////////////////////////////////////////////////
// TEObjectFactory template class function implementation
//////////////////////////////////////////////////////////////////////////

template <typename ClassName>
TEObjectFactory<ClassName>::TEObjectFactory( void )
{
}

template <typename ClassName>
TEObjectFactory<ClassName>::~TEObjectFactory( void )
{
}

template <typename ClassName>
IEGenericObject* TEObjectFactory<ClassName>::createObject( void * objData )
{
    return static_cast<IEGenericObject *>(objData != nullptr ? DEBUG_NEW ClassName(objData) : DEBUG_NEW ClassName());
}

template <typename ClassName>
void TEObjectFactory<ClassName>::destroyObject( IEGenericObject * obj )
{
    if (obj != nullptr)
    {
        obj->destroy( );
    }
}

#endif  // AREG_BASE_IEGENERICOBJECT_HPP
