#ifndef AREG_PERSIST_CONFIGLISTENER_HPP
#define AREG_PERSIST_CONFIGLISTENER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/ConfigListener.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the configuration object to read configuration file.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/persist/ConfigManager.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/
namespace areg {
    class ConfigManager;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ConfigListener interface declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Configuration listener is an object, which methods are triggered
 *          when the application loads, saves or sets default configuration data.
 *          It is normally used by Application object when initializing the application
 *          and loads the configuration. In this case the developers may change
 *          the configuration during run-time and manually update the configuration.
 *          For example, the developers may set temporary property, which will
 *          have the influence on the application, but not saved in the config file.
 **/
class AREG_API ConfigListener
{
//////////////////////////////////////////////////////////////////////////
// Protected constructors / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    ConfigListener() = default;
    virtual ~ConfigListener() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ConfigListener interface overrides
/************************************************************************/

    /**
     * \brief   Called by configuration manager before the configuration is saved in the file.
     * \param   config      The instance of configuration manager.
     **/
    virtual void prepare_save_configuration(ConfigManager& config) = 0;

    /**
     * \brief   Called by configuration manager after the configuration is saved in the file.
     * \param   config      The instance of configuration manager.
     **/
    virtual void post_save_configuration(ConfigManager& config) = 0;

    /**
     * \brief   Called by configuration manager before the configuration is loaded from the file.
     * \param   config      The instance of configuration manager.
     **/
    virtual void prepare_read_configuration(ConfigManager& config) = 0;

    /**
     * \brief   Called by configuration manager after configuration loading is completed from the file.
     * \param   config      The instance of configuration manager.
     **/
    virtual void post_read_configuration(ConfigManager& config) = 0;

    /**
     * \brief   Called by configuration manager after setting read-only and writable properties. For
     *          example, when the default configuration is set.
     * \param   listReadonly    The list of read-only properties to set in the configuration.
     * \param   listWritable    The list of module / process specific properties to set in the configuration.
     * \param   config          The instance of configuration manager.
     **/
    virtual void on_setup_configuration(const areg::ListProperties& listReadonly, const areg::ListProperties& listWritable, ConfigManager& config) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(ConfigListener);
};

} // namespace areg
#endif  // AREG_PERSIST_CONFIGLISTENER_HPP
