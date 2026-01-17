#pragma once

#include <queue>
#include <memory>
#include "Commands.h"
#include "StateSimulation.h"

class MowerController {
public:
    MowerController() = default;

    void move(double cm);
    void rotate(short deg);

    void update(StateSimulation& sim, double dt);

    bool hasCommands() const;

private:
    std::queue<std::unique_ptr<ICommand>> command_queue_;
};
