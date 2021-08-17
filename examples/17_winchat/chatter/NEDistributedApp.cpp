/************************************************************************
 * \file            chatter/NEDistributedApp.cpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "chatter/NEDistributedApp.hpp"
#include "generated/NECommon.hpp"
#include "generated/NEDirectConnection.hpp"

static inline void _createRoleName( const String & nickName, uint32_t cookie, const char * prefix, String & output )
{
    if ( (nickName.isEmpty( ) == false) && (cookie != NEDirectConnection::InvalidCookie) )
        output.formatString( "%s%s_%u", prefix, nickName.getBuffer( ), cookie );
}

static inline void _createRoleName( const String & nickName, uint32_t cookie, uint64_t session, const char * prefix, String & output )
{
    if ( (nickName.isEmpty( ) == false) && (cookie != NEDirectConnection::InvalidCookie) && (session != NEDirectConnection::InvalidSession) )
        output.formatString( "%s%s_%u%llu", prefix, nickName.getBuffer( ), cookie, session );
}

String NEDistributedApp::getConnectionServiceRole( const String & nickName, uint32_t cookie )
{
    String result;
    _createRoleName(nickName, cookie, NEDirectConnection::ServicePrefixStub, result);
    return result;
}

String NEDistributedApp::getDirectMessagingRole(const String & nickName, uint32_t cookie, uint64_t session, bool isInitiator)
{
    String result;
    _createRoleName(nickName, cookie, session, isInitiator ? PREFIX_INITIATOR : PREFIX_PARTICIPANT, result);
    return result;
}

/***
String NEDistributedApp::getConnectionServiceClientRole( const String & nickName, uint32_t cookie, uint64_t session )
{
    String result;
    _createRoleName( nickName, cookie, session, NEDirectConnection::ServicePrefixClient, result );
    return result;
}
***/

String NEDistributedApp::getDirectConnectionName( const String & nickName, uint32_t cookie, uint64_t session )
{
    String result;
    if ( (nickName.isEmpty( ) == false) && (cookie != NEDirectConnection::InvalidCookie) )
    {
        if ( session != NEDirectConnection::InvalidSession )
            result.formatString( "%s_%u%llu", nickName.getString( ), cookie, session );
        else
            result.formatString( "%s_%u", nickName.getString( ), cookie);
    }
    return result;
}
