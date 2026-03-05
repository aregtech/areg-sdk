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
namespace areg {

namespace
{
    inline uint32_t _findPosition( const ArrayList<Property>& propList
                                 , uint32_t startAt
                                 , const String& section
                                 , const String& module
                                 , const String& propName
                                 , const String& position
                                 , bool exact
                                 , areg::ConfigEntry configKey)
    {
        uint32_t result { areg::INVALID_POSITION };
        const auto& list{ propList.data() };
        uint32_t count{ static_cast<uint32_t>(list.size()) };

        for (uint32_t pos = startAt; pos < count; ++ pos)
        {
            const PropertyKey& key = propList[pos].key();
            if ((configKey == areg::ConfigEntry::AnyKey) || (configKey == key.key_type()))
            {
                if ((exact && key.is_exact_property(section, module, propName, position)) ||
                    (!exact && key.is_module_property(section, module, propName, position)))
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
                                 , const String& propName
                                 , const String& position
                                 , areg::ConfigEntry confKey
                                 , const Type& newValue
                                 , bool is_temporary)
    {
        uint32_t readPos  = _findPosition(readList , 0, section, areg::SYNTAX_ALL_MODULES, propName, position, false, confKey);
        uint32_t writePos = _findPosition(writeList, 0, section, module, propName, position, true , confKey);

        while ((writePos != areg::INVALID_POSITION) && (writeList[writePos].is_temporary() != is_temporary))
        {
            writePos = _findPosition(writeList, writePos + 1, section, module, propName, position, true, confKey);
        }

        if (readPos == areg::INVALID_POSITION)
        {
            if (writePos != areg::INVALID_POSITION)
            {
                writeList[readPos].value() = newValue;
            }
            else
            {
                writeList.add(Property(PropertyKey(section, module, propName, position, confKey), PropertyValue(newValue), String::EmptyString, is_temporary));
            }
        }
        else
        {
            const PropertyValue& readValue = readList[readPos].value();
            if (newValue != static_cast<const Type&>(readValue))
            {
                if ((writePos != areg::INVALID_POSITION) && (writeList[writePos].is_temporary() == is_temporary))
                {
                    writeList[writePos].value() = newValue;
                }
                else
                {
                    writeList.add(Property(PropertyKey(section, module, propName, position, confKey), PropertyValue(newValue), String::EmptyString, is_temporary));
                }
            }
            else if (writePos != areg::INVALID_POSITION)
            {
                writeList.remove_at(writePos);
            }
        }
    }

    inline const Property* _get_property( const areg::ListProperties& list
                                        , const String& section
                                        , const String& module
                                        , const String& propName
                                        , const String& position
                                        , areg::ConfigEntry keyType
                                        , bool exactMatch)
    {
        ASSERT(keyType != areg::ConfigEntry::Invalid);

        uint32_t elemPos = _findPosition(list, 0, section, module, propName, position, exactMatch, keyType);
        return (elemPos != areg::INVALID_POSITION ? &list[elemPos] : nullptr);
    }

    uint32_t _read_config(const FileBase& file, areg::ListProperties& listWritable, areg::ListProperties& listReadonly, const String& module)
    {
        uint32_t result{ 0 };

        if (file.is_opened())
        {
            file.move_to_begin();

            String line;
            Property newProperty;

            while (file.read_line(line) > 0)
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

    inline bool _saveConfig( const areg::ListProperties& listWritable
                           , const areg::ListProperties& listReadonly
                           , const String& module
                           , const FileBase& srcFile
                           , FileBase& dstFile
                           , bool saveAll)
    {
        bool result{ false };
        if (srcFile.can_read() && dstFile.can_write())
        {
            uint32_t count{ 0 };
            srcFile.move_to_begin();
            dstFile.move_to_begin();

            if (saveAll)
            {
                count = listReadonly.size();
                for (uint32_t i = 0; i < count; ++ i)
                {
                    const auto& prop = listReadonly[i];
                    if (prop.is_temporary() == false)
                    {
                        dstFile.write_line(prop.to_string());
                    }
                }

                count = listWritable.size();
                for (uint32_t i = 0; i < count; ++ i)
                {
                    const auto& prop = listWritable[i];
                    if (prop.is_temporary() == false)
                    {
                        dstFile.write_line(prop.to_string());
                    }
                }
            }
            else
            {
                String line;
                Property newProperty;

                while (srcFile.read_line(line) > 0)
                {
                    if ((newProperty.parse(line) == false) || (newProperty.key().module() != module))
                    {
                        dstFile.write_line(line);
                    }
                    else
                    {
                        const PropertyKey& key{ newProperty.key() };
                        const Property* prop = _get_property(listWritable, key.section(), module, key.property(), key.position(), key.key_type(), true);
                        if ((prop != nullptr) && prop->is_temporary())
                        {
                            dstFile.write_line(line);
                        }
                    }
                }

                count = listWritable.size();
                for (uint32_t i = 0; i < count; ++i)
                {
                    const auto& prop = listWritable[i];
                    if (prop.is_temporary() == false)
                    {
                        dstFile.write_line(prop.to_string());
                    }
                }
            }

            result = true;
        }

        return result;
    }
} // namespace

ConfigManager::ConfigManager()
    : mModule               (Process::instance().app_name())
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
                                , key.key_type()) != areg::INVALID_POSITION;

    if (result == false)
    {
        result = _findPosition(  mReadonlyProperties
                                , 0
                                , key.section()
                                , areg::SYNTAX_ALL_MODULES
                                , key.property()
                                , key.position()
                                , false
                                , key.key_type()) != areg::INVALID_POSITION;
    }

    return result;
}

areg::ListProperties ConfigManager::section_properties(const String& section) const
{
    Lock lock(mLock);
    areg::ListProperties result;
    if (section.is_empty())
    {
        return result;
    }

    for (const auto& prop : mWritableProperties.data())
    {
        if (section == prop.key().section())
        {
            result.add(prop);
        }
    }

    for (const auto& prop : mReadonlyProperties.data())
    {
        const String& propSec = prop.key().section();
        if ((section == propSec) && (section == areg::SYNTAX_ANY_VALUE) && (propSec == areg::SYNTAX_ANY_VALUE))
        {
            result.add_if_unique(prop, false);
        }
    }

    return result;
}

const Property* ConfigManager::property( const String& section
                                       , const String& propName
                                       , const String& position
                                       , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/) const
{
    Lock lock(mLock);

    keyType = keyType == areg::ConfigEntry::Invalid ? areg::ConfigEntry::AnyKey : keyType;
    const Property* result{ _get_property(mWritableProperties, section, mModule, propName, position, keyType, true)};
    return (result != nullptr ? result : _get_property(mReadonlyProperties, section, areg::SYNTAX_ALL_MODULES, propName, position, keyType, false));
}

const Property * ConfigManager::module_property( const String& section
                                                 , const String& propName
                                                 , const String& position
                                                 , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/) const
{
    Lock lock(mLock);

    keyType = keyType == areg::ConfigEntry::Invalid ? areg::ConfigEntry::AnyKey : keyType;
    return _get_property(mWritableProperties, section, mModule, propName, position, keyType, true);
}

void ConfigManager::set_module_property( const String& section
                                     , const String& propName
                                     , const String& position
                                     , const String& value
                                     , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/
                                     , bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    keyType = keyType == areg::ConfigEntry::Invalid ? areg::ConfigEntry::AnyKey : keyType;
    _setPositionValue<String>(mWritableProperties, mReadonlyProperties, section, mModule, propName, position, keyType, value, is_temporary);
}

void ConfigManager::remove_module_property(const String& section, const String& propName, const String& position, areg::ConfigEntry keyType)
{
    Lock lock(mLock);
    uint32_t elemPos = _findPosition(mWritableProperties, 0, section, mModule, propName, position, true, keyType);
    if (elemPos != areg::INVALID_POSITION)
    {
        mWritableProperties.remove_at(elemPos);
    }
}

int32_t ConfigManager::remove_module_properties(const String& section, const String& propName, areg::ConfigEntry keyType)
{
    Lock lock(mLock);
    int32_t result{ 0 };
    uint32_t i{ 0 };
    while (i < mWritableProperties.size())
    {
        const PropertyKey& key = mWritableProperties[i].key();
        if ((keyType == areg::ConfigEntry::AnyKey) || (keyType == key.key_type()))
        {
            if ((section == key.section()) && (propName == key.property()))
            {
                ++result;
                mWritableProperties.remove_at(i);
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
    for (uint32_t i = 0; i < mWritableProperties.size(); ++i)
    {
        if (mWritableProperties[i].key().section() == section)
        {
            mWritableProperties.remove_at(i);
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
        ASSERT(mFilePath.is_empty());
        String path;
        if (filePath.is_empty())
        {
            path = areg::DEFAULT_CONFIG_FILE;
        }
        else
        {
            path = filePath;
        }

        path = File::file_full_path(File::normalize_path(path));
        File fileConfig(path, static_cast<uint32_t>(File::OpenMode::Exist) 
                            | static_cast<uint32_t>(File::OpenMode::Read)
                            | static_cast<uint32_t>(File::OpenMode::Text)
                            | static_cast<uint32_t>(File::OpenMode::ShareRead));
        if (fileConfig.open() && read_config(fileConfig, listener))
        {
            mFilePath = fileConfig.name();
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

        mIsConfigured = _read_config(file, mWritableProperties, mReadonlyProperties, mModule) != 0;

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
    String tempFile = File::temp_name();
    
    if (filePath.is_empty() == false)
    {
        dstPath = filePath;
    }
    else if (mFilePath.is_empty() == false)
    {
        dstPath = mFilePath;
    }
    else
    {
        dstPath = areg::DEFAULT_CONFIG_FILE;
        saveAll = true;
    }

    dstPath = File::file_full_path( File::normalize_path(dstPath) );
    srcPath = mFilePath.is_empty() ? dstPath : mFilePath;

    String dstDir = File::file_directory(dstPath);
    if ((dstDir.is_empty() == false) && (File::has_dir(dstDir) == false))
    {
        File::create_dir_cascaded(dstDir);
    }

    File srcFile(srcPath, modeRead );
    File dstFile(tempFile, modeWrite);

    if (srcFile.open() && dstFile.open())
    {
        if (save_config(srcFile, dstFile, saveAll, listener))
        {
            srcFile.close();
            dstFile.close();
            result = File::copy_file(tempFile, dstPath, true);
            File::delete_file(tempFile);
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

void ConfigManager::set_configuration(const areg::ListProperties& listReadonly, const areg::ListProperties& listWritable, ConfigListener* listener /*= nullptr*/)
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

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ConfigVersion;
    const areg::ConfigKey& key = areg::config_version();

    const Property* prop = _get_property(mReadonlyProperties, key.section, areg::SYNTAX_ALL_MODULES, key.property, key.position, confKey, true);

    Version result;
    if (prop != nullptr)
    {
        result = prop->value_string();
    }
    else
    {
        result = areg::CONFIG_VERSION;
    }

    return result;
}

std::vector<Identifier> ConfigManager::service_list() const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ServiceList;
    const areg::ConfigKey& key = areg::service_list();

    const Property* prop = property(key.section, key.property, key.position, confKey);

    std::vector<Identifier> result;
    if (prop != nullptr)
    {
        result = prop->value().identifier_list(areg::RemoteServiceIdentifiers).data();
    }

    return result;
}

std::vector<Identifier> ConfigManager::log_targets() const
{
    Lock lock(mLock);
    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogTarget;
    const areg::ConfigKey& key = areg::log_target();

    const Property* prop = property(key.section, key.property, key.position, confKey);

    std::vector<Identifier> result;
    if (prop != nullptr)
    {
        result = prop->value().identifier_list(areg::LogTypeIdentifiers).data();
    }

    return result;
}

bool ConfigManager::logging_status() const noexcept
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogStatus;
    const areg::ConfigKey& key = areg::log_status();

    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);
    return (value != nullptr ? value->as_boolean() : areg::DEFAULT_LOG_ENABLED);
}

Version ConfigManager::log_version() const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogVersion;
    const areg::ConfigKey& key = areg::log_version();

    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);
    Version result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = areg::CONFIG_VERSION;
    }
    return result;
}

bool ConfigManager::log_enabled(const String& logType) const
{
    Lock lock(mLock);

    bool result{ false };
    if (logging_status())
    {
        constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogEnable;
        const areg::ConfigKey& key = areg::log_enable();

        const PropertyValue* value = property_value(key.section, key.property, logType, confKey);
        result = (value != nullptr ? value->as_boolean() : areg::DEFAULT_LOG_ENABLED);
    }

    return result;
}

bool ConfigManager::log_enabled(areg::LogTarget logType) const
{
    String id = Identifier::to_string( static_cast<uint32_t>(logType)
                                        , areg::LogTypeIdentifiers
                                        , static_cast<uint32_t>(areg::LogTarget::Undefined));
    return log_enabled(id);
}

void ConfigManager::set_log_enabled(areg::LogTarget logType, bool newValue, bool is_temporary /*= false*/)
{
    String id = Identifier::to_string( static_cast<uint32_t>(logType)
                                        , areg::LogTypeIdentifiers
                                        , static_cast<uint32_t>(areg::LogTarget::Undefined));
    set_log_enabled(id, newValue, is_temporary);
}

String ConfigManager::log_file_location() const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogFileLocation;
    const areg::ConfigKey& key = areg::log_file_location();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = areg::DEFAULT_LOG_FILE;
    }

    return result;
}

bool ConfigManager::log_file_append() const noexcept
{
    Lock lock(mLock);
    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogFileAppend;
    const areg::ConfigKey& key = areg::log_file_append();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);
    return (value != nullptr ? value->as_boolean() : false);
}

void ConfigManager::set_file_append(bool newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);
    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogFileAppend;
    const areg::ConfigKey& key = areg::log_file_append();
    set_module_property(key.section, key.property, key.position, String::make_string(newValue), confKey, is_temporary);
}

uint32_t ConfigManager::remote_queue_size() const noexcept
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogRemoteQueueSize;
    const areg::ConfigKey& key = areg::remote_queue_size();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);
    return (value != nullptr ? value->as_integer() : areg::DEFAULT_LOG_QUEUE_SIZE);
}

void ConfigManager::set_remote_queue_size(uint32_t newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogRemoteQueueSize;
    const areg::ConfigKey& key = areg::remote_queue_size();
    set_module_property(key.section, key.property, key.position, String::make_string(newValue), confKey, is_temporary);
}

String ConfigManager::log_layout_enter() const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogLayoutEnter;
    const areg::ConfigKey& key = areg::log_layout_enter();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = areg::DEFAULT_LAYOUT_SCOPE_ENTER;
    }

    return result;
}

void ConfigManager::set_layout_enter(const String& newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogLayoutEnter;
    const areg::ConfigKey& key = areg::log_layout_enter();
    set_module_property(key.section, key.property, key.position, newValue, confKey, is_temporary);
}

String ConfigManager::log_layout_message() const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogLayoutMessage;
    const areg::ConfigKey& key = areg::log_layout_message();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = areg::DEFAULT_LAYOUT_LOG_MESSAGE;
    }
    
    return result;
}

void ConfigManager::set_layout_message(const String& newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogLayoutMessage;
    const areg::ConfigKey& key = areg::log_layout_message();
    set_module_property(key.section, key.property, key.position, newValue, confKey, is_temporary);
}

String ConfigManager::log_layout_exit() const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogLayoutExit;
    const areg::ConfigKey& key = areg::log_layout_exit();
    const PropertyValue* value = property_value(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = areg::DEFAULT_LAYOUT_SCOPE_EXIT;
    }
    
    return result;
}

void ConfigManager::set_layout_exit(const String& newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::LogLayoutExit;
    const areg::ConfigKey& key = areg::log_layout_exit();
    set_module_property(key.section, key.property, key.position, newValue, confKey, is_temporary);
}

uint32_t ConfigManager::module_log_scopes(std::vector<Property>& scopeList) const
{
    Lock lock(mLock);

    const auto& listRead{ mReadonlyProperties.data() };
    for (const auto& entry : listRead)
    {
        const PropertyKey& key = entry.key();
        if ((key.key_type() == areg::ConfigEntry::LogScope) && key.is_all_modules())
        {
            scopeList.push_back(entry);
        }
    }

    const auto& listWrite{ mWritableProperties.data() };
    for (const auto& entry : listWrite)
    {
        const PropertyKey& key = entry.key();
        if (key.key_type() == areg::ConfigEntry::LogScope)
        {
            ASSERT(key.module() == mModule);
            scopeList.push_back(entry);
        }
    }

    return static_cast<uint32_t>(scopeList.size());
}

void ConfigManager::add_log_scopes(const std::vector<Property>& scopeList, bool search)
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

void ConfigManager::add_log_scope(const String& scopeName, const String& prio)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey{ areg::ConfigEntry::LogScope };
    const areg::ConfigKey& key{ areg::log_scope() };
    set_module_property(key.section, key.property, scopeName, prio, confKey, false);
}

void ConfigManager::add_log_scope(const String& scopeName, uint32_t prio)
{
    PropertyValue value;
    value.set_identifier_list(prio, areg::LogScopePriorityIndentifiers);
    add_log_scope(scopeName, value.as_string());
}

bool ConfigManager::remove_scope(const String& scopeName)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey{ areg::ConfigEntry::LogScope };
    const areg::ConfigKey& key = areg::log_scope();
    uint32_t pos = _findPosition(mWritableProperties, 0, key.section, mModule, key.property, scopeName, true, confKey);
    if (pos != areg::INVALID_POSITION)
    {
        mWritableProperties.remove_at(pos);
    }

    return (pos != areg::INVALID_POSITION);
}

int32_t ConfigManager::remove_module_scopes()
{
    constexpr areg::ConfigEntry confKey{ areg::ConfigEntry::LogScope };
    const areg::ConfigKey& key = areg::log_scope();
    return remove_module_properties(key.section, key.property, confKey);
}

std::vector<Identifier> ConfigManager::remote_service_connections(const String& service) const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ServiceConnection;
    const areg::ConfigKey& key = areg::service_connection();

    std::vector<Identifier> result;
    const PropertyValue* value = property_value(service, key.property, key.position, confKey);
    if (value != nullptr)
    {
        result = value->identifier_list(areg::ConnectionIdentifiers).data();
    }

    return result;
}

String ConfigManager::remote_service_name(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ServiceName;
    const areg::ConfigKey& key = areg::service_name();
    const PropertyValue* value = property_value(service, key.property, connectType, confKey);
    return (value != nullptr ? value->as_string() : String(String::EmptyString));
}

String ConfigManager::remote_service_name(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , areg::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , areg::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return remote_service_name(service, connect);
}

bool ConfigManager::remote_service_enable(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ServiceEnable;
    const areg::ConfigKey& key = areg::service_enable();
    const PropertyValue* value = property_value(service, key.property, connectType, confKey);
    return (value != nullptr ? value->as_boolean() : areg::DEFAULT_SERVICE_ENABLED);
}

bool ConfigManager::remote_service_enable(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , areg::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , areg::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return remote_service_enable(service, connect);
}

void ConfigManager::set_service_enable(const String& service, const String& connectType, bool newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ServiceEnable;
    const areg::ConfigKey& key = areg::service_enable();
    set_module_property(service, key.property, connectType, String::make_string(newValue), confKey, is_temporary);
}

void ConfigManager::set_service_enable(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, bool newValue, bool is_temporary /*= false*/)
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , areg::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , areg::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    set_service_enable(service, connect, newValue, is_temporary);
}

String ConfigManager::remote_service_address(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ServiceAddress;
    const areg::ConfigKey& key = areg::service_address();
    const PropertyValue* value = property_value(service, key.property, connectType, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->as_string();
    }
    else
    {
        result = areg::DEFAULT_SERVICE_HOST;
    }
    
    return result;
}

String ConfigManager::remote_service_address(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , areg::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , areg::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return remote_service_address(service, connect);
}

void ConfigManager::set_service_address(const String& service, const String& connectType, const String& newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ServiceAddress;
    const areg::ConfigKey& key = areg::service_address();
    set_module_property(service, key.property, connectType, newValue, confKey, is_temporary);
}

void ConfigManager::set_service_address(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, const String& newValue, bool is_temporary /*= false*/)
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , areg::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , areg::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    set_service_address(service, connect, newValue, is_temporary);
}

uint16_t ConfigManager::remote_service_port(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ServicePort;
    const areg::ConfigKey& key = areg::service_port();
    const PropertyValue* value = property_value(service, key.property, connectType, confKey);
    return static_cast<uint16_t>(value != nullptr ? value->as_integer() : areg::DEFAULT_ROUTER_PORT);
}

uint16_t ConfigManager::remote_service_port(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                    , areg::RemoteServiceIdentifiers
                                                    , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                    , areg::ConnectionIdentifiers
                                                    , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return remote_service_port(service, connect);
}

void ConfigManager::set_service_port(const String& service, const String& connectType, uint16_t newValue, bool is_temporary /*= false*/)
{
    Lock lock(mLock);

    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::ServicePort;
    const areg::ConfigKey& key = areg::service_port();
    set_module_property(service, key.property, connectType, String::make_string(static_cast<uint32_t>(newValue)), confKey, is_temporary);
}

void ConfigManager::set_service_port(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, uint16_t newValue, bool is_temporary /*= false*/)
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                 , areg::RemoteServiceIdentifiers
                                                 , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string(static_cast<uint32_t>(connectType)
                                                  , areg::ConnectionIdentifiers
                                                  , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    set_service_port(service, connect, newValue, is_temporary);
}

String ConfigManager::log_database_property(const String& whichPosition)
{
    const areg::ConfigKey& key = areg::log_database_name();
    const PropertyValue* value = property_value(key.section, key.property, whichPosition);
    return (value != nullptr ? value->value() : String::empty_string());
}

void ConfigManager::set_db_property(const String& whichPosition, const String& newValue, bool is_temporary /*= false*/)
{
    const areg::ConfigKey& key = areg::log_database_name();
    set_module_property(key.section, key.property, whichPosition, newValue, areg::ConfigEntry::AnyKey, is_temporary);
}

uint32_t ConfigManager::buffer_block_size(const String& whichModule /*= areg::EmptyStringA*/)
{
    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::DefaultBufferBlock;
    const areg::ConfigKey& key = areg::buffer_block_size();
    const Property* prop = _get_property(mReadonlyProperties, key.section, whichModule.is_empty() ? areg::SYNTAX_ALL_MODULES : whichModule, key.property, key.position, confKey, true);
    return (prop != nullptr ? prop->value().as_integer() : 0u);
}

uint32_t ConfigManager::message_queue_size(const String& whichModule /*= areg::EmptyStringA*/)
{
    constexpr areg::ConfigEntry confKey = areg::ConfigEntry::DefaultMessageQueue;
    const areg::ConfigKey& key = areg::message_queue_size();
    const Property* prop = _get_property(mReadonlyProperties, key.section, whichModule.is_empty() ? areg::SYNTAX_ALL_MODULES : whichModule, key.property, key.position, confKey, true);
    return ( prop != nullptr ? prop->value().as_integer() : std::numeric_limits<uint32_t>::max() );
}

} // namespace areg
