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

File::File(const String& fileName, unsigned int mode /* = (FileBase::FO_MODE_WRITE | FileBase::FO_MODE_BINARY) */)
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

bool File::open(const String& fileName, unsigned int mode)
{
    bool result = false;
    if ((isOpened() == false) && (fileName.isEmpty() == false))
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
    bool result{ false };
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

const String & File::getExecutableDir(void)
{
    return Process::getInstance().getPath();
}

String File::getFileNameWithExtension( const char* filePath )
{
    const char * result = nullptr;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        NEString::CharPos pos = NEString::getStringLength<char>(filePath) - 1;
        if (filePath[pos] != File::PATH_SEPARATOR)
        {
            pos = NEString::findLast<char>(File::PATH_SEPARATOR, filePath, pos - 1, nullptr);
            if (NEString::isPositionValid(pos))
            {
                result = filePath + pos + 1;
            }
        }
    }

    return (result != nullptr ? String(result) : String::EmptyString);
}

String File::getFileName( const char* filePath )
{
    String result(File::getFileNameWithExtension(filePath));
    NEString::CharPos pos = result.findLast(File::EXTENSION_SEPARATOR, NEString::END_POS, true);
    if (NEString::isPositionValid(pos) && NEString::isAlphanumeric<char>(result[pos + 1]) )
    {
        result.substring(0, pos);
    }

    return result;
}

String File::getFileExtension( const char* filePath )
{
    String result;
    String fileName(File::getFileNameWithExtension(filePath));
    NEString::CharPos pos = fileName.findLast(File::EXTENSION_SEPARATOR, NEString::END_POS, true);
    if (NEString::isPositionValid(pos) && NEString::isAlphanumeric<char>(fileName[pos + 1]) )
    {
        fileName.substring(result, pos, NEString::COUNT_ALL);
    }

    return result;
}

String File::getFileDirectory(const char* filePath)
{
    NEString::CharPos pos = NEString::isEmpty<char>(filePath) ? NEString::INVALID_POS : NEString::findLast<char>(File::PATH_SEPARATOR, filePath, NEString::END_POS, nullptr);
    return (NEString::isPositionValid(pos) ? String(filePath, pos) : String(String::EmptyString));
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
#if !defined(WINDOWS) && !defined(_WINDOWS)
    static const String _PSEUDO_ROOT( "<root>" );
#endif // !defined(WINDOWS) && !defined(_WINDOWS)

    String result;
    StringList list;
    if (fileName != nullptr)
    {
        if ( File::_nameHasCurrentFolder(fileName, false) || _nameHasParentFolder(fileName, false) )
        {
            String curDir( File::getCurrentDir() );
            File::splitPath(curDir, list);
#if !defined(WINDOWS) && !defined(_WINDOWS)
            list.pushFirst(_PSEUDO_ROOT);
#endif // !defined(WINDOWS) && !defined(_WINDOWS)
        }
        else if (*fileName == '%')
        {
            for ( int i = 0; i < File::LEN_SPECIAL_MASKS; ++ i )
            {
                if (NEString::stringStartsWith<char>(fileName, SPEACIAL_MASKS[i].data(), false))
                {
                    String special(File::getSpecialDir(static_cast<File::eSpecialFolder>(i)));
                    File::splitPath(special, list);
#if !defined(WINDOWS) && !defined(_WINDOWS)
                    list.pushFirst(_PSEUDO_ROOT);
#endif // !defined(WINDOWS) && !defined(_WINDOWS)
                    break;
                }
            }
        }
#if !defined(WINDOWS) && !defined(_WINDOWS)
        else if ((*fileName == File::UNIX_SEPARATOR))
        {
            list.pushFirst(_PSEUDO_ROOT);
        }
#endif // !defined(WINDOWS) && !defined(_WINDOWS)
    }

    File::splitPath(fileName, list);

    bool isInvalid  = false;
    for (StringList::LISTPOS pos = list.firstPosition(); list.isValidPosition(pos); )
    {
        const String & node = list.valueAtPosition(pos);
        if (File::_nameHasParentFolder(node, true))
        {
            pos = list.removeAt(pos);
            StringList::LISTPOS prev = list.prevPosition(pos);
            if (list.isValidPosition(prev))
            {
                list.removeAt(prev);
            }
            else
            {
                isInvalid = true;
                break;
            }
        }
        else if (File::_nameHasCurrentFolder(node, true))
        {
            pos = list.removeAt(pos);
        }
        else
        {
            pos = list.nextPosition(pos);
        }
    }

    if (isInvalid || list.isEmpty())
        return result;

    String first(list.getFirstEntry());
    list.removeFirst();
#if !defined(WINDOWS) && !defined(_WINDOWS)
    if (first == _PSEUDO_ROOT)
    {
        result = File::PATH_SEPARATOR;
        if (list.isEmpty() == false)
        {
            list.removeFirst(first);
        }
    }
#endif // !defined(WINDOWS) && !defined(_WINDOWS)

    result += first;
    StringList::LISTPOS end = list.invalidPosition();
    for (StringList::LISTPOS pos = list.firstPosition(); pos != end; pos = list.nextPosition(pos))
    {
        result.append(File::PATH_SEPARATOR).append(list.valueAtPosition(pos));
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
            int pos = NEString::findLast(File::PATH_SEPARATOR, filePath, NEString::END_POS, nextPos);
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
        result.assign(filePath, MACRO_ELEM_COUNT(filePath, end));
    }

    return result;
}

int File::splitPath(const char * filePath, StringList & in_out_List)
{
    int oldCount        { static_cast<int>(in_out_List.getSize()) };
    const char * start  { filePath };
    const char * end    { filePath };

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
