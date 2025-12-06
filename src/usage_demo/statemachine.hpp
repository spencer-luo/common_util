#include "common.hpp"
#include "common_util/statemachine.h"

enum DownloadState
{
    Idel,
    Accepted,
    Downloading,
    Succeed,
    Failure,
    Canceling,
    Canceled,
};

std::string getDownloadStateName(DownloadState state)
{
    static std::unordered_map<DownloadState, std::string> stateNames = {
        { DownloadState::Idel, "Idel" },
        { DownloadState::Accepted, "Accepted" },
        { DownloadState::Downloading, "Downloading" },
        { DownloadState::Succeed, "Succeed" },
        { DownloadState::Failure, "Failure" },
        { DownloadState::Canceling, "Canceling" },
        { DownloadState::Canceled, "Canceled" },
    };
    if (stateNames.count(state))
    {
        return stateNames.at(state);
    }
    return "";
}

void OnStateChange(const DownloadState& from, const DownloadState& to, bool result)
{
    if (result)
    {
        std::cout << "State from " << getDownloadStateName(from) << " converte to "
                  << getDownloadStateName(to) << std::endl;
    }
    else
    {
        std::cerr << "State Can not from " << getDownloadStateName(from) << " convert to "
                  << getDownloadStateName(to) << std::endl;
    }
}

void TestStateMachine()
{
    cutl::state_machine<DownloadState> state(DownloadState::Idel, OnStateChange);
    state.add_roadmap(DownloadState::Idel, DownloadState::Accepted);
    state.add_roadmap(DownloadState::Accepted, DownloadState::Downloading);
    state.add_roadmap(DownloadState::Accepted, DownloadState::Canceling);
    state.add_roadmap(DownloadState::Downloading, DownloadState::Succeed);
    state.add_roadmap(DownloadState::Downloading, DownloadState::Failure);
    state.add_roadmap(DownloadState::Downloading, DownloadState::Canceling);
    state.add_roadmap(DownloadState::Canceling, DownloadState::Failure);
    state.add_roadmap(DownloadState::Canceling, DownloadState::Canceled);

    std::cout << "init: " << state.get_state() << std::endl;
    std::cout << state.set_state(DownloadState::Accepted) << std::endl;
    std::cout << "cur: " << state.get_state() << std::endl;
    std::cout << state.set_state(DownloadState::Succeed) << std::endl;
    std::cout << "cur: " << state.get_state() << std::endl;
    state.set_state(DownloadState::Downloading);
    state.set_state(DownloadState::Accepted);
}