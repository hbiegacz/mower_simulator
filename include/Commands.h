/*
    Author: Hanna Biegacz

    This file contains command definitions. 
    All commands must implement the execute method defined in the interface ICommand. 
    Which is necessary for implementing user code and defining what happens in the simulation.
*/
#pragma once

#include "StateSimulation.h"

// COMMAND INTERFACE

class ICommand {
public:
    ICommand() {} 
    virtual ~ICommand() = default;
    virtual bool execute(StateSimulation& sim, double dt) = 0;
    ICommand(const ICommand&) = delete;
    ICommand& operator=(const ICommand&) = delete;
};

// SPECIFIC COMMAND DECLARATIONS

class MoveCommand : public ICommand {
public:
    explicit MoveCommand(double distance);
    MoveCommand(const double* distance_ptr, double scale);
    bool execute(StateSimulation& sim, double dt) override;

    MoveCommand(const MoveCommand&) = delete;
    MoveCommand& operator=(const MoveCommand&) = delete;

private:
    double distance_left_;
    const double* deferred_distance_ = nullptr;
    double scale_ = 1.0;
    bool initialized_ = false;
};

class RotateCommand : public ICommand {
public:
    explicit RotateCommand(short angle);
    bool execute(StateSimulation& sim, double dt) override;

    RotateCommand(const RotateCommand&) = delete;
    RotateCommand& operator=(const RotateCommand&) = delete;
private:
    short angle_left_;
    double rotation_accumulator_ = 0.0;

    double calculateRotationStepForFrame(double dt) const;
    void updateInternalRotationState(double step);
    void applyAccumulatedRotationToSimulation(StateSimulation& sim);
    bool isRotationFinished() const;
};

class MowingOptionCommand : public ICommand {
public:
    explicit MowingOptionCommand(bool enable);
    bool execute(StateSimulation& sim, double dt) override;

    MowingOptionCommand(const MowingOptionCommand&) = delete;
    MowingOptionCommand& operator=(const MowingOptionCommand&) = delete;
private:
    bool enable_;
};

class AddPointCommand : public ICommand {
public:
    AddPointCommand(double x, double y);
    bool execute(StateSimulation& sim, double dt) override;

    AddPointCommand(const AddPointCommand&) = delete;
    AddPointCommand& operator=(const AddPointCommand&) = delete;
private:
    double x_;
    double y_;
};

class DeletePointCommand : public ICommand {
public:
    explicit DeletePointCommand(unsigned int id);
    bool execute(StateSimulation& sim, double dt) override;

    DeletePointCommand(const DeletePointCommand&) = delete;
    DeletePointCommand& operator=(const DeletePointCommand&) = delete;
private:
    unsigned int id_;
};

class MoveToPointCommand : public ICommand {
public:
    explicit MoveToPointCommand(unsigned int pointId);
    bool execute(StateSimulation& sim, double dt) override;

    MoveToPointCommand(const MoveToPointCommand&) = delete;
    MoveToPointCommand& operator=(const MoveToPointCommand&) = delete;
private:
    unsigned int point_id_;
        
    bool initialized_ = false;
    double target_x_ = 0.0;
    double target_y_ = 0.0;
    double rotation_accumulator_ = 0.0;

    bool initializeTarget(StateSimulation& sim);
    void applyAccumulatedRotation(StateSimulation& sim);
    void executeRotationLogic(StateSimulation& sim, double dt, short rotationNeeded);
    void executeMovementLogic(StateSimulation& sim, double dt, double distanceToTarget);

    double calculateDistanceToTarget(const StateSimulation& sim) const;
    bool hasArrivedAtTarget(StateSimulation& sim, double currentDistance) const;
    bool isAlignedWithTarget(short rotationNeeded, double distanceToTarget) const;
};

class GetDistanceToPointCommand : public ICommand {
public:
    GetDistanceToPointCommand(unsigned int pointId, double& outDistance);
    bool execute(StateSimulation& sim, double dt) override;

    GetDistanceToPointCommand(const GetDistanceToPointCommand&) = delete;
    GetDistanceToPointCommand& operator=(const GetDistanceToPointCommand&) = delete;
private:
    unsigned int point_id_;
    double& out_distance_;
    
    void logPointNotFoundError(StateSimulation& sim) const;
    double calculateDistanceFromMowerToPoint(const StateSimulation& sim, double target_x, double target_y) const;
    void logDistanceResult(StateSimulation& sim, double distance) const;
};


class RotateTowardsPointCommand : public ICommand {
public:
    explicit RotateTowardsPointCommand(unsigned int pointId);
    bool execute(StateSimulation& sim, double dt) override;

    RotateTowardsPointCommand(const RotateTowardsPointCommand&) = delete;
    RotateTowardsPointCommand& operator=(const RotateTowardsPointCommand&) = delete;
private:
    unsigned int point_id_;
    bool initialized_ = false;
    double target_x_ = 0.0;
    double target_y_ = 0.0;
    double rotation_accumulator_ = 0.0;

    bool initializeTarget(StateSimulation& sim);
    void executeRotationLogic(StateSimulation& sim, double dt, short rotationNeeded);
    void applyAccumulatedRotation(StateSimulation& sim);
    bool isAlignedWithTarget(short rotationNeeded) const;
};
