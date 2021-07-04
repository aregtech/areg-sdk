/************************************************************************
 * \file        areg/component/private/ClientBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client Base class implementation
 *
 ************************************************************************/
#include "areg/component/ClientBase.hpp"
#include "areg/component/NEService.hpp"

const char * const  ClientBase::UNKNOWN_NAME    = "<Unknown>";

ClientBase::ClientBase( const char * clientName )
    : mClientName   ( NEString::isEmpty<char>(clientName) ? ClientBase::UNKNOWN_NAME : clientName )
{
}

ClientBase::~ClientBase( void )
{
}

#if defined(DEBUG) && defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_ERROR)

void ClientBase::requestFailedNotImplemented( unsigned int requestId ) const
{
    OUTPUT_ERR("[ %s ] client request failed function of request msgId [ %d ] is not implemented.", mClientName.getString(), GET_REQ_INDEX(requestId));
    ASSERT(false);
}

void ClientBase::responseNotImplemented( unsigned int responseId ) const
{
    OUTPUT_ERR("[ %s ] client response function of msgId [ %d ] is not implemented.", mClientName.getString(), GET_RESP_INDEX(responseId));
    ASSERT(false);
}

void ClientBase::broadcastNotImplemented( unsigned int broadcastId ) const
{
    OUTPUT_ERR("[ %s ] client broadcast subscription function of msgId [ %d ] is not implemented.", mClientName.getString(), GET_RESP_INDEX(broadcastId));
    ASSERT(false);
}

void ClientBase::responseInvalidNotImpelemnted( unsigned int responseId ) const
{
    OUTPUT_ERR("[ %s ] client response invalid function of response msgId [ %d ] is not implemented.", mClientName.getString(), GET_RESP_INDEX(responseId));
    ASSERT(false);
}

void ClientBase::onUpdateNotImplemented( unsigned int updateId ) const
{
    OUTPUT_ERR("[ %s ] client update function of msgId [ %d ] is not implemented.", mClientName.getString(), GET_ATTR_INDEX(updateId));
    ASSERT(false);
}

#else

void ClientBase::requestFailedNotImplemented( unsigned int /*requestId*/ ) const
{
}

void ClientBase::responseNotImplemented( unsigned int /*responseId*/ ) const
{
}

void ClientBase::broadcastNotImplemented( unsigned int /*broadcastId*/ ) const
{
}

void ClientBase::responseInvalidNotImpelemnted( unsigned int /*responseId*/ ) const
{
}

void ClientBase::onUpdateNotImplemented( unsigned int /*updateId*/ ) const
{
}

#endif
