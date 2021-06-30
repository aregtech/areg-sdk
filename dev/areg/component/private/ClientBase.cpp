/************************************************************************
 * \file        areg/component/private/ClientBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client Base class implementation
 *
 ************************************************************************/
#include "areg/component/ClientBase.hpp"
#include "areg/component/NEService.hpp"

ClientBase::ClientBase( void )
{
    ; // do nothing
}

ClientBase::~ClientBase( void )
{
    ; // do nothing
}


void ClientBase::requestFailedNotImplemented( const char* clientName, unsigned int requestId ) const
{
    OUTPUT_ERR("[ %s ] client request failed function of request msgId [ %d ] is not implemented.", clientName, GET_REQ_INDEX(requestId));
    ASSERT(false);
}

void ClientBase::responseNotImplemented( const char* clientName, unsigned int responseId ) const
{
    OUTPUT_ERR("[ %s ] client response function of msgId [ %d ] is not implemented.", clientName, GET_RESP_INDEX(responseId));
    ASSERT(false);
}

void ClientBase::broadcastNotImplemented( const char* clientName, unsigned int broadcastId ) const
{
    OUTPUT_ERR("[ %s ] client broadcast subscription function of msgId [ %d ] is not implemented.", clientName, GET_RESP_INDEX(broadcastId));
    ASSERT(false);
}

void ClientBase::responseInvalidNotImpelemnted( const char* clientName, unsigned int responseId ) const
{
    OUTPUT_ERR("[ %s ] client response invalid function of response msgId [ %d ] is not implemented.", clientName, GET_RESP_INDEX(responseId));
    ASSERT(false);
}

void ClientBase::onUpdateNotImplemented( const char* clientName, unsigned int updateId ) const
{
    OUTPUT_ERR("[ %s ] client update function of msgId [ %d ] is not implemented.", clientName, GET_ATTR_INDEX(updateId));
    ASSERT(false);
}

#if 0
// #if defined(DEBUG) && defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_ERROR)
// #else

void ClientBase::requestFailedNotImplemented( const char* /*clientName*/, unsigned int /*requestId*/ ) const
{
}

void ClientBase::responseNotImplemented( const char* /*clientName*/, unsigned int /*responseId*/ ) const
{
}

void ClientBase::broadcastNotImplemented( const char* /*clientName*/, unsigned int /*broadcastId*/ ) const
{
}

void ClientBase::responseInvalidNotImpelemnted( const char* /*clientName*/, unsigned int /*responseId*/ ) const
{
}

void ClientBase::onUpdateNotImplemented( const char* /*clientName*/, unsigned int /*updateId*/ ) const
{
}

#endif
