/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/persist/ConfigManager.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, ConfigManager object to read configuration file.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/persist/ConfigManager.hpp"

#include "areg/appbase/NEApplication.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/persist/IEConfigurationListener.hpp"

namespace
{
    inline uint32_t _findPosition( const TEArrayList<Property>& propList
                                 , uint32_t startAt
                                 , const String& section
                                 , const String& module
                                 , const String& property
                                 , const String& position
                                 , bool exact
                                 , NEPersistence::eConfigKeys configKey)
    {
        uint32_t result { NECommon::INVALID_POSITION };
        const auto& list{ propList.getData() };
        uint32_t count{ static_cast<uint32_t>(list.size()) };

        for (uint32_t pos = startAt; pos < count; ++ pos)
        {
            const PropertyKey& key = propList[pos].getKey();
            if ((configKey == NEPersistence::eConfigKeys::EntryAnyKey) || (configKey == key.getKeyType()))
            {
                if ((exact && key.isExactProperty(section, module, property, position)) ||
                    (!exact && key.isModuleProperty(section, module, property, position)))
                {
                    result = pos;
                    break;
                }
            }
        }

        return result;
    }

    template <typename Type>
    inline void _setPositionValue( TEArrayList<Property>& writeList
                                 , const TEArrayList<Property>& readList
                                 , const String& section
                                 , const String& module
                                 , const String& property
                                 , const String& position
                                 , NEPersistence::eConfigKeys confKey
                                 , const Type& newValue
                                 , bool isTemporary)
    {
        uint32_t readPos  = _findPosition(readList , 0, section, NEPersistence::SYNTAX_ALL_MODULES, property, position, false, confKey);
        uint32_t writePos = _findPosition(writeList, 0, section, module, property, position, true , confKey);

        while ((writePos != NECommon::INVALID_POSITION) && (writeList[writePos].isTemporary() != isTemporary))
        {
            writePos = _findPosition(writeList, writePos + 1, section, module, property, position, true, confKey);
        }

        if (readPos == NECommon::INVALID_POSITION)
        {
            if (writePos != NECommon::INVALID_POSITION)
            {
                writeList[readPos].getValue() = newValue;
            }
            else
            {
                writeList.add(Property(PropertyKey(section, module, property, position, confKey), PropertyValue(newValue), String::EmptyString, isTemporary));
            }
        }
        else
        {
            const PropertyValue& readValue = readList[readPos].getValue();
            if (newValue != static_cast<const Type&>(readValue))
            {
                if ((writePos != NECommon::INVALID_POSITION) && (writeList[writePos].isTemporary() == isTemporary))
                {
                    writeList[writePos].getValue() = newValue;
                }
                else
                {
                    writeList.add(Property(PropertyKey(section, module, property, position, confKey), PropertyValue(newValue), String::EmptyString, isTemporary));
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
                                       , NEPersistence::eConfigKeys keyType
                                       , bool exactMatch)
    {
        ASSERT(keyType != NEPersistence::eConfigKeys::EntryInvalid);

        uint32_t elemPos = _findPosition(list, 0, section, module, property, position, exactMatch, keyType);
        return (elemPos != NECommon::INVALID_POSITION ? &list[elemPos] : nullptr);
    }

    uint32_t _readConfig(const FileBase& file, NEPersistence::ListProperties& OUT listWritable, NEPersistence::ListProperties& OUT listReadonly, const String& module)
    {
        uint32_t result{ 0 };

        if (file.isOpened())
        {
            file.moveToBegin();

            String line;
            Property newProperty;

            while (file.readLine(line) > 0)
            {
                if (newProperty.parseProperty(line))
                {
                    // add new entry if unique. otherwise, update existing.
                    const PropertyKey& Key = newProperty.getKey();
                    if (Key.isAllModules())
                    {
                        listReadonly.add(newProperty);
                        ++result;
                    }
                    else if (newProperty.isModuleProperty(module))
                    {
                        listWritable.add(newProperty);
                        ++result;
                    }

                    newProperty.resetData();
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
                    if (prop.isTemporary() == false)
                    {
                        dstFile.writeLine(prop.convToString());
                    }
                }

                count = listWritable.getSize();
                for (uint32_t i = 0; i < count; ++ i)
                {
                    const auto& prop = listWritable[i];
                    if (prop.isTemporary() == false)
                    {
                        dstFile.writeLine(prop.convToString());
                    }
                }
            }
            else
            {
                String line;
                Property newProperty;

                while (srcFile.readLine(line) > 0)
                {
                    if ((newProperty.parseProperty(line) == false) || (newProperty.getKey().getModule() != module))
                    {
                        dstFile.writeLine(line);
                    }
                    else
                    {
                        const PropertyKey& key{ newProperty.getKey() };
                        const Property* prop = _getProperty(listWritable, key.getSection(), module, key.getProperty(), key.getPosition(), key.getKeyType(), true);
                        if ((prop != nullptr) && prop->isTemporary())
                        {
                            dstFile.writeLine(line);
                        }
                    }
                }

                count = listWritable.getSize();
                for (uint32_t i = 0; i < count; ++i)
                {
                    const auto& prop = listWritable[i];
                    if (prop.isTemporary() == false)
                    {
                        dstFile.writeLine(prop.convToString());
                    }
                }
            }

            result = true;
        }

        return result;
    }
} // namespace

ConfigManager::ConfigManager( void )
    : mModule               (Process::getInstance().getAppName())
    , mWritableProperties   ( )
    , mReadonlyProperties   ( )
    , mIsConfigured         (false)
    , mFilePath             ( )
    , mLock                 (false)
{
}

bool ConfigManager::existProperty(const PropertyKey& key) const
{
    Lock lock(mLock);
    bool result = _findPosition(  mWritableProperties
                                , 0
                                , key.getSection()
                                , mModule
                                , key.getProperty()
                                , key.getPosition()
                                , true
                                , key.getKeyType()) != NECommon::INVALID_POSITION;

    if (result == false)
    {
        result = _findPosition(  mReadonlyProperties
                                , 0
                                , key.getSection()
                                , NEPersistence::SYNTAX_ALL_MODULES
                                , key.getProperty()
                                , key.getPosition()
                                , false
                                , key.getKeyType()) != NECommon::INVALID_POSITION;
    }

    return result;
}

NEPersistence::ListProperties ConfigManager::getSectionProperties(const String& section) const
{
    Lock lock(mLock);
    NEPersistence::ListProperties result;
    if (section.isEmpty())
    {
        return result;
    }

    for (const auto& prop : mWritableProperties.getData())
    {
        if (section == prop.getKey().getSection())
        {
            result.add(prop);
        }
    }

    for (const auto& prop : mReadonlyProperties.getData())
    {
        const String& propSec = prop.getKey().getSection();
        if ((section == propSec) && (section == NEPersistence::SYNTAX_ANY_VALUE) && (propSec == NEPersistence::SYNTAX_ANY_VALUE))
        {
            result.addIfUnique(prop, false);
        }
    }

    return result;
}

const Property* ConfigManager::getProperty( const String& section
                                         , const String& property
                                         , const String& position
                                         , NEPersistence::eConfigKeys keyType /*= NEPersistence::eConfigKeys::EntryAnyKey*/) const
{
    Lock lock(mLock);

    keyType = keyType == NEPersistence::eConfigKeys::EntryInvalid ? NEPersistence::eConfigKeys::EntryAnyKey : keyType;
    const Property* result{ _getProperty(mWritableProperties, section, mModule, property, position, keyType, true)};
    return (result != nullptr ? result : _getProperty(mReadonlyProperties, section, NEPersistence::SYNTAX_ALL_MODULES, property, position, keyType, false));
}

const Property * ConfigManager::getModuleProperty( const String& section
                                                , const String& property
                                                , const String& position
                                                , NEPersistence::eConfigKeys keyType /*= NEPersistence::eConfigKeys::EntryAnyKey*/) const
{
    Lock lock(mLock);

    keyType = keyType == NEPersistence::eConfigKeys::EntryInvalid ? NEPersistence::eConfigKeys::EntryAnyKey : keyType;
    return _getProperty(mWritableProperties, section, mModule, property, position, keyType, true);
}

void ConfigManager::setModuleProperty( const String& section
                                    , const String& property
                                    , const String& position
                                    , const String& value
                                    , NEPersistence::eConfigKeys keyType /*= NEPersistence::eConfigKeys::EntryAnyKey*/
                                    , bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    keyType = keyType == NEPersistence::eConfigKeys::EntryInvalid ? NEPersistence::eConfigKeys::EntryAnyKey : keyType;
    _setPositionValue<String>(mWritableProperties, mReadonlyProperties, section, mModule, property, position, keyType, value, isTemporary);
}

void ConfigManager::removeModuleProperty(const String& section, const String& property, const String& position, NEPersistence::eConfigKeys keyType)
{
    Lock lock(mLock);
    uint32_t elemPos = _findPosition(mWritableProperties, 0, section, mModule, property, position, true, keyType);
    if (elemPos != NECommon::INVALID_POSITION)
    {
        mWritableProperties.removePosition(elemPos);
    }
}

int ConfigManager::removeModuleProperties(const String& section, const String& property, NEPersistence::eConfigKeys keyType)
{
    Lock lock(mLock);
    int result{ 0 };
    uint32_t i{ 0 };
    while (i < mWritableProperties.getSize())
    {
        const PropertyKey& key = mWritableProperties[i].getKey();
        if ((keyType == NEPersistence::eConfigKeys::EntryAnyKey) || (keyType == key.getKeyType()))
        {
            if ((section == key.getSection()) && (property == key.getProperty()))
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

void ConfigManager::removeSectionProperties(const String& section)
{
    Lock lock(mLock);
    for (uint32_t i = 0; i < mWritableProperties.getSize(); ++i)
    {
        if (mWritableProperties[i].getKey().getSection() == section)
        {
            mWritableProperties.removeAt(i);
        }
        else
        {
            ++i;
        }
    }
}

bool ConfigManager::readConfig(const String& filePath /*= String::EmptyString*/, IEConfigurationListener * listener /*= nullptr*/)
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
        File fileConfig(path, FileBase::FO_MODE_EXIST | FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_SHARE_READ);
        if (fileConfig.open() && readConfig(fileConfig, listener))
        {
            mFilePath = fileConfig.getName();
        }
    }

    return mIsConfigured;
}

bool ConfigManager::readConfig(const FileBase& file, IEConfigurationListener * listener /*= nullptr*/)
{
    Lock lock(mLock);
    if (mIsConfigured == false)
    {
        mWritableProperties.clear();
        mReadonlyProperties.clear();
        if (listener != nullptr)
        {
            listener->prepareReadConfiguration(*this);
        }

        mIsConfigured = _readConfig(file, mWritableProperties, mReadonlyProperties, mModule) != 0;

        if (listener != nullptr)
        {
            listener->postReadConfiguration(*this);
        }
    }

    return mIsConfigured;
}

bool ConfigManager::saveConfig(const String& filePath, IEConfigurationListener * listener /*= nullptr*/)
{
    Lock lock(mLock);
    bool result{ false };

    constexpr unsigned int modeRead {   File::FO_MODE_READ      |
                                        File::FO_MODE_TEXT      |
                                        File::FO_MODE_CREATE    |
                                        File::FO_MODE_SHARE_READ};

    constexpr unsigned int modeWrite{   FileBase::FO_MODE_READ       | 
                                        FileBase::FO_MODE_TEXT       | 
                                        FileBase::FO_MODE_CREATE     | 
                                        FileBase::FO_MODE_WRITE };

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

    File srcFile(srcPath, modeRead );
    File dstFile(tempFile, modeWrite);

    if (srcFile.open() && dstFile.open())
    {
        if (saveConfig(srcFile, dstFile, saveAll, listener))
        {
            srcFile.close();
            dstFile.close();
            result = File::copyFile(tempFile, dstPath, true);
            File::deleteFile(tempFile);
        }
    }

    return result;
}

bool ConfigManager::saveConfig(const FileBase& srcFile, FileBase& dstFile, bool saveAll, IEConfigurationListener * listener /*= nullptr*/)
{
    Lock lock(mLock);
    if (listener != nullptr)
    {
        listener->prepareSaveConfiguration(*this);
    }

    bool result = _saveConfig(mWritableProperties, mReadonlyProperties, mModule, srcFile, dstFile, saveAll);

    if (listener != nullptr)
    {
        listener->postSaveConfiguration(*this);
    }

    return result;
}

void ConfigManager::setConfiguration(const NEPersistence::ListProperties& listReadonly, const NEPersistence::ListProperties& listWritable, IEConfigurationListener* listener /*= nullptr*/)
{
    Lock lock(mLock);

    mIsConfigured = true;
    mWritableProperties = listWritable;
    mReadonlyProperties = listReadonly;

    if (listener != nullptr)
    {
        listener->onSetupConfiguration(listReadonly, listWritable, *this);
    }
}


Version ConfigManager::getConfigVersion(void) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryConfigVersion;
    const NEPersistence::sPropertyKey& key = NEPersistence::getConfigVersion();

    const Property* prop = _getProperty(mReadonlyProperties, key.section, NEPersistence::SYNTAX_ALL_MODULES, key.property, key.position, confKey, true);

    Version result;
    if (prop != nullptr)
    {
        result = prop->getValueString();
    }
    else
    {
        result = NEPersistence::CONFIG_VERSION;
    }

    return result;
}

std::vector<Identifier> ConfigManager::getServiceList(void) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryServiceList;
    const NEPersistence::sPropertyKey& key = NEPersistence::getServiceList();

    const Property* prop = getProperty(key.section, key.property, key.position, confKey);

    std::vector<Identifier> result;
    if (prop != nullptr)
    {
        result = prop->getValue().getIdentifierList(NEApplication::RemoteServiceIdentifiers).getData();
    }

    return result;
}

std::vector<Identifier> ConfigManager::getLogTargets(void) const
{
    Lock lock(mLock);
    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogTarget;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogTarget();

    const Property* prop = getProperty(key.section, key.property, key.position, confKey);

    std::vector<Identifier> result;
    if (prop != nullptr)
    {
        result = prop->getValue().getIdentifierList(NEApplication::LogTypeIdentifiers).getData();
    }

    return result;
}

bool ConfigManager::getLoggingStatus(void) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogStatus;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogStatus();

    const PropertyValue* value = getPropertyValue(key.section, key.property, key.position, confKey);
    return (value != nullptr ? value->getBoolean() : NEApplication::DEFAULT_LOG_ENABLED);
}

Version ConfigManager::getLogVersion(void) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogVersion;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogVersion();

    const PropertyValue* value = getPropertyValue(key.section, key.property, key.position, confKey);
    Version result;
    if (value != nullptr)
    {
        result = value->getString();
    }
    else
    {
        result = NEPersistence::CONFIG_VERSION;
    }
    return result;
}

bool ConfigManager::getLogEnabled(const String& logType) const
{
    Lock lock(mLock);

    bool result{ false };
    if (getLoggingStatus())
    {
        constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogEnable;
        const NEPersistence::sPropertyKey& key = NEPersistence::getLogEnable();

        const PropertyValue* value = getPropertyValue(key.section, key.property, logType, confKey);
        result = (value != nullptr ? value->getBoolean() : NEApplication::DEFAULT_LOG_ENABLED);
    }

    return result;
}

bool ConfigManager::getLogEnabled(NELogging::eLogingTypes logType) const
{
    String id = Identifier::convToString( static_cast<unsigned int>(logType)
                                        , NEApplication::LogTypeIdentifiers
                                        , static_cast<unsigned int>(NELogging::eLogingTypes::LogTypeUndefined));
    return getLogEnabled(id);
}

void ConfigManager::setLogEnabled(NELogging::eLogingTypes logType, bool newValue, bool isTemporary /*= false*/)
{
    String id = Identifier::convToString( static_cast<unsigned int>(logType)
                                        , NEApplication::LogTypeIdentifiers
                                        , static_cast<unsigned int>(NELogging::eLogingTypes::LogTypeUndefined));
    setLogEnabled(id, newValue, isTemporary);
}

String ConfigManager::getLogFileLocation(void) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogFileLocation;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogFileLocation();
    const PropertyValue* value = getPropertyValue(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->getString();
    }
    else
    {
        result = NEApplication::DEFAULT_LOG_FILE;
    }

    return result;
}

bool ConfigManager::getLogFileAppend(void) const
{
    Lock lock(mLock);
    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogFileAppend;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogFileAppend();
    const PropertyValue* value = getPropertyValue(key.section, key.property, key.position, confKey);
    return (value != nullptr ? value->getBoolean() : false);
}

void ConfigManager::setLogFileAppend(bool newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);
    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogFileAppend;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogFileAppend();
    setModuleProperty(key.section, key.property, key.position, String::makeString(newValue), confKey, isTemporary);
}

uint32_t ConfigManager::getLogRemoteQueueSize(void) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogRemoteQueueSize;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogRemoteQueueSize();
    const PropertyValue* value = getPropertyValue(key.section, key.property, key.position, confKey);
    return (value != nullptr ? value->getInteger() : NEApplication::DEFAULT_LOG_QUEUE_SIZE);
}

void ConfigManager::setLogRemoteQueueSize(uint32_t newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogRemoteQueueSize;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogRemoteQueueSize();
    setModuleProperty(key.section, key.property, key.position, String::makeString(newValue), confKey, isTemporary);
}

String ConfigManager::getLogLayoutEnter(void) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogLayoutEnter;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogLayoutEnter();
    const PropertyValue* value = getPropertyValue(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->getString();
    }
    else
    {
        result = NEApplication::DEFAULT_LAYOUT_SCOPE_ENTER;
    }

    return result;
}

void ConfigManager::setLogLayoutEnter(const String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogLayoutEnter;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogLayoutEnter();
    setModuleProperty(key.section, key.property, key.position, newValue, confKey, isTemporary);
}

String ConfigManager::getLogLayoutMessage(void) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogLayoutMessage;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogLayoutMessage();
    const PropertyValue* value = getPropertyValue(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->getString();
    }
    else
    {
        result = NEApplication::DEFAULT_LAYOUT_LOG_MESSAGE;
    }
    
    return result;
}

void ConfigManager::setLogLayoutMessage(const String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogLayoutMessage;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogLayoutMessage();
    setModuleProperty(key.section, key.property, key.position, newValue, confKey, isTemporary);
}

String ConfigManager::getLogLayoutExit(void) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogLayoutExit;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogLayoutExit();
    const PropertyValue* value = getPropertyValue(key.section, key.property, key.position, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->getString();
    }
    else
    {
        result = NEApplication::DEFAULT_LAYOUT_SCOPE_EXIT;
    }
    
    return result;
}

void ConfigManager::setLogLayoutExit(const String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryLogLayoutExit;
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogLayoutExit();
    setModuleProperty(key.section, key.property, key.position, newValue, confKey, isTemporary);
}

uint32_t ConfigManager::getModuleLogScopes(std::vector<Property>& OUT scopeList) const
{
    Lock lock(mLock);

    const auto& listRead{ mReadonlyProperties.getData() };
    for (const auto& entry : listRead)
    {
        const PropertyKey& key = entry.getKey();
        if ((key.getKeyType() == NEPersistence::eConfigKeys::EntryLogScope) && key.isAllModules())
        {
            scopeList.push_back(entry);
        }
    }

    const auto& listWrite{ mWritableProperties.getData() };
    for (const auto& entry : listWrite)
    {
        const PropertyKey& key = entry.getKey();
        if (key.getKeyType() == NEPersistence::eConfigKeys::EntryLogScope)
        {
            ASSERT(key.getModule() == mModule);
            scopeList.push_back(entry);
        }
    }

    return static_cast<uint32_t>(scopeList.size());
}

void ConfigManager::addModuletLogScopes(const std::vector<Property>& scopeList, bool search)
{
    Lock lock(mLock);

    for (const auto& scope : scopeList)
    {
        if (search)
        {
            setModuleProperty(scope.getKey(), scope.getValueString(), false);
        }
        else
        {
            mWritableProperties.add(scope);
        }
    }
}

void ConfigManager::addModuleLogScope(const String& scopeName, const String& prio)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey{ NEPersistence::eConfigKeys::EntryLogScope };
    const NEPersistence::sPropertyKey& key{ NEPersistence::getLogScope() };
    setModuleProperty(key.section, key.property, scopeName, prio, confKey, false);
}

void ConfigManager::addModuleLogScope(const String& scopeName, unsigned int prio)
{
    PropertyValue value;
    value.setIdentifierList(prio, NEApplication::LogScopePriorityIndentifiers);
    addModuleLogScope(scopeName, value.getString());
}

bool ConfigManager::removeScope(const String& scopeName)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey{ NEPersistence::eConfigKeys::EntryLogScope };
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogScope();
    uint32_t pos = _findPosition(mWritableProperties, 0, key.section, mModule, key.property, scopeName, true, confKey);
    if (pos != NECommon::INVALID_POSITION)
    {
        mWritableProperties.removeAt(pos);
    }

    return (pos != NECommon::INVALID_POSITION);
}

int ConfigManager::removeModuleScopes(void)
{
    constexpr NEPersistence::eConfigKeys confKey{ NEPersistence::eConfigKeys::EntryLogScope };
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogScope();
    return removeModuleProperties(key.section, key.property, confKey);
}

std::vector<Identifier> ConfigManager::getRemoteServiceConnections(const String& service) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryServiceConnection;
    const NEPersistence::sPropertyKey& key = NEPersistence::getServiceConnection();

    std::vector<Identifier> result;
    const PropertyValue* value = getPropertyValue(service, key.property, key.position, confKey);
    if (value != nullptr)
    {
        result = value->getIdentifierList(NEApplication::ConnectionIdentifiers).getData();
    }

    return result;
}

String ConfigManager::getRemoteServiceName(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryServiceName;
    const NEPersistence::sPropertyKey& key = NEPersistence::getServiceName();
    const PropertyValue* value = getPropertyValue(service, key.property, connectType, confKey);
    return (value != nullptr ? value->getString() : String(String::EmptyString));
}

String ConfigManager::getRemoteServiceName(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType) const
{
    const String& service = Identifier::convToString( static_cast<unsigned int>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown));
    const String & connect = Identifier::convToString(static_cast<unsigned int>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined));
    return getRemoteServiceName(service, connect);
}

bool ConfigManager::getRemoteServiceEnable(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryServiceEnable;
    const NEPersistence::sPropertyKey& key = NEPersistence::getServiceEnable();
    const PropertyValue* value = getPropertyValue(service, key.property, connectType, confKey);
    return (value != nullptr ? value->getBoolean() : NEApplication::DEFAULT_SERVICE_ENABLED);
}

bool ConfigManager::getRemoteServiceEnable(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType) const
{
    const String& service = Identifier::convToString( static_cast<unsigned int>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown));
    const String & connect = Identifier::convToString(static_cast<unsigned int>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined));
    return getRemoteServiceEnable(service, connect);
}

void ConfigManager::setRemoteServiceEnable(const String& service, const String& connectType, bool newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryServiceEnable;
    const NEPersistence::sPropertyKey& key = NEPersistence::getServiceEnable();
    setModuleProperty(service, key.property, connectType, String::makeString(newValue), confKey, isTemporary);
}

void ConfigManager::setRemoteServiceEnable(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType, bool newValue, bool isTemporary /*= false*/)
{
    const String& service = Identifier::convToString( static_cast<unsigned int>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown));
    const String & connect = Identifier::convToString(static_cast<unsigned int>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined));
    setRemoteServiceEnable(service, connect, newValue, isTemporary);
}

String ConfigManager::getRemoteServiceAddress(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryServiceAddress;
    const NEPersistence::sPropertyKey& key = NEPersistence::getServiceAddress();
    const PropertyValue* value = getPropertyValue(service, key.property, connectType, confKey);

    String result;
    if (value != nullptr)
    {
        result = value->getString();
    }
    else
    {
        result = NEApplication::DEFAULT_SERVICE_HOST;
    }
    
    return result;
}

String ConfigManager::getRemoteServiceAddress(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType) const
{
    const String& service = Identifier::convToString( static_cast<unsigned int>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown));
    const String & connect = Identifier::convToString(static_cast<unsigned int>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined));
    return getRemoteServiceAddress(service, connect);
}

void ConfigManager::setRemoteServiceAddress(const String& service, const String& connectType, const String& newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryServiceAddress;
    const NEPersistence::sPropertyKey& key = NEPersistence::getServiceAddress();
    setModuleProperty(service, key.property, connectType, newValue, confKey, isTemporary);
}

void ConfigManager::setRemoteServiceAddress(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType, const String& newValue, bool isTemporary /*= false*/)
{
    const String& service = Identifier::convToString( static_cast<unsigned int>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown));
    const String & connect = Identifier::convToString(static_cast<unsigned int>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined));
    setRemoteServiceAddress(service, connect, newValue, isTemporary);
}

uint16_t ConfigManager::getRemoteServicePort(const String& service, const String& connectType) const
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryServicePort;
    const NEPersistence::sPropertyKey& key = NEPersistence::getServicePort();
    const PropertyValue* value = getPropertyValue(service, key.property, connectType, confKey);
    return static_cast<uint16_t>(value != nullptr ? value->getInteger() : NEApplication::DEFAULT_ROUTER_PORT);
}

uint16_t ConfigManager::getRemoteServicePort(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType) const
{
    const String& service = Identifier::convToString( static_cast<unsigned int>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown));
    const String & connect = Identifier::convToString(static_cast<unsigned int>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined));
    return getRemoteServicePort(service, connect);
}

void ConfigManager::setRemoteServicePort(const String& service, const String& connectType, uint16_t newValue, bool isTemporary /*= false*/)
{
    Lock lock(mLock);

    constexpr NEPersistence::eConfigKeys confKey = NEPersistence::eConfigKeys::EntryServicePort;
    const NEPersistence::sPropertyKey& key = NEPersistence::getServicePort();
    setModuleProperty(service, key.property, connectType, String::makeString(static_cast<uint32_t>(newValue)), confKey, isTemporary);
}

void ConfigManager::setRemoteServicePort(NERemoteService::eRemoteServices serviceType, NERemoteService::eConnectionTypes connectType, uint16_t newValue, bool isTemporary /*= false*/)
{
    const String& service = Identifier::convToString( static_cast<unsigned int>(serviceType)
                                                    , NEApplication::RemoteServiceIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown));
    const String & connect = Identifier::convToString(static_cast<unsigned int>(connectType)
                                                    , NEApplication::ConnectionIdentifiers
                                                    , static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined));
    setRemoteServicePort(service, connect, newValue, isTemporary);
}

String ConfigManager::getLogDatabaseProperty(const String& whichPosition)
{
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogDatabaseName();
    const PropertyValue* value = getPropertyValue(key.section, key.property, whichPosition);
    return (value != nullptr ? value->getValue() : String::getEmptyString());
}

void ConfigManager::setLogDatabaseProperty(const String& whichPosition, const String& newValue, bool isTemporary /*= false*/)
{
    const NEPersistence::sPropertyKey& key = NEPersistence::getLogDatabaseName();
    setModuleProperty(key.section, key.property, whichPosition, newValue, NEPersistence::EntryAnyKey, isTemporary);
}
