/* 
    Author: Maciej Cieslik
    
    Tests Lawn class methods.
*/

#include <gtest/gtest.h>
#include "../include/Lawn.h"


TEST(Getters, Getters) {
    unsigned int lawn_width = 100;
    unsigned int lawn_length = 100;
    Lawn lawn = Lawn(lawn_width, lawn_length);

    unsigned int width = lawn.getWidth();
    unsigned int length = lawn.getWidth();

    EXPECT_EQ(width, lawn_width);
    EXPECT_EQ(length, lawn_length);
}


TEST(IsPointInLawn, isPointInLawnCorrect) {
    unsigned int lawn_width = 100;
    unsigned int lawn_length = 100;
    Lawn lawn = Lawn(lawn_width, lawn_length);
    double x = 0.5;
    double y = 0.3;

    bool result = lawn.isPointInLawn(x, y);

    EXPECT_EQ(true, result);
}

TEST(IsPointInLawn, isPointInLawnCorrectMinimalValues) {
    unsigned int lawn_width = 100;
    unsigned int lawn_length = 100;
    Lawn lawn = Lawn(lawn_width, lawn_length);
    double x = 0.0;
    double y = 0.0;

    bool result = lawn.isPointInLawn(x, y);

    EXPECT_EQ(true, result);
}


TEST(IsPointInLawn, isPointInLawnCorrectMaximalValues) {
    unsigned int lawn_width = 100000;
    unsigned int lawn_length = 100000;
    Lawn lawn = Lawn(lawn_width, lawn_length);
    double x = 100000.0;
    double y = 100000.0;

    bool result = lawn.isPointInLawn(x, y);

    EXPECT_EQ(true, result);
}


TEST(IsPointInLawn, isPointInLawnIncorrectSquareLawn) {
    unsigned int lawn_width = 60000;
    unsigned int lawn_length = 60000;
    Lawn lawn = Lawn(lawn_width, lawn_length);
    double x = 60000.1;
    double y = 50000.1;

    bool result = lawn.isPointInLawn(x, y);

    EXPECT_EQ(false, result);
}


TEST(IsPointInLawn, isPointInLawnIncorrectRectangularLawn) {
    unsigned int lawn_width = 60000;
    unsigned int lawn_length = 6000;
    Lawn lawn = Lawn(lawn_width, lawn_length);
    double x = 50000.1;
    double y = 6000.1;

    bool result = lawn.isPointInLawn(x, y);

    EXPECT_EQ(false, result);
}
