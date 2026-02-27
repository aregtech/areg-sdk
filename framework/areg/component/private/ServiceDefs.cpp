/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, classes of NEService namespace.
 *
 ************************************************************************/

#include "areg/component/ServiceDefs.hpp"

//////////////////////////////////////////////////////////////////////////
// class areg::StateArray implementation
//////////////////////////////////////////////////////////////////////////
areg::StateArray::StateArray(uint32_t count)
    : StateArrayBase(count)
    , mExternal     (false)
{
    ASSERT((count != 0) || (mValueList == nullptr));
    resetStates();
}

areg::StateArray::StateArray( uint8_t* thisBuffer, int32_t elemCount )
    : StateArrayBase( )
    , mExternal     (true)
{
    mValueList  = reinterpret_cast<areg::DataState *>(thisBuffer);
    mElemCount  = static_cast<uint32_t>(elemCount);
    resetStates();
}

areg::StateArray::~StateArray()
{
    if (mExternal)
    {
        mValueList = nullptr;
        mElemCount = 0;
    }
}

//////////////////////////////////////////////////////////////////////////
// class areg::ParameterArray implementation
//////////////////////////////////////////////////////////////////////////

areg::ParameterArray::ParameterArray( const areg::InterfaceData& ifData )
    : mElemCount (0)
    , mParamList(nullptr)
{
    construct(ifData.idResponseParamCountMap, static_cast<int32_t>(ifData.idResponseCount));
}

areg::ParameterArray::ParameterArray( const uint32_t* paramCountMap, int32_t count )
{
    construct(paramCountMap, count);
}

areg::ParameterArray::ParameterArray( areg::ParameterArray && src ) noexcept
    : mElemCount    ( std::move(src.mElemCount) )
    , mParamList    ( std::move(src.mParamList) )
{
    src.mElemCount = 0;
    src.mParamList = nullptr;
}

areg::ParameterArray::~ParameterArray()
{
    if (mParamList != nullptr)
    {
        delete[] reinterpret_cast <uint8_t *>(mParamList);
    }

    mParamList= nullptr;
    mElemCount = 0;
}

areg::ParameterArray & areg::ParameterArray::operator = ( areg::ParameterArray && src) noexcept
{
    if ( this != &src )
    {
        if ( mParamList != nullptr )
        {
            delete[] reinterpret_cast <uint8_t *>(mParamList);
        }

        mElemCount  = src.mElemCount;
        mParamList  = src.mParamList;
        src.mElemCount = 0;
        src.mParamList = nullptr;
    }

    return (*this);
}

void areg::ParameterArray::construct( const uint32_t * params, int32_t count )
{
    if ( (params != nullptr) && (count > 0) )
    {
        uint32_t single = static_cast<uint32_t>(sizeof(areg::StateArray *));
        // count pointers to state array
        uint32_t size   = static_cast<uint32_t>(count) * single;

        // how many bytes need to skip to start param Elements
        uint32_t skipList   = size;

        // reserve space for one "no param" element
        size += static_cast<uint32_t>( sizeof(areg::StateArray) );

        // here we start having parameter list.
        uint32_t skipBegin  = size;
        // space for parameters
        size += countParamSpace(params, count);

        uint8_t* buffer = DEBUG_NEW uint8_t[size];
        if (buffer != nullptr)
        {
            // set element count
            mElemCount = count;
            // array of pointers to Param objects start from beginning
            mParamList  = reinterpret_cast<areg::StateArray **>(buffer);

            // here is reserved "no param" element
            areg::StateArray* noParam = reinterpret_cast<areg::StateArray  *>(buffer + skipList);

            // here start actual params
            uint8_t* paramElem  = buffer + skipBegin;

            // initialize "no param" element
            new (noParam) areg::StateArray(0);

            // start initializing
            for ( int i = 0; i < mElemCount; ++ i )
            {
                // initially "no param" element
                areg::StateArray *param = noParam;
                if (params[i] != 0)
                {
                    // if parameter count is not zero
                    param = reinterpret_cast<areg::StateArray *>(paramElem);
                    // initialize by calling private construct, implemented for this case.
                    new (param) areg::StateArray(paramElem + sizeof(areg::StateArray), static_cast<int32_t>(params[i]));

                    // go to next elem
                    uint32_t next = static_cast<uint32_t>(sizeof(areg::StateArray) + params[i] * sizeof(areg::DataState));
                    paramElem += next;
                }

                // make sure that do not jump over the buffer
                ASSERT(paramElem <= buffer + size);
                mParamList[i] = param;
            }
        }
    }
}

uint32_t areg::ParameterArray::countParamSpace( const uint32_t* params, int32_t count )
{
    uint32_t result = 0;
    // space for size of class areg::StateArray + 
    // space for size of areg::DataState multiplied on number of parameters.
    // If number of parameters is zero, do not reserve.
    for ( int i = 0; i < count; ++ i )
        result += params[i] != 0 ? static_cast<uint32_t>(sizeof(areg::StateArray) + params[i] * sizeof(areg::DataState)) : 0;
    return result;
}

void areg::ParameterArray::resetParamState( uint32_t whichParam )
{
    ASSERT((static_cast<int32_t>(whichParam) >= 0) && (static_cast<int32_t>(whichParam) < mElemCount));
    mParamList[whichParam]->resetStates();
}

//////////////////////////////////////////////////////////////////////////
// class areg::ProxyData implementation
//////////////////////////////////////////////////////////////////////////
areg::ProxyData::ProxyData( const areg::InterfaceData& ifData )
    : mImplVersion  (areg::DataState::DataIsUnavailable)
    , mIfData       (ifData)
    , mAttrState    (static_cast<uint32_t>(ifData.idAttributeCount))
    , mParamState   (ifData)
{
    resetStates(); 
}

void areg::ProxyData::resetStates()
{
    mImplVersion    = areg::DataState::DataIsUnavailable;
    mAttrState.resetStates();
    mParamState.resetAllStates();
}

void areg::ProxyData::setDataState( uint32_t msgId, areg::DataState newState )
{
    if ( areg::isAttributeId(msgId) )
    {
        if ( areg::isVersionNotifyId(msgId) )
        {
            mImplVersion    = newState;
        }
        else
        {
            mAttrState[areg::attrIndex(msgId)]   = newState;
        }
    }
    else if ( areg::isResponseId(msgId) )
    {
        mParamState.setParamState(areg::respIndex(msgId), newState);
    }
    // else ignore
}

areg::DataState areg::ProxyData::getDataState( uint32_t msgId ) const
{
    areg::DataState result = areg::DataState::DataUnexpectedError;
    if (areg::isAttributeId(msgId))
        result = getAttributeState(msgId);
    else if (areg::isResponseId(msgId))
        result = getParamState(msgId);
    // else, ignore

    return result;
}

uint32_t areg::ProxyData::getResponseId( uint32_t requestId ) const
{
    uint32_t index = areg::reqIndex(requestId);
    return  (
                (static_cast<int32_t>(index) >= 0) && (index < mIfData.idRequestCount) ? 
                        static_cast<uint32_t>(mIfData.idRequestToResponseMap[index]) :
                        areg::INVALID_MESSAGE_ID
            );
}

AREG_API_IMPL const areg::Version areg::EmptyServiceVersion (1, 0, 0);

AREG_API_IMPL areg::InterfaceData & areg::getEmptyInterface()
{
    /**
     * \brief   System Service Interface data
     **/
    static areg::InterfaceData _InterfaceData = 
    {
          areg::EmptyServiceName
        , areg::EmptyServiceVersion
        , areg::ServiceType::Public
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
