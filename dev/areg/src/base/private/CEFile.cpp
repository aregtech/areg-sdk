/************************************************************************
 * \file        areg/src/base/CEFile.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, File object
 *              common parts
 ************************************************************************/

#include "areg/src/base/CEFile.hpp"

#include "areg/src/base/CESharedBuffer.hpp"
#include "areg/src/base/CEWideString.hpp"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/CEDateTime.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/NEString.hpp"
#include "areg/src/base/EContainers.hpp"

//////////////////////////////////////////////////////////////////////////
// CEFile class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
const char * const  CEFile::SPEACIAL_MASKS[]    = {"%home%", "%personal", "%appdata%", "%temp%"};
const int           CEFile::LEN_SPECIAL_MASKS   = MACRO_ARRAYLEN(SPEACIAL_MASKS);
const char * const  CEFile::TEMP_FILE_PREFIX    = "_areg";
const char * const  CEFile::FILE_MASK_TIMESTAMP = "%time%";
const char * const  CEFile::TIMESTAMP_FORMAT    = "%04d_%02d_%02d_%02d_%02d_%02d_%03d"; // yyyy_mm_dd_hh_mm_ss_ms
const char * const  CEFile::FILE_MASK_APPNAME   = "%appname%";
const char          CEFile::EXTENSION_SEPARATOR = '.';
const char          CEFile::UNIX_SEPARATOR      = '/';
const char          CEFile::DOS_SEPARATOR       = '\\';

const int           CEFile::FILE_MASK_TIMESTAMP_LEN = NEString::getStringLength<char>(CEFile::FILE_MASK_TIMESTAMP);
const int           CEFile::FILE_MASK_APPNAME_LEN   = NEString::getStringLength<char>(CEFile::FILE_MASK_APPNAME);
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
CEFile::CEFile( void )
    : CEFileBase    ( )
    , mFileName     ( )
    , mFileMode     (CEFileBase::FO_MODE_INVALID)
    , mFileHandle   (CEFile::INVALID_HANDLE)
{
    ; // do nothing
}

CEFile::CEFile(const char* fileName, int mode /* = (CEFileBase::FO_MODE_WRITE | CEFileBase::FO_MODE_BINARY) */)
    : CEFileBase    ( )
    , mFileName     ( CEFile::NormalizeFileName(fileName) )
    , mFileMode     (mode)
    , mFileHandle   (CEFile::INVALID_HANDLE)
{
    ; // do nothing
}

CEFile::~CEFile( void )
{
    _closeFile();

    mFileHandle = CEFile::INVALID_HANDLE;
    mFileMode   = CEFileBase::FO_MODE_INVALID;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

const char* CEFile::GetName( void ) const
{
    return static_cast<const char *>(mFileName);
}

int CEFile::GetMode( void ) const
{
    return mFileMode;
}

bool CEFile::Open(const char* fileName, int mode)
{
    bool result = false;
    if ((IsOpened() == false) && (NEString::isEmpty<char>(fileName) == false) )
    {
        mFileMode = mode;
        mFileName = CEFile::NormalizeFileName(fileName);

        result = Open();
    }
    else
    {
        OUTPUT_WARN("File is already opened. Close file.");
    }
    return result;
}

unsigned int CEFile::Read( CEString & asciiString ) const
{
    return ReadString(asciiString);
}

unsigned int CEFile::Read( CEWideString & wideString ) const
{
    return ReadString(wideString);
}

unsigned int CEFile::GetReadableSize( void ) const
{
    unsigned int lenRead = 0;
    unsigned int lenUsed = 0;
    if (IsOpened())
    {
        lenRead = GetPosition();
        lenUsed = GetLength();
    }
    ASSERT(lenRead <= lenUsed);
    return (lenUsed - lenRead);
}

unsigned int CEFile::Write( const CEString & asciiString )
{
    const char * buffer = asciiString.GetBuffer();
    unsigned int legth  = IsTextMode() != 0 ? asciiString.GetLength() : asciiString.GetOccupiedBytes();

    return Write(reinterpret_cast<const unsigned char *>(buffer), legth * sizeof(char));
}

unsigned int CEFile::Write( const CEWideString & wideString )
{
    const wchar_t * buffer  = wideString.GetBuffer();
    unsigned int legth      = IsTextMode() != 0 ? wideString.GetLength() : wideString.GetOccupiedBytes();

    return Write(reinterpret_cast<const unsigned char *>(buffer), legth * sizeof(wchar_t));
}

unsigned int CEFile::GetWritableSize( void ) const
{
    unsigned int lenWritten     = 0;
    unsigned int lenAvailable   = 0;
    if (IsOpened())
    {
        lenWritten  = GetPosition();
        lenAvailable= GetLength();
    }
    ASSERT(lenWritten <= lenAvailable);
    return (lenAvailable - lenWritten);
}

bool CEFile::Remove( void )
{
    bool result = false;

    if (IsOpened())
    {
        ASSERT(mFileName.IsEmpty() == false);

        Close();
        result = CEFile::FileDelete(mFileName);
    }

    mFileHandle = CEFile::INVALID_HANDLE;
    mFileMode   = CEFileBase::FO_MODE_INVALID;
    mFileName   = CEString::EmptyString;

    return result;
}

bool CEFile::IsOpened() const
{
    return (mFileHandle != CEFile::INVALID_HANDLE);
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

inline bool CEFile::_nameHasCurrentFolder(const char * filePath, bool skipSep)
{
    return ( (NEString::isEmpty<char>(filePath) == false) &&
             (filePath[0] == '.') &&
             (skipSep || (filePath[1] == '/') || (filePath[1] == '\\')));
}

inline bool CEFile::_nameHasParentFolder(const char * filePath, bool skipSep)
{
    return ( (NEString::isEmpty<char>(filePath) == false) &&
             (filePath[0] == '.') &&
             (filePath[1] == '.') &&
             (skipSep || (filePath[2] == '/') || (filePath[2] == '\\')));
}

CEString CEFile::CreateTempFileName()
{
    return CreateTempFileName(NULL, true, true);
}

const char * CEFile::GetExecutableFolder(void)
{
    return CEProcess::GetProcess().GetProcessPath();
}

CEString CEFile::GetFileName( const char* filePath )
{
    const char * result = "";
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        int len = NEString::getStringLength<char>(filePath);
        if (filePath[len - 1] != CEFile::PATH_SEPARATOR)
        {
            NEString::CharPos pos = NEString::searchLastOf<char>(CEFile::PATH_SEPARATOR, filePath, NEString::EndPos, NULL);
            if (pos != NEString::InvalidPos)
                result = filePath + pos + 1;
        }
    }

    return CEString(result);
}

CEString CEFile::ExtractFileName( const char* filePath )
{
    CEString result;
    CEString fileName = CEFile::GetFileName(filePath);
    NEString::CharPos pos = fileName.FindLastOf(CEFile::EXTENSION_SEPARATOR, NEString::EndPos, true);
    if (pos != NEString::InvalidPos && NEString::isAlphanumeric<char>(fileName.GetCharAt(pos + 1)) )
    {
        result = fileName.Substr( 0, pos );
    }

    return result;
}

CEString CEFile::GetFileExtension( const char* filePath )
{
    CEString result;
    CEString fileName = CEFile::GetFileName(filePath);
    NEString::CharPos pos = fileName.FindLastOf(CEFile::EXTENSION_SEPARATOR, NEString::EndPos, true);
    if (pos != NEString::InvalidPos && NEString::isAlphanumeric<char>(fileName.GetCharAt(pos + 1)) )
    {
        result = fileName.Substr( pos, NEString::CountAll );
    }

    return result;
}

CEString CEFile::GetFileDirectory(const char* filePath)
{
    NEString::CharPos pos = NEString::isEmpty<char>(filePath) ? NEString::InvalidPos : NEString::searchLastOf<char>(CEFile::PATH_SEPARATOR, filePath, NEString::EndPos, NULL);
    return (pos > 0 ? CEString(filePath, pos) : CEString());
}

bool CEFile::CreateFoldersCascaded( const char* dirPath )
{
    bool result = false;
    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        char * buffer = DEBUG_NEW char[CEFile::MAXIMUM_PATH + 1];
        if ( buffer != NULL )
        {
            const char * src    = dirPath;
            char *       dst    = buffer;
            bool doCreate       = false;

            do 
            {
                if (*src == CEFile::PATH_SEPARATOR)
                    *dst ++ = *src++;

                while (*src != CEFile::PATH_SEPARATOR && *src != '\0')
                    *dst ++ = *src ++;

                *dst    = '\0';

                doCreate = CEFile::IsDirectoryExist(buffer) || CEFile::_createFolder(buffer);

            } while (doCreate && (*src != '\0'));

            result = doCreate;
            delete [] buffer;
        }
    }

    return result;
}

CEString CEFile::NormalizeFileName( const char * fileName )
{
#ifndef WINDOWS
    static const CEString _PSEUDO_ROOT( "<root>" );
#endif // !WINDOWS

    
    CEString result;
    CEStringList list;
    if (fileName != NULL)
    {
        if ( CEFile::_nameHasCurrentFolder(fileName, false) )
        {
            CEString cd = CEFile::GetCurrentDir();
            CEFile::SplitFilePath(cd.GetBuffer(), list);
#ifndef WINDOWS
            list.AddHead(_PSEUDO_ROOT);
#endif // WINDOWS
        }
        else if (*fileName == '%')
        {
            for ( int i = 0; i < CEFile::LEN_SPECIAL_MASKS; ++ i )
            {
                if (NEString::startsWith<char>(fileName, SPEACIAL_MASKS[i], false))
                {
                    CEString special = CEFile::GetSpecialFolder(static_cast<CEFile::eSpecialFolder>(i));
                    CEFile::SplitFilePath(special.GetBuffer(), list);
#ifndef WINDOWS
                    list.AddHead(_PSEUDO_ROOT);
#endif // WINDOWS
                    break;
                }
            }
        }
#ifndef WINDOWS
        else if ((*fileName == CEFile::UNIX_SEPARATOR))
        {
            list.AddHead( _PSEUDO_ROOT);
        }
#endif  // !WINDOWS
    }

    CEFile::SplitFilePath(fileName, list);

    bool isInvalid  = false;
    for (LISTPOS pos = list.GetHeadPosition(); pos != NULL; )
    {
        const CEString & node = list.GetAt(pos);
        if (CEFile::_nameHasCurrentFolder(node, true))
        {
            LISTPOS next = list.GetNextPosition(pos);
            list.RemoveAt(pos);
            pos = next;
        }
        else if (CEFile::_nameHasParentFolder(node, true))
        {
            LISTPOS next = list.GetNextPosition(pos);
            LISTPOS prev = list.GetPrevPosition(pos);
            list.RemoveAt(pos);
            if (prev != NULL)
            {
                list.RemoveAt(prev);
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
            pos = list.GetNextPosition(pos);
        }
    }

    if (isInvalid || list.IsEmpty())
        return result;

    char fmt[32];
    NEUtilities::sSystemTime st;
    CEDateTime::GetNow(st);
    CEString::PrintString(fmt, 32, CEFile::TIMESTAMP_FORMAT, st.stYear, st.stMonth, st.stDay, st.stHour, st.stMinute, st.stSecond, st.stMillisecs);

    CEString first = list.RemoveHead();
#ifndef WINDOWS
    if (first == _PSEUDO_ROOT)
    {
        result = CEFile::PATH_SEPARATOR;
        if (list.IsEmpty() == false)
            first = list.RemoveHead();
    }
#endif // !WINDOWS

    result += first;
    for (LISTPOS pos = list.GetHeadPosition(); pos != NULL; pos = list.GetNextPosition(pos))
    {
        result += CEFile::PATH_SEPARATOR;
        result += list.GetAt(pos);
    }

    do 
    {
        NEString::CharPos index = result.FindFirstOf(CEFile::FILE_MASK_TIMESTAMP, 0, false);
        if (index == NEString::InvalidPos)
            break;

        result.Replace(fmt, index, CEFile::FILE_MASK_TIMESTAMP_LEN );
    } while (true);

    CEString appName = CEProcess::GetProcess().GetAppName();
    do 
    {
        NEString::CharPos index = result.FindFirstOf(CEFile::FILE_MASK_APPNAME, 0, false);
        if (index == NEString::InvalidPos)
            break;

        result.Replace(appName, index, CEFile::FILE_MASK_APPNAME_LEN );
    } while (true);

    return result;
}

bool CEFile::FindParent(const char * filePath, const char ** nextPos, const char * lastPos /*= NULL*/)
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
            if ( filePath[length - 1] == CEFile::PATH_SEPARATOR )
                -- length;
        }

        if (length != 0)
        {
            int pos = NEString::searchLastOf(CEFile::PATH_SEPARATOR, filePath, NEString::EndPos, nextPos);
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

CEString CEFile::GetParent(const char * filePath)
{
    CEString result;
    const char * end = NULL;
    if (CEFile::FindParent(filePath, &end))
    {
        result.CopyString(filePath, static_cast<NEString::CharCount>(end - filePath));
    }

    return result;
}

int CEFile::SplitFilePath(const char * filePath, CEStringList & outList)
{
    int count           = 0;
    const char * start  = filePath;
    const char * end    = filePath;

    while (*end != NEString::EndOfString)
    {
        if ((*end == CEFile::UNIX_SEPARATOR) || (*end == CEFile::DOS_SEPARATOR))
        {
            CEString node(start, static_cast<int>(end - start));
            if (node.IsEmpty() == false)
            {
                outList.AddTail( node );
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
        CEString node(start, static_cast<int>(end - start));
        if (node.IsEmpty() == false)
            outList.AddTail( node );
    }

    return count;
}
