#ifndef AREG_PERSIST_CONFIGMANAGER_HPP
#define AREG_PERSIST_CONFIGMANAGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/persist/ConfigManager.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, the configuration object to read configuration file.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/TEMap.hpp"
#include "areg/base/Version.hpp"
#include "areg/persist/NEPersistence.hpp"
#include "areg/persist/Property.hpp"
#include "areg/trace/NETrace.hpp"
#include "areg/ipc/NERemoteService.hpp"

#include <vector>

/************************************************************************
 * Dependencies.
 ************************************************************************/
class FileBase;

//////////////////////////////////////////////////////////////////////////
// ConfigManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The class to read, initialize and write application configuration
 *          properties from the file. The object reads configuration from
 *          the file, initializes read-only and writable properties,
 *          allows to modify configuration and save to the file.
 *          The read-only properties are generic and global for all applications,
 *          which cannot be overwritten. The writable properties are application
 *          specific and can be changed only for the application.
 **/
class AREG_API ConfigManager
{
//////////////////////////////////////////////////////////////////////////
// Object specific types and constants
//////////////////////////////////////////////////////////////////////////
public:
    //!< The type to initialize list of properties.
    using ListProperties  = TEArrayList<Property>;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ConfigManager( void );

    ~ConfigManager(void) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Generic methods.
 ************************************************************************/

    /**
     * \brief   Return true if the application is configured.
     **/
    inline bool isConfigured(void) const;

    /**
     * \brief   Returns configuration file path name.
     **/
    inline const String& getConfigFile(void) const;

    /**
     * \brief   Returns the list of read-only configuration properties.
     **/
    inline const ConfigManager::ListProperties & getReadonlyProperties( void ) const;

    /**
     * \brief   Returns the list of writable configuration properties.
     **/
    inline const ConfigManager::ListProperties & getModuleProperties( void ) const;

    /**
     * \brief   Returns true if the specified property exists.
     * \param   key     The property key to check.
     **/
    bool existProperty(const PropertyKey& key) const;

    /**
     * \brief   Returns true if there is a property with specified parameters for current module.
     * \param   section     The section of the key. If '*' searches only in read-only list.
     * \param   property    The property of the key to search.
     * \param   position    The optional position of the key to search.
     * \return  Returns true if found either module specific entry or a global entry.
     **/
    inline bool existProperty(const String& section, const String& property, const String& position = String::EmptyString) const;

    /**
     * \brief   Returns merged list of read-only and writable properties of the specified section.
     **/
    ConfigManager::ListProperties getSectionProperties(const String& section) const;

    /**
     * \brief   Returns the pointer of the property searched by specified parameters.
     *          The property is searched in both read-only and writable lists.
     * \param   section     The section to search in the list of configuration properties.
     * \param   property    The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns valid pointer if succeeded to find a property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    const Property * getProperty( const String& section
                                , const String& property
                                , const String& position
                                , NEPersistence::eConfigKeys keyType = NEPersistence::eConfigKeys::EntryAnyKey) const;

    /**
     * \brief   Returns the pointer of the property searched by specified key parameters.
     *          The property is searched in both read-only and writable lists.
     *          The module value of the key is ignored, so that only the current name
     *          of the process (module) is used.
     * \param   key     The key value of the property containing section, property and position information to search.
     * \return  Returns valid pointer if succeeded to find a property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    inline const Property * getProperty(const PropertyKey& key) const;

    /**
     * \brief   Returns the pointer of the module specific property searched by specified parameters.
     *          The property is searched only in writable lists.
     * \param   section     The section to search in the list of configuration properties.
     * \param   property    The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns valid pointer if succeeded to find a module property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    const Property * getModuleProperty( const String& section
                                      , const String& property
                                      , const String& position
                                      , NEPersistence::eConfigKeys keyType = NEPersistence::eConfigKeys::EntryAnyKey) const;

    /**
     * \brief   Returns the pointer of the property searched by specified key parameters.
     *          The property is searched only in writable lists.
     *          The module value of the key is ignored, so that only the current name
     *          of the process (module) is used.
     * \param   key     The key value of the property containing section, property and position information to search.
     * \return  Returns valid pointer if succeeded to find a property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    inline const Property* getModuleProperty(const PropertyKey& key) const;

    /**
     * \brief   Adds new or updates the existing configuration entry. The searching is done only
     *          in writable list. If entry with specified section, property and position exists,
     *          it updates the existing entry. Otherwise, it adds new entry in the list.
     *          It is as well possible to mark the entry as temporary, so that when saving
     *          current configuration in the file, the temporary entry is ignored.
     * \param   section     The section to search in the list of configuration properties.
     * \param   property    The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   value       The new value to set in specified configuration entry.
     * \param   keyType     The property key type.
     * \param   isTemporary Flag, indicating whether the change can be saved in the configuration file
     *                      or should be ignored.
     **/
    void setModuleProperty( const String& section
                          , const String& property
                          , const String& position
                          , const String& value
                          , NEPersistence::eConfigKeys keyType = NEPersistence::eConfigKeys::EntryAnyKey
                          , bool isTemporary = false);

    /**
     * \brief   Adds new or updates the existing configuration entry. The searching is done only
     *          in writable list. If entry with specified section, property and position exists,
     *          it updates the existing entry. Otherwise, it adds new entry in the list.
     *          It is as well possible to mark the entry as temporary, so that when saving
     *          current configuration in the file, the temporary entry is ignored.
     * \param   key     The key value of the property containing section, property and position information to search.
     * \param   value       The new value to set in specified configuration entry.
     * \param   isTemporary Flag, indicating whether the change can be saved in the configuration file
     *                      or should be ignored.
     **/
    inline void setModuleProperty( const PropertyKey& key, const String& value, bool isTemporary = false);

    /**
     * \brief   Returns the pointer to the value of the property searched by specified parameters.
     *          The property is searched in both read-only and writable lists.
     * \param   section     The section to search in the list of configuration properties.
     * \param   property    The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns valid pointer to property value if succeeded to find a property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    inline const PropertyValue * getPropertyValue( const String& section
                                                 , const String& property
                                                 , const String& position = String::EmptyString
                                                 , NEPersistence::eConfigKeys keyType = NEPersistence::eConfigKeys::EntryAnyKey) const;

    /**
     * \brief   Returns the pointer to the value of the property searched by specified parameters.
     *          The property is searched in both read-only and writable lists.
     * \param   key     The key value of the property containing section, property and position information to search.
     * \return  Returns valid pointer to property value if succeeded to find a property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    inline const PropertyValue * getPropertyValue(const PropertyKey& key) const;

    /**
     * \brief   Returns the pointer to the value of the module specific property searched by specified parameters.
     *          The property is searched only in writable lists.
     * \param   section     The section to search in the list of configuration properties.
     * \param   property    The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns valid pointer if succeeded to find a module property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    inline const PropertyValue * getModulePropertyValue( const String& section
                                                       , const String& property
                                                       , const String& position = String::EmptyString
                                                       , NEPersistence::eConfigKeys keyType = NEPersistence::eConfigKeys::EntryAnyKey) const;
    inline PropertyValue * getModulePropertyValue( const String& section
                                                 , const String& property
                                                 , const String& position = String::EmptyString
                                                 , NEPersistence::eConfigKeys keyType = NEPersistence::eConfigKeys::EntryAnyKey);

    /**
     * \brief   Returns the pointer to the value of the module specific property searched by specified key parameter.
     *          The property is searched only in writable lists.
     * \param   key     The key value of the property containing section, property and position information to search.
     * \return  Returns valid pointer if succeeded to find a module property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    inline const PropertyValue* getModulePropertyValue(const PropertyKey& key) const;
    inline PropertyValue* getModulePropertyValue(const PropertyKey& key);

    /**
     * \brief   Adds new properties to the module configuration.
     * \param   propList    The list of new properties to add.
     * \return  Returns number of properties added to the writable list of the module.
     **/
    inline int addModuleProperties(const ConfigManager::ListProperties& propList);

    /**
     * \brief   Overwrites the existing list of writable properties. All existing properties will be lost.
     **/
    inline void replaceModuleProperty(const ConfigManager::ListProperties& listProperties);

    /**
     * \brief   Removes the existing configuration entry from the writable list.
     *          The search of the entry is done by specified parameters.
     * \param   section     The section to search in the list of configuration properties.
     * \param   property    The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     **/
    void removeModuleProperty( const String& section
                             , const String& property
                             , const String& position = String::EmptyString
                             , NEPersistence::eConfigKeys keyType = NEPersistence::eConfigKeys::EntryAnyKey);

    /**
     * \brief   Removes the existing configuration entry from the writable list.
     *          The search of the entry is done by specified parameters.
     * \param   key     The key value of the property containing section, property and position information to search.
     **/
    inline void removeModuleProperty(const PropertyKey& key);

    /**
     * \brief   Removes all configuration entries from the writable list matching the specified section and property.
     * \param   section     The section to search in the list of configuration properties.
     * \param   property    The property to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns the number of entries removed from the writable list.
     **/
    int removeModuleProperties(const String& section, const String& property, NEPersistence::eConfigKeys keyType = NEPersistence::eConfigKeys::EntryAnyKey);

    /**
     * \brief   Removes all configuration entries from the writable list matching the specified section.
     * \param   section     The section to search in the list of configuration properties.
     **/
    void removeSectionProperties(const String& section);

    /**
     * \brief   Reads the configuration from the specified configuration file.
     *          The file should be either full or relative path.
     *          If the specified file path is empty, it uses default 
     *          NEApplication::DEFAULT_CONFIG_FILE file path.
     * \param   filePath    The relative of full path to the configuration file.
     *                      If empty, uses NEApplication::DEFAULT_CONFIG_FILE file path.
     * \return  Returns true if succeeded to read and initialize configuration.
     **/
    bool readConfig(const String& filePath = String::EmptyString);

    /**
     * \brief   Reads the configuration from the specified configuration file.
     *          The file should be either full or relative path.
     *          If the specified file path is empty, it uses default
     *          NEApplication::DEFAULT_CONFIG_FILE file path.
     * \param   file        The instance of file object opened for reading.
     * \return  Returns true if succeeded to read and initialize configuration.
     **/
    bool readConfig(const FileBase& file);

    /**
     * \brief   Saves the current configuration in the specified file.
     *          When saving, the existing entries are not changed, only module specific entries
     *          that are not marked as 'temporary'.
     * \param   filePath    The relative of full path to the configuration file to write.
     *                      If file is empty, uses the same name of previously opened file.
     *                      If the current configuration file path is empty (defaults are initialized),
     *                      it uses NEApplication::DEFAULT_CONFIG_FILE file to save.
     * \return  Returns true if succeeded to save configuration.
     **/
    bool saveConfig(const String& filePath = String::EmptyString);

    /**
     * \brief   Saves the current configuration in the specified file object opened with write access.
     *          When saving, the existing entries are not changed, only module specific entries
     *          that are not marked as 'temporary'.
     * \param   filePath    The relative of full path to the configuration file to write.
     *                      If file is empty, uses the same name of previously opened file.
     *                      If the current configuration file path is empty (defaults are initialized),
     *                      it uses NEApplication::DEFAULT_CONFIG_FILE file to save.
     * \return  Returns true if succeeded to save configuration.
     **/
    bool saveConfig(const FileBase& srcFile, FileBase& dstFile);

    /**
     * \brief   Releases all module specific entries.
     *          This will clean only the writable entries
     **/
    inline void releaseModuleProperties(void);

    /**
     * \brief   Releases all module specific entries.
     *          This will clean only the writable entries
     **/
    inline void releaseProperties(void);

    /**
     * \brief   Sets the read-only and writable configuration entries.
     * \param   listReadonly    The list of read-only configuration properties.
     * \param   listWritable    The list of writable configuration properties.
     **/
    inline void setConfiguration(const ConfigManager::ListProperties& listReadonly, const ConfigManager::ListProperties& listWritable);

/************************************************************************
 * Configuration properties.
 ************************************************************************/

    /**
     * \brief   Returns the configuration version.
     **/
    Version getConfigVersion(void) const;

/************************************************************************
 * Log properties.
 ************************************************************************/

    /**
     * \brief   Returns logging version.
     **/
    Version getLogVersion(void) const;

    /**
     * \brief   Returns list of logging targets specified in the NETrace::eLogingTypes.
     **/
    std::vector<Identifier> getLogTargets(void) const;

    /**
     * \brief   Returns logging status.
     * \return  If returns true, the logging is enabled. Otherwise, the logging is disabled.
     **/
    bool getLoggingStatus(void) const;

    /**
     * \brief   Sets the logging state for the current module (process).
     * \param   newValue    Specifies enabled or disabled state of logging for the current module.
     *                      The value 'true' enables logging.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    inline void setLoggingStatus(bool newValue, bool isTemporary = false);

    /**
     * \brief   Returns log enable or disable state for the target defined in NETrace::eLogingTypes.
     * \param   logType     The string value of NETrace::eLogingTypes.
     * \return  If returns true, the logging for the specified target is enabled. Otherwise, it is disabled.
     **/
    bool getLogEnabled(const String& logType) const;

    /**
     * \brief   Returns log enable or disable state for the target defined in NETrace::eLogingTypes.
     * \param   logType     The Identifier object containing NETrace::eLogingTypes value and string equivalent.
     * \return  If returns true, the logging for the specified target is enabled. Otherwise, it is disabled.
     **/
    inline bool getLogEnabled(const Identifier& logType) const;

    /**
     * \brief   Returns log enable or disable state for the given target.
     * \param   logType     The logging target to check.
     * \return  If returns true, the logging for the specified target is enabled. Otherwise, it is disabled.
     **/
    bool getLogEnabled(NETrace::eLogingTypes logType) const;

    /**
     * \brief   Sets the logging state for the target defined in NETrace::eLogingTypes.
     * \param   logType     The Identifier object containing NETrace::eLogingTypes value and string equivalent.
     * \param   newValue    If 'true' enables the logging for the target output.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    inline void setLogEnabled(const Identifier& logType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Sets the logging state for the target defined in NETrace::eLogingTypes.
     * \param   logType     The string value of NETrace::eLogingTypes.
     * \param   newValue    If 'true' enables the logging for the target output.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    inline void setLogEnabled(const String& logType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Sets the logging state for the target defined in NETrace::eLogingTypes.
     * \param   logType     The logging target to set enabling flag.
     * \param   newValue    If 'true' enables the logging for the target output.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogEnabled(NETrace::eLogingTypes logType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Returns the path of the log file that contains messages.
     **/
    String getLogFileLocation(void) const;

    /**
     * \brief   Sets the path of the log file that contains messages.
     * \param   newValue    Relative of absolute file with or without a mask to write logs.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    inline void setLogFileLocation(const String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns flag indicating whether the log messages should be added in the existing file or not.
     **/
    bool getLogFileAppend(void) const;

    /**
     * \brief   Sets the value of the flag that indicates whether the log messages are
     *          added to the existing file or not.
     * \param   newValue    The flag indicating whether the logs should be added to the existing
     *                      log files or not.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogFileAppend(bool newValue, bool isTemporary = false);

    /**
     * \brief   Returns the maximum queue size of log messages while there is no connection with remote logger.
     **/
    uint32_t getLogRemoteQueueSize(void) const;

    /**
     * \brief   Set the maximum queue size for message logging on remote logger while there is no connection established.
     *          The value 0 means do not queue at all and send message to remote logger if there is a connection.
     * \param   newValue    The size of queue when make remote logging.
     *                      Value 0 means disable queuing and send message immediately.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogRemoteQueueSize(uint32_t newValue, bool isTemporary = false);

    /**
     * \brief   Returns the layout format of the log enter scope message.
     **/
    String getLogLayoutEnter(void) const;

    /**
     * \brief   Sets the layout format string of the log enter scope message.
     * \param   newValue    The new layout format.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogLayoutEnter(const String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns the layout format of the log message.
     **/
    String getLogLayoutMessage(void) const;

    /**
     * \brief   Sets the layout format string of the log message.
     * \param   newValue    The new layout format.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogLayoutMessage(const String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns the layout format of the log e exit cope message.
     **/
    String getLogLayoutExit(void) const;

    /**
     * \brief   Sets the layout format string of the log exit scope message.
     * \param   newValue    The new layout format.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogLayoutExit(const String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns the list of log scope properties of the module.
     **/
    inline std::vector<Property> getModuleLogScopes(void) const;

    /**
     * \brief   Gets the list of log scope properties of the module.
     * \param   scopeList   On output this contains the list of log scopes properties of the module.
     * \return  The number scopes in the list.
     **/
    uint32_t getModuleLogScopes(std::vector<Property>& OUT scopeList) const;

    /**
     * \brief   Adds the list of scopes properties to the existing list of the module.
     * \param   scopeList   The list of the scope properties to set for module.
     * \param   search      Flag, indicating whether it should search the scope in the existing list.
     *                      If true, searches the scope property, and if found, replaces the existing.
     *                      Otherwise, it adds a scope property to the module list.
     **/
    void addModuletLogScopes(const std::vector<Property>& scopeList, bool search);

    /**
     * \brief   Adds a property of a scope with the priority value to the existing list of scope of the module.
     * \param   scopeName   The name of the scope to add.
     * \param   prio        The scope priority or priorities separated by logical OR ( '|' ) of the scope.
     **/
    void addModuleLogScope(const String& scopeName, const String& prio);

    /**
     * \brief   Adds a property of a scope with the priority value to the existing list of scope of the module.
     * \param   scopeName   The name of the scope to add.
     * \param   prio        The scope priority of the scope.
     **/
    void addModuleLogScope(const String& scopeName, unsigned int prio);

    /**
     * \brief   Remotes a property entry of specified scope name.
     * \param   scopeName   The name of the scope to remove.
     * \return  Returns true if succeeded to remove the scope.
     **/
    bool removeScope(const String& scopeName);

    /**
     * \brief   Removes all scopes of the module and return number of removed entries.
     **/
    int removeModuleScopes(void);

/************************************************************************
 * Remote service properties.
 ************************************************************************/

    /**
     * \brief   Returns list of remote services.
     **/
    std::vector<Identifier> getServiceList(void) const;

    /**
     * \brief   Returns list of connection identifiers of the remote service.
     * \param   service The remote service, which identifiers should be initialized.
     **/
    std::vector<Identifier> getRemoteServiceConnections(const String& service) const;

    /**
     * \brief   Returns the name of the service of the remote connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type, which name should be read out.
     **/
    String getRemoteServiceName(const String& service, const String& connectType) const;

    /**
     * \brief   Returns the name of the service of the remote connection.
     * \param   service     The remote service.
     * \param   connectType The connection type, which name should be read out.
     **/
    String getRemoteServiceName(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType) const;

    /**
     * \brief   Returns the remote service connection enabled or disabled flag.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type, which name should be read out.
     * \return  If returns true, the specified remote service supports the specified connection type.
     *          Otherwise, it does not support
     **/
    bool getRemoteServiceEnable(const String& service, const String& connectType) const;

    /**
     * \brief   Returns the remote service connection enabled or disabled flag.
     * \param   service     The remote service.
     * \param   connectType The connection type, which name should be read out.
     * \return  If returns true, the specified remote service supports the specified connection type.
     *          Otherwise, it does not support
     **/
    bool getRemoteServiceEnable(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType) const;

    /**
     * \brief   Sets the enabled flag of the specified connection of the remote service.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServiceEnable(const String& service, const String& connectType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Sets the enabled flag of the specified connection of the remote service.
     * \param   service     The remote service.
     * \param   connectType The connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServiceEnable(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Returns the address of the remote service that supports the specified connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type, which name should be read out.
     **/
    String getRemoteServiceAddress(const String& service, const String& connectType) const;

    /**
     * \brief   Returns the address of the remote service that supports the specified connection.
     * \param   service     The remote service.
     * \param   connectType The connection type, which name should be read out.
     **/
    String getRemoteServiceAddress(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType) const;

    /**
     * \brief   Sets the address of the remote service to establish specified connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServiceAddress(const String& service, const String& connectType, const String& newValue, bool isTemporary = false);

    /**
     * \brief   Sets the address of the remote service to establish specified connection.
     * \param   service     The remote service.
     * \param   connectType The connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServiceAddress(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType, const String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns the port number the remote service that supports the specified connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type, which name should be read out.
     **/
    uint16_t getRemoteServicePort(const String& service, const String& connectType) const;

    /**
     * \brief   Returns the port number the remote service that supports the specified connection.
     * \param   service     The remote service.
     * \param   connectType The connection type, which name should be read out.
     **/
    uint16_t getRemoteServicePort(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType) const;

    /**
     * \brief   Sets the port number of the remote service to establish specified connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServicePort(const String& service, const String& connectType, uint16_t newValue, bool isTemporary = false);

    /**
     * \brief   Sets the port number of the remote service to establish specified connection.
     * \param   service     The remote service.
     * \param   connectType The connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServicePort(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType, uint16_t newValue, bool isTemporary = false);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The module name (the process name) to use when initialize configuration.
     **/
    const String    mModule;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   The list of writable properties of the configuration, which can be modified for current process.
     **/
    ListProperties   mWritableProperties;

    /**
     * \brief   The list of read-only properties of the configuration, which cannot be modified.
     **/
    ListProperties  mReadonlyProperties;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    /**
     * \brief   The flag indicating whether the application is configured from configuration file.
     **/
    bool            mIsConfigured;

    /**
     * \brief   The path of the configuration file.
     **/
    String          mFilePath;

    /**
     * \brief   Synchronization object for multithreading.
     **/
    mutable Mutex   mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(ConfigManager);
};

//////////////////////////////////////////////////////////////////////////
// Configuration inline methods
//////////////////////////////////////////////////////////////////////////
inline bool ConfigManager::isConfigured(void) const
{
    Lock lock(mLock);
    return mIsConfigured;
}

inline const String& ConfigManager::getConfigFile(void) const
{
    Lock lock(mLock);
    return mFilePath;
}

inline const ConfigManager::ListProperties& ConfigManager::getReadonlyProperties(void) const
{
    Lock lock(mLock);
    return mReadonlyProperties;
}

inline const ConfigManager::ListProperties& ConfigManager::getModuleProperties(void) const
{
    Lock lock(mLock);
    return mWritableProperties;
}

inline bool ConfigManager::existProperty(const String& section, const String& property, const String& position) const
{
    return existProperty(PropertyKey(section, mModule, property, position));
}

inline const Property* ConfigManager::getProperty(const PropertyKey& key) const
{
    return getProperty(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline const Property* ConfigManager::getModuleProperty(const PropertyKey& key) const
{
    return getModuleProperty(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline void ConfigManager::setModuleProperty(const PropertyKey& key, const String& value, bool isTemporary)
{
    setModuleProperty(key.getSection(), key.getProperty(), key.getPosition(), value, key.getKeyType(), isTemporary);
}

inline const PropertyValue * ConfigManager::getPropertyValue( const String& section
                                                           , const String& property
                                                           , const String& position /*= String::EmptyString*/
                                                           , NEPersistence::eConfigKeys keyType /*= NEPersistence::eConfigKeys::EntryAnyKey*/) const
{
    Lock lock(mLock);

    const Property* result{ getProperty(section, property, position, keyType)};
    return (result != nullptr ? &result->getValue() : nullptr);
}

inline const PropertyValue* ConfigManager::getPropertyValue(const PropertyKey& key) const
{
    return getPropertyValue(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline const PropertyValue* ConfigManager::getModulePropertyValue(const PropertyKey& key) const
{
    return getModulePropertyValue(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline const PropertyValue* ConfigManager::getModulePropertyValue( const String& section
                                                                , const String& property
                                                                , const String& position /*= String::EmptyString*/
                                                                , NEPersistence::eConfigKeys keyType /*= NEPersistence::eConfigKeys::EntryAnyKey*/) const
{
    Lock lock(mLock);

    const Property* result{ getModuleProperty(section, property, position, keyType) };
    return (result != nullptr ? &result->getValue() : nullptr);
}

inline PropertyValue* ConfigManager::getModulePropertyValue(const PropertyKey& key)
{
    return getModulePropertyValue(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline PropertyValue * ConfigManager::getModulePropertyValue( const String& section
                                                           , const String& property
                                                           , const String& position /*= String::EmptyString*/
                                                           , NEPersistence::eConfigKeys keyType /*= NEPersistence::eConfigKeys::EntryAnyKey*/)
{
    Lock lock(mLock);

    const Property* result{ getModuleProperty(section, property, position, keyType) };
    return (result != nullptr ? &const_cast<Property *>(result)->getValue() : nullptr);
}

inline int ConfigManager::addModuleProperties(const ConfigManager::ListProperties& propList)
{
    int result{ 0 };
    Lock lock(mLock);
    const std::vector<Property>& list = propList.getData();
    for (const auto& prop : list)
    {
        if (existProperty(prop.getKey()) == false)
        {
            ++result;
            mWritableProperties.add(prop);
        }
    }

    return result;
}

inline void ConfigManager::replaceModuleProperty(const ConfigManager::ListProperties& listProperties)
{
    Lock lock(mLock);
    mWritableProperties = listProperties;
}

inline void ConfigManager::removeModuleProperty(const PropertyKey& key)
{
    removeModuleProperty(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline void ConfigManager::releaseModuleProperties(void)
{
    Lock lock(mLock);
    mWritableProperties.clear();
}

inline void ConfigManager::releaseProperties(void)
{
    Lock lock(mLock);
    mIsConfigured = false;
    mWritableProperties.clear();
    mReadonlyProperties.clear();
}

inline void ConfigManager::setConfiguration(const ConfigManager::ListProperties& listReadonly, const ConfigManager::ListProperties& listWritable)
{
    Lock lock(mLock);
    mIsConfigured = true;
    mWritableProperties = listWritable;
    mReadonlyProperties = listReadonly;
}

inline void ConfigManager::setLoggingStatus(bool newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogStatus;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogStatus();

    setModuleProperty(key.section, key.property, key.position, String::toString(newValue), confKey, isTemporary);
}

inline bool ConfigManager::getLogEnabled(const Identifier& logType) const
{
    return getLogEnabled(logType.getName());
}

inline void ConfigManager::setLogEnabled(const String& logType, bool newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogEnable;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogEnable();
    setModuleProperty(key.section, key.property, key.position, String::toString(newValue), confKey, isTemporary);
}

inline void ConfigManager::setLogEnabled(const Identifier& logType, bool newValue, bool isTemporary /*= false*/)
{
    setLogEnabled(logType.getName(), newValue, isTemporary);
}

inline void ConfigManager::setLogFileLocation(const String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogFileLocation;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogFileLocation();
    setModuleProperty(key.section, key.property, key.position, newValue, confKey, isTemporary);
}

inline std::vector<Property> ConfigManager::getModuleLogScopes(void) const
{
    std::vector<Property> result;
    getModuleLogScopes(result);
    return result;
}

#endif // AREG_PERSIST_CONFIGMANAGER_HPP
