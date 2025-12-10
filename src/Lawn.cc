/* 
    Author: Maciej Cieslik
    
    Describes Lawn, on which mower is cutting grass.
*/

#include <iostream>
#include <cmath>
#include "../include/Lawn.h"
#include "../include/Config.h"

using namespace std;


Lawn::Lawn(const unsigned int& lawn_width, const unsigned int& lawn_length)
    : width_(lawn_width), length_(lawn_length)
    {
        Config::initializeRuntimeConstants(width_, length_);
        fields_.resize(Config::VERTICAL_FIELDS_NUMBER, std::vector<bool>(Config::HORIZONTAL_FIELDS_NUMBER, false));
        std::cout << "Lawn initialized: " << Config::HORIZONTAL_FIELDS_NUMBER << "x" << Config::VERTICAL_FIELDS_NUMBER << std::endl;
    }

unsigned int Lawn::getWidth() {
    return width_;
}


unsigned int Lawn::getLength() {
    return length_;
}


std::vector<std::vector<bool>> Lawn::getFields() {
    return fields_;
}


bool Lawn::isPointInLawn(const double& x, const double& y) {
    bool is_point_in_lawn_vertical = Lawn::countIfCoordInSection(length_, y);
    bool is_point_in_lawn_horizontal = Lawn::countIfCoordInSection(width_, x);

    return is_point_in_lawn_vertical && is_point_in_lawn_horizontal;
}


bool Lawn::countIfCoordInSection(const unsigned int& section_length,
        const double& coord_value) {
    return coord_value <= static_cast<double>(section_length);
}


pair<unsigned int, unsigned int> Lawn::calculateFieldIndexes(const double& x, const double& y) {
    unsigned int x_index = Lawn::calculateIndexInSection(width_, x, fields_[0].size());
    unsigned int y_index = Lawn::calculateIndexInSection(length_, y, fields_.size());

    pair<unsigned int, unsigned int> field_indexes = pair<unsigned int, unsigned int>(x_index, y_index);

    return field_indexes;
}


unsigned int Lawn::calculateIndexInSection(const unsigned int& section_length, const double& coord_value, 
        const unsigned int& vector_size) {
    unsigned int INDEX_OFFSET = 1;

    unsigned int index = static_cast<unsigned int>(ceil(coord_value / Config::FIELD_WIDTH));
    if (index != 0) {
        index -= INDEX_OFFSET;
    }

    return index;
}


void Lawn::cutGrassOnField(const pair<unsigned int, unsigned int>& indexes) {
    fields_[indexes.second][indexes.first] = true;
}


double Lawn::calculateShavedArea() {
    unsigned int allFieldsNumber = Config::HORIZONTAL_FIELDS_NUMBER * Config::VERTICAL_FIELDS_NUMBER;
    unsigned int shavedFieldsNumber = 0;

    for (std::vector<bool> shavedRow : getFields()) {
        for (bool field : shavedRow) {
            if (field) shavedFieldsNumber ++;
        }
    }

    return static_cast<double>(shavedFieldsNumber) / static_cast<double>(allFieldsNumber);
}


void Lawn::cutGrass(const pair<double, double>& blade_middle, const unsigned int& blade_diameter) {
    pair<unsigned int, unsigned int> first_indexes = calculateFieldIndexes(blade_middle.first - blade_diameter / 2,
        blade_middle.second - blade_diameter / 2);
    double beginning_x = static_cast<double>(first_indexes.first) * Config::FIELD_WIDTH;
    double beginning_y = static_cast<double>(first_indexes.second) * Config::FIELD_WIDTH;
    double ending_x = max(beginning_x + blade_diameter, static_cast<double>(width_));
    double ending_y = max(beginning_y + blade_diameter, static_cast<double>(length_));
    double current_y = beginning_y;
    double current_x = beginning_x;

    while (current_y < ending_y) {
        while (current_x < ending_x) {
            if (isFieldInMowingArea(current_x, current_y, blade_middle, blade_diameter)) {
                pair<unsigned int, unsigned int> indexes = calculateFieldIndexes(current_x, current_y);
                cutGrassOnField(indexes);
            }
            current_x = current_x + Config::FIELD_WIDTH;
        }
        current_y = current_y + Config::FIELD_WIDTH;
        current_x = beginning_x;
    }
}


bool Lawn::isFieldInMowingArea(const double& x, const double& y, const std::pair<double, double>& blade_middle, 
        const double& blade_diameter) {
    unsigned int counter = countCornersInArea(x, y, blade_middle, blade_diameter);
    
    if (counter > 2) {
        return true;
    }
    else if (counter == 2) {
        return calculateDistanceBetweenPoints(x + Config::FIELD_WIDTH / 2, y + Config::FIELD_WIDTH / 2,
            blade_middle) <= blade_diameter;
    }
    else {
        return false;
    }
}


double Lawn::calculateDistanceBetweenPoints(const double& x, const double& y, 
    const std::pair<double, double>& destination_point) {
    double dx = x - destination_point.first;
    double dy = y - destination_point.second;

    return std::sqrt(dx*dx + dy*dy);
}


unsigned int Lawn::countCornersInArea(const double& x, const double& y, const std::pair<double, double>& blade_middle, 
        const double& blade_diameter) {
    pair<double, double> points[4] = {
        {x, y},
        {x + Config::FIELD_WIDTH, y},
        {x + Config::FIELD_WIDTH, y + Config::FIELD_WIDTH},
        {x, y + Config::FIELD_WIDTH}
    };

    unsigned int counter = 0;
    for (pair<double, double> point : points) {
        if (calculateDistanceBetweenPoints(x, y, blade_middle) <= blade_diameter) {
            counter ++;
        }
    }

    return counter;
}
