#ifndef AREG_PERSIST_IECONFIGURATIONLISTENER_HPP
#define AREG_PERSIST_IECONFIGURATIONLISTENER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/persist/IEConfigurationListener.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, the configuration object to read configuration file.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/persist/ConfigManager.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/
class ConfigManager;

//////////////////////////////////////////////////////////////////////////
// IEConfigurationListener interface declaration
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
class AREG_API IEConfigurationListener
{
//////////////////////////////////////////////////////////////////////////
// Protected constructors / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    IEConfigurationListener(void) = default;
    virtual ~IEConfigurationListener(void) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEConfigurationListener interface overrides
/************************************************************************/

    /**
     * \brief   Called by configuration manager before the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void prepareSaveConfiguration(ConfigManager& config) = 0;

    /**
     * \brief   Called by configuration manager after the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void postSaveConfiguration(ConfigManager& config) = 0;

    /**
     * \brief   Called by configuration manager before the configuration is loaded from the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void prepareReadConfiguration(ConfigManager& config) = 0;

    /**
     * \brief   Called by configuration manager when configuration is completed to load data from the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void postReadConfiguration(ConfigManager& config) = 0;

    /**
     * \brief   Called by configuration manager after setting read-only and writable properties.
     *          For example, when the default configuration is set.
     * \param   listReadonly    The list of read-only properties to set in the configuration.
     * \param   listWritable    The list of module / process specific properties to set in the configuration;
     * \param   config          The instance of configuration manager.
     **/
    virtual void onSetupConfiguration(const NEPersistence::ListProperties& listReadonly, const NEPersistence::ListProperties& listWritable, ConfigManager& config) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(IEConfigurationListener);
};

#endif  // AREG_PERSIST_IECONFIGURATIONLISTENER_HPP
