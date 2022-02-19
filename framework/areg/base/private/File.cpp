/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/File.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, File object
 *              common parts
 ************************************************************************/

#include "areg/base/File.hpp"

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NEString.hpp"
#include "areg/base/Containers.hpp"

//////////////////////////////////////////////////////////////////////////
// File class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
File::File( void )
    : FileBase    ( )
    , mFileHandle   (File::INVALID_HANDLE)
{
}

File::File(const char* fileName, unsigned int mode /* = (FileBase::FO_MODE_WRITE | FileBase::FO_MODE_BINARY) */)
    : FileBase    ( )
    , mFileHandle   (File::INVALID_HANDLE)
{
    mFileName = File::normalizePath(fileName);
    mFileMode = mode;
}

File::~File( void )
{
    _closeFile();

    mFileHandle = File::INVALID_HANDLE;
    mFileMode   = FileBase::FO_MODE_INVALID;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool File::open(const char* fileName, unsigned int mode)
{
    bool result = false;
    if ((isOpened() == false) && (NEString::isEmpty<char>(fileName) == false) )
    {
        mFileMode = mode;
        mFileName = File::normalizePath(fileName);

        result = open();
    }
    else
    {
        OUTPUT_WARN("File is already opened. Close file.");
    }
    return result;
}

unsigned int File::getSizeReadable( void ) const
{
    unsigned int lenRead = 0;
    unsigned int lenUsed = 0;
    if (isOpened())
    {
        lenRead = getPosition();
        lenUsed = getLength();
    }
    ASSERT(lenRead <= lenUsed);
    return (lenUsed - lenRead);
}

unsigned int File::getSizeWritable( void ) const
{
    unsigned int lenWritten     = 0;
    unsigned int lenAvailable   = 0;
    if (isOpened())
    {
        lenWritten  = getPosition();
        lenAvailable= getLength();
    }
    ASSERT(lenWritten <= lenAvailable);
    return (lenAvailable - lenWritten);
}

bool File::remove( void )
{
    bool result = false;

    if (isOpened())
    {
        ASSERT(mFileName.isEmpty() == false);

        close();
        result = File::deleteFile(mFileName);
    }

    mFileHandle = File::INVALID_HANDLE;
    mFileMode   = FileBase::FO_MODE_INVALID;
    mFileName   = String::EmptyString.data();

    return result;
}

bool File::isOpened() const
{
    return (mFileHandle != File::INVALID_HANDLE);
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

inline bool File::_nameHasCurrentFolder(const char * filePath, bool skipSep)
{
    bool result = false;
    if (NEString::isEmpty<char>(filePath) == false)
    {
        if (filePath[0] == File::DIR_CURRENT[0])
        {
            result = (skipSep && filePath[1] == NEString::EndOfString) || (filePath[1] == File::UNIX_SEPARATOR) ||  (filePath[1] == File::DOS_SEPARATOR);
        }
    }

    return result;
}

inline bool File::_nameHasParentFolder(const char * filePath, bool skipSep)
{
    bool result = false;
    if (NEString::isEmpty<char>(filePath) == false)
    {
        if ((filePath[0] == File::DIR_PARENT[0]) && (filePath[1] == File::DIR_PARENT[1]))
        {
            result = (skipSep && filePath[2] == NEString::EndOfString) || (filePath[2] == File::UNIX_SEPARATOR) ||  (filePath[2] == File::DOS_SEPARATOR);
        }
    }

    return result;
}

String File::genTempFileName()
{
    return genTempFileName(nullptr, true, true);
}

String File::getFileNameWithExtension( const char* filePath )
{
    const char * result = "";
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        int len = NEString::getStringLength<char>(filePath);
        if (filePath[len - 1] != File::PATH_SEPARATOR)
        {
            NEString::CharPos pos = NEString::findLastOf<char>(File::PATH_SEPARATOR, filePath, NEString::END_POS, nullptr);
            if (pos != NEString::INVALID_POS)
                result = filePath + pos + 1;
        }
    }

    return String(result);
}

String File::getFileName( const char* filePath )
{
    String result;
    String fileName = File::getFileNameWithExtension(filePath);
    NEString::CharPos pos = fileName.findLastOf(File::EXTENSION_SEPARATOR, NEString::END_POS, true);
    if (pos != NEString::INVALID_POS && NEString::isAlphanumeric<char>(fileName.getAt(pos + 1)) )
    {
        result = fileName.substring( 0, pos );
    }

    return result;
}

String File::getFileExtension( const char* filePath )
{
    String result;
    String fileName = File::getFileNameWithExtension(filePath);
    NEString::CharPos pos = fileName.findLastOf(File::EXTENSION_SEPARATOR, NEString::END_POS, true);
    if (pos != NEString::INVALID_POS && NEString::isAlphanumeric<char>(fileName.getAt(pos + 1)) )
    {
        result = fileName.substring( pos, NEString::COUNT_ALL );
    }

    return result;
}

String File::getFileDirectory(const char* filePath)
{
    NEString::CharPos pos = NEString::isEmpty<char>(filePath) ? NEString::INVALID_POS : NEString::findLastOf<char>(File::PATH_SEPARATOR, filePath, NEString::END_POS, nullptr);
    return (pos > 0 ? String(filePath, pos) : String(String::EmptyString.data(), 0));
}

bool File::createDirCascaded( const char* dirPath )
{
    bool result = false;
    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        char * buffer = DEBUG_NEW char[ static_cast<unsigned int>(File::MAXIMUM_PATH + 1) ];
        if ( buffer != nullptr )
        {
            const char * src    = dirPath;
            char *       dst    = buffer;
            bool doCreate       = false;

            do 
            {
                if (*src == File::PATH_SEPARATOR)
                    *dst ++ = *src++;

                while (*src != File::PATH_SEPARATOR && *src != '\0')
                    *dst ++ = *src ++;

                *dst    = '\0';

                doCreate = File::existDir(buffer) || File::_createFolder(buffer);

            } while (doCreate && (*src != '\0'));

            result = doCreate;
            delete [] buffer;
        }
    }

    return result;
}

String File::normalizePath( const char * fileName )
{
#ifndef WINDOWS
    static const String _PSEUDO_ROOT( "<root>" );
#endif // !WINDOWS

    
    String result;
    StringList list;
    if (fileName != nullptr)
    {
        if ( File::_nameHasCurrentFolder(fileName, false) || _nameHasParentFolder(fileName, false) )
        {
            String curDir = File::getCurrentDir();
            File::splitPath(curDir.getString(), list);
#ifndef WINDOWS
            list.pushFirst(_PSEUDO_ROOT);
#endif // WINDOWS
        }
        else if (*fileName == '%')
        {
            for ( int i = 0; i < File::LEN_SPECIAL_MASKS; ++ i )
            {
                if (NEString::stringStartsWith<char>(fileName, SPEACIAL_MASKS[i].data(), false))
                {
                    String special = File::getSpecialDir(static_cast<File::eSpecialFolder>(i));
                    File::splitPath(special.getString(), list);
#ifndef WINDOWS
                    list.pushFirst(_PSEUDO_ROOT);
#endif // WINDOWS
                    break;
                }
            }
        }
#ifndef WINDOWS
        else if ((*fileName == File::UNIX_SEPARATOR))
        {
            list.pushFirst(_PSEUDO_ROOT);
        }
#endif  // !WINDOWS
    }

    File::splitPath(fileName, list);

    bool isInvalid  = false;
    for (LISTPOS pos = list.firstPosition(); pos != nullptr; )
    {
        const String & node = list.getAt(pos);
        if (File::_nameHasParentFolder(node, true))
        {
            LISTPOS next = list.nextPosition(pos);
            LISTPOS prev = list.prevPosition(pos);
            list.removeAt(pos);
            if (prev != nullptr)
            {
                list.removeAt(prev);
            }
            else
            {
                isInvalid = true;
                break;
            }
            
            pos = next;
        }
        else if (File::_nameHasCurrentFolder(node, true))
        {
            LISTPOS next = list.nextPosition(pos);
            list.removeAt(pos);
            pos = next;
        }
        else
        {
            pos = list.nextPosition(pos);
        }
    }

    if (isInvalid || list.isEmpty())
        return result;

    String first = list.removeFirst();
#ifndef WINDOWS
    if (first == _PSEUDO_ROOT)
    {
        result = File::PATH_SEPARATOR;
        if (list.isEmpty() == false)
            first = list.removeFirst();
    }
#endif // !WINDOWS

    result += first;
    for (LISTPOS pos = list.firstPosition(); pos != nullptr; pos = list.nextPosition(pos))
    {
        result += File::PATH_SEPARATOR;
        result += list.getAt(pos);
    }

    FileBase::normalizeName(result);

    return result;
}

bool File::findParent(const char * filePath, const char ** nextPos, const char * lastPos /*= nullptr*/)
{
    bool result = false;
    if ( NEString::isEmpty<char>(filePath) == false)
    {
        int length = 0;
        if (nextPos != nullptr)
            *nextPos = nullptr;
        
        if (lastPos != nullptr)
        {
            length = lastPos == filePath ? 0 : static_cast<int>(lastPos - filePath - 1);
        }
        else
        {
            length = NEString::getStringLength<char>(filePath); 
            if ( filePath[length - 1] == File::PATH_SEPARATOR )
                -- length;
        }

        if (length != 0)
        {
            int pos = NEString::findLastOf(File::PATH_SEPARATOR, filePath, NEString::END_POS, nextPos);
            if ((pos > 0) && (pos < length))
            {
                result = true;
                if (nextPos != nullptr)
                    *nextPos = filePath + pos;
            }
        }
    }

    return result;
}

String File::getParentDir(const char * filePath)
{
    String result;
    const char * end = nullptr;
    if (File::findParent(filePath, &end))
    {
        result.copy(filePath, static_cast<NEString::CharCount>(end - filePath));
    }

    return result;
}

int File::splitPath(const char * filePath, StringList & in_out_List)
{
    int oldCount        = in_out_List.getSize();
    const char * start  = filePath;
    const char * end    = filePath;

    while (*end != NEString::EndOfString)
    {
        if ((*end == File::UNIX_SEPARATOR) || (*end == File::DOS_SEPARATOR))
        {
            String node(start, MACRO_ELEM_COUNT(start, end));
            if (node.isEmpty() == false)
                in_out_List.pushLast( node );

            start = ++ end;
        }
        else
        {
            ++ end;
        }
    }

    if (start != end)
    {
        String node(start, MACRO_ELEM_COUNT(start, end));
        if (node.isEmpty() == false)
            in_out_List.pushLast( node );
    }

    return (in_out_List.getSize() - oldCount);
}

unsigned int File::read(IEByteBuffer & buffer) const
{
    return FileBase::read(buffer);
}

unsigned int File::read(String & asciiString) const
{
    return FileBase::read(asciiString);
}

unsigned int File::read(WideString & wideString) const
{
    return FileBase::read(wideString);
}

unsigned int File::write(const IEByteBuffer & buffer)
{
    return FileBase::write(buffer);
}

unsigned int File::write(const String & asciiString)
{
    return FileBase::write(asciiString);
}

unsigned int File::write(const WideString & wideString)
{
    return FileBase::write(wideString);
}
