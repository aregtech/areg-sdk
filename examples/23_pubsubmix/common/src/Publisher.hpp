#ifndef PUBSUBMIX_COMMON_PUBLISHER_HPP
#define PUBSUBMIX_COMMON_PUBLISHER_HPP

/************************************************************************
 * \file        common/src/Publisher.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is demonstration of data PubSub (Publish / Subscribe) features.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/IETimerConsumer.hpp"
#include "examples/23_pubsubmix/services/PubSubMixStub.hpp"

#include "areg/component/Timer.hpp"
#include "aregextend/console/OptionParser.hpp"

/**
 * \brief   This service provider contains attributes (data) that send notifications on update.
 *          There are 2 types of attributes (data)
 *              1.  Update only when value is changed.
 *              2.  Update always when value is set.
 *          The service consumers that connect to the service provider should be able
 *          automatically receive update notifications:
 *              -   When first time subscribe on data update to get the actual value.
 *              -   When data is updated, depending on the update type (always or only on update).
 *              -   When data is invalidated.
 *          All updates must happen automatic, service providers must not receive unnecessary update notifications.
 *          There are 2 types of data updated: on change and always.
 *              - On change: this means to receive update notifications
 *                           only if the value is changed. For example,
 *                           the string should be received only when value changed.
 *              - Always   : this means to receive update notification each
 *                           the value is set even if the value is not updated.
 **/
class Publisher : protected PubSubMixStub
                , private   IETimerConsumer
{
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates the component object.
     * \param   owner   The service owning component.
     **/
    Publisher( Component & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~Publisher( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    //!< Starts updating values.
    void start(void);

    //!< Pauses and stop updating values.
    void stop(void);

    //!< Invalidates the values, on the next start the value should be reset and validated.
    void invalidate(void);

    //!< Quits the service provider application.
    void quit(void);

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

/************************************************************************/
// StubBase overrides.
/************************************************************************/

    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client  The address of proxy client, which connection status is changed.
     * \param   status  The service consumer connection status.
     * \return  Returns true if connected service consumer is relevant to the provider.
     **/
    virtual bool clientConnected(const ProxyAddress & client, NEService::eServiceConnection status) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    //! Wrapper of the this pointer
    inline Publisher & self(void);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    Timer       mTimerOnChange;     //!< The timer to update values with feature to notify on update
    Timer       mTimerAlways;       //!< The timer to update values with feature to notify always
    int32_t     mClientCount;       //!< The number of connected clients.
    uint32_t    mSeqString;         //!< The sequence number of the string.
    uint16_t    mCountString;       //!< The count number of the string to change.
    uint32_t    mSeqInteger;        //!< The sequence number of the integer.
    uint16_t    mCountInteger;      //!< The count number of the integer to change.
    Mutex       mLock;              //!< Synchronization object for multithreading environment.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    Publisher(void) = delete;
    DECLARE_NOCOPY_NOMOVE(Publisher);
};

#endif // PUBLISHER_SRC_PUBLISHER_HPP
