#ifndef AREG_SERVICE_CONNECTIVITY_IESERVICEMESSAGEDISPATCHER_HPP
#define AREG_SERVICE_CONNECTIVITY_IESERVICEMESSAGEDISPATCHER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        service/connectivity/IEServiceMessageDispatcher.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of service message dispatcher.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

  /************************************************************************
   * Dependencies
   ************************************************************************/
class RemoteMessage;

//////////////////////////////////////////////////////////////////////////
// IEServiceMessageDispatcher interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The interface of remote communication message dispatcher for the
 *          services. Should be separately implemented for each service.
 **/
class IEServiceMessageDispatcher
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    IEServiceMessageDispatcher( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~IEServiceMessageDispatcher( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEServiceMessageDispatcher interface overrides
/************************************************************************/
    /**
     * \brief   Called when received a communication message to dispatch and process.
     * \param   msgReceived     The received the communication message.
     **/
    virtual void onServiceMessageReceived(const RemoteMessage& msgReceived) = 0;

    /**
     * \brief   Called when need to send a communication message.
     * \param   msgReceived     The received the communication message.
     **/
    virtual void onServiceMessageSend(const RemoteMessage& msgSend) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(IEServiceMessageDispatcher);
};

#endif // AREG_SERVICE_CONNECTIVITY_IESERVICEMESSAGEDISPATCHER_HPP
