/************************************************************************
 * \file        areg/component/private/NEService.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, classes of NEService namespace.
 *
 ************************************************************************/

#include "areg/component/NEService.hpp"

//////////////////////////////////////////////////////////////////////////
// class NEService::CEStateArray implementation
//////////////////////////////////////////////////////////////////////////
NEService::CEStateArray::CEStateArray( int size /*= 0*/ )
    : TEFixedArray<NEService::eDataStateType, NEService::eDataStateType>(size)
{
    ResetStates();
}

NEService::CEStateArray::CEStateArray( const CEStateArray& src )
    : TEFixedArray<NEService::eDataStateType, NEService::eDataStateType>(static_cast<const TEFixedArray<NEService::eDataStateType, NEService::eDataStateType> &>(src))
{
    ; // do nothing
}

NEService::CEStateArray::CEStateArray( unsigned char* thisBuffer, int elemCount )
    : TEFixedArray<NEService::eDataStateType, NEService::eDataStateType>()
{
    this->mValueList    = reinterpret_cast<NEService::eDataStateType *>(thisBuffer + sizeof(NEService::CEStateArray));
    this->mElemCount    = elemCount;
    ResetStates();
}

NEService::CEStateArray::~CEStateArray( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// class NEService::CEParameterArray implementation
//////////////////////////////////////////////////////////////////////////
NEService::CEParameterArray::CEParameterArray( const NEService::SInterfaceData& ifData )
    : mElemCount (0)
    , mParamList(NULL)
{
    Construct(ifData.idResponseParamCountMap, ifData.idResponseCount);
}

NEService::CEParameterArray::CEParameterArray( const unsigned int* paramCountMap, int count )
{
    Construct(paramCountMap, count);
}

NEService::CEParameterArray::~CEParameterArray( void )
{
    if (mParamList != NULL)
        delete [] reinterpret_cast <unsigned char *>(mParamList);
    mParamList= NULL;
    mElemCount = 0;
}

void NEService::CEParameterArray::Construct( const unsigned int* params, int count )
{
    if ( (params != static_cast<const unsigned int *>(NULL)) && (count > 0) )
    {
        // count pointers to state array
        unsigned int size       = count * sizeof(NEService::CEStateArray *);

        // how many bytes need to skip to start param Elements
        unsigned int skipList   = size;

        // reserve space for one "no param" element
        size += sizeof(NEService::CEStateArray);

        // here we start having parameter list.
        unsigned int skipBegin  = size;
        // space for parameters
        size += CountParamSpace(params, count);

        unsigned char* buffer = DEBUG_NEW unsigned char[size];
        if (buffer != NULL)
        {
            // set element count
            mElemCount = count;
            // array of pointers to Param objects start from beginning
            mParamList  = reinterpret_cast<NEService::CEStateArray **>(buffer);

            // here is reserved "no param" element
            NEService::CEStateArray*  noParam      = reinterpret_cast<NEService::CEStateArray  *>(buffer + skipList);

            // here start actual params
            unsigned char* paramElem                = buffer + skipBegin;

            // initialize "no param" element
            new (noParam) NEService::CEStateArray();

            // start initializing
            for ( int i = 0; i < mElemCount; ++ i )
            {
                // initially "no param" element
                NEService::CEStateArray *param = noParam;
                if (params[i] != 0)
                {
                    // if parameter count is not zero
                    param = reinterpret_cast<NEService::CEStateArray *>(paramElem);
                    // initialize by calling private construct, implemented for this case.
                    new (param) NEService::CEStateArray(paramElem, params[i]);

                    // go to next elem
                    unsigned int next = sizeof(NEService::CEStateArray) + params[i] * sizeof(NEService::eDataStateType);
                    paramElem += next;
                }
                else
                {
                    ;  // no param, do nothing
                }
                // make sure that do not jump over the buffer
                ASSERT(paramElem <= buffer + size);
                mParamList[i] = param;
            }
        }
    }
}

unsigned int NEService::CEParameterArray::CountParamSpace( const unsigned int* params, int count )
{
    unsigned int result = 0;
    // space for size of class NEService::CEStateArray + 
    // space for size of NEService::eDataStateType multiplied on number of parameters.
    // If number of parameters is zero, do not reserve.
    for ( int i = 0; i < count; ++ i )
        result += params[i] != 0 ? (sizeof(NEService::CEStateArray) + params[i] * sizeof(NEService::eDataStateType)) : 0;
    return result;
}

void NEService::CEParameterArray::ResetParamState( int whichParam )
{
    ASSERT(whichParam >= 0 && whichParam < mElemCount);
    mParamList[whichParam]->ResetStates();
}

//////////////////////////////////////////////////////////////////////////
// class NEService::CEProxyData implementation
//////////////////////////////////////////////////////////////////////////
NEService::CEProxyData::CEProxyData( const NEService::SInterfaceData& ifData )
    : mImplVersion  (NEService::DATA_UNAVAILABLE)
    , mIfData       (ifData)
    , mAttrState    (ifData.idAttributeCount)
    , mParamState   (ifData)
{
    ResetStates(); 
}

NEService::CEProxyData::~CEProxyData( void )
{
    ; // do noting
}

void NEService::CEProxyData::ResetStates( void )
{
    mImplVersion    = NEService::DATA_UNAVAILABLE;
    mAttrState.ResetStates();
    mParamState.ResetAllStates();
}

void NEService::CEProxyData::SetDataState( unsigned int msgId, NEService::eDataStateType newState )
{
    if ( NEService::IsAttributeId(msgId) )
    {
        if ( NEService::IsVersionNotifyId(msgId) )
        {
            mImplVersion    = newState;
        }
        else
        {
            mAttrState[GET_ATTR_INDEX(msgId)]   = newState;
        }
    }
    else if ( NEService::IsResponseId(msgId) )
    {
        mParamState.SetParamState(GET_RESP_INDEX(msgId), newState);
    }
    // else ignore
}

NEService::eDataStateType NEService::CEProxyData::GetDataState( unsigned int msgId ) const
{
    NEService::eDataStateType result = NEService::DATA_UNEXPECTED_ERROR;
    if (NEService::IsAttributeId(msgId))
        result = GetAttributeState(msgId);
    else if (NEService::IsResponseId(msgId))
        result = GetParamState(msgId);
    // else, ignore

    return result;
}

unsigned int NEService::CEProxyData::GetResponseId( unsigned int requestId ) const
{
    int index = GET_REQ_INDEX(requestId);
    return  (
                index >= 0 && index < static_cast<int>(mIfData.idRequestCount) ? 
                        static_cast<unsigned int>(mIfData.idRequestToResponseMap[index]) :
                        NEService::INVALID_MESSAGE_ID
            );
}

NEService::S_InterfaceData::S_InterfaceData( void )
    : idIsInitialized           ( false )
    , idServiceName             ( NULL  )
    , idVersion                 ( 0, 0,0)
    , idServiceType             ( NEService::ServiceInvalid )
    , idRequestCount            ( 0     )
    , idResponseCount           ( 0     )
    , idAttributeCount          ( 0     )
    , idRequestToResponseMap    ( NULL  )
    , idResponseParamCountMap   ( NULL  )
{
}
