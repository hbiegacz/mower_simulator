/* 
    Author: Hanna Biegacz
    
    // TODO: description
*/
#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

class Lawn;

class SimulationEngine {
public:
    explicit SimulationEngine(Lawn* lawn);
    ~SimulationEngine();
    
    SimulationEngine(const SimulationEngine&) = delete;
    SimulationEngine& operator=(const SimulationEngine&) = delete;
    
    void start();
    void stop();
    void setSimulationSpeed(const double speed_multiplier);
    double getSimulationSpeed() const;
    
    std::mutex& getLawnMutex();
    bool isRunning() const;
    
private:
    void simulationLoop();
    void executeSimulationStep();
    
    Lawn* lawn_;
    std::thread simulation_thread_;
    std::atomic<bool> is_running_;
    std::atomic<double> speed_multiplier_;
    std::mutex lawn_mutex_;
    
    unsigned int current_row_;
    unsigned int current_col_;
    
    static constexpr int TIMESTEP_MS = 10;
};