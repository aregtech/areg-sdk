/************************************************************************
 * \file        areg/base/File.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
#include "areg/base/EContainers.hpp"

//////////////////////////////////////////////////////////////////////////
// File class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
const char * const  File::SPEACIAL_MASKS[]    = {"%home%", "%personal", "%appdata%", "%temp%"};
const int           File::LEN_SPECIAL_MASKS   = MACRO_ARRAYLEN(SPEACIAL_MASKS);
const char * const  File::TEMP_FILE_PREFIX    = "_areg";
const char * const  File::FILE_MASK_TIMESTAMP = "%time%";
const char * const  File::TIMESTAMP_FORMAT    = "%04d_%02d_%02d_%02d_%02d_%02d_%03d"; // yyyy_mm_dd_hh_mm_ss_ms
const char * const  File::FILE_MASK_APPNAME   = "%appname%";
const char          File::EXTENSION_SEPARATOR = '.';
const char          File::UNIX_SEPARATOR      = '/';
const char          File::DOS_SEPARATOR       = '\\';

const int           File::FILE_MASK_TIMESTAMP_LEN = NEString::getStringLength<char>(File::FILE_MASK_TIMESTAMP);
const int           File::FILE_MASK_APPNAME_LEN   = NEString::getStringLength<char>(File::FILE_MASK_APPNAME);
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
File::File( void )
    : FileBase    ( )
    , mFileHandle   (File::INVALID_HANDLE)
{
    ; // do nothing
}

File::File(const char* fileName, int mode /* = (FileBase::FO_MODE_WRITE | FileBase::FO_MODE_BINARY) */)
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

unsigned int File::read( String & asciiString ) const
{
    return readString(asciiString);
}

unsigned int File::read( WideString & wideString ) const
{
    return readString(wideString);
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

unsigned int File::write( const String & asciiString )
{
    const char * buffer = asciiString.getString();
    unsigned int legth  = isTextMode() != 0 ? asciiString.getLength() : asciiString.getUsedSize();

    return write(reinterpret_cast<const unsigned char *>(buffer), legth * sizeof(char));
}

unsigned int File::write( const WideString & wideString )
{
    const wchar_t * buffer  = wideString.getString();
    unsigned int legth      = isTextMode() != 0 ? wideString.getLength() : wideString.getUsedSize();

    return write(reinterpret_cast<const unsigned char *>(buffer), legth * sizeof(wchar_t));
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
    mFileName   = String::EmptyString;

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
    return ( (NEString::isEmpty<char>(filePath) == false) &&
             (filePath[0] == '.') &&
             (skipSep || (filePath[1] == '/') || (filePath[1] == '\\')));
}

inline bool File::_nameHasParentFolder(const char * filePath, bool skipSep)
{
    return ( (NEString::isEmpty<char>(filePath) == false) &&
             (filePath[0] == '.') &&
             (filePath[1] == '.') &&
             (skipSep || (filePath[2] == '/') || (filePath[2] == '\\')));
}

String File::genTempFileName()
{
    return genTempFileName(NULL, true, true);
}

const char * File::getExecutableDir(void)
{
    return Process::getInstance().getPath();
}

String File::getFileNameWithExtension( const char* filePath )
{
    const char * result = "";
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        int len = NEString::getStringLength<char>(filePath);
        if (filePath[len - 1] != File::PATH_SEPARATOR)
        {
            NEString::CharPos pos = NEString::findLastOf<char>(File::PATH_SEPARATOR, filePath, NEString::EndPos, NULL);
            if (pos != NEString::InvalidPos)
                result = filePath + pos + 1;
        }
    }

    return String(result);
}

String File::getFileName( const char* filePath )
{
    String result;
    String fileName = File::getFileNameWithExtension(filePath);
    NEString::CharPos pos = fileName.findLastOf(File::EXTENSION_SEPARATOR, NEString::EndPos, true);
    if (pos != NEString::InvalidPos && NEString::isAlphanumeric<char>(fileName.getAt(pos + 1)) )
    {
        result = fileName.substring( 0, pos );
    }

    return result;
}

String File::getFileExtension( const char* filePath )
{
    String result;
    String fileName = File::getFileNameWithExtension(filePath);
    NEString::CharPos pos = fileName.findLastOf(File::EXTENSION_SEPARATOR, NEString::EndPos, true);
    if (pos != NEString::InvalidPos && NEString::isAlphanumeric<char>(fileName.getAt(pos + 1)) )
    {
        result = fileName.substring( pos, NEString::CountAll );
    }

    return result;
}

String File::getFileDirectory(const char* filePath)
{
    NEString::CharPos pos = NEString::isEmpty<char>(filePath) ? NEString::InvalidPos : NEString::findLastOf<char>(File::PATH_SEPARATOR, filePath, NEString::EndPos, NULL);
    return (pos > 0 ? String(filePath, pos) : String(String::EmptyString));
}

bool File::createDirCascaded( const char* dirPath )
{
    bool result = false;
    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];
        if ( buffer != NULL )
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
    if (fileName != NULL)
    {
        if ( File::_nameHasCurrentFolder(fileName, false) )
        {
            String cd = File::getCurrentDir();
            File::splitPath(cd.getString(), list);
#ifndef WINDOWS
            list.pushFirst(_PSEUDO_ROOT);
#endif // WINDOWS
        }
        else if (*fileName == '%')
        {
            for ( int i = 0; i < File::LEN_SPECIAL_MASKS; ++ i )
            {
                if (NEString::stringStartsWith<char>(fileName, SPEACIAL_MASKS[i], false))
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
    for (LISTPOS pos = list.firstPosition(); pos != NULL; )
    {
        const String & node = list.getAt(pos);
        if (File::_nameHasCurrentFolder(node, true))
        {
            LISTPOS next = list.nextPosition(pos);
            list.removeAt(pos);
            pos = next;
        }
        else if (File::_nameHasParentFolder(node, true))
        {
            LISTPOS next = list.nextPosition(pos);
            LISTPOS prev = list.prevPosition(pos);
            list.removeAt(pos);
            if (prev != NULL)
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
        else
        {
            pos = list.nextPosition(pos);
        }
    }

    if (isInvalid || list.isEmpty())
        return result;

    char fmt[32];
    NEUtilities::sSystemTime st;
    DateTime::getNow(st);
    String::formatString(fmt, 32, File::TIMESTAMP_FORMAT, st.stYear, st.stMonth, st.stDay, st.stHour, st.stMinute, st.stSecond, st.stMillisecs);

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
    for (LISTPOS pos = list.firstPosition(); pos != NULL; pos = list.nextPosition(pos))
    {
        result += File::PATH_SEPARATOR;
        result += list.getAt(pos);
    }

    do 
    {
        NEString::CharPos index = result.findFirstOf(File::FILE_MASK_TIMESTAMP, 0, false);
        if (index == NEString::InvalidPos)
            break;

        result.replace(fmt, index, File::FILE_MASK_TIMESTAMP_LEN );
    } while (true);

    String appName = Process::getInstance().getAppName();
    do 
    {
        NEString::CharPos index = result.findFirstOf(File::FILE_MASK_APPNAME, 0, false);
        if (index == NEString::InvalidPos)
            break;

        result.replace(appName, index, File::FILE_MASK_APPNAME_LEN );
    } while (true);

    return result;
}

bool File::findParent(const char * filePath, const char ** nextPos, const char * lastPos /*= NULL*/)
{
    bool result = false;
    if ( NEString::isEmpty<char>(filePath) == false)
    {
        int length = 0;
        if (nextPos != NULL)
            *nextPos = NULL;
        
        if (lastPos != NULL)
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
            int pos = NEString::findLastOf(File::PATH_SEPARATOR, filePath, NEString::EndPos, nextPos);
            if ((pos > 0) && (pos < length))
            {
                result = true;
                if (nextPos != NULL)
                    *nextPos = filePath + pos;
            }
        }
    }

    return result;
}

String File::getParentDir(const char * filePath)
{
    String result;
    const char * end = NULL;
    if (File::findParent(filePath, &end))
    {
        result.copy(filePath, static_cast<NEString::CharCount>(end - filePath));
    }

    return result;
}

int File::splitPath(const char * filePath, StringList & in_out_List)
{
    int count           = 0;
    const char * start  = filePath;
    const char * end    = filePath;

    while (*end != NEString::EndOfString)
    {
        if ((*end == File::UNIX_SEPARATOR) || (*end == File::DOS_SEPARATOR))
        {
            String node(start, MACRO_ELEM_COUNT(start, end));
            if (node.isEmpty() == false)
            {
                in_out_List.pushFirst( node );
                ++ count;
            }
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
            in_out_List.pushFirst( node );
    }

    return count;
}
