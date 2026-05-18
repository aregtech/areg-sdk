/************************************************************************
 * \file            chatter/DistributedAppDefs.cpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "chatter/DistributedAppDefs.hpp"
#include "examples/20_winchat/services/DirectConnection.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX

#include <Windows.h>
#include <tchar.h>

static inline void _createRoleName( const areg::String & nickName, uint32_t cookie, const char * prefix, areg::String & output )
{
    if ( (nickName.is_empty( ) == false) && (cookie != DirectConnection::InvalidCookie) )
        output.format( "%s%s_%u", prefix, nickName.as_string( ), cookie );
}

static inline void _createRoleName( const areg::String & nickName, uint32_t cookie, uint64_t session, const char * prefix, areg::String & output )
{
    if ( (nickName.is_empty( ) == false) && (cookie != DirectConnection::InvalidCookie) && (session != DirectConnection::InvalidSession) )
        output.format( "%s%s_%u%llu", prefix, nickName.as_string( ), cookie, session );
}

areg::String NEDistributedApp::getConnectionServiceRole( const areg::String & nickName, uint32_t cookie )
{
    areg::String result;
    _createRoleName(nickName, cookie, DirectConnection::ServicePrefixProvider, result);
    return result;
}

areg::String NEDistributedApp::getDirectMessagingRole(const areg::String & nickName, uint32_t cookie, uint64_t session, bool isInitiator)
{
    areg::String result;
    _createRoleName(nickName, cookie, session, isInitiator ? PREFIX_INITIATOR : PREFIX_PARTICIPANT, result);
    return result;
}

areg::String NEDistributedApp::getDirectConnectionName( const areg::String & nickName, uint32_t cookie, uint64_t session )
{
    areg::String result;
    if ( (nickName.is_empty( ) == false) && (cookie != DirectConnection::InvalidCookie) )
    {
        if ( session != DirectConnection::InvalidSession )
            result.format( "%s_%u%llu", nickName.as_string( ), cookie, session );
        else
            result.format( "%s_%u", nickName.as_string( ), cookie);
    }
    return result;
}
