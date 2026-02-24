/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/ConfigManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, ConfigManager object to read configuration file.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/persist/ConfigManager.hpp"

#include "areg/appbase/AppDefs.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/persist/ConfigListener.hpp"

namespace
{
    inline uint32_t _findPosition( const ArrayList<Property>& propList
                                 , uint32_t startAt
                                 , const String& section
                                 , const String& module
                                 , const String& property
                                 , const String& position
                                 , bool exact
                                 , NEPersistence::ConfigEntry configKey)
    {
        uint32_t result { NECommon::INVALID_POSITION };
        const auto& list{ propList.getData() };
        uint32_t count{ static_cast<uint32_t>(list.size()) };

        for (uint32_t pos = startAt; pos < count; ++ pos)
        {
            const PropertyKey& key = propList[pos].key();
            if ((configKey == NEPersistence::ConfigEntry::AnyKey) || (configKey == key.key_type()))
            {
                if ((exact && key.is_exact_property(section, module, property, position)) ||
                    (!exact && key.is_module_property(section, module, property, position)))
                {
                    result = pos;
                    break;
                }
            }
        }

        return result;
    }

    template <typename Type>
    inline void _setPositionValue( ArrayList<Property>& writeList
                                 , const ArrayList<Property>& readList
                                 , const String& section
                                 , const String& module
                                 , const String& property
                                 , const String& position
                                 , NEPersistence::ConfigEntry confKey
                                 , const Type& newValue
                                 , bool is_temporary)
    {
        uint32_t readPos  = _findPosition(readList , 0, section, NEPersistence::SYNTAX_ALL_MODULES, property, position, false, confKey);
        uint32_t writePos = _findPosition(writeList, 0, section, module, property, position, true , confKey);

        while ((writePos != NECommon::INVALID_POSITION) && (writeList[writePos].is_temporary() != is_temporary))
        {
            writePos = _findPosition(writeList, writePos + 1, section, module, property, position, true, confKey);
        }

        if (readPos == NECommon::INVALID_POSITION)
        {
            if (writePos != NECommon::INVALID_POSITION)
            {
                writeList[readPos].value() = newValue;
            }
            else
            {
                writeList.add(Property(PropertyKey(section, module, property, position, confKey), PropertyValue(newValue), String::EmptyString, is_temporary));
            }
        }
        else
        {
            const PropertyValue& readValue = readList[readPos].value();
            if (newValue != static_cast<const Type&>(readValue))
            {
                if ((writePos != NECommon::INVALID_POSITION) && (writeList[writePos].is_temporary() == is_temporary))
                {
                    writeList[writePos].value() = newValue;
                }
                else
                {
                    writeList.add(Property(PropertyKey(section, module, property, position, confKey), PropertyValue(newValue), String::EmptyString, is_temporary));
                }
            }
            else if (writePos != NECommon::INVALID_POSITION)
            {
                writeList.removeAt(writePos);
            }
        }
    }

    inline const Property* _getProperty( const NEPersistence::ListProperties& list
                                       , const String& section
                                       , const String& module
                                       , const String& property
                                       , const String& position
                                       , NEPersistence::ConfigEntry keyType
                                       , bool exactMatch)
    {
        ASSERT(keyType != NEPersistence::ConfigEntry::Invalid);

        uint32_t elemPos = _findPosition(list, 0, section, module, property, position, exactMatch, keyType);
        return (elemPos != NECommon::INVALID_POSITION ? &list[elemPos] : nullptr);
    }

    uint32_t _readConfig(const FileBase& file, NEPersistence::ListProperties& listWritable, NEPersistence::ListProperties& listReadonly, const String& module)
    {
        uint32_t result{ 0 };

        if (file.isOpened())
        {
            file.moveToBegin();

            String line;
            Property newProperty;

            while (file.readLine(line) > 0)
            {
                if (newProperty.parse(line))
                {
                    // add new entry if unique. otherwise, update existing.
                    const PropertyKey& Key = newProperty.key();
                    if (Key.is_all_modules())
                    {
                        listReadonly.add(newProperty);
                        ++result;
                    }
                    else if (newProperty.is_module_property(module))
                    {
                        listWritable.add(newProperty);
                        ++result;
                    }

                    newProperty.reset();
                }
            }
        }

        return result;
    }

    inline bool _saveConfig( const NEPersistence::ListProperties& listWritable
                           , const NEPersistence::ListProperties& listReadonly
                           , const String& module
                           , const FileBase& srcFile
                           , FileBase& dstFile
                           , bool saveAll)
    {
        bool result{ false };
        if (srcFile.canRead() && dstFile.canWrite())
        {
            uint32_t count{ 0 };
            srcFile.moveToBegin();
            dstFile.moveToBegin();

            if (saveAll)
            {
                count = listReadonly.getSize();
                for (uint32_t i = 0; i < count; ++ i)
                {
                    const auto& prop = listReadonly[i];
                    if (prop.is_temporary() == false)
                    {
                        dstFile.writeLine(prop.to_string());
                    }
                }

                count = listWritable.getSize();
                for (uint32_t i = 0; i < count; ++ i)
                {
                    const auto& prop = listWritable[i];
                    if (prop.is_temporary() == false)
                    {
                        dstFile.writeLine(prop.to_string());
                    }
                }
            }
            else
            {
                String line;
                Property newProperty;

                while (srcFile.readLine(line) > 0)
                {
                    if ((newProperty.parse(line) == false) || (newProperty.key().module() != module))
                    {
                        dstFile.writeLine(line);
                    }
                    else
                    {
                        const PropertyKey& key{ newProperty.key() };
                        const Property* prop = _getProperty(listWritable, key.section(), module, key.property(), key.position(), key.key_type(), true);
                        if ((prop != nullptr) && prop->is_temporary())
                        {
                            dstFile.writeLine(line);
                        }
                    }
                }

                count = listWritable.getSize();
                for (uint32_t i = 0; i < count; ++i)
                {
                    const auto& prop = listWritable[i];
                    if (prop.is_temporary() == false)
                    {
                        dstFile.writeLine(prop.to_string());
                    }
                }
            }

            result = true;
        }

        return result;
    }
} // namespace

ConfigManager::ConfigManager()
    : mModule               (Process::getInstance().getAppName())
    , mWritableProperties   ( )
    , mReadonlyProperties   ( )
    , mIsConfigured         (false)
    , mFilePath             ( )
    , mLock                 (false)
{
}

bool ConfigManager::exist(const PropertyKey& key) const
{
    Lock lock(mLock);
    bool result = _findPosition(  mWritableProperties
                                , 0
                                , key.section()
                                , mModule
                                , key.property()
                                , key.position()
                                , true
                                , key.key_type()) != NECommon::INVALID_POSITION;

    if (result == false)
    {
        result = _findPosition(  mReadonlyProperties
                                , 0
                                , key.section()
                                , NEPersistence::SYNTAX_ALL_MODULES
                                , key.property()
                                , key.position()
                                , false
                                , key.key_type()) != NECommon::INVALID_POSITION;
    }

    return result;
}

NEPersistence::ListProperties ConfigManager::section_properties(const String& section) const
{
    Lock lock(mLock);
    NEPersistence::ListProperties result;
    if (section.isEmpty())
    {
        return result;
    }

    for (const auto& prop : mWritableProperties.getData())
    {
        if (section == prop.key().section())
        {
            result.add(prop);
        }
    }

    for (const auto& prop : mReadonlyProperties.getData())
    {
        const String& propSec = prop.key().section();
        if ((section == propSec) && (section == NEPersistence::SYNTAX_ANY_VALUE) && (propSec == NEPersistence::SYNTAX_ANY_VALUE))
        {
            result.addIfUnique(prop, false);
        }
    }

    return result;
}

const Property* ConfigManager::property( const String& section
                                          , const String& property
                                          , const String& position
                                          , NEPersistence::ConfigEntry keyType /*= NEPersistence::ConfigEntry::AnyKey*/) const
{
    Lock lock(mLock);

    keyType = keyType == NEPersistence::ConfigEntry::Invalid ? NEPersistence::ConfigEntry::AnyKey : keyType;
    const Property* result{ _getProperty(mWritableProperties, section, mModule, property, position, keyType, true)};
    return (result != nullptr ? result : _getProperty(mReadonlyProperties, section, NEPersistence::SYNTAX_ALL_MODULES, property, position, keyType, false));
}

const Property * ConfigManager::module_property( const String& section
                                                 , const String& property
                                                 , const String& position
                                                 , NEPersistence::ConfigEntry keyType /*= NEPersistence::ConfigEntry::AnyKey*/) const
{
    Lock lock(mLock);

    keyType = keyType == NEPersistence::ConfigEntry::Invalid ? NEPersistence::ConfigEntry::AnyKey : keyType;
    return _getProperty(mWritableProperties, section, mModule, property, position, keyType, true);
}

void ConfigManager::set_module_property( const String& section
                                     , const String& property
                                     , const String& position
                                     , const String& value
                                     , NEPersistence::ConfigEntry keyType /*= NEPersistence::ConfigEntry::AnyKey*/
                                     , bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    keyType = keyType == NEPersistence::ConfigEntry::Invalid ? NEPersistence::ConfigEntry::AnyKey : keyType;
    _setPositionValue<String>(mWritableProperties, mReadonlyProperties, section, mModule, property, position, keyType, value, is_temporary);
}

void ConfigManager::remove_module_property(const String& section, const String& property, const String& position, NEPersistence::ConfigEntry keyType)
{
    Lock lock(mLock);
    uint32_t elemPos = _findPosition(mWritableProperties, 0, section, mModule, property, position, true, keyType);
    if (elemPos != NECommon::INVALID_POSITION)
    {
        mWritableProperties.removePosition(elemPos);
    }
}

int32_t ConfigManager::remove_module_properties(const String& section, const String& property, NEPersistence::ConfigEntry keyType)
{
    Lock lock(mLock);
    int32_t result{ 0 };
    uint32_t i{ 0 };
    while (i < mWritableProperties.getSize())
    {
        const PropertyKey& key = mWritableProperties[i].key();
        if ((keyType == NEPersistence::ConfigEntry::AnyKey) || (keyType == key.key_type()))
        {
            if ((section == key.section()) && (property == key.property()))
            {
                ++result;
                mWritableProperties.removeAt(i);
            }
            else
            {
                ++i;
            }
        }
        else
        {
            ++i;
        }
    }

    return result;
}

void ConfigManager::remove_section_properties(const String& section)
{
    Lock lock(mLock);
    for (uint32_t i = 0; i < mWritableProperties.getSize(); ++i)
    {
        if (mWritableProperties[i].key().section() == section)
        {
            mWritableProperties.removeAt(i);
        }
        else
        {
            ++i;
        }
    }
}

bool ConfigManager::read_config(const String& filePath /*= String::EmptyString*/, ConfigListener * listener /*= nullptr*/)
{
    Lock lock(mLock);
    if (mIsConfigured == false)
    {
        ASSERT(mFilePath.isEmpty());
        String path;
        if (filePath.isEmpty())
        {
            path = NEApplication::DEFAULT_CONFIG_FILE;
        }
        else
        {
            path = filePath;
        }

        path = File::getFileFullPath(File::normalizePath(path));
        File fileConfig(path, static_cast<uint32_t>(File::OpenMode::Exist) 
                            | static_cast<uint32_t>(File::OpenMode::Read)
                            | static_cast<uint32_t>(File::OpenMode::Text)
                            | static_cast<uint32_t>(File::OpenMode::ShareRead));
        if (fileConfig.open() && read_config(fileConfig, listener))
        {
            mFilePath = fileConfig.getName();
        }
    }

    return mIsConfigured;
}

bool ConfigManager::read_config(const FileBase& file, ConfigListener * listener /*= nullptr*/)
{
    Lock lock(mLock);
    if (mIsConfigured == false)
    {
        mWritableProperties.clear();
        mReadonlyProperties.clear();
        if (listener != nullptr)
        {
            listener->prepare_read_configuration(*this);
        }

        mIsConfigured = _readConfig(file, mWritableProperties, mReadonlyProperties, mModule) != 0;

        if (listener != nullptr)
        {
            listener->post_read_configuration(*this);
        }
    }

    return mIsConfigured;
}

bool ConfigManager::save_config(const String& filePath, ConfigListener * listener /*= nullptr*/)
{
    Lock lock(mLock);
    bool result{ false };

    constexpr uint32_t modeRead { static_cast<uint32_t>(File::OpenMode::Read)
                                | static_cast<uint32_t>(File::OpenMode::Text)
                                | static_cast<uint32_t>(File::OpenMode::Create)
                                | static_cast<uint32_t>(File::OpenMode::ShareRead)};
    constexpr uint32_t modeWrite{ static_cast<uint32_t>(File::OpenMode::Read)
                                | static_cast<uint32_t>(File::OpenMode::Text)
                                | static_cast<uint32_t>(File::OpenMode::Create)
                                | static_cast<uint32_t>(File::OpenMode::Write) };
    bool saveAll{ false };

    String srcPath, dstPath;
    String tempFile = File::genTempFileName();
    
    if (filePath.isEmpty() == false)
    {
        dstPath = filePath;
    }
    else if (mFilePath.isEmpty() == false)
    {
        dstPath = mFilePath;
    }
    else
    {
        dstPath = NEApplication::DEFAULT_CONFIG_FILE;
        saveAll = true;
    }

    dstPath = File::getFileFullPath( File::normalizePath(dstPath) );
    srcPath = mFilePath.isEmpty() ? dstPath : mFilePath;

    String dstDir = File::getFileDirectory(dstPath);
    if ((dstDir.isEmpty() == false) && File::existDir(dstDir) == false)
    {
        File::createDirCascaded(dstDir);
    }

    File srcFile(srcPath, modeRead );
    File dstFile(tempFile, modeWrite);

    if (srcFile.open() && dstFile.open())
    {
        if (save_config(srcFile, dstFile, saveAll, listener))
        {
            srcFile.close();
            dstFile.close();
            result = File::copyFile(tempFile, dstPath, true);
            File::deleteFile(tempFile);
        }
    }

    return result;
}

bool ConfigManager::save_config(const FileBase& srcFile, FileBase& dstFile, bool saveAll, ConfigListener * listener /*= nullptr*/)
{
    Lock lock(mLock);
    if (listener != nullptr)
    {
        listener->prepare_save_configuration(*this);
    }

    bool result = _saveConfig(mWritableProperties, mReadonlyProperties, mModule, srcFile, dstFile, saveAll);

    if (listener != nullptr)
    {
        listener->post_save_configuration(*this);
    }

    return result;
}

void ConfigManager::set_configuration(const NEPersistence::ListProperties& listReadonly, const NEPersistence::ListProperties& listWritable, ConfigListener* listener /*= nullptr*/)
{
    Lock lock(mLock);

    mIsConfigured = true;
    mWritableProperties = listWritable;
    mReadonlyProperties = listReadonly;

    if (listener != nullptr)
    {
        listener->on_setup_configuration(listReadonly, listWritable, *this);
    }
}


Version ConfigManager::config_version() const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ConfigVersion;
    const NEPersistence::ConfigKey& key = NEPersistence::config_version();

    const Property* prop = _getProperty(mReadonlyProperties, key.section, NEPersistence::SYNTAX_ALL_MODULES, key.property, key.position, confKey, true);

    Version result;
    if (prop != nullptr)
    {
        result = prop->value_string();
    }
    else
    {
        result = NEPersistence::CONFIG_VERSION;
    }

    return result;
}

std::vector<Identifier> ConfigManager::service_list() const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ServiceList;
    const NEPersistence::ConfigKey& key = NEPersistence::service_list();

    const Property* prop = property(key.section, key.property, key.position, confKey);

    std::vector<Identifier> result;
    if (prop != nullptr)
    {
        result = prop->value().identifier_list(NEApplication::RemoteServiceIdentifiers).getData();
    }

    return result;
}

std::vector<Identifier> ConfigManager::log_targets() const
{
    Lock lock(mLock);
    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogTarget;
    const NEPersistence::ConfigKey& key = NEPersistence::log_target();

    const Property* prop = property(key.section, key.property, key.position, confKey);

    std::vector<Identifier> result;
    if (prop != nullptr)
    {
        result = prop->value().identifier_list(NEApplication::LogTypeIdentifiers).getData();
    }

    return result;
}

bool ConfigManager::logging_status() const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogStatus;
    const NEPersistence::ConfigKey& key = NEPersistence::log_status();

    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);
    return (value != nullptr ? value->as_boolean() : NEApplication::DEFAULT_LOG_ENABLED);
}

Version ConfigManager::log_version() const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogVersion;
    const NEPersistence::ConfigKey& key = NEPersistence::log_version();

    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);
    Version result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = NEPersistence::CONFIG_VERSION;
    }
    return result;
}

bool ConfigManager::log_enabled(const String& logType) const
{
    Lock lock(mLock);

    bool result{ false };
    if (logging_status())
    {
        constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogEnable;
        const NEPersistence::ConfigKey& key = NEPersistence::log_enable();

        const PropertyValue* value = property_value(key.section, key.property, logType, confKey);
        result = (value != nullptr ? value->as_boolean() : NEApplication::DEFAULT_LOG_ENABLED);
    }

    return result;
}

bool ConfigManager::log_enabled(NELogging::LogTarget logType) const
{
    String id = Identifier::to_string( static_cast<uint32_t>(logType)
                                        , NEApplication::LogTypeIdentifiers
                                        , static_cast<uint32_t>(NELogging::LogTarget::Undefined));
    return log_enabled(id);
}

void ConfigManager::set_log_enabled(NELogging::LogTarget logType, bool newValue, bool is_temporary /*= false*/)
{
    String id = Identifier::to_string( static_cast<uint32_t>(logType)
                                        , NEApplication::LogTypeIdentifiers
                                        , static_cast<uint32_t>(NELogging::LogTarget::Undefined));
    set_log_enabled(id, newValue, is_temporary);
}

String ConfigManager::log_file_location() const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogFileLocation;
    const NEPersistence::ConfigKey& key = NEPersistence::log_file_location();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = NEApplication::DEFAULT_LOG_FILE;
    }

    return result;
}

bool ConfigManager::log_file_append() const
{
    Lock lock(mLock);
    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogFileAppend;
    const NEPersistence::ConfigKey& key = NEPersistence::log_file_append();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);
    return (value != nullptr ? value->as_boolean() : false);
}

void ConfigManager::set_log_file_append(bool newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);
    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogFileAppend;
    const NEPersistence::ConfigKey& key = NEPersistence::log_file_append();
    set_module_property(key.section, key.property, key.position, String::makeString(newValue), confKey, is_temporary);
}

uint32_t ConfigManager::log_remote_queue_size() const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogRemoteQueueSize;
    const NEPersistence::ConfigKey& key = NEPersistence::log_remote_queue_size();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);
    return (value != nullptr ? value->as_integer() : NEApplication::DEFAULT_LOG_QUEUE_SIZE);
}

void ConfigManager::set_log_remote_queue_size(uint32_t newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogRemoteQueueSize;
    const NEPersistence::ConfigKey& key = NEPersistence::log_remote_queue_size();
    set_module_property(key.section, key.property, key.position, String::makeString(newValue), confKey, is_temporary);
}

String ConfigManager::log_layout_enter() const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogLayoutEnter;
    const NEPersistence::ConfigKey& key = NEPersistence::log_layout_enter();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = NEApplication::DEFAULT_LAYOUT_SCOPE_ENTER;
    }

    return result;
}

void ConfigManager::set_log_layout_enter(const String& newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogLayoutEnter;
    const NEPersistence::ConfigKey& key = NEPersistence::log_layout_enter();
    set_module_property(key.section, key.property, key.position, newValue, confKey, is_temporary);
}

String ConfigManager::log_layout_message() const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogLayoutMessage;
    const NEPersistence::ConfigKey& key = NEPersistence::log_layout_message();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = NEApplication::DEFAULT_LAYOUT_LOG_MESSAGE;
    }
    
    return result;
}

void ConfigManager::set_log_layout_message(const String& newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogLayoutMessage;
    const NEPersistence::ConfigKey& key = NEPersistence::log_layout_message();
    set_module_property(key.section, key.property, key.position, newValue, confKey, is_temporary);
}

String ConfigManager::log_layout_exit() const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogLayoutExit;
    const NEPersistence::ConfigKey& key = NEPersistence::log_layout_exit();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = NEApplication::DEFAULT_LAYOUT_SCOPE_EXIT;
    }
    
    return result;
}

void ConfigManager::set_log_layout_exit(const String& newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::LogLayoutExit;
    const NEPersistence::ConfigKey& key = NEPersistence::log_layout_exit();
    set_module_property(key.section, key.property, key.position, newValue, confKey, is_temporary);
}

uint32_t ConfigManager::module_log_scopes(std::vector<Property>& scopeList) const
{
    Lock lock(mLock);

    const auto& listRead{ mReadonlyProperties.getData() };
    for (const auto& entry : listRead)
    {
        const PropertyKey& key = entry.key();
        if ((key.key_type() == NEPersistence::ConfigEntry::LogScope) && key.is_all_modules())
        {
            scopeList.push_back(entry);
        }
    }

    const auto& listWrite{ mWritableProperties.getData() };
    for (const auto& entry : listWrite)
    {
        const PropertyKey& key = entry.key();
        if (key.key_type() == NEPersistence::ConfigEntry::LogScope)
        {
            ASSERT(key.module() == mModule);
            scopeList.push_back(entry);
        }
    }

    return static_cast<uint32_t>(scopeList.size());
}

void ConfigManager::add_module_log_scopes(const std::vector<Property>& scopeList, bool search)
{
    Lock lock(mLock);

    for (const auto& scope : scopeList)
    {
        if (search)
        {
            set_module_property(scope.key(), scope.value_string(), false);
        }
        else
        {
            mWritableProperties.add(scope);
        }
    }
}

void ConfigManager::add_module_log_scope(const String& scopeName, const String& prio)
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey{ NEPersistence::ConfigEntry::LogScope };
    const NEPersistence::ConfigKey& key{ NEPersistence::log_scope() };
    set_module_property(key.section, key.property, scopeName, prio, confKey, false);
}

void ConfigManager::add_module_log_scope(const String& scopeName, uint32_t prio)
{
    PropertyValue value;
    value.set_identifier_list(prio, NEApplication::LogScopePriorityIndentifiers);
    add_module_log_scope(scopeName, value.as_string());
}

bool ConfigManager::remove_scope(const String& scopeName)
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey{ NEPersistence::ConfigEntry::LogScope };
    const NEPersistence::ConfigKey& key = NEPersistence::log_scope();
    uint32_t pos = _findPosition(mWritableProperties, 0, key.section, mModule, key.property, scopeName, true, confKey);
    if (pos != NECommon::INVALID_POSITION)
    {
        mWritableProperties.removeAt(pos);
    }

    return (pos != NECommon::INVALID_POSITION);
}

int32_t ConfigManager::remove_module_scopes()
{
    constexpr NEPersistence::ConfigEntry confKey{ NEPersistence::ConfigEntry::LogScope };
    const NEPersistence::ConfigKey& key = NEPersistence::log_scope();
    return remove_module_properties(key.section, key.property, confKey);
}

std::vector<Identifier> ConfigManager::remote_service_connections(const String& service) const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ServiceConnection;
    const NEPersistence::ConfigKey& key = NEPersistence::service_connection();

    std::vector<Identifier> result;
    const PropertyValue* value = property_value(service, key.property, key.position, confKey);
    if (value != nullptr)
    {
        result = value->identifier_list(NEApplication::ConnectionIdentifiers).getData();
    }

    return result;
}

String ConfigManager::remote_service_name(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ServiceName;
    const NEPersistence::ConfigKey& key = NEPersistence::service_name();
    const PropertyValue* value = property_value(service, key.property, connectType, confKey);
    return (value != nullptr ? value->as_string() : String(String::EmptyString));
}

String ConfigManager::remote_service_name(NERemoteService::RemoteServiceKind serviceType, NERemoteService::ConnectionType connectType) const
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Undefined));
    return remote_service_name(service, connect);
}

bool ConfigManager::remote_service_enable(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ServiceEnable;
    const NEPersistence::ConfigKey& key = NEPersistence::service_enable();
    const PropertyValue* value = property_value(service, key.property, connectType, confKey);
    return (value != nullptr ? value->as_boolean() : NEApplication::DEFAULT_SERVICE_ENABLED);
}

bool ConfigManager::remote_service_enable(NERemoteService::RemoteServiceKind serviceType, NERemoteService::ConnectionType connectType) const
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Undefined));
    return remote_service_enable(service, connect);
}

void ConfigManager::set_remote_service_enable(const String& service, const String& connectType, bool newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ServiceEnable;
    const NEPersistence::ConfigKey& key = NEPersistence::service_enable();
    set_module_property(service, key.property, connectType, String::makeString(newValue), confKey, is_temporary);
}

void ConfigManager::set_remote_service_enable(NERemoteService::RemoteServiceKind serviceType, NERemoteService::ConnectionType connectType, bool newValue, bool is_temporary /*= false*/)
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Undefined));
    set_remote_service_enable(service, connect, newValue, is_temporary);
}

String ConfigManager::remote_service_address(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ServiceAddress;
    const NEPersistence::ConfigKey& key = NEPersistence::service_address();
    const PropertyValue* value = property_value(service, key.property, connectType, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = NEApplication::DEFAULT_SERVICE_HOST;
    }
    
    return result;
}

String ConfigManager::remote_service_address(NERemoteService::RemoteServiceKind serviceType, NERemoteService::ConnectionType connectType) const
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Undefined));
    return remote_service_address(service, connect);
}

void ConfigManager::set_remote_service_address(const String& service, const String& connectType, const String& newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ServiceAddress;
    const NEPersistence::ConfigKey& key = NEPersistence::service_address();
    set_module_property(service, key.property, connectType, newValue, confKey, is_temporary);
}

void ConfigManager::set_remote_service_address(NERemoteService::RemoteServiceKind serviceType, NERemoteService::ConnectionType connectType, const String& newValue, bool is_temporary /*= false*/)
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Undefined));
    set_remote_service_address(service, connect, newValue, is_temporary);
}

uint16_t ConfigManager::remote_service_port(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ServicePort;
    const NEPersistence::ConfigKey& key = NEPersistence::service_port();
    const PropertyValue* value = property_value(service, key.property, connectType, confKey);
    return static_cast<uint16_t>(value != nullptr ? value->as_integer() : NEApplication::DEFAULT_ROUTER_PORT);
}

uint16_t ConfigManager::remote_service_port(NERemoteService::RemoteServiceKind serviceType, NERemoteService::ConnectionType connectType) const
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Undefined));
    return remote_service_port(service, connect);
}

void ConfigManager::set_remote_service_port(const String& service, const String& connectType, uint16_t newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::ServicePort;
    const NEPersistence::ConfigKey& key = NEPersistence::service_port();
    set_module_property(service, key.property, connectType, String::makeString(static_cast<uint32_t>(newValue)), confKey, is_temporary);
}

void ConfigManager::set_remote_service_port(NERemoteService::RemoteServiceKind serviceType, NERemoteService::ConnectionType connectType, uint16_t newValue, bool is_temporary /*= false*/)
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Undefined));
    set_remote_service_port(service, connect, newValue, is_temporary);
}

String ConfigManager::log_database_property(const String& whichPosition)
{
    const NEPersistence::ConfigKey& key = NEPersistence::log_database_name();
    const PropertyValue* value = property_value(key.section, key.property, whichPosition);
    return (value != nullptr ? value->value() : String::getEmptyString());
}

void ConfigManager::set_log_database_property(const String& whichPosition, const String& newValue, bool is_temporary /*= false*/)
{
    const NEPersistence::ConfigKey& key = NEPersistence::log_database_name();
    set_module_property(key.section, key.property, whichPosition, newValue, NEPersistence::ConfigEntry::AnyKey, is_temporary);
}

uint32_t ConfigManager::default_buffer_block_size(const String& whichModule /*= NEString::EmptyStringA*/)
{
    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::DefaultBufferBlock;
    const NEPersistence::ConfigKey& key = NEPersistence::default_buffer_block_size();
    const Property* prop = _getProperty(mReadonlyProperties, key.section, whichModule.isEmpty() ? NEPersistence::SYNTAX_ALL_MODULES : whichModule, key.property, key.position, confKey, true);
    return (prop != nullptr ? prop->value().as_integer() : 0u);
}

uint32_t ConfigManager::default_message_queue_size(const String& whichModule /*= NEString::EmptyStringA*/)
{
    constexpr NEPersistence::ConfigEntry confKey = NEPersistence::ConfigEntry::DefaultMessageQueue;
    const NEPersistence::ConfigKey& key = NEPersistence::default_message_queue_size();
    const Property* prop = _getProperty(mReadonlyProperties, key.section, whichModule.isEmpty() ? NEPersistence::SYNTAX_ALL_MODULES : whichModule, key.property, key.position, confKey, true);
    return ( prop != nullptr ? prop->value().as_integer() : std::numeric_limits<uint32_t>::max() );
}
