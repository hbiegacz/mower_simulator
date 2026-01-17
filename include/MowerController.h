/*
    Author: Hanna Biegacz
    This class is responsible for handling user input and executing commands.
    Manages a command queue for the lawn mower.
    Provides simple methods to control the mower (move, rotate, mowing on/off)
    and executes commands sequentially during simulation updates.
*/

#pragma once

#include <queue>
#include <memory>
#include "Commands.h"
#include "StateSimulation.h"

class MowerController {
public:
    MowerController() = default;
    ~MowerController() = default;
    MowerController(const MowerController&) = delete;
    MowerController& operator=(const MowerController&) = delete;

    void move(double cm);
    void move(const double* distance_ptr, double scale = 1.0);
    void rotate(short deg);
    void setMowing(bool enable);
    void addPoint(double x, double y);
    void deletePoint(unsigned int point_id);
    void moveToPoint(unsigned int point_id);
    void getDistanceToPoint(unsigned int point_id, double& out_distance);
    void rotateTowardsPoint(unsigned int point_id);


    void update(StateSimulation& sim, double dt);

private:
    std::queue<std::unique_ptr<ICommand>> command_queue_;
};
