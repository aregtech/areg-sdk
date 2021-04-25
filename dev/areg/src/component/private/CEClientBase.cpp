/************************************************************************
 * \file        areg/src/component/private/CEClientBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Client Base class implementation
 *
 ************************************************************************/
#include "areg/src/component/CEClientBase.hpp"
#include "areg/src/component/NEService.hpp"

CEClientBase::CEClientBase( void )
{
    ; // do nothing
}

CEClientBase::~CEClientBase( void )
{
    ; // do nothing
}

#if defined(DEBUG) && defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_ERROR)

void CEClientBase::RequestFailedNotImplemented( const char* clientName, unsigned int requestId ) const
{
    OUTPUT_ERR("[ %s ] client request failed function of request msgId [ %d ] is not implemented.", clientName, GET_REQ_INDEX(requestId));
    ASSERT(false);
}

void CEClientBase::ResponseNotImplemented( const char* clientName, unsigned int responseId ) const
{
    OUTPUT_ERR("[ %s ] client response function of msgId [ %d ] is not implemented.", clientName, GET_RESP_INDEX(responseId));
    ASSERT(false);
}

void CEClientBase::BroadcastNotImplemented( const char* clientName, unsigned int broadcastId ) const
{
    OUTPUT_ERR("[ %s ] client broadcast subscription function of msgId [ %d ] is not implemented.", clientName, GET_RESP_INDEX(broadcastId));
    ASSERT(false);
}

void CEClientBase::ResponseInvalidNotImpelemnted( const char* clientName, unsigned int responseId ) const
{
    OUTPUT_ERR("[ %s ] client response invalid function of response msgId [ %d ] is not implemented.", clientName, GET_RESP_INDEX(responseId));
    ASSERT(false);
}

void CEClientBase::OnUpdateNotImplemented( const char* clientName, unsigned int updateId ) const
{
    OUTPUT_ERR("[ %s ] client update function of msgId [ %d ] is not implemented.", clientName, GET_ATTR_INDEX(updateId));
    ASSERT(false);
}

#else

void CEClientBase::RequestFailedNotImplemented( const char* /*clientName*/, unsigned int /*requestId*/ ) const
{
}

void CEClientBase::ResponseNotImplemented( const char* /*clientName*/, unsigned int /*responseId*/ ) const
{
}

void CEClientBase::BroadcastNotImplemented( const char* /*clientName*/, unsigned int /*broadcastId*/ ) const
{
}

void CEClientBase::ResponseInvalidNotImpelemnted( const char* /*clientName*/, unsigned int /*responseId*/ ) const
{
}

void CEClientBase::OnUpdateNotImplemented( const char* /*clientName*/, unsigned int /*updateId*/ ) const
{
}

#endif
