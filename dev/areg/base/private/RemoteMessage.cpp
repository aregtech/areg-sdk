/************************************************************************
 * \file        areg/base/private/RemoteMessage.cpp
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
#include "areg/base/RemoteMessage.hpp"

#include "areg/base/NEMemory.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_base_RemoteMessage_checksumCalculate);
inline unsigned int RemoteMessage::_checksumCalculate( const NEMemory::sRemoteMessage & remoteMessage )
{
    TRACE_SCOPE(areg_base_RemoteMessage_checksumCalculate);

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

        result = NEMath::crc32Calculate(data, length);

        TRACE_INFO("offset = [ %u ], data = [ %p ], remote data = [ %p ], remain = [ %u ], used = [ %u ], correct = [ %u ], length = [ %u ], checksum = [ %u ]"
                    , offset, data, reinterpret_cast<const unsigned char *>(&remoteMessage), remain, used, correct, length, result);
    }
    return result;
}

RemoteMessage::RemoteMessage(unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : SharedBuffer  ( blockSize )
{
    ; // do nothing
}

RemoteMessage::RemoteMessage(unsigned int reserveSize, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : SharedBuffer  ( blockSize )
{
    resize(reserveSize, false);
}

RemoteMessage::RemoteMessage(const unsigned char * buffer, unsigned int size, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : SharedBuffer  ( blockSize )
{
    resize(size, false);
    writeData(buffer, size);
}

RemoteMessage::RemoteMessage(NEMemory::sRemoteMessage & remoteMessage)
    : SharedBuffer    ( reinterpret_cast<NEMemory::sByteBuffer &>(remoteMessage) )
{
    ; // do nothing
}

RemoteMessage::RemoteMessage( const RemoteMessage & src )
    : SharedBuffer    ( static_cast<const SharedBuffer &>(src) )
{
    ; // do nothing
}

RemoteMessage::~RemoteMessage(void)
{
    ; // do nothing
}

const RemoteMessage & RemoteMessage::operator = ( const RemoteMessage & src )
{
    static_cast<SharedBuffer &>(*this) = static_cast<const SharedBuffer &>(src);
    return (*this);
}

unsigned int RemoteMessage::initBuffer(unsigned char *newBuffer, unsigned int bufLength, bool makeCopy) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;

    if (newBuffer != NULL)
    {
        result                  = 0;
        unsigned int dataOffset = this->getDataOffset();
        unsigned int dataLength = bufLength - dataOffset;

        NEMemory::sRemoteMessageHeader & header = NEMemory::constructElems<NEMemory::sRemoteMessage>(newBuffer, 1)->rbHeader;
        header.rbhBufHeader.biBufSize   = bufLength;
        header.rbhBufHeader.biLength    = dataLength;
        header.rbhBufHeader.biOffset    = dataOffset;
        header.rbhBufHeader.biBufType   = NEMemory::BufferRemote;
        header.rbhBufHeader.biRefCount  = 1;

        if ( makeCopy )
        {
            unsigned char* data         = NEMemory::getBufferDataWrite(reinterpret_cast<NEMemory::sByteBuffer &>(header.rbhBufHeader));
            const unsigned char* srcBuf = NEMemory::getBufferDataRead(mByteBuffer);
            unsigned int srcCount       = getSizeUsed();
            srcCount                    = MACRO_MIN(srcCount, dataLength);
            result                      = srcCount;

            header.rbhBufHeader.biUsed  = srcCount;
            header.rbhTarget            = getTarget();
            header.rbhChecksum          = getChecksum();
            header.rbhSource            = getSource();
            header.rbhMessageId         = getMessageId();
            header.rbhResult            = getResult();
            header.rbhSequenceNr        = getSequenceNr();

            NEMemory::memCopy(data, dataLength, srcBuf, srcCount);
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

bool RemoteMessage::isChecksumValid(void) const
{
    return isValid() ? getChecksum() == RemoteMessage::_checksumCalculate( _getRemoteMessage() ) : false;
}

DEF_TRACE_SCOPE(areg_base_RemoteMessage_bufferCompletionFix);
void RemoteMessage::bufferCompletionFix(void)
{
    TRACE_SCOPE(areg_base_RemoteMessage_bufferCompletionFix);

    if ( isValid() )
    {
        NEMemory::sRemoteMessage & msg = _getRemoteMessage();
        NEMemory::sRemoteMessageHeader & header = msg.rbHeader;

#ifdef  DEBUG
        unsigned int bufLength = MACRO_ALIGN_SIZE(header.rbhBufHeader.biUsed, NEMemory::BLOCK_SIZE);
        ASSERT( header.rbhBufHeader.biLength >= bufLength);
#endif  // DEBUG

        unsigned int bufSize    = header.rbhBufHeader.biUsed + header.rbhBufHeader.biOffset;
        unsigned int dataUsed   = header.rbhBufHeader.biUsed;
        unsigned int dataLen    = header.rbhBufHeader.biUsed;
        unsigned int checksum   = RemoteMessage::_checksumCalculate( msg );

        header.rbhBufHeader.biBufSize   = MACRO_MAX(bufSize, sizeof(NEMemory::sRemoteMessage));
        header.rbhBufHeader.biLength    = MACRO_MAX(dataUsed, sizeof(NEMemory::BufferData));
        header.rbhChecksum              = checksum;

        TRACE_INFO("Remote message completion: bufSize [ %u ], msg.biBufSize = [ %u ]; dataLen [ %u ], msg.biLength = [ %u ], dataUsed [ %u ], checksum [ %u ]"
                        , bufSize, header.rbhBufHeader.biBufSize
                        , dataLen, header.rbhBufHeader.biLength
                        , dataUsed, checksum);
    }
}

unsigned char * RemoteMessage::initMessage(const NEMemory::sRemoteMessageHeader & rmHeader )
{
    decreaseLock();

    unsigned int sizeUsed   = MACRO_MAX(rmHeader.rbhBufHeader.biUsed, 1);
    unsigned int sizeData   = MACRO_ALIGN_SIZE(sizeUsed, mBlockSize);
    unsigned int sizeBuffer = getHeaderSize() + sizeData;
    unsigned char * result  = DEBUG_NEW unsigned char[sizeBuffer];
    if ( result != NULL )
    {
        NEMemory::sRemoteMessage * msg      = NEMemory::constructElems<NEMemory::sRemoteMessage>(result, 1);
        NEMemory::sRemoteMessageHeader & dst= msg->rbHeader;
        dst.rbhBufHeader.biBufSize  = sizeBuffer;
        dst.rbhBufHeader.biLength   = sizeData;
        dst.rbhBufHeader.biOffset   = getDataOffset();
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

    return getBuffer();
}

int RemoteMessage::getDataOffset(void) const
{
    return sizeof(NEMemory::sRemoteMessageHeader);
}

int RemoteMessage::getHeaderSize(void) const
{
    return sizeof(NEMemory::sRemoteMessage);
}
