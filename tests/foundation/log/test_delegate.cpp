/**************************************************************************/
/* test_delegate.cpp                                                      */
/**************************************************************************/
/*                          This file is part of:                         */
/*                                SushiLog                                */
/*                https://github.com/SushiSystems/SushiLog                */
/*                         https://sushisystems.io                        */
/**************************************************************************/
/* Copyright (c) 2026-present Mustafa Garip & Sushi Systems               */
/*                                                                        */
/* Licensed under the PolyForm Noncommercial License 1.0.0 (the           */
/* "License"); you may not use this file except in compliance with the    */
/* License. You may obtain a copy of the License at                       */
/*                                                                        */
/*     https://polyformproject.org/licenses/noncommercial/1.0.0           */
/*                                                                        */
/* Noncommercial use is free. Commercial use requires a separate licence  */
/* from Sushi Systems; see COMMERCIAL.md. The software is provided        */
/* "as is", without warranty of any kind.                                 */
/**************************************************************************/

/**
 * @file test_delegate.cpp
 * @brief Tests Delegate: emptiness, calls, moves and the destruction of what it holds.
 * @author Mustafa Garip
 * @see tests/foundation/log/README.md
 */

#include <SushiLog/log/delegate.hpp>

#include <memory>
#include <utility>

#include <gtest/gtest.h>

namespace
{
    /** @brief Counts the destructions of the copies of one value. */
    struct DestructionCounter
    {
        int* count; /**< The count each destruction of a live copy raises. */

        explicit DestructionCounter(int* target) noexcept : count(target)
        {

        }

        DestructionCounter(DestructionCounter&& other) noexcept : count(other.count)
        {
            other.count = nullptr;
        }

        DestructionCounter& operator=(DestructionCounter&&) = delete;
        DestructionCounter(const DestructionCounter&) = delete;
        DestructionCounter& operator=(const DestructionCounter&) = delete;

        ~DestructionCounter()
        {
            if (count != nullptr)
                ++*count;
        }

        void operator()() const noexcept
        {

        }
    };
} // namespace

TEST(Small_Delegate, DefaultAndNullAreEmpty)
{
    const SushiLog::Delegate<void()> empty{};
    const SushiLog::Delegate<void()> null(nullptr);

    EXPECT_FALSE(empty);
    EXPECT_FALSE(null);
}

TEST(Small_Delegate, CallsTheHeldCallableWithItsArguments)
{
    const SushiLog::Delegate<int(int, int)> add([](int a, int b)
    {
        return a + b;
    });

    ASSERT_TRUE(add);
    EXPECT_EQ(add(2, 3), 5);
}

TEST(Small_Delegate, HoldsAMoveOnlyCallable)
{
    auto value = std::make_unique<int>(7);
    const SushiLog::Delegate<int()> read([value = std::move(value)]()
    {
        return *value;
    });

    EXPECT_EQ(read(), 7);
}

TEST(Small_Delegate, MoveLeavesTheSourceEmpty)
{
    SushiLog::Delegate<int()> source([]()
    {
        return 1;
    });
    SushiLog::Delegate<int()> target(std::move(source));

    EXPECT_FALSE(source);
    ASSERT_TRUE(target);
    EXPECT_EQ(target(), 1);
}

TEST(Small_Delegate, AssigningNullDestroysTheCallableOnce)
{
    int destroyed = 0;
    SushiLog::Delegate<void()> held{DestructionCounter(&destroyed)};

    held = nullptr;

    EXPECT_FALSE(held);
    EXPECT_EQ(destroyed, 1);
}

TEST(Small_Delegate, MoveAssignmentDestroysTheOldCallable)
{
    int destroyed = 0;
    SushiLog::Delegate<void()> held{DestructionCounter(&destroyed)};
    SushiLog::Delegate<void()> other([]()
    {

    });

    held = std::move(other);

    EXPECT_EQ(destroyed, 1);
    EXPECT_TRUE(held);
}
