#include "../include/Commands.h"
#include "../include/Constants.h"
#include <cmath>
#include <algorithm>

using namespace std;

MoveCommand::MoveCommand(double distance) 
    : distance_left_(distance), initialized_(true) {}

MoveCommand::MoveCommand(const double* distance_ptr, double scale)
    : distance_left_(0.0), deferred_distance_(distance_ptr), scale_(scale), initialized_(false) {}


// Moves the mower forward by a certain distance. Supports deferred initialization
// where the actual distance is calculated from a pointer value on first execution. 
// Example of a situation where this is useful is in main where we first calculate
// the total distance of the path and then pass a pointer to it to the MoveCommand.
bool MoveCommand::execute(StateSimulation& sim, double dt) {
    if (!initialized_) {
        if (deferred_distance_) {
            distance_left_ = (*deferred_distance_) * scale_;
            sim.getFileLogger().saveMessage("MoveCommand init. Deferred val: " + to_string(*deferred_distance_) + ", Scale: " + to_string(scale_) + ", DistLeft: " + to_string(distance_left_));
        } else {
             sim.getFileLogger().saveMessage("MoveCommand init. No deferred pointer. DistLeft: " + to_string(distance_left_));
        }
        initialized_ = true;
    }

    if (distance_left_ <= 0) return true;

    double speed = sim.getMower().getSpeed();
    double step = speed * dt;
    double actual_step = min(step, distance_left_);

    sim.simulateMovement(actual_step);
    distance_left_ -= actual_step;

    return distance_left_ <= Constants::DISTANCE_PRECISION;
}




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





MowingOptionCommand::MowingOptionCommand(bool enable) : enable_(enable) {}

bool MowingOptionCommand::execute(StateSimulation& sim, double dt) {
    if (enable_) {
        sim.simulateMowingOptionOn();
    } else {
        sim.simulateMowingOptionOff();
    }
    return true; 
}





AddPointCommand::AddPointCommand(double x, double y) : x_(x), y_(y) {}

bool AddPointCommand::execute(StateSimulation& sim, double dt) {
    sim.simulateAddPoint(x_, y_);
    return true;
}





DeletePointCommand::DeletePointCommand(unsigned int id) : id_(id) {}

bool DeletePointCommand::execute(StateSimulation& sim, double dt) {
    sim.simulateDeletePoint(id_);
    return true;
}






MoveToPointCommand::MoveToPointCommand(unsigned int pointId) 
    : point_id_(pointId) 
{}

// Navigates the mower to a specific point. Handles rotation towards the target
// and movement in that direction. Runs over multiple frames until arrival.
bool MoveToPointCommand::execute(StateSimulation& sim, double dt) {
    if (!initialized_) {
        if (!initializeTarget(sim)) {
            return true; 
        }
    }

    double distance = calculateDistanceToTarget(sim);
    if (hasArrivedAtTarget(sim, distance)) {
        return true; 
    }

    auto nav = sim.calculateNavigationVector(target_x_, target_y_);
    short rotation_needed = nav.first;

    if (rotation_needed != 0) {
        executeRotationLogic(sim, dt, rotation_needed);
    }

    if (!isAlignedWithTarget(rotation_needed, distance)) {
        return false; 
    }

    executeMovementLogic(sim, dt, distance);
    return false;
}

// Retrieves target point coordinates on first execution.
bool MoveToPointCommand::initializeTarget(StateSimulation& sim) {
    auto coords = sim.getPointCoordinates(point_id_);
    if (!coords) {
        return false;
    }
    target_x_ = coords->first;
    target_y_ = coords->second;
    initialized_ = true;
    return true;
}

double MoveToPointCommand::calculateDistanceToTarget(const StateSimulation& sim) const {
    double current_x = sim.getMower().getX();
    double current_y = sim.getMower().getY();
    double dx = target_x_ - current_x;
    double dy = target_y_ - current_y;
    return sqrt(dx*dx + dy*dy);
}

bool MoveToPointCommand::hasArrivedAtTarget(StateSimulation& sim, double currentDistance) const {
    if (currentDistance <= 3.0) {
        sim.logArrivalAtPoint(point_id_);
        return true;
    }
    return false;
}

void MoveToPointCommand::executeRotationLogic(StateSimulation& sim, double dt, short rotationNeeded) {
    double rot_speed = static_cast<double>(Constants::ROTATION_SPEED);
    double max_step = rot_speed * dt;

    double step = (rotationNeeded > 0) 
        ? min((double)rotationNeeded, max_step)
        : max((double)rotationNeeded, -max_step);
    
    rotation_accumulator_ += step;

    applyAccumulatedRotation(sim);
}

void MoveToPointCommand::applyAccumulatedRotation(StateSimulation& sim) {
    if (abs(rotation_accumulator_) >= 1.0) {
        short actual_rot = static_cast<short>(rotation_accumulator_);
        sim.simulateRotation(actual_rot);
        rotation_accumulator_ -= actual_rot;
    }
}

// Checks if the mower is facing the target within acceptable tolerance.
// Uses tighter tolerance when close to the target for precision.
bool MoveToPointCommand::isAlignedWithTarget(short rotationNeeded, double distanceToTarget) const {
    const double CLOSE_RANGE_THRESHOLD = 20.0;
    const short TIGHT_ANGLE_TOLERANCE = 2;
    const short LOOSE_ANGLE_TOLERANCE = 10;
    
    bool is_close_range = (distanceToTarget < CLOSE_RANGE_THRESHOLD);
    short angle_tolerance = is_close_range ? TIGHT_ANGLE_TOLERANCE : LOOSE_ANGLE_TOLERANCE;

    return abs(rotationNeeded) <= angle_tolerance;
}

void MoveToPointCommand::executeMovementLogic(StateSimulation& sim, double dt, double distanceToTarget) {
    double speed = sim.getMower().getSpeed();
    double move_step = speed * dt;
    double move_dist = min(distanceToTarget, move_step);
    
    sim.simulateMovement(move_dist);
}





GetDistanceToPointCommand::GetDistanceToPointCommand(unsigned int pointId, double& outDistance)
    : point_id_(pointId), out_distance_(outDistance) {}

// Calculates the distance from the mower to a specific point and stores it
// in the output reference variable (completes in one frame).
bool GetDistanceToPointCommand::execute(StateSimulation& sim, double dt) {
    auto coords = sim.getPointCoordinates(point_id_);
    if (!coords) {
        logPointNotFoundError(sim);
        return true; 
    }

    double distance = calculateDistanceFromMowerToPoint(sim, coords->first, coords->second);
    out_distance_ = distance;
    logDistanceResult(sim, distance);

    return true;
}

void GetDistanceToPointCommand::logPointNotFoundError(StateSimulation& sim) const {
    std::string msg = "Error: Point " + std::to_string(point_id_) + " not found for GetDistanceToPoint.";
    sim.getFileLogger().saveMessage(msg);
}

double GetDistanceToPointCommand::calculateDistanceFromMowerToPoint(const StateSimulation& sim, double target_x, double target_y) const {
    double current_x = sim.getMower().getX();
    double current_y = sim.getMower().getY();
    double dx = target_x - current_x;
    double dy = target_y - current_y;
    return std::sqrt(dx*dx + dy*dy);
}

void GetDistanceToPointCommand::logDistanceResult(StateSimulation& sim, double distance) const {
    std::string msg = "Distance to point " + std::to_string(point_id_) + ": " + std::to_string(distance);
    sim.getFileLogger().saveMessage(msg);
}






RotateTowardsPointCommand::RotateTowardsPointCommand(unsigned int pointId)
    : point_id_(pointId) {}
    
// Rotates the mower to face a specific point. Runs over multiple frames
// until the mower is aligned with the target direction.
bool RotateTowardsPointCommand::execute(StateSimulation& sim, double dt) {
    if (!initialized_) {
        if (!initializeTarget(sim)) {
            return true; 
        }
    }

    auto nav = sim.calculateNavigationVector(target_x_, target_y_);
    short rotation_needed = nav.first;

    if (rotation_needed != 0) {
        executeRotationLogic(sim, dt, rotation_needed);
    }

    if (isAlignedWithTarget(rotation_needed)) {
        return true;
    }

    return false;
}

bool RotateTowardsPointCommand::initializeTarget(StateSimulation& sim) {
    auto coords = sim.getPointCoordinates(point_id_);
    if (!coords) {
        return false;
    }
    target_x_ = coords->first;
    target_y_ = coords->second;
    initialized_ = true;
    return true;
}

void RotateTowardsPointCommand::executeRotationLogic(StateSimulation& sim, double dt, short rotationNeeded) {
    double rot_speed = static_cast<double>(Constants::ROTATION_SPEED);
    double max_step = rot_speed * dt;

    double step = (rotationNeeded > 0) 
        ? min((double)rotationNeeded, max_step)
        : max((double)rotationNeeded, -max_step);
    
    rotation_accumulator_ += step;

    applyAccumulatedRotation(sim);
}

void RotateTowardsPointCommand::applyAccumulatedRotation(StateSimulation& sim) {
    if (abs(rotation_accumulator_) >= 1.0) {
        short actual_rot = static_cast<short>(rotation_accumulator_);
        sim.simulateRotation(actual_rot);
        rotation_accumulator_ -= actual_rot;
    }
}

bool RotateTowardsPointCommand::isAlignedWithTarget(short rotationNeeded) const {
    return abs(rotationNeeded) <= 2;
}