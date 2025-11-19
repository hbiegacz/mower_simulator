/* 
    Author: Maciej Cieslik
    
    Describes Lawn, on which mover is cutting grass.
*/

#include "../include/Lawn.h"

using namespace std;


Lawn::Lawn(const unsigned int& lawn_width, const unsigned int& lawn_length)
    : width_(lawn_width), length_(lawn_length), fields_(lawn_width, std::vector<bool>(lawn_length, false)) {}


unsigned int Lawn::getWidth() {
    return width_;
}


unsigned int Lawn::getLength(){
    return length_;
}
