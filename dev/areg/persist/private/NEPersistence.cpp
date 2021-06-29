/************************************************************************
 * \file        areg/persist/private/NEPersistence.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       File data persist syntax namespace
 ************************************************************************/
#include "areg/persist/private/NEPersistence.hpp"

#include "areg/base/NEString.hpp"

const int        NEPersistence::SYNTAX_COMMENT_LEN = static_cast<int>(NEString::getStringLength<char>( NEPersistence::SYNTAX_COMMENT ));
