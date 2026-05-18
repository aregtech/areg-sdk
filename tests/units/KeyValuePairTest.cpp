/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/KeyValuePairTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of KeyValuePair object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/KeyValuePair.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SharedBuffer.hpp"

/**
 * \brief   Test KeyValuePair constructors.
 **/
TEST(KeyValuePairTest, test_constructors)
{
    using Property = areg::KeyValuePair<areg::String, uint32_t>;
    areg::String  propName{ "Areg Property" };

    // Step 1: test constructors.
    Property propDef;
    EXPECT_TRUE(propDef.mValue.first.is_empty());

    Property propInit(propName, 0u);
    EXPECT_FALSE(propInit.mValue.first.is_empty());

    Property propMove(std::move(propName), std::move(0u));
    EXPECT_FALSE(propMove.mValue.first.is_empty());
    EXPECT_TRUE(propName.is_empty());
    EXPECT_EQ(propInit, propMove);

    Property propCopy(propInit);
    EXPECT_FALSE(propCopy.mValue.first.is_empty());
    EXPECT_EQ(propInit, propCopy);

    Property propMove2(std::move(propInit));
    EXPECT_FALSE(propMove2.mValue.first.is_empty());
    EXPECT_TRUE(propInit.mValue.first.is_empty());
    EXPECT_NE(propInit, propMove2);
}

/**
 * \brief   Test KeyValuePair operators.
 **/
TEST(KeyValuePairTest, test_operators)
{
    using Property = areg::KeyValuePair<areg::String, uint32_t>;
    areg::String  propName{ "Areg Property" };

    // Step 1: test constructors.
    Property propDef;
    EXPECT_TRUE(propDef.mValue.first.is_empty());

    Property propInit(propName, 0u);
    EXPECT_FALSE(propInit.mValue.first.is_empty());
    EXPECT_TRUE(propInit != propDef);
    EXPECT_FALSE(propInit == propDef);

    Property propCopy;
    EXPECT_TRUE(propCopy.mValue.first.is_empty());
    propCopy = propInit;
    EXPECT_FALSE(propCopy.mValue.first.is_empty());
    EXPECT_TRUE(propInit == propCopy);

    Property propMove;
    EXPECT_TRUE(propMove.mValue.first.is_empty());
    propMove = std::move(propInit);
    EXPECT_TRUE(propInit.mValue.first.is_empty());
    EXPECT_FALSE(propMove.mValue.first.is_empty());
    EXPECT_TRUE(propMove == propCopy);
    EXPECT_FALSE(propMove != propCopy);

    EXPECT_TRUE(propInit != propMove);
    EXPECT_FALSE(propInit == propMove);
}

/**
 * \brief   Test KeyValuePair attributes.
 **/
TEST(KeyValuePairTest, test_attributes)
{
    using Property = areg::KeyValuePair<areg::String, areg::String>;
    const areg::String propName{ "Areg Property" };
    const areg::String propValue{ "Areg Value" };

    areg::String otherName{ "Another name" };
    areg::String otherValue{ "Another value" };

    // Step 1: test constructors.
    Property propCopy;
    EXPECT_TRUE(propCopy.key().is_empty() && propCopy.value().is_empty());
    propCopy.set_data(propName, propValue);
    EXPECT_FALSE(propCopy.key().is_empty() && propCopy.value().is_empty());
    EXPECT_EQ(propCopy.key(), propName);
    EXPECT_EQ(propCopy.value(), propValue);

    areg::String moveName = propName;
    areg::String moveValue = propValue;
    Property propMove;
    EXPECT_TRUE(propMove.key().is_empty() && propMove.value().is_empty());
    propMove.set_data(std::move(moveName), std::move(moveValue));
    EXPECT_FALSE(propMove.key().is_empty() && propMove.value().is_empty());
    EXPECT_EQ(propMove.key(), propName);
    EXPECT_EQ(propMove.value(), propValue);

    EXPECT_TRUE(propCopy == propMove);

    propCopy.set_key(otherName);
    propCopy.set_value(otherValue);
    EXPECT_EQ(propCopy.key(), otherName);
    EXPECT_EQ(propCopy.value(), otherValue);

    moveName = otherName;
    moveValue = otherValue;
    propMove.set_key(std::move(moveName));
    propMove.set_value(std::move(moveValue));
    EXPECT_EQ(propMove.key(), otherName);
    EXPECT_EQ(propMove.value(), otherValue);
    EXPECT_TRUE(propMove == propCopy);

    propCopy.key() = propName;
    propCopy.value() = propValue;
    EXPECT_TRUE(propMove != propCopy);
}

/**
 * \brief   Test KeyValuePair streaming.
 **/
TEST(KeyValuePairTest, test_streaming)
{
    using Property = areg::KeyValuePair<areg::String, areg::String>;
    const areg::String propName{ "Areg Property" };
    const areg::String propValue{ "Areg Value" };

    areg::SharedBuffer stream;
    Property src(propName, propValue), dst("key", "value");
    EXPECT_NE(src, dst);

    stream << src;
    stream.move_to_begin();
    stream >> dst;
    EXPECT_EQ(src, dst);
}
