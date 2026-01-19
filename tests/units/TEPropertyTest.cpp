/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/TEPropertyTest.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Tests of TEProperty object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/TEProperty.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SharedBuffer.hpp"

/**
 * \brief   Test TEProperty constructors.
 **/
TEST(TEPropertyTest, TestConstructors)
{
    using Property = TEProperty<String, uint32_t>;
    String  propName{ "AREG Property" };

    // Step 1: test constructors.
    Property propDef;
    EXPECT_TRUE(propDef.mValue.first.isEmpty());

    Property propInit(propName, 0u);
    EXPECT_FALSE(propInit.mValue.first.isEmpty());

    Property propMove(std::move(propName), std::move(0u));
    EXPECT_FALSE(propMove.mValue.first.isEmpty());
    EXPECT_TRUE(propName.isEmpty());
    EXPECT_EQ(propInit, propMove);

    Property propCopy(propInit);
    EXPECT_FALSE(propCopy.mValue.first.isEmpty());
    EXPECT_EQ(propInit, propCopy);

    Property propMove2(std::move(propInit));
    EXPECT_FALSE(propMove2.mValue.first.isEmpty());
    EXPECT_TRUE(propInit.mValue.first.isEmpty());
    EXPECT_NE(propInit, propMove2);
}

/**
 * \brief   Test TEProperty operators.
 **/
TEST(TEPropertyTest, TestOperators)
{
    using Property = TEProperty<String, uint32_t>;
    String  propName{ "AREG Property" };

    // Step 1: test constructors.
    Property propDef;
    EXPECT_TRUE(propDef.mValue.first.isEmpty());

    Property propInit(propName, 0u);
    EXPECT_FALSE(propInit.mValue.first.isEmpty());
    EXPECT_TRUE(propInit != propDef);
    EXPECT_FALSE(propInit == propDef);

    Property propCopy;
    EXPECT_TRUE(propCopy.mValue.first.isEmpty());
    propCopy = propInit;
    EXPECT_FALSE(propCopy.mValue.first.isEmpty());
    EXPECT_TRUE(propInit == propCopy);

    Property propMove;
    EXPECT_TRUE(propMove.mValue.first.isEmpty());
    propMove = std::move(propInit);
    EXPECT_TRUE(propInit.mValue.first.isEmpty());
    EXPECT_FALSE(propMove.mValue.first.isEmpty());
    EXPECT_TRUE(propMove == propCopy);
    EXPECT_FALSE(propMove != propCopy);

    EXPECT_TRUE(propInit != propMove);
    EXPECT_FALSE(propInit == propMove);
}

/**
 * \brief   Test TEProperty attributes.
 **/
TEST(TEPropertyTest, TestAttributes)
{
    using Property = TEProperty<String, String>;
    const String propName{ "AREG Property" };
    const String propValue{ "AREG Value" };

    String otherName{ "Another name" };
    String otherValue{ "Another value" };

    // Step 1: test constructors.
    Property propCopy;
    EXPECT_TRUE(propCopy.getKey().isEmpty() && propCopy.getValue().isEmpty());
    propCopy.setData(propName, propValue);
    EXPECT_FALSE(propCopy.getKey().isEmpty() && propCopy.getValue().isEmpty());
    EXPECT_EQ(propCopy.getKey(), propName);
    EXPECT_EQ(propCopy.getValue(), propValue);

    String moveName = propName;
    String moveValue = propValue;
    Property propMove;
    EXPECT_TRUE(propMove.getKey().isEmpty() && propMove.getValue().isEmpty());
    propMove.setData(std::move(moveName), std::move(moveValue));
    EXPECT_FALSE(propMove.getKey().isEmpty() && propMove.getValue().isEmpty());
    EXPECT_EQ(propMove.getKey(), propName);
    EXPECT_EQ(propMove.getValue(), propValue);

    EXPECT_TRUE(propCopy == propMove);

    propCopy.setKey(otherName);
    propCopy.setValue(otherValue);
    EXPECT_EQ(propCopy.getKey(), otherName);
    EXPECT_EQ(propCopy.getValue(), otherValue);

    moveName = otherName;
    moveValue = otherValue;
    propMove.setKey(std::move(moveName));
    propMove.setValue(std::move(moveValue));
    EXPECT_EQ(propMove.getKey(), otherName);
    EXPECT_EQ(propMove.getValue(), otherValue);
    EXPECT_TRUE(propMove == propCopy);

    propCopy.getKey() = propName;
    propCopy.getValue() = propValue;
    EXPECT_TRUE(propMove != propCopy);
}

/**
 * \brief   Test TEProperty streaming.
 **/
TEST(TEPropertyTest, TestStreaming)
{
    using Property = TEProperty<String, String>;
    const String propName{ "AREG Property" };
    const String propValue{ "AREG Value" };

    SharedBuffer stream;
    Property src(propName, propValue), dst("key", "value");
    EXPECT_NE(src, dst);

    stream << src;
    stream.moveToBegin();
    stream >> dst;
    EXPECT_EQ(src, dst);
}
