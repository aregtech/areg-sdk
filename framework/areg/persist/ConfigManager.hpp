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
#include "areg/base/areg_global.h"

#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Version.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/persist/Property.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"

#include <vector>
namespace areg {

/************************************************************************
 * Dependencies.
 ************************************************************************/
class FileBase;
class ConfigListener;

//////////////////////////////////////////////////////////////////////////
// ConfigManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Manages configuration properties stored in files, separating read-only global properties
 *          from application-specific writable properties.
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
     * \brief   Returns true if configuration has been initialized successfully.
     **/
    [[nodiscard]]
    inline bool is_configured() const noexcept;

    /**
     * \brief   Returns the path of the configuration file.
     **/
    [[nodiscard]]
    inline const String& config_file() const;

    /**
     * \brief   Returns the list of read-only global configuration properties.
     **/
    [[nodiscard]]
    inline const areg::ListProperties & readonly_properties() const;

    /**
     * \brief   Returns the list of application-specific writable configuration properties.
     **/
    [[nodiscard]]
    inline const areg::ListProperties & module_properties() const;

    /**
     * \brief   Returns a reference to the internal mutex, enabling RAII locking:
     *          `Lock guard(config.lockable())`.
     *
     * Use this to hold the lock across multiple operations that must be atomic.
     * Individual ConfigManager methods already acquire the lock internally, so there
     * is no need to wrap a single call — only use `lockable()` when you need to hold
     * the lock across a sequence of calls.
     **/
    [[nodiscard]]
    inline Mutex& lockable() noexcept;

    /**
     * \brief   Returns a const reference to the internal mutex.
     *
     * \see lockable()
     **/
    [[nodiscard]]
    inline const Mutex& lockable() const noexcept;

    /**
     * \brief   Returns true if the specified property exists.
     *
     * \param   key     The property key to check.
     **/
    [[nodiscard]]
    bool exist(const PropertyKey& key) const noexcept;

    /**
     * \brief   Returns true if a property with specified parameters exists for the current module.
     *
     * \param   section     The section of the key. If '*', searches only in the read-only list.
     * \param   propNme     The property name to search.
     * \param   position    The optional position of the key to search.
     * \return  Returns true if found either a module-specific entry or a global entry.
     **/
    [[nodiscard]]
    inline bool exist(const String& section, const String& propNme, const String& position = String::EmptyString) const;

    /**
     * \brief   Returns a merged list of read-only and writable properties for the specified
     *          section.
     *
     * \param   section     The section name to retrieve properties for.
     **/
    [[nodiscard]]
    areg::ListProperties section_properties(const String& section) const;

    /**
     * \brief   Returns the pointer to a property searched by specified parameters in both read-only
     *          and writable lists.
     *
     * \param   section     The section to search in the list of configuration properties.
     * \param   propNme     The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns a valid pointer if a property matching the specified parameters is found;
     *          otherwise returns nullptr.
     **/
    [[nodiscard]]
    const Property * property( const String& section
                             , const String& propNme
                             , const String& position
                             , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey) const noexcept;

    /**
     * \brief   Returns the pointer to a property searched by the specified key. The module value of
     *          the key is ignored, using only the current process module name instead.
     *
     * \param   key     The key containing section, property, and position information to search.
     * \return  Returns a valid pointer if a property matching the specified parameters is found;
     *          otherwise returns nullptr.
     **/
    [[nodiscard]]
    inline const Property * property(const PropertyKey& key) const;

    /**
     * \brief   Returns the pointer to a module-specific property searched by specified parameters
     *          in the writable list only.
     *
     * \param   section     The section to search in the list of configuration properties.
     * \param   propNme     The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns a valid pointer if a module property matching the specified parameters is
     *          found; otherwise returns nullptr.
     **/
    const Property * module_property( const String& section
                                    , const String& propNme
                                    , const String& position
                                    , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey) const noexcept;

    /**
     * \brief   Returns the pointer to a module-specific property searched by the specified key. The
     *          module value of the key is ignored, using only the current process module name
     *          instead.
     *
     * \param   key     The key containing section, property, and position information to search.
     * \return  Returns a valid pointer if a module property matching the specified parameters is
     *          found; otherwise returns nullptr.
     **/
    [[nodiscard]]
    inline const Property* module_property(const PropertyKey& key) const;

    /**
     * \brief   Adds a new or updates an existing configuration entry in the writable list.
     *          Optionally marks the entry as temporary so it is ignored when saving.
     *
     * \param   section         The section to search in the list of configuration properties.
     * \param   propNme         The property to search in the list of configuration properties.
     * \param   position        The optional position to search in the list of configuration
     *                          properties.
     * \param   value           The new value to set in the specified configuration entry.
     * \param   keyType         The property key type.
     * \param   temporary       If true, the change is not saved to the configuration file.
     **/
    void set_module_property( const String& section
                            , const String& propNme
                            , const String& position
                            , const String& value
                            , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey
                            , bool temporary = false);

    /**
     * \brief   Adds a new or updates an existing configuration entry in the writable list using a
     *          PropertyKey. Optionally marks the entry as temporary so it is ignored when saving.
     *
     * \param   key             The key containing section, property, and position information.
     * \param   value           The new value to set in the specified configuration entry.
     * \param   temporary       If true, the change is not saved to the configuration file.
     **/
    inline void set_module_property( const PropertyKey& key, const String& value, bool temporary = false);

    /**
     * \brief   Returns the pointer to the value of a property searched by specified parameters in
     *          both read-only and writable lists.
     *
     * \param   section     The section to search in the list of configuration properties.
     * \param   propNme     The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns a valid pointer to property value if a property matching the specified
     *          parameters is found; otherwise returns nullptr.
     **/
    [[nodiscard]]
    inline const PropertyValue * property_value( const String& section
                                               , const String& propNme
                                               , const String& position = String::EmptyString
                                               , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey) const;

    /**
     * \brief   Returns the pointer to the value of a property searched by the specified key in both
     *          read-only and writable lists.
     *
     * \param   key     The key containing section, property, and position information.
     * \return  Returns a valid pointer to property value if a property matching the specified
     *          parameters is found; otherwise returns nullptr.
     **/
    [[nodiscard]]
    inline const PropertyValue * property_value(const PropertyKey& key) const;

    /**
     * \brief   Returns the pointer to the value of a module-specific property searched by specified
     *          parameters in the writable list only.
     *
     * \param   section     The section to search in the list of configuration properties.
     * \param   propNme     The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns a valid pointer if a module property matching the specified parameters is
     *          found; otherwise returns nullptr.
     **/
    [[nodiscard]]
    inline const PropertyValue * module_property_value( const String& section
                                                       , const String& propNme
                                                       , const String& position = String::EmptyString
                                                       , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey) const;
    /**
     * \brief   Returns a non-const pointer to the value of a module-specific property, allowing
     *          modification. Searched in the writable list only.
     *
     * \param   section     The section to search in the list of configuration properties.
     * \param   propNme     The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns a valid pointer if a module property matching the specified parameters is
     *          found; otherwise returns nullptr.
     * \note    Allows modification of the returned value.
     **/
    [[nodiscard]]
    inline PropertyValue * module_property_value( const String& section
                                                 , const String& propNme
                                                 , const String& position = String::EmptyString
                                                 , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey);

    /**
     * \brief   Returns the pointer to the value of a module-specific property searched by the
     *          specified key in the writable list only.
     *
     * \param   key     The key containing section, property, and position information.
     * \return  Returns a valid pointer if a module property matching the specified parameters is
     *          found; otherwise returns nullptr.
     **/
    [[nodiscard]]
    inline const PropertyValue* module_property_value(const PropertyKey& key) const;
    /**
     * \brief   Returns a non-const pointer to the value of a module-specific property using a key,
     *          allowing modification. Searched in the writable list only.
     *
     * \param   key     The key containing section, property, and position information.
     * \return  Returns a valid pointer if a module property matching the specified parameters is
     *          found; otherwise returns nullptr.
     * \note    Allows modification of the returned value.
     **/
    [[nodiscard]]
    inline PropertyValue* module_property_value(const PropertyKey& key);

    /**
     * \brief   Adds new properties to the module configuration.
     *
     * \param   propList    The list of new properties to add.
     * \return  Returns the number of properties added to the writable list.
     **/
    inline int32_t add_module_properties(const areg::ListProperties& propList);

    /**
     * \brief   Replaces the entire list of writable properties. All existing properties are
     *          discarded.
     *
     * \param   listProperties      The new list of properties to set.
     **/
    inline void replace_module_property(const areg::ListProperties& listProperties);

    /**
     * \brief   Removes a configuration entry from the writable list.
     *
     * \param   section     The section to search in the list of configuration properties.
     * \param   propNme     The property to search in the list of configuration properties.
     * \param   position    The optional position to search in the list of configuration properties.
     * \param   keyType     The property key type.
     **/
    void remove_module_property( const String& section
                               , const String& propNme
                               , const String& position = String::EmptyString
                               , areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey);

    /**
     * \brief   Removes a configuration entry from the writable list using a PropertyKey.
     *
     * \param   key     The key containing section, property, and position information.
     **/
    inline void remove_module_property(const PropertyKey& key);

    /**
     * \brief   Removes all configuration entries from the writable list matching the specified
     *          section and property.
     *
     * \param   section     The section to search in the list of configuration properties.
     * \param   propNme     The property to search in the list of configuration properties.
     * \param   keyType     The property key type.
     * \return  Returns the number of entries removed from the writable list.
     **/
    int32_t remove_module_properties(const String& section, const String& propNme, areg::ConfigEntry keyType = areg::ConfigEntry::AnyKey);

    /**
     * \brief   Removes all configuration entries from the writable list matching the specified
     *          section.
     *
     * \param   section     The section to search in the list of configuration properties.
     **/
    void remove_section_properties(const String& section);

    /**
     * \brief   Reads configuration from the specified file. If filePath is empty, uses the default
     *          configuration file path.
     *
     * \param   filePath    The relative or full path to the configuration file. If empty, uses
     *                      areg::DEFAULT_CONFIG_FILE.
     * \param   listener    Optional pointer to a ConfigListener to notify before and after reading.
     *                      If nullptr, no notification is sent.
     * \return  Returns true if the configuration was successfully read and initialized.
     **/
    bool read_config(const String& filePath = String::EmptyString, ConfigListener * listener = nullptr) noexcept;

    /**
     * \brief   Reads configuration from the specified file object opened for reading.
     *
     * \param   file        The file object opened for reading.
     * \param   listener    Optional pointer to a ConfigListener to notify before and after reading.
     *                      If nullptr, no notification is sent.
     * \return  Returns true if the configuration was successfully read and initialized.
     **/
    bool read_config(const FileBase& file, ConfigListener * listener = nullptr) noexcept;

    /**
     * \brief   Saves the current configuration to the specified file, preserving existing entries
     *          and only updating module-specific entries not marked as temporary.
     *
     * \param   filePath    The relative or full path to the configuration file to write. If empty,
     *                      uses the previously opened file path or
     *                      areg::DEFAULT_CONFIG_FILE if no file was previously opened.
     * \param   listener    Optional pointer to a ConfigListener to notify before and after saving.
     *                      If nullptr, no notification is sent.
     * \return  Returns true if the configuration was successfully saved.
     **/
    bool save_config(const String& filePath = String::EmptyString, ConfigListener * listener = nullptr);

    /**
     * \brief   Saves the current configuration to a destination file opened for writing, reading
     *          from a source file.
     *
     * \param   srcFile     The source file containing configuration information.
     * \param[in,out] dstFile     The destination file opened for writing where configuration will
     *                            be saved.
     * \param   saveAll     If true, saves all entries including temporary ones. If false, temporary
     *                      entries are excluded.
     * \param   listener    Optional pointer to a ConfigListener to notify before and after saving.
     *                      If nullptr, no notification is sent.
     * \return  Returns true if the configuration was successfully saved.
     **/
    bool save_config(const FileBase& srcFile, FileBase& dstFile, bool saveAll, ConfigListener * listener = nullptr);

    /**
     * \brief   Sets the read-only and writable configuration entries, optionally notifying a
     *          listener.
     *
     * \param   listReadonly    The list of read-only configuration properties.
     * \param   listWritable    The list of writable configuration properties.
     * \param   listener        Optional pointer to a ConfigListener to notify when configuration
     *                          data is set. If nullptr, no notification is sent.
     **/
    void set_configuration(const areg::ListProperties& listReadonly, const areg::ListProperties& listWritable, ConfigListener * listener = nullptr);

    /**
     * \brief   Clears all module-specific (writable) configuration entries.
     **/
    inline void release_module_properties();

    /**
     * \brief   Clears all configuration entries.
     **/
    inline void release_properties();

/************************************************************************
 * Configuration properties.
 ************************************************************************/

    /**
     * \brief   Returns the version of the configuration.
     **/
    [[nodiscard]]
    Version config_version() const noexcept;

/************************************************************************
 * Log properties.
 ************************************************************************/

    /**
     * \brief   Returns the version of the logging configuration.
     **/
    [[nodiscard]]
    Version log_version() const noexcept;

    /**
     * \brief   Returns the list of available log targets.
     **/
    [[nodiscard]]
    std::vector<Identifier> log_targets() const noexcept;

    /**
     * \brief   Returns true if logging is enabled for the current module.
     **/
    [[nodiscard]]
    bool logging_status() const noexcept;

    /**
     * \brief   Sets the logging state for the current module.
     *
     * \param   newValue        If true, enables logging; if false, disables logging for the current
     *                          module.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    inline void set_logging_status(bool newValue, bool is_temporary = false);

    /**
     * \brief   Returns whether logging is enabled for the specified log target.
     *
     * \param   logType     The string value of the logging target (areg::LogTarget).
     * \return  Returns true if logging is enabled for the specified target; false otherwise.
     **/
    [[nodiscard]]
    bool log_enabled(const String& logType) const noexcept;

    /**
     * \brief   Returns whether logging is enabled for the specified log target.
     *
     * \param   logType     The Identifier object containing the areg::LogTarget value.
     * \return  Returns true if logging is enabled for the specified target; false otherwise.
     **/
    [[nodiscard]]
    inline bool log_enabled(const Identifier& logType) const;

    /**
     * \brief   Returns whether logging is enabled for the specified log target.
     *
     * \param   logType     The logging target to check.
     * \return  Returns true if logging is enabled for the specified target; false otherwise.
     **/
    [[nodiscard]]
    bool log_enabled( areg::LogTarget logType) const noexcept;

    /**
     * \brief   Sets whether logging is enabled for the specified log target.
     *
     * \param   logType         The Identifier object containing the areg::LogTarget value.
     * \param   newValue        If true, enables logging for the target; if false, disables it.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    inline void set_log_enabled(const Identifier& logType, bool newValue, bool is_temporary = false);

    /**
     * \brief   Sets whether logging is enabled for the specified log target.
     *
     * \param   logType         The string value of the logging target (areg::LogTarget).
     * \param   newValue        If true, enables logging for the target; if false, disables it.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    inline void set_log_enabled(const String& logType, bool newValue, bool is_temporary = false);

    /**
     * \brief   Sets whether logging is enabled for the specified log target.
     *
     * \param   logType         The logging target to set.
     * \param   newValue        If true, enables logging for the target; if false, disables it.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_log_enabled(areg::LogTarget logType, bool newValue, bool is_temporary = false);

    /**
     * \brief   Returns the path where log messages are written.
     **/
    [[nodiscard]]
    String log_file_location() const noexcept;

    /**
     * \brief   Sets the path of the log file where messages are written.
     *
     * \param   newValue        The relative or absolute file path with optional mask to write logs.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    inline void set_file_location(const String& newValue, bool is_temporary = false);

    /**
     * \brief   Returns true if log messages are appended to the existing log file.
     **/
    [[nodiscard]]
    bool log_file_append() const noexcept;

    /**
     * \brief   Sets whether log messages should be appended to the existing file or create a new
     *          one.
     *
     * \param   newValue        If true, log messages are appended to the existing log file; if
     *                          false, a new file is created.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_file_append(bool newValue, bool is_temporary = false);

    /**
     * \brief   Returns the maximum queue size for log messages when there is no connection to the
     *          remote logger.
     **/
    [[nodiscard]]
    uint32_t remote_queue_size() const noexcept;

    /**
     * \brief   Sets the maximum queue size for log messages when there is no connection to the
     *          remote logger.
     *
     * \param   newValue        The maximum queue size. A value of 0 means messages are sent
     *                          immediately without queuing.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_remote_queue_size(uint32_t newValue, bool is_temporary = false);

    /**
     * \brief   Returns the layout format string for log messages when entering a scope.
     **/
    [[nodiscard]]
    String log_layout_enter() const noexcept;

    /**
     * \brief   Sets the layout format string for log messages when entering a scope.
     *
     * \param   newValue        The new layout format string.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_layout_enter(const String& newValue, bool is_temporary = false);

    /**
     * \brief   Returns the layout format string for log messages.
     **/
    [[nodiscard]]
    String log_layout_message() const noexcept;

    /**
     * \brief   Sets the layout format string for log messages.
     *
     * \param   newValue        The new layout format string.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_layout_message(const String& newValue, bool is_temporary = false);

    /**
     * \brief   Returns the layout format string for log messages when exiting a scope.
     **/
    [[nodiscard]]
    String log_layout_exit() const;

    /**
     * \brief   Sets the layout format string for log messages when exiting a scope.
     *
     * \param   newValue        The new layout format string.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_layout_exit(const String& newValue, bool is_temporary = false);

    /**
     * \brief   Returns the list of log scope properties for the module.
     **/
    inline std::vector<Property> module_log_scopes() const;

    /**
     * \brief   Gets the list of log scope properties for the module.
     *
     * \param[out] scopeList       The vector to be filled with the module's log scope properties.
     * \return  Returns the number of scopes in the list.
     **/
    uint32_t module_log_scopes(std::vector<Property>& scopeList) const noexcept;

    /**
     * \brief   Adds log scope properties to the module's list, optionally replacing existing scopes
     *          with the same name.
     *
     * \param   scopeList       The list of scope properties to add.
     * \param   search          If true, replaces existing scopes with matching names. If false,
     *                          adds all scopes to the list.
     **/
    void add_log_scopes(const std::vector<Property>& scopeList, bool search);

    /**
     * \brief   Adds a log scope property with a priority value to the module's scope list.
     *
     * \param   scopeName       The name of the scope to add.
     * \param   prio            The scope priority or priorities separated by logical OR ('|').
     **/
    void add_log_scope(const String& scopeName, const String& prio);

    /**
     * \brief   Adds a log scope property with a numeric priority value to the module's scope list.
     *
     * \param   scopeName       The name of the scope to add.
     * \param   prio            The numeric scope priority value.
     **/
    void add_log_scope(const String& scopeName, uint32_t prio);

    /**
     * \brief   Removes a log scope property by its name.
     *
     * \param   scopeName       The name of the scope to remove.
     * \return  Returns true if the scope was successfully removed; false if not found.
     **/
    bool remove_scope(const String& scopeName);

    /**
     * \brief   Removes all log scopes from the module.
     *
     * \return  Returns the number of scope entries removed.
     **/
    int32_t remove_module_scopes();

/************************************************************************
 * Remote service properties.
 ************************************************************************/

    /**
     * \brief   Returns the list of configured remote service identifiers.
     **/
    [[nodiscard]]
    std::vector<Identifier> service_list() const noexcept;

    /**
     * \brief   Returns the list of connection identifiers for the specified remote service.
     *
     * \param   service     The remote service name.
     **/
    [[nodiscard]]
    std::vector<Identifier> remote_service_connections(const String& service) const noexcept;

    /**
     * \brief   Returns the service name for the specified remote service and connection type.
     *
     * \param   service         The string value of the remote service.
     * \param   connectType     The string value of the connection type.
     **/
    [[nodiscard]]
    String remote_service_name(const String& service, const String& connectType) const noexcept;

    /**
     * \brief   Returns the service name for the specified remote service and connection type.
     *
     * \param   serviceType     The remote service type.
     * \param   connectType     The connection type.
     **/
    [[nodiscard]]
    String remote_service_name(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept;

    /**
     * \brief   Returns whether the specified remote service connection is enabled.
     *
     * \param   service         The string value of the remote service.
     * \param   connectType     The string value of the connection type.
     * \return  Returns true if the specified remote service supports the specified connection type;
     *          false otherwise.
     **/
    [[nodiscard]]
    bool remote_service_enable(const String& service, const String& connectType) const noexcept;

    /**
     * \brief   Returns whether the specified remote service connection is enabled.
     *
     * \param   serviceType     The remote service type.
     * \param   connectType     The connection type.
     * \return  Returns true if the specified remote service supports the specified connection type;
     *          false otherwise.
     **/
    [[nodiscard]]
    bool remote_service_enable(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept;

    /**
     * \brief   Sets whether the specified remote service connection is enabled.
     *
     * \param   service         The string value of the remote service.
     * \param   connectType     The string value of the connection type.
     * \param   newValue        If true, enables the connection; if false, disables it.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_service_enable(const String& service, const String& connectType, bool newValue, bool is_temporary = false);

    /**
     * \brief   Sets whether the specified remote service connection is enabled.
     *
     * \param   serviceType     The remote service type.
     * \param   connectType     The connection type.
     * \param   newValue        If true, enables the connection; if false, disables it.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_service_enable(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, bool newValue, bool is_temporary = false);

    /**
     * \brief   Returns the address of the remote service for the specified connection type.
     *
     * \param   service         The string value of the remote service.
     * \param   connectType     The string value of the connection type.
     **/
    String remote_service_address(const String& service, const String& connectType) const noexcept;

    /**
     * \brief   Returns the address of the remote service for the specified connection type.
     *
     * \param   serviceType     The remote service type.
     * \param   connectType     The connection type.
     **/
    String remote_service_address(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept;

    /**
     * \brief   Sets the address of the remote service for the specified connection type.
     *
     * \param   service         The string value of the remote service.
     * \param   connectType     The string value of the connection type.
     * \param   newValue        The new address value.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_service_address(const String& service, const String& connectType, const String& newValue, bool is_temporary = false);

    /**
     * \brief   Sets the address of the remote service for the specified connection type.
     *
     * \param   serviceType     The remote service type.
     * \param   connectType     The connection type.
     * \param   newValue        The new address value.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_service_address(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, const String& newValue, bool is_temporary = false);

    /**
     * \brief   Returns the port number of the remote service for the specified connection type.
     *
     * \param   service         The string value of the remote service.
     * \param   connectType     The string value of the connection type.
     **/
    uint16_t remote_service_port(const String& service, const String& connectType) const noexcept;

    /**
     * \brief   Returns the port number of the remote service for the specified connection type.
     *
     * \param   serviceType     The remote service type.
     * \param   connectType     The connection type.
     **/
    uint16_t remote_service_port(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept;

    /**
     * \brief   Sets the port number of the remote service for the specified connection type.
     *
     * \param   service         The string value of the remote service.
     * \param   connectType     The string value of the connection type.
     * \param   newValue        The new port number value.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_service_port(const String& service, const String& connectType, uint16_t newValue, bool is_temporary = false);

    /**
     * \brief   Sets the port number of the remote service for the specified connection type.
     *
     * \param   serviceType     The remote service type.
     * \param   connectType     The connection type.
     * \param   newValue        The new port number value.
     * \param   is_temporary    If true, the change is not saved to the configuration file.
     **/
    void set_service_port(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, uint16_t newValue, bool is_temporary = false);

    /**
     * \brief   Returns the log database property for the specified position.
     *
     * \param   whichPosition       The position identifier of the log database property.
     **/
    String log_database_property(const String& whichPosition) const noexcept;

    /**
     * \brief   Sets the log database property value for the specified position.
     *
     * \param   whichPosition       The position identifier of the log database property.
     * \param   newValue            The new value to set.
     * \param   is_temporary        If true, the change is not saved to the configuration file.
     **/
    void set_db_property(const String & whichPosition, const String & newValue, bool is_temporary = false);

    /**
     * \brief   Returns the configured SO_SNDBUF size (bytes) for the remote service connection.
     *          Used to override the compile-time default on every accepted/connected socket.
     *
     * \param   service         The string value of the remote service (e.g. "router", "logger").
     * \param   connectType     The string value of the connection type (e.g. "tcpip").
     * \return  Returns the configured send-buffer size, or the compile-time default if not set.
     **/
    uint32_t remote_service_sndbuf(const String& service, const String& connectType) const noexcept;

    /**
     * \brief   Returns the configured SO_SNDBUF size (bytes) for the remote service connection.
     *
     * \param   serviceType     The remote service type.
     * \param   connectType     The connection type.
     * \return  Returns the configured send-buffer size, or the compile-time default if not set.
     **/
    uint32_t remote_service_sndbuf(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept;

    /**
     * \brief   Returns the configured SO_RCVBUF size (bytes) for the remote service connection.
     *
     * \param   service         The string value of the remote service (e.g. "router", "logger").
     * \param   connectType     The string value of the connection type (e.g. "tcpip").
     * \return  Returns the configured receive-buffer size, or the compile-time default if not set.
     **/
    uint32_t remote_service_rcvbuf(const String& service, const String& connectType) const noexcept;

    /**
     * \brief   Returns the configured SO_RCVBUF size (bytes) for the remote service connection.
     *
     * \param   serviceType     The remote service type.
     * \param   connectType     The connection type.
     * \return  Returns the configured receive-buffer size, or the compile-time default if not set.
     **/
    uint32_t remote_service_rcvbuf(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept;

    /**
     * \brief   Returns the default buffer block size for growing buffers.
     *
     * \param   whichModule     The module name or '*' for generic settings.
     * \return  Returns the buffer block size in bytes.
     **/
    uint32_t buffer_block_size(const String& whichModule = areg::EmptyStringA) noexcept;

    /**
     * \brief   Returns the default message queue size for message queues.
     *
     * \param   whichModule     The module name or '*' for generic settings.
     * \return  Returns the default message queue size.
     **/
    uint32_t message_queue_size(const String& whichModule = areg::EmptyStringA) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The module name (the process name) to use when initialize configuration.
     **/
    const String    mModule;

#if defined(_MSC_VER)
    #pragma warning(push)
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

#if defined(_MSC_VER)
    #pragma warning(pop)
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
    AREG_NOCOPY_NOMOVE(ConfigManager);
};

//////////////////////////////////////////////////////////////////////////
// Configuration inline methods
//////////////////////////////////////////////////////////////////////////
inline bool ConfigManager::is_configured() const noexcept
{
    Lock lock(mLock);
    return mIsConfigured;
}

inline const String& ConfigManager::config_file() const
{
    Lock lock(mLock);
    return mFilePath;
}

inline const areg::ListProperties& ConfigManager::readonly_properties() const
{
    Lock lock(mLock);
    return mReadonlyProperties;
}

inline const areg::ListProperties& ConfigManager::module_properties() const
{
    Lock lock(mLock);
    return mWritableProperties;
}

inline Mutex& ConfigManager::lockable() noexcept
{
    return mLock;
}

inline const Mutex& ConfigManager::lockable() const noexcept
{
    return mLock;
}

inline bool ConfigManager::exist(const String& section, const String& propNme, const String& position) const
{
    return exist(PropertyKey(section, mModule, propNme, position));
}

inline const Property* ConfigManager::property(const PropertyKey& key) const
{
    return property(key.section(), key.property(), key.position(), key.key_type());
}

inline const Property* ConfigManager::module_property(const PropertyKey& key) const
{
    return module_property(key.section(), key.property(), key.position(), key.key_type());
}

inline void ConfigManager::set_module_property(const PropertyKey& key, const String& value, bool is_temporary)
{
    set_module_property(key.section(), key.property(), key.position(), value, key.key_type(), is_temporary);
}

inline const PropertyValue * ConfigManager::property_value( const String& section
                                                            , const String& propNme
                                                            , const String& position /*= String::EmptyString*/
                                                            , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/) const
{
    Lock lock(mLock);

    const Property* result{ property(section, propNme, position, keyType)};
    return (result != nullptr ? &result->value() : nullptr);
}

inline const PropertyValue* ConfigManager::property_value(const PropertyKey& key) const
{
    return property_value(key.section(), key.property(), key.position(), key.key_type());
}

inline const PropertyValue* ConfigManager::module_property_value(const PropertyKey& key) const
{
    return module_property_value(key.section(), key.property(), key.position(), key.key_type());
}

inline const PropertyValue * ConfigManager::module_property_value( const String& section
                                                                  , const String& propNme
                                                                  , const String& position /*= String::EmptyString*/
                                                                  , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/) const
{
    Lock lock(mLock);

    const Property* result{ module_property(section, propNme, position, keyType) };
    return (result != nullptr ? &result->value() : nullptr);
}

inline PropertyValue* ConfigManager::module_property_value(const PropertyKey& key)
{
    return module_property_value(key.section(), key.property(), key.position(), key.key_type());
}

inline PropertyValue * ConfigManager::module_property_value( const String& section
                                                           , const String& propNme
                                                           , const String& position /*= String::EmptyString*/
                                                           , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/)
{
    Lock lock(mLock);

    const Property* result{ module_property(section, propNme, position, keyType) };
    return (result != nullptr ? &const_cast<Property *>(result)->value() : nullptr);
}

inline int32_t ConfigManager::add_module_properties(const areg::ListProperties& propList)
{
    int32_t result{ 0 };
    Lock lock(mLock);
    const std::vector<Property>& list = propList.data();
    for (const auto& prop : list)
    {
        if (!exist(prop.key()))
        {
            ++result;
            mWritableProperties.add(prop);
        }
    }

    return result;
}

inline void ConfigManager::replace_module_property(const areg::ListProperties& listProperties)
{
    Lock lock(mLock);
    mWritableProperties = listProperties;
}

inline void ConfigManager::remove_module_property(const PropertyKey& key)
{
    remove_module_property(key.section(), key.property(), key.position(), key.key_type());
}

inline void ConfigManager::release_module_properties()
{
    Lock lock(mLock);
    mWritableProperties.clear();
}

inline void ConfigManager::release_properties()
{
    Lock lock(mLock);
    mIsConfigured = false;
    mWritableProperties.clear();
    mReadonlyProperties.clear();
}

inline void ConfigManager::set_logging_status(bool newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogStatus;
    const areg::ConfigKey& key = areg::log_status();

    set_module_property(key.section, key.property, key.position, String::make_string(newValue), confKey, is_temporary);
}

inline bool ConfigManager::log_enabled(const Identifier& logType) const
{
    return log_enabled(logType.name());
}

inline void ConfigManager::set_log_enabled(const String& logType, bool newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogEnable;
    const areg::ConfigKey& key = areg::log_enable();
    set_module_property(key.section, key.property, logType, String::make_string(newValue), confKey, is_temporary);
}

inline void ConfigManager::set_log_enabled(const Identifier& logType, bool newValue, bool is_temporary /*= false*/)
{
    set_log_enabled(logType.name(), newValue, is_temporary);
}

inline void ConfigManager::set_file_location(const String& newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogFileLocation;
    const areg::ConfigKey& key = areg::log_file_location();
    set_module_property(key.section, key.property, key.position, newValue, confKey, is_temporary);
}

inline std::vector<Property> ConfigManager::module_log_scopes() const
{
    std::vector<Property> result;
    module_log_scopes(result);
    return result;
}

} // namespace areg
#endif // AREG_PERSIST_CONFIGMANAGER_HPP
