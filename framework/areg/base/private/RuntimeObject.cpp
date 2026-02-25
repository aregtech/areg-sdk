/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/RuntimeObject.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform Runtime Object class
 *
 ************************************************************************/
#include "areg/base/RuntimeObject.hpp"
#include <new>

//////////////////////////////////////////////////////////////////////////
// RuntimeObject class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Implement runtime standard functions and initialize variables
/************************************************************************/
AREG_IMPLEMENT_RUNTIME(RuntimeObject, RuntimeBase)

void RuntimeObject::destroy()
{
    delete this;
}

RuntimeObject::operator uint32_t() const
{
    return runtime_class_number();
}

void* RuntimeObject::operator new(size_t size)
{
    return ::operator new(size);
}

void* RuntimeObject::operator new[](size_t size)
{
    return ::operator new[](size);
}

void* RuntimeObject::operator new(size_t /*size*/, void* ptr)
{
    return ptr;
}

void* RuntimeObject::operator new[](size_t /*size*/, void* ptr)
{
    return ptr;
}


#if defined(_DEBUG) && defined(_MSC_VER)
void* RuntimeObject::operator new(size_t size, int32_t /*block*/, const char* file, int32_t line)
{
    return ::operator new(size, 1, file, line);
}
#else   // _DEBUG
void* RuntimeObject::operator new(size_t size, int32_t /*block*/, const char* /*file*/, int32_t /*line*/)
{
    return ::operator new (size);
}
#endif  // _DEBUG

#if defined(_DEBUG) && defined(_MSC_VER)
void* RuntimeObject::operator new[](size_t size, int32_t /*block*/, const char* file, int32_t line)
{
    return ::operator new(size, 1, file, line);
}
#else   // _DEBUG
void* RuntimeObject::operator new[](size_t size, int32_t /*block*/, const char* /*file*/, int32_t /*line*/)
{
    return ::operator new[](size);
}
#endif  // _DEBUG

void RuntimeObject::operator delete(void* ptr)
{
    ::operator delete(ptr);
}

void RuntimeObject::operator delete(void* ptr, size_t /*size*/)
{
    ::operator delete(ptr);
}

void RuntimeObject::operator delete(void* ptr, int32_t, const char*, int32_t)
{
    ::operator delete (ptr);
}

void RuntimeObject::operator delete[](void* ptr)
{
    ::operator delete[](ptr);
}

void RuntimeObject::operator delete[](void* ptr, size_t /*size*/)
{
    ::operator delete[](ptr);
}

void RuntimeObject::operator delete[](void* ptr, int32_t, const char*, int32_t)
{
    ::operator delete[](ptr);
}
