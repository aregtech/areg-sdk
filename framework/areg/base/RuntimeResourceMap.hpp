#ifndef AREG_BASE_RUNTIMERESOURCEMAP_HPP
#define AREG_BASE_RUNTIMERESOURCEMAP_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/RuntimeResourceMap.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Blocking and Non-blocking
 *              Runtime Resource Object container.
 *
 *
 *              This class is a container for registered resources.
 *              As an example, class used to keep mapping of created Thread
 *              Objects, where the Key value can be unique Thread ID.
 *              Having Resource Mapping object, it is possible accessing
 *              to registered object by unique Key value. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/HashMap.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/ResourceMap.hpp"
#include "areg/base/SyncPrimitives.hpp"

namespace areg {

/************************************************************************
 * Hierarchies and list of declared classes
 ************************************************************************/
// HashMap<RuntimeClassID, RUNTIME_DELEGATE>
//      template <typename RUNTIME_DELEGATE> class RuntimeHashMap;
//          template <typename LOCKABLE, class RUNTIME_DELEGATE, class Deleter> ResourceMapBase<RuntimeClassID, RUNTIME_DELEGATE, RuntimeHashMap<RUNTIME_DELEGATE>>
//              template <class RUNTIME_DELEGATE, class Deleter> class RuntimeResourceMapBase;
//                  template <class RUNTIME_DELEGATE, class Deleter> class RuntimeResourceMap;
//                  template <class RUNTIME_DELEGATE, class Deleter> class ConcurrentRuntimeResourceMap;

/**
 * \brief   Hash map template for tracking run-time objects by Runtime Class ID.
 **/
template <typename RUNTIME_DELEGATE>
using RuntimeHashMap = HashMap<RuntimeClassID, RUNTIME_DELEGATE>;

/**
 * \brief   Base class for resource maps tracking run-time objects by Runtime Class ID.
 *          Thread-safety depends on the synchronization object passed to the constructor.
 **/
template <typename LOCKABLE, typename RUNTIME_DELEGATE, class Deleter>
using RuntimeResourceMapBase        = ResourceMapBase<RuntimeClassID, RUNTIME_DELEGATE, LOCKABLE, RuntimeHashMap<RUNTIME_DELEGATE>, Deleter>;

/**
 * \brief   Non-thread-safe resource map template for tracking run-time objects by Runtime Class ID.
 *          Use in single-threaded contexts only.
 **/
template <class RUNTIME_DELEGATE, class Deleter>
using RuntimeResourceMap            = RuntimeResourceMapBase<NolockSyncObject, RUNTIME_DELEGATE, Deleter>;

/**
 * \brief   Thread-safe resource map template for tracking run-time objects by Runtime Class ID in
 *          multi-threaded contexts.
 **/
template <class RUNTIME_DELEGATE, class Deleter>
using ConcurrentRuntimeResourceMap  = RuntimeResourceMapBase<ResourceLock, RUNTIME_DELEGATE, Deleter>;

} // namespace areg
#endif  // AREG_BASE_RUNTIMERESOURCEMAP_HPP
