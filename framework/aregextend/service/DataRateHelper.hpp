#ifndef AREG_AREGEXTEND_SERVICE_DATARATEHELPER_HPP
#define AREG_AREGEXTEND_SERVICE_DATARATEHELPER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/DataRateHelper.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Extended features.
 *              Data rate helper object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"

#include <utility>
#include <string>

/************************************************************************
 * Dependencies.
 ************************************************************************/
namespace areg::ext {
    class ServiceCommunicationBase;
}

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// DataRateHelper class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A simple helper class to calculate data rate.
 **/
class DataRateHelper
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
public:
    //!< Bytes in 1 Kilobyte.
    static constexpr uint32_t           ONE_KILOBYTE    { areg::ONE_KILOBYTE };
    //!< Bytes in 1 megabyte.
    static constexpr uint32_t           ONE_MEGABYTE    { areg::ONE_MEGABYTE };
    //!< String kilobytes per second
    static constexpr std::string_view   MSG_KILOBYTES   { "KBytes / sec." };
    //!< String megabytes per second
    static constexpr std::string_view   MSG_MEGABYTES   { "MBytes / sec." };
    //!< String bytes per second
    static constexpr std::string_view   MSG_BYTES       { " Bytes / sec." };

    //!< The type of data rate. Contains value and the associated literal.
    using DataRate  = std::pair<float, std::string>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the object, sets threads that the rate can be queried. If passed
     *          'verbose' parameter is 'false' on each query it returns zero. Otherwise, returns the
     *          actual value.
     *
     * \param   sendThread          The thread that can be queried the data size sent.
     * \param   receiveThread       The threat that can be queried the data size received.
     * \param   verbose             The flag, indicating whether the actual size should be computed
     *                              or should return zero.
     **/
    DataRateHelper(ServiceCommunicationBase& server, bool verbose);

    ~DataRateHelper() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and actions.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets the verbose flag, indicating whether the data send / receive computation is enabled.
     *          If verbose is false, the data rate queries will return zero.
     **/
    void set_verbose(bool verbose) noexcept;

    /**
     * \brief   Returns the verbose flag, indicating whether the data send / receive computation is enabled.
     **/
    [[nodiscard]]
    bool is_verbose() const noexcept;

    void query_data_sent(uint64_t& sizeSent, uint32_t& msgSent) noexcept;

    void query_data_received(uint64_t& sizeRecv, uint32_t& msgRecv) noexcept;

    /**
     * \brief   Converts byte size to a formatted DataRate with appropriate units.
     *
     * \param   sizeBytes       The size in bytes to convert.
     * \return  Returns a DataRate object with the converted value and appropriate unit.
     **/
    [[nodiscard]]
    static DataRate convert_data_rate_literals(uint64_t sizeBytes);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    ServiceCommunicationBase& mServer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DataRateHelper() = delete;
    AREG_NOCOPY_NOMOVE(DataRateHelper);
};

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_DATARATEHELPER_HPP
