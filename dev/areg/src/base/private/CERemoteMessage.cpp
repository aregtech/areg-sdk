/************************************************************************
 * \file        areg/src/base/private/CERemoteMessage.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Remote Shared Buffer class. This Buffer is used for 
 *              Read and Write of data during remote communication
 *              between different processes running either on same or 
 *              different hosts. The Data in buffer within same process
 *              remains valid until  the number of references to the 
 *              buffer is more than zero.
 *
 ************************************************************************/
#include "areg/src/base/CERemoteMessage.hpp"

#include "areg/src/base/NEMemory.hpp"
#include "areg/src/base/NEMath.hpp"
#include "areg/src/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_base_CERemoteMessage_BufferCompletionFix);
DEF_TRACE_SCOPE(areg_base_CERemoteMessage_checksumCalculate);

inline unsigned int CERemoteMessage::_checksumCalculate( const NEMemory::sRemoteMessage & remoteMessage )
{
    TRACE_SCOPE(areg_base_CERemoteMessage_checksumCalculate);

    unsigned int result = NEMemory::INVALID_VALUE;
    if ( &remoteMessage != &NEMemory::InvalidRemoteMessage )
    {
        const unsigned int offset   = MACRO_OFFSETOF(NEMemory::sRemoteMessageHeader, rbhSource);
        const unsigned char * data  = reinterpret_cast<const unsigned char *>(&remoteMessage.rbHeader.rbhSource);
        const unsigned int remain   = remoteMessage.rbHeader.rbhBufHeader.biOffset - offset;
        const unsigned int used     = remoteMessage.rbHeader.rbhBufHeader.biUsed;
        const unsigned int correct  = MACRO_MAX(used, sizeof(NEMemory::BufferData));
        const unsigned int length   = used + remain;
        // const unsigned char * data      = NEMemory::GetBufferDataRead(reinterpret_cast<const NEMemory::sByteBuffer &>(remoteMessage.rbHeader.rbhBufHeader));
        // const unsigned int dataLength   = remoteMessage.rbHeader.rbhBufHeader.biUsed + (remoteMessage.rbHeader.rbhBufHeader.biOffset - offset);

        result = NEMath::Crc32Calculate(data, length);

        TRACE_INFO("offset = [ %u ], data = [ %p ], remote data = [ %p ], remain = [ %u ], used = [ %u ], correct = [ %u ], length = [ %u ], checksum = [ %u ]"
                    , offset, data, reinterpret_cast<const unsigned char *>(&remoteMessage), remain, used, correct, length, result);
    }
    return result;
}

CERemoteMessage::CERemoteMessage(unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : CESharedBuffer  ( blockSize )
{
    ; // do nothing
}

CERemoteMessage::CERemoteMessage(unsigned int reserveSize, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : CESharedBuffer  ( blockSize )
{
    ResizeBuffer(reserveSize, false);
}

CERemoteMessage::CERemoteMessage(const unsigned char * buffer, unsigned int size, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : CESharedBuffer  ( blockSize )
{
    ResizeBuffer(size, false);
    WriteData(buffer, size);
}

CERemoteMessage::CERemoteMessage(NEMemory::sRemoteMessage & remoteMessage)
    : CESharedBuffer    ( reinterpret_cast<NEMemory::sByteBuffer &>(remoteMessage) )
{
    ; // do nothing
}

CERemoteMessage::CERemoteMessage( const CERemoteMessage & src )
    : CESharedBuffer    ( static_cast<const CESharedBuffer &>(src) )
{
    ; // do nothing
}

CERemoteMessage::~CERemoteMessage(void)
{
    ; // do nothing
}

const CERemoteMessage & CERemoteMessage::operator = ( const CERemoteMessage & src )
{
    static_cast<CESharedBuffer &>(*this) = static_cast<const CESharedBuffer &>(src);
    return (*this);
}

unsigned int CERemoteMessage::InitBuffer(unsigned char *newBuffer, unsigned int bufLength, bool makeCopy) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;

    if (newBuffer != NULL)
    {
        result                  = 0;
        unsigned int dataOffset = SizeDataOffset();
        unsigned int dataLength = bufLength - dataOffset;

        NEMemory::sRemoteMessageHeader & header = NEMemory::ConstructElems<NEMemory::sRemoteMessage>(newBuffer, 1)->rbHeader;
        header.rbhBufHeader.biBufSize   = bufLength;
        header.rbhBufHeader.biLength    = dataLength;
        header.rbhBufHeader.biOffset    = dataOffset;
        header.rbhBufHeader.biBufType   = NEMemory::BufferRemote;
        header.rbhBufHeader.biRefCount  = 1;

        if ( makeCopy )
        {
            unsigned char* data         = NEMemory::GetBufferDataWrite(reinterpret_cast<NEMemory::sByteBuffer &>(header.rbhBufHeader));
            const unsigned char* srcBuf = NEMemory::GetBufferDataRead(mByteBuffer);
            unsigned int srcCount       = GetUsedSize();
            srcCount                    = MACRO_MIN(srcCount, dataLength);
            result                      = srcCount;

            header.rbhBufHeader.biUsed  = srcCount;
            header.rbhTarget            = GetTarget();
            header.rbhChecksum          = GetChecksum();
            header.rbhSource            = GetSource();
            header.rbhMessageId         = GetMessageId();
            header.rbhResult            = GetResult();
            header.rbhSequenceNr        = GetSequenceNr();

            NEMemory::MemCopy(data, dataLength, srcBuf, srcCount);
        }
        else
        {
            header.rbhBufHeader.biUsed  = 0;
            header.rbhTarget            = NEMemory::IGNORE_VALUE;
            header.rbhChecksum          = NEMemory::IGNORE_VALUE;
            header.rbhSource            = NEMemory::IGNORE_VALUE;
            header.rbhMessageId         = NEMemory::IGNORE_VALUE;
            header.rbhResult            = NEMemory::ResultSucceed;
            header.rbhSequenceNr        = NEMemory::IGNORE_VALUE;;
        }
    }
    else
    {
        ; // do nothing
    }

    return result;
}

bool CERemoteMessage::IsChecksumValid(void) const
{
    return IsValid() ? GetChecksum() == CERemoteMessage::_checksumCalculate( getRemoteMessage() ) : false;
}

void CERemoteMessage::BufferCompletionFix(void)
{
    TRACE_SCOPE(areg_base_CERemoteMessage_BufferCompletionFix);

    if ( IsValid() )
    {
        NEMemory::sRemoteMessage & msg = getRemoteMessage();
        NEMemory::sRemoteMessageHeader & header = msg.rbHeader;

#ifdef  DEBUG
        unsigned int bufLength = MACRO_ALIGN_SIZE(header.rbhBufHeader.biUsed, NEMemory::BLOCK_SIZE);
        ASSERT( header.rbhBufHeader.biLength >= bufLength);
#endif  // DEBUG

        unsigned int bufSize    = header.rbhBufHeader.biUsed + header.rbhBufHeader.biOffset;
        unsigned int dataUsed   = header.rbhBufHeader.biUsed;
        unsigned int dataLen    = header.rbhBufHeader.biUsed;
        unsigned int checksum   = CERemoteMessage::_checksumCalculate( msg );

        header.rbhBufHeader.biBufSize   = MACRO_MAX(bufSize, sizeof(NEMemory::sRemoteMessage));
        header.rbhBufHeader.biLength    = MACRO_MAX(dataUsed, sizeof(NEMemory::BufferData));
        header.rbhChecksum              = checksum;

        TRACE_INFO("Remote message completion: bufSize [ %u ], msg.biBufSize = [ %u ]; dataLen [ %u ], msg.biLength = [ %u ], dataUsed [ %u ], checksum [ %u ]"
                        , bufSize, header.rbhBufHeader.biBufSize
                        , dataLen, header.rbhBufHeader.biLength
                        , dataUsed, checksum);
    }
}

unsigned char * CERemoteMessage::InitializeMessage(const NEMemory::sRemoteMessageHeader & rmHeader )
{
    DecreaseLock();

    unsigned int sizeUsed   = MACRO_MAX(rmHeader.rbhBufHeader.biUsed, 1);
    unsigned int sizeData   = MACRO_ALIGN_SIZE(sizeUsed, mBlockSize);
    unsigned int sizeBuffer = SizeBufferStruct() + sizeData;
    unsigned char * result  = DEBUG_NEW unsigned char[sizeBuffer];
    if ( result != NULL )
    {
        NEMemory::sRemoteMessage * msg      = NEMemory::ConstructElems<NEMemory::sRemoteMessage>(result, 1);
        NEMemory::sRemoteMessageHeader & dst= msg->rbHeader;
        dst.rbhBufHeader.biBufSize  = sizeBuffer;
        dst.rbhBufHeader.biLength   = sizeData;
        dst.rbhBufHeader.biOffset   = SizeDataOffset();
        dst.rbhBufHeader.biBufType  = NEMemory::BufferRemote;
        dst.rbhBufHeader.biUsed     = rmHeader.rbhBufHeader.biUsed;
        dst.rbhBufHeader.biRefCount = 1;
        dst.rbhTarget               = rmHeader.rbhTarget;
        dst.rbhChecksum             = rmHeader.rbhChecksum;
        dst.rbhSource               = rmHeader.rbhSource;
        dst.rbhMessageId            = rmHeader.rbhMessageId;
        dst.rbhResult               = rmHeader.rbhResult;
        dst.rbhSequenceNr           = rmHeader.rbhSequenceNr;
        msg->rbData[0]              = static_cast<NEMemory::BufferData>(0);

        mByteBuffer = reinterpret_cast<NEMemory::sByteBuffer *>(msg);
    }

    return GetBuffer();
}

int CERemoteMessage::SizeDataOffset(void) const
{
    return sizeof(NEMemory::sRemoteMessageHeader);
}

int CERemoteMessage::SizeBufferStruct(void) const
{
    return sizeof(NEMemory::sRemoteMessage);
}
