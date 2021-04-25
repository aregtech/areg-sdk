#ifndef AREG_COMPONENT_CECLIENTBASE_HPP
#define AREG_COMPONENT_CECLIENTBASE_HPP
/************************************************************************
 * \file        areg/src/component/CEClientBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client Base class.
 *              Base class for all client objects. In case of appropriate
 *              function call failure, it will generate error message and
 *              trigger assertion.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

//////////////////////////////////////////////////////////////////////////
// CEClientBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Base class for all clients.
 * 
 * \details     The purpose of this class is to generate error messages
 *              and trigger assertions (valid for Debug version) in case
 *              either one of responses and updates is not implemented or
 *              appropriate error handling function is not overwritten.
 *
 **/
class AREG_API CEClientBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected members.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor. Protected, should be instantiated by child classes.
     **/
    CEClientBase( void );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEClientBase( void );

//////////////////////////////////////////////////////////////////////////
// Operations. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	This function is triggered in child class if it has no
     *          implementation of certain request call failure.
     * \param	clientName	The name of client, which triggered function call.
     * \param	requestId	The specific function request ID that cause failure
     **/
    void RequestFailedNotImplemented( const char * clientName, unsigned int requestId ) const;

    /**
     * \brief	This function is triggered in child class if it has no
     *          implementation of certain response
     * \param	clientName	The name of client, which triggered function call.
     * \param	responseId	The specific response ID that has no implementation.
     **/
    void ResponseNotImplemented(const char * clientName, unsigned int responseId ) const;

    /**
     * \brief	This function is triggered in child class if it has no
     *          implementation of certain broadcast method of service interface
     * \param	clientName	The name of client, which triggered function call.
     * \param	broadcastId The specific response ID that has no implementation.
     **/
    void BroadcastNotImplemented(const char * clientName, unsigned int broadcastId) const;

    /**
     * \brief	This function is triggered in child class if response was not
     *          handled, i.e. could not find response function.
     * \param	clientName	The name of client, which triggered function call.
     * \param	responseId	The specific response ID, which was not handled in client.
     **/
    void ResponseInvalidNotImpelemnted(const char * clientName, unsigned int responseId) const;

    /**
     * \brief	This function is triggered in child class if client assigned
     *          listener on certain attribute update, but has no on update 
     *          function implementation, i.e. the update ID was not handled.
     * \param	clientName	The name of client, which triggered function call.
     * \param	updateId	The specific ID of update message, which was not handled.
     **/
    void OnUpdateNotImplemented(const char * clientName, unsigned int updateId) const;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    CEClientBase( const CEClientBase & /*src*/ );
    const CEClientBase& operator = ( const CEClientBase & /*src*/ );
};

#endif  // AREG_COMPONENT_CECLIENTBASE_HPP
