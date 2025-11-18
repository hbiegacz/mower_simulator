/* 
    Author: Maciej Cieslik
    
    Declares all global constants that are initialized at runtime.
    These values are computed once at program start and accessible throughout the program.
*/

#pragma once

namespace Config {
    extern unsigned int MAX_BLADE_DIAMETER; // cm
    extern unsigned int MIN_BLADE_DIAMETER; // cm
    extern unsigned int BLADE_DIAMETER; // cm
    extern unsigned int MAX_MOVER_WIDTH; // cm
    extern unsigned int MIN_MOVER_WIDTH; // cm
    extern unsigned int MAX_MOVER_LENGTH; // cm
    extern unsigned int MIN_MOVER_LENGTH; // cm
    extern unsigned int MOVER_WIDTH; // cm
    extern unsigned int MOVER_LENGTH; // cm
    
    void initializeRuntimeConstants(const unsigned int& lawn_width, const unsigned int& lawn_length, 
        const unsigned int& mover_width, const unsigned int& mover_length, const unsigned int& blade_diameter);
}