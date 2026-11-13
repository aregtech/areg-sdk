/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * \copyright (c) 2017-2026 Aregtech.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/TemplateBase.hpp"

#include <utility>
#include <vector>

// Tokenization can retain empty tokens at both ends and between adjacent delimiters.
TEST(ContainersTest, boundary_tokenizer_preserves_leading_adjacent_and_trailing_empty_tokens)
{
    const areg::Tokenizer tokenizer(areg::String(",a;;b,"), areg::String(",;"));
    const auto& tokens = tokenizer.list();
    ASSERT_EQ(tokens.size(), 5u);
    EXPECT_EQ(tokens[0], areg::String(""));
    EXPECT_EQ(tokens[1], areg::String("a"));
    EXPECT_EQ(tokens[2], areg::String(""));
    EXPECT_EQ(tokens[3], areg::String("b"));
    EXPECT_EQ(tokens[4], areg::String(""));
}

// Retokenizing replaces earlier results and can omit empty tokens.
TEST(ContainersTest, boundary_retokenizing_replaces_old_tokens_and_can_drop_empty_tokens)
{
    areg::Tokenizer tokenizer(areg::String("old"), areg::String(","));
    const auto& tokens = tokenizer.tokenize(areg::String(",a,,b,"), areg::String(","), false);
    EXPECT_EQ(&tokens, &tokenizer.list());
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0], areg::String("a"));
    EXPECT_EQ(tokens[1], areg::String("b"));
    EXPECT_TRUE(tokenizer.tokenize(areg::String(",,,"), areg::String(","), false).is_empty());
    EXPECT_TRUE(tokenizer.tokenize(areg::String(""), areg::String(","), false).is_empty());
}

// Empty input and empty delimiter lists produce the documented tokens.
TEST(ContainersTest, boundary_empty_input_and_empty_delimiters_have_defined_tokens)
{
    areg::Tokenizer tokenizer;
    EXPECT_TRUE(tokenizer.list().is_empty());
    const auto& empty = tokenizer.tokenize(areg::String(""), areg::String(","));
    ASSERT_EQ(empty.size(), 1u);
    EXPECT_TRUE(empty[0].is_empty());
    const auto& unsplit = tokenizer.tokenize(areg::String("a,b"), areg::String(""));
    ASSERT_EQ(unsplit.size(), 1u);
    EXPECT_EQ(unsplit[0], areg::String("a,b"));
}

// Copying a tokenizer gives independent results, while moving transfers its tokens.
TEST(ContainersTest, boundary_tokenizer_copy_is_independent_and_move_transfers_tokens)
{
    areg::Tokenizer source(areg::String("a,b"), areg::String(","));
    areg::Tokenizer copy(source);
    source.tokenize(areg::String("changed"), areg::String(","));
    ASSERT_EQ(copy.list().size(), 2u);
    EXPECT_EQ(copy.list()[0], areg::String("a"));
    areg::Tokenizer moved(std::move(copy));
    EXPECT_TRUE(copy.list().is_empty());
    ASSERT_EQ(moved.list().size(), 2u);
    EXPECT_EQ(moved.list()[1], areg::String("b"));
    EXPECT_EQ(source.list()[0], areg::String("changed"));
}

/**
 * \brief   A const iterator of a mutable container becomes an iterator at the same element, the
 *          container is unchanged, and a mutable iterator is returned as it is.
 **/
TEST(ConstlessTest, iterator_of_a_mutable_container_points_to_the_same_element)
{
    std::vector<int> values{ 1, 2, 3 };
    areg::Constless<std::vector<int>> helper;
    std::vector<int>::const_iterator constPos{ values.cbegin() + 1 };
    std::vector<int>::iterator pos{ helper.iter(values, constPos) };
    ASSERT_EQ(values.size(), 3u);
    EXPECT_EQ(*pos, 2);
    *pos = 5;
    EXPECT_EQ(values[1], 5);

    std::vector<int>::iterator last{ values.begin() + 2 };
    EXPECT_EQ(helper.iter(values, last), last);
    EXPECT_EQ(values, (std::vector<int>{ 1, 5, 3 }));
}
