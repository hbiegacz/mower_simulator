/* 
    Author: Hanna Biegacz
    
    Implementation of Point struct for lawn mower simulator. 
    Point represents a marker on the lawn that the mower should reach.
*/

#include "../include/Point.h"
#include <cmath>
#include <iostream>

using namespace std;


unsigned int Point::idCounter_ = 0;


Point::Point() 
    : x_(0.0), y_(0.0), id_(idCounter_++) {}


Point::Point(const double& x, const double& y)
    : x_(x), y_(y), id_(idCounter_++) {}


double Point::getX() const {
    return x_;
}

double Point::getY() const {
    return y_;
}

unsigned int Point::getId() const {
    return id_;
}

void Point::setPosition(const double& x, const double& y) {
    x_ = x;
    y_ = y;
}

double Point::calcDistanceTo(const Point& other) const {
    double dx = x_ - other.x_;
    double dy = y_ - other.y_;
    
    return sqrt(dx * dx + dy * dy);
}
