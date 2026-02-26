#ifndef AREG_PERSIST_CONFIGMANAGER_HPP
#define AREG_PERSIST_CONFIGMANAGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/ConfigManager.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the configuration object to read configuration file.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Version.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/persist/Property.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"

#include <vector>

/************************************************************************
 * Dependencies.
 ************************************************************************/
class FileBase;
class ConfigListener;

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
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ConfigManager();

    ~ConfigManager() = default;

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
    inline bool isConfigured() const;

    /**
     * \brief   Returns configuration file path name.
     **/
    inline const areg::String& getConfigFile() const;

    /**
     * \brief   Returns the list of read-only configuration properties.
     **/
    inline const areg::ListProperties & getReadonlyProperties() const;

    /**
     * \brief   Returns the list of writable configuration properties.
     **/
    inline const areg::ListProperties & getModuleProperties() const;

    /**
     * \brief	Locks the access resources of configuration manager. The current thread
     *          gets access ownership until unlock method is called. If a thread already
     *          owns the resource access of the configuration, the current thread is suspended.
     * \return	Returns true if current thread successfully got configuration resource ownership.
     **/
    inline bool lock() const;

    /**
     * \brief	Unlocks / releases the configuration resource access ownership, so that other thread
     *          may access the resources. The calling thread should have resource ownership to unlock it.
     *          Otherwise, no lock state is changed.
     * \return	Returns true if succeeded.
     **/
    inline bool unlock() const;

    /**
     * \brief   Attempts to get configuration resource access ownership without blocking current thread.
     *          If the call succeeded, the calling thread takes the resource access ownership. Otherwise,
     *          the access is already locked by another thread.
     * \return  Return true if the configuration resource access ownership succeeded. Otherwise,
     *          returns false.
     **/
    inline bool tryLock() const;

    /**
     * \brief   Return true if configuration resources access is already locked by a thread.
     **/
    inline bool isLocked() const;

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
    inline bool existProperty(const areg::String& section, const areg::String& property, const areg::String& position = areg::String::EmptyString) const;

    /**
     * \brief   Returns merged list of read-only and writable properties of the specified section.
     **/
    areg::ListProperties getSectionProperties(const areg::String& section) const;

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
    const Property * getProperty( const areg::String& section
                                , const areg::String& property
                                , const areg::String& position
                                , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey) const;

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
    const Property * getModuleProperty( const areg::String& section
                                      , const areg::String& property
                                      , const areg::String& position
                                      , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey) const;

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
    void setModuleProperty( const areg::String& section
                          , const areg::String& property
                          , const areg::String& position
                          , const areg::String& value
                          , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey
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
    inline void setModuleProperty( const PropertyKey& key, const areg::String& value, bool isTemporary = false);

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
    inline const areg::PropertyValue * getPropertyValue( const areg::String& section
                                                 , const areg::String& property
                                                 , const areg::String& position = areg::String::EmptyString
                                                 , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey) const;

    /**
     * \brief   Returns the pointer to the value of the property searched by specified parameters.
     *          The property is searched in both read-only and writable lists.
     * \param   key     The key value of the property containing section, property and position information to search.
     * \return  Returns valid pointer to property value if succeeded to find a property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    inline const areg::PropertyValue * getPropertyValue(const PropertyKey& key) const;

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
    inline const areg::PropertyValue * getModulePropertyValue( const areg::String& section
                                                       , const areg::String& property
                                                       , const areg::String& position = areg::String::EmptyString
                                                       , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey) const;
    inline areg::PropertyValue * getModulePropertyValue( const areg::String& section
                                                 , const areg::String& property
                                                 , const areg::String& position = areg::String::EmptyString
                                                 , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey);

    /**
     * \brief   Returns the pointer to the value of the module specific property searched by specified key parameter.
     *          The property is searched only in writable lists.
     * \param   key     The key value of the property containing section, property and position information to search.
     * \return  Returns valid pointer if succeeded to find a module property matching specified parameters.
     *          Otherwise, returns 'nullptr'.
     **/
    inline const areg::PropertyValue* getModulePropertyValue(const PropertyKey& key) const;
    inline areg::PropertyValue* getModulePropertyValue(const PropertyKey& key);

    /**
     * \brief   Adds new properties to the module configuration.
     * \param   propList    The list of new properties to add.
     * \return  Returns number of properties added to the writable list of the module.
     **/
    inline int32_t addModuleProperties(const areg::ListProperties& propList);

    /**
     * \brief   Overwrites the existing list of writable properties. All existing properties will be lost.
     **/
    inline void replaceModuleProperty(const areg::ListProperties& listProperties);

    /**
     * \brief   Removes the existing configuration entry from the writable list.
     *          The search of the entry is done by specified parameters.
     * \param   section     The section to search in the list of configuration properties.
     * \param   property    The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     **/
    void removeModuleProperty( const areg::String& section
                             , const areg::String& property
                             , const areg::String& position = areg::String::EmptyString
                             , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey);

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
    int32_t removeModuleProperties(const areg::String& section, const areg::String& property, areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey);

    /**
     * \brief   Removes all configuration entries from the writable list matching the specified section.
     * \param   section     The section to search in the list of configuration properties.
     **/
    void removeSectionProperties(const areg::String& section);

    /**
     * \brief   Reads the configuration from the specified configuration file.
     *          The file should be either full or relative path.
     *          If the specified file path is empty, it uses default 
     *          areg::DEFAULT_CONFIG_FILE file path.
     * \param   filePath    The relative of full path to the configuration file.
     *                      If empty, uses areg::DEFAULT_CONFIG_FILE file path.
     * \param   listener    The pointer to the configuration listener to notify
     *                      before and after reading configuration from the file.
     *                      If nullptr, no notification is triggered.
     * \return  Returns true if succeeded to read and initialize configuration.
     **/
    bool readConfig(const areg::String& filePath = areg::String::EmptyString, ConfigListener * listener = nullptr);

    /**
     * \brief   Reads the configuration from the specified configuration file.
     *          The file should be either full or relative path.
     *          If the specified file path is empty, it uses default
     *          areg::DEFAULT_CONFIG_FILE file path.
     * \param   file        The instance of file object opened for reading.
     * \param   listener    The pointer to the configuration listener to notify
     *                      before and after reading configuration from the file.
     *                      If nullptr, no notification is triggered.
     * \return  Returns true if succeeded to read and initialize configuration.
     **/
    bool readConfig(const FileBase& file, ConfigListener * listener = nullptr);

    /**
     * \brief   Saves the current configuration in the specified file.
     *          When saving, the existing entries are not changed, only module specific entries
     *          that are not marked as 'temporary'.
     * \param   filePath    The relative of full path to the configuration file to write.
     *                      If file is empty, uses the same name of previously opened file.
     *                      If the current configuration file path is empty (defaults are initialized),
     *                      it uses areg::DEFAULT_CONFIG_FILE file to save.
     * \param   listener    The pointer to the configuration listener to notify
     *                      before and after saving configuration to the file.
     *                      If nullptr, no notification is triggered.
     * \return  Returns true if succeeded to save configuration.
     **/
    bool saveConfig(const areg::String& filePath = areg::String::EmptyString, ConfigListener * listener = nullptr);

    /**
     * \brief   Saves the current configuration in the specified file object opened with write access.
     *          When saving, the existing entries are not changed, only module specific entries
     *          that are not marked as 'temporary'.
     * \param   srcFile     The instance of the file that contains configuration information.
     * \param   dstFile     The instance of the destination file to save configuration information.
     * \param   listener    The pointer to the configuration listener to notify
     *                      before and after saving configuration to the file.
     *                      If nullptr, no notification is triggered.
     * \return  Returns true if succeeded to save configuration.
     **/
    bool saveConfig(const FileBase& srcFile, FileBase& dstFile, bool saveAll, ConfigListener * listener = nullptr);

    /**
     * \brief   Sets the read-only and writable configuration entries.
     * \param   listReadonly    The list of read-only configuration properties.
     * \param   listWritable    The list of writable configuration properties.
     * \param   listener        The pointer to the configuration listener to notify configuration data set/
     *                          If nullptr, no notification is triggered.
     **/
    void setConfiguration(const areg::ListProperties& listReadonly, const areg::ListProperties& listWritable, ConfigListener * listener = nullptr);

    /**
     * \brief   Releases all module specific entries.
     *          This will clean only the writable entries
     **/
    inline void releaseModuleProperties();

    /**
     * \brief   Releases all module specific entries.
     *          This will clean only the writable entries
     **/
    inline void releaseProperties();

/************************************************************************
 * Configuration properties.
 ************************************************************************/

    /**
     * \brief   Returns the configuration version.
     **/
    areg::Version getConfigVersion() const;

/************************************************************************
 * Log properties.
 ************************************************************************/

    /**
     * \brief   Returns logging version.
     **/
    areg::Version getLogVersion() const;

    /**
     * \brief   Returns list of logging targets specified in the areg::LogTarget.
     **/
    std::vector<areg::Identifier> getLogTargets() const;

    /**
     * \brief   Returns logging status.
     * \return  If returns true, the logging is enabled. Otherwise, the logging is disabled.
     **/
    bool getLoggingStatus() const;

    /**
     * \brief   Sets the logging state for the current module (process).
     * \param   newValue    Specifies enabled or disabled state of logging for the current module.
     *                      The value 'true' enables logging.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    inline void setLoggingStatus(bool newValue, bool isTemporary = false);

    /**
     * \brief   Returns log enable or disable state for the target defined in areg::LogTarget.
     * \param   logType     The string value of areg::LogTarget.
     * \return  If returns true, the logging for the specified target is enabled. Otherwise, it is disabled.
     **/
    bool getLogEnabled(const areg::String& logType) const;

    /**
     * \brief   Returns log enable or disable state for the target defined in areg::LogTarget.
     * \param   logType     The Identifier object containing areg::LogTarget value and string equivalent.
     * \return  If returns true, the logging for the specified target is enabled. Otherwise, it is disabled.
     **/
    inline bool getLogEnabled(const areg::Identifier& logType) const;

    /**
     * \brief   Returns log enable or disable state for the given target.
     * \param   logType     The logging target to check.
     * \return  If returns true, the logging for the specified target is enabled. Otherwise, it is disabled.
     **/
    bool getLogEnabled(areg::LogTarget logType) const;

    /**
     * \brief   Sets the logging state for the target defined in areg::LogTarget.
     * \param   logType     The Identifier object containing areg::LogTarget value and string equivalent.
     * \param   newValue    If 'true' enables the logging for the target output.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    inline void setLogEnabled(const areg::Identifier& logType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Sets the logging state for the target defined in areg::LogTarget.
     * \param   logType     The string value of areg::LogTarget.
     * \param   newValue    If 'true' enables the logging for the target output.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    inline void setLogEnabled(const areg::String& logType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Sets the logging state for the target defined in areg::LogTarget.
     * \param   logType     The logging target to set enabling flag.
     * \param   newValue    If 'true' enables the logging for the target output.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogEnabled(areg::LogTarget logType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Returns the path of the log file that contains messages.
     **/
    areg::String getLogFileLocation() const;

    /**
     * \brief   Sets the path of the log file that contains messages.
     * \param   newValue    Relative of absolute file with or without a mask to write logs.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    inline void setLogFileLocation(const areg::String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns flag indicating whether the log messages should be added in the existing file or not.
     **/
    bool getLogFileAppend() const;

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
    uint32_t getLogRemoteQueueSize() const;

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
    areg::String getLogLayoutEnter() const;

    /**
     * \brief   Sets the layout format string of the log enter scope message.
     * \param   newValue    The new layout format.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogLayoutEnter(const areg::String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns the layout format of the log message.
     **/
    areg::String getLogLayoutMessage() const;

    /**
     * \brief   Sets the layout format string of the log message.
     * \param   newValue    The new layout format.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogLayoutMessage(const areg::String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns the layout format of the log e exit cope message.
     **/
    areg::String getLogLayoutExit() const;

    /**
     * \brief   Sets the layout format string of the log exit scope message.
     * \param   newValue    The new layout format.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setLogLayoutExit(const areg::String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns the list of log scope properties of the module.
     **/
    inline std::vector<Property> getModuleLogScopes() const;

    /**
     * \brief   Gets the list of log scope properties of the module.
     * \param[out]  scopeList   On output this contains the list of log scopes properties of the module.
     * \return  The number scopes in the list.
     **/
    uint32_t getModuleLogScopes(std::vector<Property>& scopeList) const;

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
    void addModuleLogScope(const areg::String& scopeName, const areg::String& prio);

    /**
     * \brief   Adds a property of a scope with the priority value to the existing list of scope of the module.
     * \param   scopeName   The name of the scope to add.
     * \param   prio        The scope priority of the scope.
     **/
    void addModuleLogScope(const areg::String& scopeName, uint32_t prio);

    /**
     * \brief   Remotes a property entry of specified scope name.
     * \param   scopeName   The name of the scope to remove.
     * \return  Returns true if succeeded to remove the scope.
     **/
    bool removeScope(const areg::String& scopeName);

    /**
     * \brief   Removes all scopes of the module and return number of removed entries.
     **/
    int32_t removeModuleScopes();

/************************************************************************
 * Remote service properties.
 ************************************************************************/

    /**
     * \brief   Returns list of remote services.
     **/
    std::vector<areg::Identifier> getServiceList() const;

    /**
     * \brief   Returns list of connection identifiers of the remote service.
     * \param   service The remote service, which identifiers should be initialized.
     **/
    std::vector<areg::Identifier> getRemoteServiceConnections(const areg::String& service) const;

    /**
     * \brief   Returns the name of the service of the remote connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type, which name should be read out.
     **/
    areg::String getRemoteServiceName(const areg::String& service, const areg::String& connectType) const;

    /**
     * \brief   Returns the name of the service of the remote connection.
     * \param   serviceType The remote service.
     * \param   connectType The connection type, which name should be read out.
     **/
    areg::String getRemoteServiceName(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const;

    /**
     * \brief   Returns the remote service connection enabled or disabled flag.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type, which name should be read out.
     * \return  If returns true, the specified remote service supports the specified connection type.
     *          Otherwise, it does not support
     **/
    bool getRemoteServiceEnable(const areg::String& service, const areg::String& connectType) const;

    /**
     * \brief   Returns the remote service connection enabled or disabled flag.
     * \param   serviceType The remote service.
     * \param   connectType The connection type, which name should be read out.
     * \return  If returns true, the specified remote service supports the specified connection type.
     *          Otherwise, it does not support
     **/
    bool getRemoteServiceEnable(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const;

    /**
     * \brief   Sets the enabled flag of the specified connection of the remote service.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServiceEnable(const areg::String& service, const areg::String& connectType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Sets the enabled flag of the specified connection of the remote service.
     * \param   serviceType The remote service.
     * \param   connectType The connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServiceEnable(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, bool newValue, bool isTemporary = false);

    /**
     * \brief   Returns the address of the remote service that supports the specified connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type, which name should be read out.
     **/
    areg::String getRemoteServiceAddress(const areg::String& service, const areg::String& connectType) const;

    /**
     * \brief   Returns the address of the remote service that supports the specified connection.
     * \param   serviceType The remote service.
     * \param   connectType The connection type, which name should be read out.
     **/
    areg::String getRemoteServiceAddress(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const;

    /**
     * \brief   Sets the address of the remote service to establish specified connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServiceAddress(const areg::String& service, const areg::String& connectType, const areg::String& newValue, bool isTemporary = false);

    /**
     * \brief   Sets the address of the remote service to establish specified connection.
     * \param   serviceType The remote service.
     * \param   connectType The connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServiceAddress(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, const areg::String& newValue, bool isTemporary = false);

    /**
     * \brief   Returns the port number the remote service that supports the specified connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type, which name should be read out.
     **/
    uint16_t getRemoteServicePort(const areg::String& service, const areg::String& connectType) const;

    /**
     * \brief   Returns the port number the remote service that supports the specified connection.
     * \param   serviceType The remote service.
     * \param   connectType The connection type, which name should be read out.
     **/
    uint16_t getRemoteServicePort(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const;

    /**
     * \brief   Sets the port number of the remote service to establish specified connection.
     * \param   service     The string value of the remote service.
     * \param   connectType The string value of the connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServicePort(const areg::String& service, const areg::String& connectType, uint16_t newValue, bool isTemporary = false);

    /**
     * \brief   Sets the port number of the remote service to establish specified connection.
     * \param   serviceType The remote service.
     * \param   connectType The connection type to set the value.
     * \param   isTemporary Flag, indicating whether the modification is temporary or not.
     *                      The temporary changes are not saved in the configuration file.
     **/
    void setRemoteServicePort(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, uint16_t newValue, bool isTemporary = false);

    /**
     * \brief   Returns the log database property entry of specified position.
     * \param   whichPosition   The position of log database property.
     **/
    areg::String getLogDatabaseProperty(const areg::String& whichPosition);

    /**
     * \brief   Sets the permanent or temporary value of the log database of the specified position.
     * \param   whichPosition   The position of the log database to set the value.
     * \param   newValue        The value to set for the specified position of the log database.
     * \param   isTemporary     The flag, indicating whether the new value is permanent of temporary.
     *                          Unlike the permanent value, the temporary values are not saved in
     *                          the configuration file.
     **/
    void setLogDatabaseProperty(const areg::String & whichPosition, const areg::String & newValue, bool isTemporary = false);

    /**
     * \brief   Returns the buffer default block size set in the configuration file.
     * \param   whichModule     The name of the module or `*` for generic settings.
     * \return  Returns the buffer block size in bytes to set when grow buffer.
     **/
    uint32_t getDefaultBufferBlockSize(const areg::String& whichModule = areg::EmptyStringA);

    /**
     * \brief   Returns the default message queue size set in the configuration file.
     * \param   whichModule     The name of the module or `*` for generic settings.
     * \return  Returns the default message queue size to set for message queues.
     **/
    uint32_t getDefaultMessageQueueSize(const areg::String& whichModule = areg::EmptyStringA);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The module name (the process name) to use when initialize configuration.
     **/
    const areg::String    mModule;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   The list of writable properties of the configuration, which can be modified for current process.
     **/
    areg::ListProperties   mWritableProperties;

    /**
     * \brief   The list of read-only properties of the configuration, which cannot be modified.
     **/
    areg::ListProperties  mReadonlyProperties;

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
    areg::String          mFilePath;

    /**
     * \brief   Synchronization object for multithreading.
     **/
    mutable Mutex   mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(ConfigManager);
};

//////////////////////////////////////////////////////////////////////////
// Configuration inline methods
//////////////////////////////////////////////////////////////////////////
inline bool ConfigManager::isConfigured() const
{
    Lock lock(mLock);
    return mIsConfigured;
}

inline const areg::String& ConfigManager::getConfigFile() const
{
    Lock lock(mLock);
    return mFilePath;
}

inline const areg::ListProperties& ConfigManager::getReadonlyProperties() const
{
    Lock lock(mLock);
    return mReadonlyProperties;
}

inline const areg::ListProperties& ConfigManager::getModuleProperties() const
{
    Lock lock(mLock);
    return mWritableProperties;
}

inline bool ConfigManager::lock() const
{
    return mLock.lock();
}

inline bool ConfigManager::unlock() const
{
    return mLock.unlock();
}

inline bool ConfigManager::tryLock() const
{
    return mLock.tryLock();
}

inline bool ConfigManager::isLocked() const
{
    return mLock.isLocked();
}

inline bool ConfigManager::existProperty(const areg::String& section, const areg::String& property, const areg::String& position) const
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

inline void ConfigManager::setModuleProperty(const PropertyKey& key, const areg::String& value, bool isTemporary)
{
    setModuleProperty(key.getSection(), key.getProperty(), key.getPosition(), value, key.getKeyType(), isTemporary);
}

inline const areg::PropertyValue * ConfigManager::getPropertyValue( const areg::String& section
                                                            , const areg::String& property
                                                            , const areg::String& position /*= areg::String::EmptyString*/
                                                            , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/) const
{
    Lock lock(mLock);

    const Property* result{ getProperty(section, property, position, keyType)};
    return (result != nullptr ? &result->getValue() : nullptr);
}

inline const areg::PropertyValue* ConfigManager::getPropertyValue(const PropertyKey& key) const
{
    return getPropertyValue(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline const areg::PropertyValue* ConfigManager::getModulePropertyValue(const PropertyKey& key) const
{
    return getModulePropertyValue(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline const areg::PropertyValue * ConfigManager::getModulePropertyValue( const areg::String& section
                                                                  , const areg::String& property
                                                                  , const areg::String& position /*= areg::String::EmptyString*/
                                                                  , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/) const
{
    Lock lock(mLock);

    const Property* result{ getModuleProperty(section, property, position, keyType) };
    return (result != nullptr ? &result->getValue() : nullptr);
}

inline areg::PropertyValue* ConfigManager::getModulePropertyValue(const PropertyKey& key)
{
    return getModulePropertyValue(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline areg::PropertyValue * ConfigManager::getModulePropertyValue( const areg::String& section
                                                           , const areg::String& property
                                                           , const areg::String& position /*= areg::String::EmptyString*/
                                                           , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/)
{
    Lock lock(mLock);

    const Property* result{ getModuleProperty(section, property, position, keyType) };
    return (result != nullptr ? &const_cast<Property *>(result)->getValue() : nullptr);
}

inline int32_t ConfigManager::addModuleProperties(const areg::ListProperties& propList)
{
    int32_t result{ 0 };
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

inline void ConfigManager::replaceModuleProperty(const areg::ListProperties& listProperties)
{
    Lock lock(mLock);
    mWritableProperties = listProperties;
}

inline void ConfigManager::removeModuleProperty(const PropertyKey& key)
{
    removeModuleProperty(key.getSection(), key.getProperty(), key.getPosition(), key.getKeyType());
}

inline void ConfigManager::releaseModuleProperties()
{
    Lock lock(mLock);
    mWritableProperties.clear();
}

inline void ConfigManager::releaseProperties()
{
    Lock lock(mLock);
    mIsConfigured = false;
    mWritableProperties.clear();
    mReadonlyProperties.clear();
}

inline void ConfigManager::setLoggingStatus(bool newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogStatus;
    const areg::ConfigKey& key = areg::getLogStatus();

    setModuleProperty(key.section, key.property, key.position, areg::String::makeString(newValue), confKey, isTemporary);
}

inline bool ConfigManager::getLogEnabled(const areg::Identifier& logType) const
{
    return getLogEnabled(logType.getName());
}

inline void ConfigManager::setLogEnabled(const areg::String& logType, bool newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogEnable;
    const areg::ConfigKey& key = areg::getLogEnable();
    setModuleProperty(key.section, key.property, logType, areg::String::makeString(newValue), confKey, isTemporary);
}

inline void ConfigManager::setLogEnabled(const areg::Identifier& logType, bool newValue, bool isTemporary /*= false*/)
{
    setLogEnabled(logType.getName(), newValue, isTemporary);
}

inline void ConfigManager::setLogFileLocation(const areg::String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogFileLocation;
    const areg::ConfigKey& key = areg::getLogFileLocation();
    setModuleProperty(key.section, key.property, key.position, newValue, confKey, isTemporary);
}

inline std::vector<Property> ConfigManager::getModuleLogScopes() const
{
    std::vector<Property> result;
    getModuleLogScopes(result);
    return result;
}

#endif // AREG_PERSIST_CONFIGMANAGER_HPP
