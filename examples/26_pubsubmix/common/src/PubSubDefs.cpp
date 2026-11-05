
#include "common/src/PubSubDefs.hpp"

#include <algorithm>
#include <mutex>

namespace
{
    std::mutex                  _infoLock;              //!< Guards the coordinate of the information line.
    areg::ext::Console::Coord   _infoCoord  { 0, 0 };   //!< The coordinate of the information line.
}

namespace pubsub
{
    void next_info_row()
    {
        std::lock_guard<std::mutex> lock(_infoLock);
        _infoCoord.posY = std::max(static_cast<decltype(_infoCoord.posY)>(_infoCoord.posY + 1), CoordSeparator.posY);
    }

    areg::ext::Console::Coord info_coord()
    {
        std::lock_guard<std::mutex> lock(_infoLock);
        return _infoCoord;
    }
}
