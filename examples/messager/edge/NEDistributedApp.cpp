/************************************************************************
 * \file            edge/NEDistributedApp.cpp
 * \brief           Configurations, settings, etc..
 ************************************************************************/

#include "edge/NEDistributedApp.hpp"
#include "shared/NECommonSettings.hpp"
#include "shared/generated/NEDirectConnection.hpp"

#pragma comment(lib, "areg.lib")

static inline void _createRoleName( const CEString & nickName, uint32_t cookie, const char * prefix, CEString & output )
{
    if ( (nickName.IsEmpty( ) == false) && (cookie != NEDirectConnection::InvalidCookie) )
        output.Format( "%s%s_%u", prefix, nickName.GetBuffer( ), cookie );
}

static inline void _createRoleName( const CEString & nickName, uint32_t cookie, uint64_t session, const char * prefix, CEString & output )
{
    if ( (nickName.IsEmpty( ) == false) && (cookie != NEDirectConnection::InvalidCookie) && (session != NEDirectConnection::InvalidSession) )
        output.Format( "%s%s_%u%llu", prefix, nickName.GetBuffer( ), cookie, session );
}

CEString NEDistributedApp::getConnectionServiceRole( const CEString & nickName, uint32_t cookie )
{
    CEString result;
    _createRoleName(nickName, cookie, NEDirectConnection::ServicePrefixStub, result);
    return result;
}

CEString NEDistributedApp::getDirectMessagingRole(const CEString & nickName, uint32_t cookie, uint64_t session, bool isInitiator)
{
    CEString result;
    _createRoleName(nickName, cookie, session, isInitiator ? PREFIX_INITIATOR : PREFIX_PARTICIPANT, result);
    return result;
}

/***
CEString NEDistributedApp::getConnectionServiceClientRole( const CEString & nickName, uint32_t cookie, uint64_t session )
{
    CEString result;
    _createRoleName( nickName, cookie, session, NEDirectConnection::ServicePrefixClient, result );
    return result;
}
***/

CEString NEDistributedApp::getDirectConnectionName( const CEString & nickName, uint32_t cookie, uint64_t session )
{
    CEString result;
    if ( (nickName.IsEmpty( ) == false) && (cookie != NEDirectConnection::InvalidCookie) )
    {
        if ( session != NEDirectConnection::InvalidSession )
            result.Format( "%s_%u%llu", nickName.String( ), cookie, session );
        else
            result.Format( "%s_%u", nickName.String( ), cookie);
    }
    return result;
}
