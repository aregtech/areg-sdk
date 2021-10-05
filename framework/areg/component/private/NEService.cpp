/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/NEService.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, classes of NEService namespace.
 *
 ************************************************************************/

#include "areg/component/NEService.hpp"

//////////////////////////////////////////////////////////////////////////
// class NEService::StateArray implementation
//////////////////////////////////////////////////////////////////////////
NEService::StateArray::StateArray( int size /*= 0*/ )
    : StateArrayBase(size)
{
    resetStates();
}

NEService::StateArray::StateArray( unsigned char* thisBuffer, int elemCount )
    : StateArrayBase( )
{
    this->mValueList    = reinterpret_cast<NEService::eDataStateType *>(thisBuffer + sizeof(NEService::StateArray));
    this->mElemCount    = elemCount;
    resetStates();
}

//////////////////////////////////////////////////////////////////////////
// class NEService::ParameterArray implementation
//////////////////////////////////////////////////////////////////////////

NEService::ParameterArray::ParameterArray( const NEService::SInterfaceData& ifData )
    : mElemCount (0)
    , mParamList(nullptr)
{
    construct(ifData.idResponseParamCountMap, static_cast<int>(ifData.idResponseCount));
}

NEService::ParameterArray::ParameterArray( const unsigned int* paramCountMap, int count )
{
    construct(paramCountMap, count);
}

NEService::ParameterArray::ParameterArray( NEService::ParameterArray && src ) noexcept
    : mElemCount    ( std::move(src.mElemCount) )
    , mParamList    ( std::move(src.mParamList) )
{
    src.mElemCount = 0;
    src.mParamList = nullptr;
}

NEService::ParameterArray::~ParameterArray( void )
{
    if (mParamList != nullptr)
    {
        delete[] reinterpret_cast <unsigned char *>(mParamList);
    }

    mParamList= nullptr;
    mElemCount = 0;
}

NEService::ParameterArray & NEService::ParameterArray::operator = ( NEService::ParameterArray && src) noexcept
{
    if ( this != &src )
    {
        if ( mParamList != nullptr )
        {
            delete[] reinterpret_cast <unsigned char *>(mParamList);
        }

        mElemCount  = src.mElemCount;
        mParamList  = src.mParamList;
        src.mElemCount = 0;
        src.mParamList = nullptr;
    }

    return (*this);
}

void NEService::ParameterArray::construct( const unsigned int * params, int count )
{
    if ( (params != nullptr) && (count > 0) )
    {
        unsigned int single		= static_cast<unsigned int>(sizeof(NEService::StateArray *));
        // count pointers to state array
        unsigned int size       = count * single;

        // how many bytes need to skip to start param Elements
        unsigned int skipList   = size;

        // reserve space for one "no param" element
        size += static_cast<unsigned int>( sizeof(NEService::StateArray) );

        // here we start having parameter list.
        unsigned int skipBegin  = size;
        // space for parameters
        size += countParamSpace(params, count);

        unsigned char* buffer = DEBUG_NEW unsigned char[size];
        if (buffer != nullptr)
        {
            // set element count
            mElemCount = count;
            // array of pointers to Param objects start from beginning
            mParamList  = reinterpret_cast<NEService::StateArray **>(buffer);

            // here is reserved "no param" element
            NEService::StateArray*  noParam      = reinterpret_cast<NEService::StateArray  *>(buffer + skipList);

            // here start actual params
            unsigned char* paramElem                = buffer + skipBegin;

            // initialize "no param" element
            new (noParam) NEService::StateArray();

            // start initializing
            for ( int i = 0; i < mElemCount; ++ i )
            {
                // initially "no param" element
                NEService::StateArray *param = noParam;
                if (params[i] != 0)
                {
                    // if parameter count is not zero
                    param = reinterpret_cast<NEService::StateArray *>(paramElem);
                    // initialize by calling private construct, implemented for this case.
                    new (param) NEService::StateArray(paramElem, static_cast<int>(params[i]));

                    // go to next elem
                    unsigned int next = static_cast<unsigned int>(sizeof(NEService::StateArray) + params[i] * sizeof(NEService::eDataStateType));
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

unsigned int NEService::ParameterArray::countParamSpace( const unsigned int* params, int count )
{
    unsigned int result = 0;
    // space for size of class NEService::StateArray + 
    // space for size of NEService::eDataStateType multiplied on number of parameters.
    // If number of parameters is zero, do not reserve.
    for ( int i = 0; i < count; ++ i )
        result += params[i] != 0 ? static_cast<unsigned int>(sizeof(NEService::StateArray) + params[i] * sizeof(NEService::eDataStateType)) : 0;
    return result;
}

void NEService::ParameterArray::resetParamState( int whichParam )
{
    ASSERT(whichParam >= 0 && whichParam < mElemCount);
    mParamList[whichParam]->resetStates();
}

//////////////////////////////////////////////////////////////////////////
// class NEService::ProxyData implementation
//////////////////////////////////////////////////////////////////////////
NEService::ProxyData::ProxyData( const NEService::SInterfaceData& ifData )
    : mImplVersion  (NEService::eDataStateType::DataIsUnavailable)
    , mIfData       (ifData)
    , mAttrState    (static_cast<int>(ifData.idAttributeCount))
    , mParamState   (ifData)
{
    resetStates(); 
}

NEService::ProxyData::~ProxyData( void )
{
    ; // do noting
}

void NEService::ProxyData::resetStates( void )
{
    mImplVersion    = NEService::eDataStateType::DataIsUnavailable;
    mAttrState.resetStates();
    mParamState.resetAllStates();
}

void NEService::ProxyData::setDataState( unsigned int msgId, NEService::eDataStateType newState )
{
    if ( NEService::isAttributeId(msgId) )
    {
        if ( NEService::isVersionNotifyId(msgId) )
        {
            mImplVersion    = newState;
        }
        else
        {
            mAttrState[GET_ATTR_INDEX(msgId)]   = newState;
        }
    }
    else if ( NEService::isResponseId(msgId) )
    {
        mParamState.setParamState(GET_RESP_INDEX(msgId), newState);
    }
    // else ignore
}

NEService::eDataStateType NEService::ProxyData::getDataState( unsigned int msgId ) const
{
    NEService::eDataStateType result = NEService::eDataStateType::DataUnexpectedError;
    if (NEService::isAttributeId(msgId))
        result = getAttributeState(msgId);
    else if (NEService::isResponseId(msgId))
        result = getParamState(msgId);
    // else, ignore

    return result;
}

unsigned int NEService::ProxyData::getResponseId( unsigned int requestId ) const
{
    int index = GET_REQ_INDEX(requestId);
    return  (
                index >= 0 && index < static_cast<int>(mIfData.idRequestCount) ? 
                        static_cast<unsigned int>(mIfData.idRequestToResponseMap[index]) :
                        NEService::INVALID_MESSAGE_ID
            );
}

AREG_API const Version NEService::EmptyServiceVersion (1, 0, 0);

AREG_API NEService::SInterfaceData & NEService::getEmptyInterface(void)
{
    /**
     * \brief   System Service Interface data
     **/
    static NEService::SInterfaceData _InterfaceData = 
    {
          NEService::EmptyServiceName
        , NEService::EmptyServiceVersion
        , NEService::eServiceType::ServicePublic
        , 0
        , 0
        , 0
        , nullptr
        , nullptr
        , nullptr
        , nullptr
        , nullptr
    };

    return _InterfaceData;
}
