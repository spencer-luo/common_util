/**
 * @file test_statemachine.cpp
 * @brief Unit tests for cutl::state_machine.
 */

#include "common_util/statemachine.h"
#include <gtest/gtest.h>

namespace
{

enum class DownloadState
{
    Idle,
    Downloading,
    Done,
    Failed,
};

}

TEST(StateMachineTest, AllowedTransitionsApply)
{
    cutl::state_machine<DownloadState> sm(DownloadState::Idle);
    sm.add_roadmap(DownloadState::Idle, DownloadState::Downloading);
    sm.add_roadmap(DownloadState::Downloading, DownloadState::Done);
    sm.add_roadmap(DownloadState::Downloading, DownloadState::Failed);

    EXPECT_EQ(sm.get_state(), DownloadState::Idle);
    EXPECT_TRUE(sm.set_state(DownloadState::Downloading));
    EXPECT_EQ(sm.get_state(), DownloadState::Downloading);
    EXPECT_TRUE(sm.set_state(DownloadState::Done));
    EXPECT_EQ(sm.get_state(), DownloadState::Done);
}

TEST(StateMachineTest, DisallowedTransitionsRejected)
{
    cutl::state_machine<DownloadState> sm(DownloadState::Idle);
    sm.add_roadmap(DownloadState::Idle, DownloadState::Downloading);

    EXPECT_FALSE(sm.set_state(DownloadState::Done)); // 没注册路径
    EXPECT_EQ(sm.get_state(), DownloadState::Idle);
}

TEST(StateMachineTest, OnChangeCallback)
{
    int success = 0;
    int failure = 0;
    auto cb = [&](const DownloadState&, const DownloadState&, bool ok) {
        if (ok) ++success;
        else ++failure;
    };
    cutl::state_machine<DownloadState> sm(DownloadState::Idle, cb);
    sm.add_roadmap(DownloadState::Idle, DownloadState::Downloading);

    sm.set_state(DownloadState::Downloading); // OK
    sm.set_state(DownloadState::Done);        // FAIL
    EXPECT_EQ(success, 1);
    EXPECT_EQ(failure, 1);
}

TEST(StateMachineTest, ResetSkipsRoadmapCheck)
{
    cutl::state_machine<DownloadState> sm(DownloadState::Idle);
    sm.reset(DownloadState::Done);
    EXPECT_EQ(sm.get_state(), DownloadState::Done);
}
