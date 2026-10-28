/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/base/Thread.hpp"
#include "areg/component/EventDefs.hpp"     // QUEUE_DEFAULT_RING_CAPACITY / QUEUE_DEFAULT_FULL_WAIT_MS
#include "areg/persist/ConfigListener.hpp"

#include <filesystem>

namespace
{
    //!< The extension of the configuration file of a process.
    constexpr std::string_view  PROCESS_CONFIG_EXT  { ".init" };

    //!< The extension of the file that the saves of one configuration file lock.
    constexpr std::string_view  CONFIG_LOCK_EXT     { ".lock" };

    //!< The extension of the file that a save writes before it replaces the configuration file.
    constexpr std::string_view  CONFIG_TEMP_EXT     { ".tmp" };

    //!< The mode to read an existing configuration file.
    constexpr uint32_t  CONFIG_READ_MODE{ static_cast<uint32_t>(areg::File::OpenMode::Read)
                                        | static_cast<uint32_t>(areg::File::OpenMode::Text)
                                        | static_cast<uint32_t>(areg::File::OpenMode::Exist)
                                        | static_cast<uint32_t>(areg::File::OpenMode::ShareRead) };

    //!< The mode to open the lock file of a configuration file, created if missing.
    constexpr uint32_t  CONFIG_LOCK_MODE{ static_cast<uint32_t>(areg::File::OpenMode::ShareWrite)
                                        | static_cast<uint32_t>(areg::File::OpenMode::OpenAlways) };

    [[nodiscard]]
    inline uint32_t _find_position( const areg::ArrayList<areg::Property>& propList
                                 , uint32_t startAt
                                 , const areg::String& section
                                 , const areg::String& module
                                 , const areg::String& propName
                                 , const areg::String& position
                                 , bool exact
                                 , areg::ConfigEntry configKey) noexcept
    {
        uint32_t result { areg::INVALID_POSITION };
        const auto& list{ propList.data() };
        uint32_t count{ static_cast<uint32_t>(list.size()) };

        for (uint32_t pos = startAt; pos < count; ++ pos)
        {
            const areg::PropertyKey& key = propList[pos].key();
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

    //!< Returns the position of an entry that applies to all modules, skipping module entries.
    [[nodiscard]]
    inline uint32_t _find_all_modules_position( const areg::ArrayList<areg::Property>& propList
                                              , const areg::String& section
                                              , const areg::String& propName
                                              , const areg::String& position
                                              , areg::ConfigEntry configKey) noexcept
    {
        uint32_t pos{ _find_position(propList, 0, section, areg::SYNTAX_ALL_MODULES, propName, position, false, configKey) };
        while ((pos != areg::INVALID_POSITION) && (propList[pos].key().is_all_modules() == false))
        {
            pos = _find_position(propList, pos + 1, section, areg::SYNTAX_ALL_MODULES, propName, position, false, configKey);
        }

        return pos;
    }

    //!< Returns the position of the module entry with the given temporary flag.
    [[nodiscard]]
    inline uint32_t _find_write_position( const areg::ArrayList<areg::Property>& writeList
                                        , const areg::String& section
                                        , const areg::String& module
                                        , const areg::String& propName
                                        , const areg::String& position
                                        , areg::ConfigEntry confKey
                                        , bool isTemporary) noexcept
    {
        uint32_t pos{ _find_position(writeList, 0, section, module, propName, position, true, confKey) };
        while ((pos != areg::INVALID_POSITION) && (writeList[pos].is_temporary() != isTemporary))
        {
            pos = _find_position(writeList, pos + 1, section, module, propName, position, true, confKey);
        }

        return pos;
    }

    //!< Removes every module entry of the key, temporary or not, and returns how many were removed.
    inline uint32_t _remove_write_positions( areg::ArrayList<areg::Property>& writeList
                                           , const areg::String& section
                                           , const areg::String& module
                                           , const areg::String& propName
                                           , const areg::String& position
                                           , areg::ConfigEntry confKey) noexcept
    {
        uint32_t result{ 0u };
        uint32_t pos{ _find_position(writeList, 0, section, module, propName, position, true, confKey) };
        while (pos != areg::INVALID_POSITION)
        {
            writeList.remove_at(pos, 1u);
            ++ result;
            pos = _find_position(writeList, pos, section, module, propName, position, true, confKey);
        }

        return result;
    }

    //!< Sets the module entry; the last value set is the one in force, and only a non-temporary one is saved.
    //!< A temporary entry stays in front of the saved entry of the same key.
    template <typename Type>
    inline void _set_position_value(areg::ArrayList<areg::Property>& writeList
                                 , const areg::ArrayList<areg::Property>& readList
                                 , const areg::String& section
                                 , const areg::String& module
                                 , const areg::String& propName
                                 , const areg::String& position
                                 , areg::ConfigEntry confKey
                                 , const Type& newValue
                                 , bool isTemporary)
    {
        uint32_t readPos  = _find_position(readList , 0, section, module, propName, position, true, confKey);
        if (readPos == areg::INVALID_POSITION)
        {
            readPos = _find_all_modules_position(readList, section, propName, position, confKey);
        }

        const uint32_t tempPos{ _find_write_position(writeList, section, module, propName, position, confKey, true) };
        uint32_t savedPos{ _find_write_position(writeList, section, module, propName, position, confKey, false) };

        if (isTemporary)
        {
            const areg::PropertyValue* below{ savedPos != areg::INVALID_POSITION ? &writeList[savedPos].value()
                                            : (readPos != areg::INVALID_POSITION ? &readList[readPos].value() : nullptr) };
            if ((below != nullptr) && (newValue == static_cast<const Type&>(*below)))
            {
                if (tempPos != areg::INVALID_POSITION)
                {
                    writeList.remove_at(tempPos);
                }
            }
            else if (tempPos != areg::INVALID_POSITION)
            {
                writeList[tempPos].value() = newValue;
                if ((savedPos != areg::INVALID_POSITION) && (tempPos > savedPos))
                {
                    const areg::Property entry{ writeList.remove_at(tempPos) };
                    writeList.insert_at(savedPos, entry);
                }
            }
            else
            {
                const uint32_t insertAt{ savedPos != areg::INVALID_POSITION ? savedPos : writeList.size() };
                writeList.insert_at(insertAt, areg::Property(areg::PropertyKey(section, module, propName, position, confKey), areg::PropertyValue(newValue), areg::String::EmptyString, true));
            }

            return;
        }

        if (tempPos != areg::INVALID_POSITION)
        {
            writeList.remove_at(tempPos);
            if ((savedPos != areg::INVALID_POSITION) && (savedPos > tempPos))
            {
                -- savedPos;
            }
        }

        if ((readPos != areg::INVALID_POSITION) && (newValue == static_cast<const Type&>(readList[readPos].value())))
        {
            if (savedPos != areg::INVALID_POSITION)
            {
                writeList.remove_at(savedPos);
            }
        }
        else if (savedPos != areg::INVALID_POSITION)
        {
            writeList[savedPos].value() = newValue;
        }
        else
        {
            writeList.add(areg::Property(areg::PropertyKey(section, module, propName, position, confKey), areg::PropertyValue(newValue), areg::String::EmptyString, false));
        }
    }

    [[nodiscard]]
    inline const areg::Property* _get_property( const areg::ListProperties& list
                                              , const areg::String& section
                                              , const areg::String& module
                                              , const areg::String& propName
                                              , const areg::String& position
                                              , areg::ConfigEntry keyType
                                              , bool exactMatch) noexcept
    {
        ASSERT(keyType != areg::ConfigEntry::Invalid);

        uint32_t elemPos = _find_position(list, 0, section, module, propName, position, exactMatch, keyType);
        return (elemPos != areg::INVALID_POSITION ? &list[elemPos] : nullptr);
    }

    //!< Returns the entry of the module, searched in the process list, then in the shared list.
    [[nodiscard]]
    inline const areg::Property* _get_module_property( const areg::ListProperties& process
                                                     , const areg::ListProperties& shared
                                                     , const areg::String& section
                                                     , const areg::String& module
                                                     , const areg::String& propName
                                                     , const areg::String& position
                                                     , areg::ConfigEntry keyType) noexcept
    {
        const areg::Property* result{ _get_property(process, section, module, propName, position, keyType, true) };
        return (result != nullptr ? result : _get_property(shared, section, module, propName, position, keyType, true));
    }

    //!< Returns the entry of the shared list that applies to all modules.
    [[nodiscard]]
    inline const areg::Property* _get_all_modules_property( const areg::ListProperties& shared
                                                          , const areg::String& section
                                                          , const areg::String& propName
                                                          , const areg::String& position
                                                          , areg::ConfigEntry keyType) noexcept
    {
        ASSERT(keyType != areg::ConfigEntry::Invalid);

        const uint32_t elemPos{ _find_all_modules_position(shared, section, propName, position, keyType) };
        return (elemPos != areg::INVALID_POSITION ? &shared[elemPos] : nullptr);
    }

    //!< Returns true if both paths name the same file, existing or not.
    [[nodiscard]]
    bool _is_same_file(const areg::String& left, const areg::String& right)
    {
        if (left.is_empty() || right.is_empty())
            return false;

        std::error_code err;
        if (std::filesystem::equivalent(left.as_string(), right.as_string(), err))
            return true;

        const std::string lhs{ std::filesystem::path(left.as_string()).lexically_normal().string() };
        const std::string rhs{ std::filesystem::path(right.as_string()).lexically_normal().string() };
#if defined(_WIN32)
        return (areg::compare_ignore_case<char, char>(lhs.c_str(), rhs.c_str()) == areg::Ordering::Equal);
#else   // !defined(_WIN32)
        return (lhs == rhs);
#endif  // defined(_WIN32)
    }

    //!< Returns the full path of the default shared configuration file.
    [[nodiscard]]
    inline areg::String _default_shared_config()
    {
        return areg::File::file_full_path(areg::File::normalize_path(areg::String(areg::DEFAULT_CONFIG_FILE)));
    }

    //!< Returns true if the path names the shared configuration file, which is never written.
    [[nodiscard]]
    inline bool _is_shared_config(const areg::String& filePath, const areg::String& sharedPath)
    {
        return _is_same_file(filePath, sharedPath) || _is_same_file(filePath, _default_shared_config());
    }

    //!< Reads the entries of the module and, if allModules is true, the entries for all modules.
    uint32_t _read_config(const areg::FileBase& file, areg::ListProperties& list, const areg::String& module, bool allModules)
    {
        if (!file.is_opened())
            return 0u;

        file.move_to_begin();

        areg::String line;
        areg::Property newProperty;
        uint32_t result{ 0 };

        while (file.read_line(line) > 0)
        {
            if (!newProperty.parse(line))
                continue;
            
            const areg::PropertyKey& key = newProperty.key();
            if ((allModules && key.is_all_modules()) || (key.module() == module))
            {
                list.add(newProperty);
                ++result;
            }

            newProperty.reset();
        }

        return result;
    }

    inline bool _save_config( const areg::ListProperties& listWritable
                           , const areg::ListProperties& listReadonly
                           , const areg::String& module
                           , const areg::FileBase& srcFile
                           , areg::FileBase& dstFile
                           , bool saveAll)
    {
        if (!dstFile.is_opened() || !dstFile.can_write())
            return false;

        uint32_t count{ 0 };
        dstFile.move_to_begin();

        if (saveAll)
        {
            count = listReadonly.size();
            for (uint32_t i = 0; i < count; ++ i)
            {
                const auto& prop = listReadonly[i];
                const areg::PropertyKey& key{ prop.key() };
                const bool overridden{ (key.is_all_modules() == false) &&
                                       (_find_position(listWritable, 0, key.section(), module, key.property(), key.position(), true, key.key_type()) != areg::INVALID_POSITION) };
                if ((prop.is_temporary() == false) && (overridden == false))
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
            areg::String line;
            areg::Property newProperty;

            const bool hasSource{ srcFile.is_opened() && srcFile.can_read() };
            if (hasSource)
            {
                srcFile.move_to_begin();
            }

            while (hasSource && (srcFile.read_line(line) > 0))
            {
                if ((newProperty.parse(line) == false) || (newProperty.key().module() != module))
                {
                    dstFile.write_line(line);
                }
                else
                {
                    const areg::PropertyKey& key{ newProperty.key() };
                    const areg::Property* prop = _get_property(listWritable, key.section(), module, key.property(), key.position(), key.key_type(), true);
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

        return true;
    }
} // namespace

namespace areg {

ConfigManager::ConfigManager()
    : mModule               (Process::instance().app_name())
    , mWritableProperties   ( )
    , mReadonlyProperties   ( )
    , mIsConfigured         (false)
    , mFilePath             ( )
    , mLock                 (false)
{
}

bool ConfigManager::exist(const PropertyKey& key) const noexcept
{
    Lock lock(mLock);
    bool result = _find_position(  mWritableProperties
                                , 0
                                , key.section()
                                , mModule
                                , key.property()
                                , key.position()
                                , true
                                , key.key_type()) != areg::INVALID_POSITION;

    if (result)
        return true;

    return (_find_position(  mReadonlyProperties
                          , 0
                          , key.section()
                          , areg::SYNTAX_ALL_MODULES
                          , key.property()
                          , key.position()
                          , false
                          , key.key_type()) != areg::INVALID_POSITION);
}

areg::ListProperties ConfigManager::section_properties(const String& section) const
{
    Lock lock(mLock);
    if (section.is_empty())
    {
        return areg::ListProperties();
    }

    areg::ListProperties result;
    for (const auto& prop : mWritableProperties.data())
    {
        if (section == prop.key().section())
        {
            result.add(prop);
        }
    }

    for (const auto& prop : mReadonlyProperties.data())
    {
        const PropertyKey& key{ prop.key() };
        const String& propSec = key.section();
        if ((section == propSec) && (key.module() == mModule))
        {
            if (_find_position(mWritableProperties, 0, propSec, mModule, key.property(), key.position(), true, key.key_type()) == areg::INVALID_POSITION)
            {
                result.add(prop);
            }
        }
        else if ((section == propSec) && (section == areg::SYNTAX_ANY_VALUE) && (propSec == areg::SYNTAX_ANY_VALUE))
        {
            result.add_if_unique(prop, false);
        }
    }

    return result;
}

const Property* ConfigManager::property( const String& section
                                       , const String& propName
                                       , const String& position
                                       , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/) const noexcept
{
    Lock lock(mLock);

    keyType = (keyType == areg::ConfigEntry::Invalid) ? areg::ConfigEntry::AnyKey : keyType;
    const Property* result{ _get_module_property(mWritableProperties, mReadonlyProperties, section, mModule, propName, position, keyType) };
    return (result != nullptr ? result : _get_all_modules_property(mReadonlyProperties, section, propName, position, keyType));
}

const Property * ConfigManager::module_property( const String& section
                                                 , const String& propName
                                                 , const String& position
                                                 , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/) const noexcept
{
    Lock lock(mLock);

    keyType = (keyType == areg::ConfigEntry::Invalid) ? areg::ConfigEntry::AnyKey : keyType;
    return _get_property(mWritableProperties, section, mModule, propName, position, keyType, true);
}

void ConfigManager::set_module_property( const String& section
                                     , const String& propName
                                     , const String& position
                                     , const String& value
                                     , areg::ConfigEntry keyType /*= areg::ConfigEntry::AnyKey*/
                                     , bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    keyType = (keyType == areg::ConfigEntry::Invalid) ? areg::ConfigEntry::AnyKey : keyType;
    _set_position_value<String>(mWritableProperties, mReadonlyProperties, section, mModule, propName, position, keyType, value, isTemporary);
}

void ConfigManager::remove_module_property(const String& section, const String& propName, const String& position, areg::ConfigEntry keyType)
{
    Lock lock(mLock);
    static_cast<void>(_remove_write_positions(mWritableProperties, section, mModule, propName, position, keyType));
}

int32_t ConfigManager::remove_module_properties(const String& section, const String& propName, areg::ConfigEntry keyType)
{
    Lock lock(mLock);
    int32_t result{ 0 };
    uint32_t i{ 0 };
    while (i < mWritableProperties.size())
    {
        const PropertyKey& key{ mWritableProperties[i].key() };
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
    uint32_t i{ 0 };
    while (i < mWritableProperties.size())
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

bool ConfigManager::read_config(const String& filePath /*= String::EmptyString*/, ConfigListener * listener /*= nullptr*/) noexcept
{
    Lock lock(mLock);
    if (mIsConfigured)
        return true;
    
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
    File fileConfig(path, CONFIG_READ_MODE);
    if (fileConfig.open() == false)
        return mIsConfigured;

    if (listener != nullptr)
    {
        listener->prepare_read_configuration(*this);
    }

    if (read_config(fileConfig, nullptr))
    {
        mFilePath = fileConfig.name();
        read_process_config();
    }

    // The listener sees both files and its values win over them.
    if (listener != nullptr)
    {
        listener->post_read_configuration(*this);
    }

    return mIsConfigured;
}

bool ConfigManager::read_config(const FileBase& file, ConfigListener * listener /*= nullptr*/) noexcept
{
    Lock lock(mLock);
    if (mIsConfigured)
        return true;

    mWritableProperties.clear();
    mReadonlyProperties.clear();
    if (listener != nullptr)
    {
        listener->prepare_read_configuration(*this);
    }

    mIsConfigured = _read_config(file, mReadonlyProperties, mModule, true) != 0;
    if (listener != nullptr)
    {
        listener->post_read_configuration(*this);
    }

    return mIsConfigured;
}

bool ConfigManager::save_config(const String& filePath, ConfigListener * listener /*= nullptr*/)
{
    Lock lock(mLock);

    constexpr uint32_t modeWrite{ static_cast<uint32_t>(File::OpenMode::Read)
                                | static_cast<uint32_t>(File::OpenMode::Text)
                                | static_cast<uint32_t>(File::OpenMode::Create)
                                | static_cast<uint32_t>(File::OpenMode::Write) };

    const String dstPath{ filePath.is_empty() ? process_config_file() : File::file_full_path(File::normalize_path(filePath)) };
    if (_is_shared_config(dstPath, mFilePath))
        return false;

    const String dstDir{ File::file_directory(dstPath) };
    if ((dstDir.is_empty() == false) && (File::has_dir(dstDir) == false))
    {
        File::create_dir_cascaded(dstDir);
    }

    // Saves of one file wait for each other, also in other processes.
    File lockFile(dstPath + CONFIG_LOCK_EXT, CONFIG_LOCK_MODE);
    if ((lockFile.open() == false) || (lockFile.lock_file(true) == false))
        return false;

    const String tempPath{ dstPath
                         + "." + String::make_string(static_cast<uint64_t>(Process::instance().id()))
                         + "." + String::make_string(static_cast<uint64_t>(Thread::current_thread_id()))
                         + CONFIG_TEMP_EXT };
    File srcFile(dstPath, CONFIG_READ_MODE);
    File dstFile(tempPath, modeWrite);
    bool result{ false };
    if (dstFile.open())
    {
        static_cast<void>(srcFile.open());
        result = save_config(srcFile, dstFile, false, listener);
        dstFile.flush();
        dstFile.close();
        srcFile.close();

        result = result && File::move_file(tempPath, dstPath);
        if (result == false)
        {
            File::delete_file(tempPath);
        }
    }

    lockFile.unlock_file();
    return result;
}

String ConfigManager::process_config_file() const
{
    Lock lock(mLock);
    const String sharedPath{ mFilePath.is_empty() ? _default_shared_config() : mFilePath };
    return File::make_full_path(File::file_directory(sharedPath), mModule + PROCESS_CONFIG_EXT);
}

bool ConfigManager::read_process_config()
{
    Lock lock(mLock);

    mWritableProperties.clear();
    const String filePath{ process_config_file() };
    if (_is_shared_config(filePath, mFilePath) || (File::has_file(filePath) == false))
        return false;

    // Reads without the lock if the lock file cannot be opened.
    File lockFile(filePath + CONFIG_LOCK_EXT, CONFIG_LOCK_MODE);
    const bool locked{ lockFile.open() && lockFile.lock_file(false) };

    File file(filePath, CONFIG_READ_MODE);
    const bool result{ file.open() && (_read_config(file, mWritableProperties, mModule, false) != 0) };
    file.close();

    if (locked)
    {
        lockFile.unlock_file();
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

    bool result = _save_config(mWritableProperties, mReadonlyProperties, mModule, srcFile, dstFile, saveAll);
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


Version ConfigManager::config_version() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ConfigVersion };
    constexpr const areg::ConfigKey& key{ areg::config_version() };

    const Property* prop = _get_property(mReadonlyProperties, key.section, areg::SYNTAX_ALL_MODULES, key.property, key.position, confKey, true);
    return (prop != nullptr ? prop->value_string() : areg::CONFIG_VERSION);
}

std::vector<Identifier> ConfigManager::service_list() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ServiceList };
    constexpr const areg::ConfigKey& key{ areg::service_list() };

    const Property* prop = property(key.section, key.property, key.position, confKey);
    return (prop != nullptr) ? prop->value().identifier_list(areg::RemoteServiceIdentifiers).data() : std::vector<Identifier>();
}

std::vector<Identifier> ConfigManager::log_targets() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey { areg::ConfigEntry::LogTarget };
    constexpr const areg::ConfigKey& key{ areg::log_target() };

    const Property* prop = property(key.section, key.property, key.position, confKey);
    return (prop != nullptr ? prop->value().identifier_list(areg::LogTypeIdentifiers).data() : std::vector<Identifier>());
}

bool ConfigManager::logging_status() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogStatus };
    constexpr const areg::ConfigKey& key{ areg::log_status() };

    const PropertyValue* prop = property_value(key.section, key.property, key.position, confKey);
    return (prop != nullptr ? prop->as_boolean() : areg::DEFAULT_LOG_ENABLED);
}

Version ConfigManager::log_version() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogVersion };
    constexpr const areg::ConfigKey& key{ areg::log_version() };

    const PropertyValue* prop = property_value(key.section, key.property, key.position, confKey);
    return (prop != nullptr ? prop->as_string() : areg::CONFIG_VERSION);
}

bool ConfigManager::log_enabled(const String& logType) const noexcept
{
    Lock lock(mLock);

    if (!logging_status())
        return false;
    
    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogEnable };
    constexpr const areg::ConfigKey& key{ areg::log_enable() };

    const PropertyValue* prop = property_value(key.section, key.property, logType, confKey);
    return (prop != nullptr ? prop->as_boolean() : areg::DEFAULT_LOG_ENABLED);
}

bool ConfigManager::log_enabled(areg::LogTarget logType) const noexcept
{
    String id = Identifier::to_string( static_cast<uint32_t>(logType)
                                     , areg::LogTypeIdentifiers
                                     , static_cast<uint32_t>(areg::LogTarget::Undefined));
    return log_enabled(id);
}

void ConfigManager::set_log_enabled(areg::LogTarget logType, bool newValue, bool isTemporary /*= false*/)
{
    String id = Identifier::to_string( static_cast<uint32_t>(logType)
                                     , areg::LogTypeIdentifiers
                                     , static_cast<uint32_t>(areg::LogTarget::Undefined));
    set_log_enabled(id, newValue, isTemporary);
}

String ConfigManager::log_file_location() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogFileLocation };
    constexpr const areg::ConfigKey& key{ areg::log_file_location() };
    const PropertyValue* prop = property_value(key.section, key.property, key.position, confKey);

    return (prop != nullptr ? prop->as_string() : areg::DEFAULT_LOG_FILE);
}

bool ConfigManager::log_file_append() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogFileAppend };
    constexpr const areg::ConfigKey& key{ areg::log_file_append() };
    const PropertyValue* prop = property_value(key.section, key.property, key.position, confKey);
    return ((prop != nullptr) && prop->as_boolean());
}

void ConfigManager::set_file_append(bool newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogFileAppend };
    constexpr const areg::ConfigKey& key{ areg::log_file_append() };
    set_module_property(key.section, key.property, key.position, String::make_string(newValue), confKey, isTemporary);
}

uint32_t ConfigManager::remote_queue_size() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogRemoteQueueSize };
    constexpr const areg::ConfigKey& key{ areg::remote_queue_size() };
    const PropertyValue* prop = property_value(key.section, key.property, key.position, confKey);
    return (prop != nullptr ? prop->as_integer() : areg::DEFAULT_LOG_QUEUE_SIZE);
}

void ConfigManager::set_remote_queue_size(uint32_t newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogRemoteQueueSize };
    constexpr const areg::ConfigKey& key{ areg::remote_queue_size() };
    set_module_property(key.section, key.property, key.position, String::make_string(newValue), confKey, isTemporary);
}

String ConfigManager::log_layout_enter() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogLayoutEnter };
    constexpr const areg::ConfigKey& key{ areg::log_layout_enter() };
    const PropertyValue* prop = property_value(key.section, key.property, key.position, confKey);

    return (prop != nullptr ? prop->as_string() : areg::DEFAULT_LAYOUT_SCOPE_ENTER);
}

void ConfigManager::set_layout_enter(const String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogLayoutEnter };
    constexpr const areg::ConfigKey& key{ areg::log_layout_enter() };
    set_module_property(key.section, key.property, key.position, newValue, confKey, isTemporary);
}

String ConfigManager::log_layout_message() const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogLayoutMessage };
    constexpr const areg::ConfigKey& key{ areg::log_layout_message() };
    const PropertyValue* prop = property_value(key.section, key.property, key.position, confKey);

    return (prop != nullptr ? prop->as_string() : areg::DEFAULT_LAYOUT_LOG_MESSAGE);
}

void ConfigManager::set_layout_message(const String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogLayoutMessage };
    constexpr const areg::ConfigKey& key{ areg::log_layout_message() };
    set_module_property(key.section, key.property, key.position, newValue, confKey, isTemporary);
}

String ConfigManager::log_layout_exit() const
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogLayoutExit };
    constexpr const areg::ConfigKey& key{ areg::log_layout_exit() };
    const PropertyValue* prop = property_value(key.section, key.property, key.position, confKey);

    return (prop != nullptr ? prop->as_string() : areg::DEFAULT_LAYOUT_SCOPE_EXIT);
}

void ConfigManager::set_layout_exit(const String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::LogLayoutExit };
    constexpr const areg::ConfigKey& key{ areg::log_layout_exit() };
    set_module_property(key.section, key.property, key.position, newValue, confKey, isTemporary);
}

uint32_t ConfigManager::module_log_scopes(std::vector<Property>& scopeList) const noexcept
{
    Lock lock(mLock);

    std::vector<Property> moduleScopes;
    std::vector<Property> processScopes;
    log_scope_layers(processScopes, moduleScopes, scopeList);
    scopeList.insert(scopeList.end(), moduleScopes.begin(), moduleScopes.end());
    scopeList.insert(scopeList.end(), processScopes.begin(), processScopes.end());

    return static_cast<uint32_t>(scopeList.size());
}

void ConfigManager::log_scope_layers( std::vector<Property>& processScopes
                                    , std::vector<Property>& moduleScopes
                                    , std::vector<Property>& sharedScopes) const
{
    Lock lock(mLock);

    for (const auto& entry : mReadonlyProperties.data())
    {
        const PropertyKey& key = entry.key();
        if (key.key_type() == areg::ConfigEntry::LogScope)
        {
            (key.is_all_modules() ? sharedScopes : moduleScopes).push_back(entry);
        }
    }

    for (const auto& entry : mWritableProperties.data())
    {
        const PropertyKey& key = entry.key();
        if (key.key_type() == areg::ConfigEntry::LogScope)
        {
            ASSERT(key.module() == mModule);
            processScopes.push_back(entry);
        }
    }
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
    const areg::ConfigKey& key{ areg::log_scope() };
    return (_remove_write_positions(mWritableProperties, key.section, mModule, key.property, scopeName, confKey) != 0u);
}

int32_t ConfigManager::remove_module_scopes()
{
    constexpr areg::ConfigEntry confKey{ areg::ConfigEntry::LogScope };
    const areg::ConfigKey& key = areg::log_scope();
    return remove_module_properties(key.section, key.property, confKey);
}

std::vector<Identifier> ConfigManager::remote_service_connections(const String& service) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ServiceConnection };
    constexpr const areg::ConfigKey& key{ areg::service_connection() };

    const PropertyValue* prop = property_value(service, key.property, key.position, confKey);
    return (prop != nullptr ? prop->identifier_list(areg::ConnectionIdentifiers).data() : std::vector<Identifier>());
}

String ConfigManager::remote_service_name(const String& service, const String& connectType) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ServiceName };
    constexpr const areg::ConfigKey& key{ areg::service_name() };
    const PropertyValue* prop = property_value(service, key.property, connectType, confKey);
    return (prop != nullptr ? prop->as_string() : String());
}

String ConfigManager::remote_service_name(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                 , areg::RemoteServiceIdentifiers
                                                 , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string( static_cast<uint32_t>(connectType)
                                                  , areg::ConnectionIdentifiers
                                                  , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return remote_service_name(service, connect);
}

bool ConfigManager::remote_service_enable(const String& service, const String& connectType) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ServiceEnable };
    constexpr const areg::ConfigKey& key{ areg::service_enable() };
    const PropertyValue* prop = property_value(service, key.property, connectType, confKey);
    return (prop != nullptr ? prop->as_boolean() : areg::DEFAULT_SERVICE_ENABLED);
}

bool ConfigManager::remote_service_enable(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                 , areg::RemoteServiceIdentifiers
                                                 , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string( static_cast<uint32_t>(connectType)
                                                  , areg::ConnectionIdentifiers
                                                  , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return remote_service_enable(service, connect);
}

void ConfigManager::set_service_enable(const String& service, const String& connectType, bool newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ServiceEnable };
    constexpr const areg::ConfigKey& key{ areg::service_enable() };
    set_module_property(service, key.property, connectType, String::make_string(newValue), confKey, isTemporary);
}

void ConfigManager::set_service_enable(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, bool newValue, bool isTemporary /*= false*/)
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                 , areg::RemoteServiceIdentifiers
                                                 , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string( static_cast<uint32_t>(connectType)
                                                  , areg::ConnectionIdentifiers
                                                  , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    set_service_enable(service, connect, newValue, isTemporary);
}

String ConfigManager::remote_service_address(const String& service, const String& connectType) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ServiceAddress };
    constexpr const areg::ConfigKey& key{ areg::service_address() };
    const PropertyValue* prop = property_value(service, key.property, connectType, confKey);
    return (prop != nullptr ? prop->as_string() : areg::LocalHost);
}

String ConfigManager::remote_service_address(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                 , areg::RemoteServiceIdentifiers
                                                 , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string( static_cast<uint32_t>(connectType)
                                                  , areg::ConnectionIdentifiers
                                                  , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return remote_service_address(service, connect);
}

void ConfigManager::set_service_address(const String& service, const String& connectType, const String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ServiceAddress };
    constexpr const areg::ConfigKey& key{ areg::service_address() };
    set_module_property(service, key.property, connectType, newValue, confKey, isTemporary);
}

void ConfigManager::set_service_address(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, const String& newValue, bool isTemporary /*= false*/)
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                 , areg::RemoteServiceIdentifiers
                                                 , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string( static_cast<uint32_t>(connectType)
                                                  , areg::ConnectionIdentifiers
                                                  , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    set_service_address(service, connect, newValue, isTemporary);
}

uint16_t ConfigManager::remote_service_port(const String& service, const String& connectType) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ServicePort };
    constexpr const areg::ConfigKey& key{ areg::service_port() };
    const PropertyValue* prop = property_value(service, key.property, connectType, confKey);
    return static_cast<uint16_t>(prop != nullptr ? prop->as_integer() : areg::DEFAULT_ROUTER_PORT);
}

uint16_t ConfigManager::remote_service_port(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                 , areg::RemoteServiceIdentifiers
                                                 , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string( static_cast<uint32_t>(connectType)
                                                  , areg::ConnectionIdentifiers
                                                  , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return remote_service_port(service, connect);
}

void ConfigManager::set_service_port(const String& service, const String& connectType, uint16_t newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::ServicePort };
    constexpr const areg::ConfigKey& key{ areg::service_port() };
    set_module_property(service, key.property, connectType, String::make_string(static_cast<uint32_t>(newValue)), confKey, isTemporary);
}

void ConfigManager::set_service_port(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType, uint16_t newValue, bool isTemporary /*= false*/)
{
    const String& service = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                 , areg::RemoteServiceIdentifiers
                                                 , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String & connect = Identifier::to_string( static_cast<uint32_t>(connectType)
                                                  , areg::ConnectionIdentifiers
                                                  , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    set_service_port(service, connect, newValue, isTemporary);
}

String ConfigManager::log_database_property(const String& whichPosition) const noexcept
{
    constexpr const areg::ConfigKey& key{ areg::log_database_name() };
    const PropertyValue* prop = property_value(key.section, key.property, whichPosition);
    return (prop != nullptr ? prop->value() : String());
}

void ConfigManager::set_db_property(const String& whichPosition, const String& newValue, bool isTemporary /*= false*/)
{
    constexpr const areg::ConfigKey& key{ areg::log_database_name() };
    set_module_property(key.section, key.property, whichPosition, newValue, areg::ConfigEntry::AnyKey, isTemporary);
}

uint32_t ConfigManager::buffer_block_size(const String& whichModule /*= areg::EmptyStringA*/) noexcept
{
    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::DefaultBufferBlock };
    constexpr const areg::ConfigKey& key{ areg::buffer_block_size() };
    const Property* prop = _get_property(mReadonlyProperties, key.section, whichModule.is_empty() ? areg::SYNTAX_ALL_MODULES : whichModule, key.property, key.position, confKey, true);
    return (prop != nullptr ? prop->value().as_integer() : 0u);
}

uint32_t ConfigManager::queue_capacity(const String& whichModule /*= areg::EmptyStringA*/) noexcept
{
    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::QueueCapacity };
    constexpr const areg::ConfigKey& key{ areg::queue_capacity() };
    const Property* prop = _get_property(mReadonlyProperties, key.section, whichModule.is_empty() ? areg::SYNTAX_ALL_MODULES : whichModule, key.property, key.position, confKey, true);
    const uint32_t value = (prop != nullptr ? static_cast<uint32_t>(prop->value().as_integer()) : 0u);
    return (value != 0u ? value : areg::QUEUE_DEFAULT_RING_CAPACITY);
}

uint32_t ConfigManager::queue_wait_timeout(const String& whichModule /*= areg::EmptyStringA*/) noexcept
{
    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::QueueWaitTimeout };
    constexpr const areg::ConfigKey& key{ areg::queue_wait_timeout() };
    const Property* prop = _get_property(mReadonlyProperties, key.section, whichModule.is_empty() ? areg::SYNTAX_ALL_MODULES : whichModule, key.property, key.position, confKey, true);
    const uint32_t value = (prop != nullptr ? static_cast<uint32_t>(prop->value().as_integer()) : 0u);
    return (value != 0u ? value : areg::QUEUE_DEFAULT_FULL_WAIT_MS);
}

bool ConfigManager::queue_drop_on_full(const String& whichModule /*= areg::EmptyStringA*/) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::QueueDropOnFull };
    constexpr const areg::ConfigKey& key{ areg::queue_drop_on_full() };

    // Step 1: module-specific entry (caller-supplied or current process).
    const String& mod{ whichModule.is_empty() ? mModule : whichModule };
    if (!mod.is_empty())
    {
        const Property* prop = _get_module_property(mWritableProperties, mReadonlyProperties, key.section, mod, key.property, key.position, confKey);
        if (prop != nullptr)
            return prop->value().as_boolean();
    }

    // Step 2: wildcard "*" entry.
    {
        const Property* prop = _get_all_modules_property(mReadonlyProperties, key.section, key.property, key.position, confKey);
        if (prop != nullptr)
            return prop->value().as_boolean();
    }

    // Step 3: compile-time default -- lossless (block, never drop).
    return false;
}

uint32_t ConfigManager::network_sndbuf(const String& module /*= areg::EmptyStringA*/, const String& connectType /*= areg::EmptyStringA*/) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::NetSocketSndbuf };
    constexpr const areg::ConfigKey& key{ areg::net_socket_sndbuf() };
    const String& transport{ connectType.is_empty() ? String(areg::SYNTAX_ALL_MODULES) : connectType };

    // Step 1: module-specific entry (caller-supplied or current process)
    const String& mod{ module.is_empty() ? mModule : module };
    if (!mod.is_empty())
    {
        const Property* prop = _get_module_property(mWritableProperties, mReadonlyProperties, key.section, mod, transport, key.position, confKey);
        if (prop != nullptr)
            return static_cast<uint32_t>(prop->value().as_integer()) * 1024u;
    }

    // Step 2: wildcard "*" entry
    {
        const Property* prop = _get_all_modules_property(mReadonlyProperties, key.section, transport, key.position, confKey);
        if (prop != nullptr)
            return static_cast<uint32_t>(prop->value().as_integer()) * 1024u;
    }

    // Step 3: compile-time default
    return areg::SOCKET_SEND_BUFFER_SIZE;
}

uint32_t ConfigManager::network_sndbuf(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept
{
    const String& module = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                , areg::RemoteServiceIdentifiers
                                                , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String& connect = Identifier::to_string( static_cast<uint32_t>(connectType)
                                                 , areg::ConnectionIdentifiers
                                                 , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return network_sndbuf(module, connect);
}

uint32_t ConfigManager::network_rcvbuf(const String& module /*= areg::EmptyStringA*/, const String& connectType /*= areg::EmptyStringA*/) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::NetSocketRcvbuf };
    constexpr const areg::ConfigKey& key{ areg::net_socket_rcvbuf() };
    const String& transport{ connectType.is_empty() ? String(areg::SYNTAX_ALL_MODULES) : connectType };

    // Step 1: module-specific entry (caller-supplied or current process)
    const String& mod{ module.is_empty() ? mModule : module };
    if (!mod.is_empty())
    {
        const Property* prop = _get_module_property(mWritableProperties, mReadonlyProperties, key.section, mod, transport, key.position, confKey);
        if (prop != nullptr)
            return static_cast<uint32_t>(prop->value().as_integer()) * 1024u;
    }

    // Step 2: wildcard "*" entry
    {
        const Property* prop = _get_all_modules_property(mReadonlyProperties, key.section, transport, key.position, confKey);
        if (prop != nullptr)
            return static_cast<uint32_t>(prop->value().as_integer()) * 1024u;
    }

    // Step 3: compile-time default
    return areg::SOCKET_RECV_BUFFER_SIZE;
}

uint32_t ConfigManager::network_rcvbuf(areg::RemoteServiceKind serviceType, areg::ConnectionType connectType) const noexcept
{
    const String& module = Identifier::to_string( static_cast<uint32_t>(serviceType)
                                                , areg::RemoteServiceIdentifiers
                                                , static_cast<uint32_t>(areg::RemoteServiceKind::Unknown));
    const String& connect = Identifier::to_string( static_cast<uint32_t>(connectType)
                                                 , areg::ConnectionIdentifiers
                                                 , static_cast<uint32_t>(areg::ConnectionType::Undefined));
    return network_rcvbuf(module, connect);
}

uint32_t ConfigManager::network_drain_limit(const String& module, const String& connectType) const noexcept
{
    Lock lock(mLock);
    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::NetSocketDrain };
    constexpr const areg::ConfigKey& key{ areg::net_socket_drain() };
    const String& transport{ connectType.is_empty() ? String(areg::SYNTAX_ALL_MODULES) : connectType };

    const String& mod{ module.is_empty() ? mModule : module };
    if (!mod.is_empty())
    {
        const Property* prop = _get_module_property(mWritableProperties, mReadonlyProperties, key.section, mod, transport, key.position, confKey);
        if (prop != nullptr)
            return static_cast<uint32_t>(prop->value().as_integer());
    }

    {
        const Property* prop = _get_all_modules_property(mReadonlyProperties, key.section, transport, key.position, confKey);
        if (prop != nullptr)
            return static_cast<uint32_t>(prop->value().as_integer());
    }

    return areg::DEFAULT_DRAIN_LIMIT;
}

uint32_t ConfigManager::network_pool_pairs(const String& module, const String& connectType) const noexcept
{
    Lock lock(mLock);
    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::NetPoolPairs };
    constexpr const areg::ConfigKey& key{ areg::net_pool_pairs() };
    const String& transport{ connectType.is_empty() ? String(areg::SYNTAX_ALL_MODULES) : connectType };

    const String& mod{ module.is_empty() ? mModule : module };
    if (!mod.is_empty())
    {
        const Property* prop = _get_module_property(mWritableProperties, mReadonlyProperties, key.section, mod, transport, key.position, confKey);
        if (prop != nullptr)
            return static_cast<uint32_t>(prop->value().as_integer());
    }

    {
        const Property* prop = _get_all_modules_property(mReadonlyProperties, key.section, transport, key.position, confKey);
        if (prop != nullptr)
            return static_cast<uint32_t>(prop->value().as_integer());
    }

    return areg::DEFAULT_POOL_PAIRS;
}

uint32_t ConfigManager::network_timeout(const String& module, const String& connectType) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::NetSocketTimeout };
    constexpr const areg::ConfigKey&  key{ areg::net_socket_timeout() };
    const String& transport{ connectType.is_empty() ? String(areg::SYNTAX_ALL_MODULES) : connectType };

    // Step 1: module-specific entry
    const String& mod{ module.is_empty() ? mModule : module };
    if (!mod.is_empty())
    {
        const Property* prop = _get_module_property(mWritableProperties, mReadonlyProperties, key.section, mod, transport, key.position, confKey);
        if ((prop != nullptr) && (prop->value().as_integer() > 0))
            return static_cast<uint32_t>(prop->value().as_integer());
    }

    // Step 2: wildcard "*" entry
    {
        const Property* prop = _get_all_modules_property(mReadonlyProperties, key.section, transport, key.position, confKey);
        if ((prop != nullptr) && (prop->value().as_integer() > 0))
            return static_cast<uint32_t>(prop->value().as_integer());
    }

    // Step 3: the build default, resolved by the connection
    return 0u;
}

uint32_t ConfigManager::network_cache(const String& module, const String& connectType) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::NetThreadCache };
    constexpr const areg::ConfigKey&  key{ areg::net_thread_cache() };
    const String& transport{ connectType.is_empty() ? String(areg::SYNTAX_ALL_MODULES) : connectType };

    // Step 1: module-specific entry
    const String& mod{ module.is_empty() ? mModule : module };
    if (!mod.is_empty())
    {
        const Property* prop = _get_module_property(mWritableProperties, mReadonlyProperties, key.section, mod, transport, key.position, confKey);
        if ((prop != nullptr) && (prop->value().as_integer() > 0))
            return static_cast<uint32_t>(prop->value().as_integer()) * areg::ONE_KILOBYTE;
    }

    // Step 2: wildcard "*" entry
    {
        const Property* prop = _get_all_modules_property(mReadonlyProperties, key.section, transport, key.position, confKey);
        if ((prop != nullptr) && (prop->value().as_integer() > 0))
            return static_cast<uint32_t>(prop->value().as_integer()) * areg::ONE_KILOBYTE;
    }

    // Step 3: compile-time default
    return areg::DEFAULT_THREAD_CACHE;
}

uint32_t ConfigManager::network_keepalive(const String& module, const String& connectType) const noexcept
{
    Lock lock(mLock);

    constexpr const areg::ConfigEntry confKey{ areg::ConfigEntry::NetKeepAlive };
    constexpr const areg::ConfigKey&  key{ areg::net_keepalive() };
    const String& transport{ connectType.is_empty() ? String(areg::SYNTAX_ALL_MODULES) : connectType };

    // Step 1: module-specific entry
    const String& mod{ module.is_empty() ? mModule : module };
    if (!mod.is_empty())
    {
        const Property* prop = _get_module_property(mWritableProperties, mReadonlyProperties, key.section, mod, transport, key.position, confKey);
        if ((prop != nullptr) && (prop->value().as_integer() > 0))
            return static_cast<uint32_t>(prop->value().as_integer());
    }

    // Step 2: wildcard "*" entry
    {
        const Property* prop = _get_all_modules_property(mReadonlyProperties, key.section, transport, key.position, confKey);
        if ((prop != nullptr) && (prop->value().as_integer() > 0))
            return static_cast<uint32_t>(prop->value().as_integer());
    }

    // Step 3: compile-time default
    return areg::SOCKET_KEEPALIVE_SEC;
}

} // namespace areg
