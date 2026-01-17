/*
    Author: Hanna Biegacz

    Implementation of a user command.
*/

#include "commands/RotateCommand.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

using namespace std;

RotateCommand::RotateCommand(short angle) : angle_left_(angle) {}

// Rotates the mower by a specified angle over multiple frames.
// Uses an accumulator to handle smooth sub-degree rotation.
bool RotateCommand::execute(StateSimulation& sim, double dt) {
    if (isRotationFinished()) {
        return true;
    }

    double step = calculateRotationStepForFrame(dt);
    updateInternalRotationState(step);
    applyAccumulatedRotationToSimulation(sim);
    return isRotationFinished();
}

double RotateCommand::calculateRotationStepForFrame(double dt) const {
    double max_rot_speed = static_cast<double>(Constants::ROTATION_SPEED);
    double max_step = max_rot_speed * dt;

    if (angle_left_ > 0) {
        return min(max_step, static_cast<double>(angle_left_));
    } else {
        return max(-max_step, static_cast<double>(angle_left_));
    }
}

void RotateCommand::updateInternalRotationState(double step) {
    rotation_accumulator_ += step;

    if (step > 0) {
        angle_left_ -= static_cast<short>(floor(step + 0.5)); // Zaokrąglanie
    } else {
        angle_left_ -= static_cast<short>(ceil(step - 0.5));
    }
}

void RotateCommand::applyAccumulatedRotationToSimulation(StateSimulation& sim) {
    if (abs(rotation_accumulator_) >= 1.0) {
        short actual_rot_to_apply = static_cast<short>(rotation_accumulator_);
        
        sim.simulateRotation(actual_rot_to_apply);
        
        rotation_accumulator_ -= actual_rot_to_apply;
    }
}

bool RotateCommand::isRotationFinished() const {
    return angle_left_ == 0 && abs(rotation_accumulator_) < 0.5;
}
