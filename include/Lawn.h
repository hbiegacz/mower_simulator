/* 
    Author: Maciej Cieslik
    
    Describes Lawn, on which mover is cutting grass. Lawn consists of fields, which are repesented by 
    bool element in 2-dimensional vector. False meaning the grass is not cut, true meaning the grass is cut. 
*/

#include <vector>

class Lawn {
private:
    unsigned int width_;
    unsigned int length_;
    std::vector<std::vector<bool>> fields_;

public:
    Lawn(const unsigned int& lawn_width, const unsigned int& lawn_length);
    Lawn(const Lawn&) = delete;
    Lawn& operator=(const Lawn&) = delete;

    unsigned int getWidth();
    unsigned int getLength();
};
