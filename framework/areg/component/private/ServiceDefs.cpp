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
 * \brief       Areg Platform, classes of areg namespace.
 *
 ************************************************************************/

#include "areg/component/ServiceDefs.hpp"
#include "areg/component/private/ServiceManager.hpp"

//////////////////////////////////////////////////////////////////////////
// class areg::StateArray implementation
//////////////////////////////////////////////////////////////////////////
areg::StateArray::StateArray(uint32_t count)
    : StateArrayBase(count)
    , mExternal     (false)
{
    ASSERT((count != 0) || (mValueList == nullptr));
    reset();
}

areg::StateArray::StateArray( uint8_t* thisBuffer, int32_t elemCount )
    : StateArrayBase( )
    , mExternal     (true)
{
    mValueList  = reinterpret_cast<areg::DataState *>(thisBuffer);
    mElemCount  = static_cast<uint32_t>(elemCount);
    reset();
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
    construct(ifData.idResponseParamCountMap, ifData.idResponseCount);
}

areg::ParameterArray::ParameterArray( const uint32_t* paramCountMap, int32_t count )
{
    construct(paramCountMap, static_cast<uint32_t>(count));
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

void areg::ParameterArray::construct( const uint32_t * params, uint32_t count ) noexcept
{
    if ((params == nullptr) || (count == 0))
        return;

    
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
    size += count_param_space(params, count);

    uint8_t* buffer = DEBUG_NEW uint8_t[size];
    if (buffer == nullptr)
        return;
    
    // set element count
    mElemCount = static_cast<int32_t>(count);
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

//////////////////////////////////////////////////////////////////////////
// class areg::ProxyData implementation
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL const areg::InterfaceData & areg::empty_interface() noexcept
{
    /**
     * \brief   System Service Interface data
     **/
    static constexpr areg::InterfaceData _InterfaceData = 
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

AREG_API_IMPL bool areg::send_raw_message(const areg::RemoteMessage& msg) noexcept
{
    return ServiceManager::send_raw_message(msg);
}

AREG_API_IMPL const areg::Channel& areg::connection_channel() noexcept
{
    return ServiceManager::connection_channel();
}
