/************************************************************************
 * \file        areg/src/component/private/CEThreadEventBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Multithreading Event base class.
 *
 ************************************************************************/
#include "areg/src/component/private/CEThreadEventBase.hpp"

//////////////////////////////////////////////////////////////////////////
// CEThreadEventBase class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CEThreadEventBase, CEEvent)

CEThreadEventBase::CEThreadEventBase( CEEvent::eEventType eventType )
    : CEEvent   (eventType)
{
    ; // do nothing
}

CEThreadEventBase::~CEThreadEventBase( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEThreadEventConsumerBase class implementation
//////////////////////////////////////////////////////////////////////////

CEThreadEventConsumerBase::CEThreadEventConsumerBase( void )
    : IEEventConsumer  ( )
{
    ; // do nothing
}

CEThreadEventConsumerBase::~CEThreadEventConsumerBase( void )
{
    ; // do nothing
}
