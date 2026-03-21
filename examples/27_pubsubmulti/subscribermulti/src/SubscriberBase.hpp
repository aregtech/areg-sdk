#pragma once

/************************************************************************
 * \file        subscribermulti/src/SubscriberBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is a demonstration of PubSub feature service consumer.
 *              It dynamically subscribes and unsubscribes on data update.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"
#include "examples/27_pubsubmulti/services/PubSubConsumerBase.hpp"

#include "areg/base/MathDefs.hpp"

/**
 * \brief   Base class to print subscriber state information.
 **/
class SubscriberBase    : public    PubSubConsumerBase
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
    SubscriberBase(const areg::DependencyEntry & entry, areg::Component & owner, areg::Coord coordInt, areg::Coord coordStr);

    virtual ~SubscriberBase() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// PubSubConsumerBase Overrides
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
    void on_string_on_change_update( const areg::String & StringOnChange, areg::DataState state ) override;

    /**
     * \brief   Triggered, when IntegerAlways attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes IntegerAlways description:
     *          This simple integer should send update notification always when value is set.
     * \param   IntegerAlways   The value of IntegerAlways attribute.
     * \param   state           The data validation flag.
     **/
    void on_integer_always_update( uint32_t IntegerAlways, areg::DataState state ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    const areg::Coord    mCoordInteger;      //!< The coordinates to print integer value change information
    const areg::Coord    mCoordString;       //!< The coordinates to print string value change information
    uint32_t                mOldInteger;        //!< Old integer value to compare when output on console.
    bool                    mOldState;          //!< Flag, indicating whether the old integer value is valid or not.
    areg::String                  mOldString;         //!< Old string value to compare when output on console.
    uint32_t            mIntEventCount;     //!< The event count of the integer value change
    uint32_t            mStrEventCount;     //!< The event count of the string value change

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SubscriberBase();
    AREG_NOCOPY_NOMOVE(SubscriberBase);
};
