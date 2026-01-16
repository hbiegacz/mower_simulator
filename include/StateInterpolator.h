/*
    Author: Hanna Biegacz
    Manages simulation state interpolation for smooth rendering.
*/

#pragma once

#include <atomic>
#include <mutex>
#include <deque>
#include "SimulationSnapshot.h"

class StateInterpolator {
public:
    void addSimulationSnapshot( const SimulationSnapshot& sim_snapshot );

    SimulationSnapshot getInterpolatedState( double render_time ) const;

    double getSimulationTime() const;
    double getSpeedMultiplier() const;
    void setSimulationSpeedMultiplier(double speed_multiplier);
private:
    bool isSnapshotOutdatedOrDuplicate( const SimulationSnapshot& snapshot ) const;

    void storeSnapshot( const SimulationSnapshot& snapshot );
    void enforceBufferSizeLimit();

    bool shouldReturnEarliestSnapshot( double render_time ) const;
    bool shouldReturnLatestSnapshot( double render_time ) const;
    SimulationSnapshot computeInterpolatedSnapshot( double render_time ) const;
    
    std::deque<SimulationSnapshot>::const_iterator findFirstSnapshotAfter( double time ) const;
    
    double calculateInterpolationAlpha( const SimulationSnapshot& before, const SimulationSnapshot& after, double render_time ) const;
    
    SimulationSnapshot blendSnapshots( const SimulationSnapshot& start, const SimulationSnapshot& end, double alpha, double render_time ) const;

    static double interpolate( double a, double b, double alpha );
    static double interpolateAngle( double start_angle, double end_angle, double alpha );

    std::deque<SimulationSnapshot> sim_snapshot_buffer_;
    static const size_t MAX_BUFFER_SIZE = 50;
    
    mutable std::mutex mutex_;
    std::atomic<double> current_speed_multiplier_{1.0};
};
