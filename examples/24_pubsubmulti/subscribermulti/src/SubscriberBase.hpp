#pragma once

/************************************************************************
 * \file        subscribermulti/src/SubscriberBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is a demonstration of PubSub feature service consumer.
 *              It dynamically subscribes and unsubscribes on data update.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generate/examples/24_pubsubmulti/PubSubClientBase.hpp"

#include "areg/base/NEMath.hpp"

/**
 * \brief   Base class to print subscriber state information.
 **/
class SubscriberBase    : public    PubSubClientBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The dependency entry.
     * \param   owner   The owning component.
     **/
    SubscriberBase(const NERegistry::DependencyEntry & entry, Component & owner, NEMath::sCoord coordInt, NEMath::sCoord coordStr);

    virtual ~SubscriberBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// PubSubClientBase Overrides
/************************************************************************/
    /**
     * \brief   Triggered, when StringOnChange attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes StringOnChange description:
     *          This simple string should send update notification only on value change.
     * \param   StringOnChange  The value of StringOnChange attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onStringOnChangeUpdate( const String & StringOnChange, NEService::eDataStateType state ) override;

    /**
     * \brief   Triggered, when IntegerAlways attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes IntegerAlways description:
     *          This simple integer should send update notification always when value is set.
     * \param   IntegerAlways   The value of IntegerAlways attribute.
     * \param   state           The data validation flag.
     **/
    virtual void onIntegerAlwaysUpdate( unsigned int IntegerAlways, NEService::eDataStateType state ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    const NEMath::sCoord    mCoordInteger;      //!< The coordinates to print integer value change information
    const NEMath::sCoord    mCoordString;       //!< The coordinates to print string value change information
    uint32_t                mOldInteger;        //!< Old integer value to compare when output on console.
    bool                    mOldState;          //!< Flag, indicating whether the old integer value is valid or not.
    String                  mOldString;         //!< Old string value to compare when output on console.
    unsigned int            mIntEventCount;     //!< The event count of the integer value change
    unsigned int            mStrEventCount;     //!< The event count of the string value change

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SubscriberBase(void);
    DECLARE_NOCOPY_NOMOVE(SubscriberBase);
};
