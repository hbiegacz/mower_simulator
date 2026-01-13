#include <gtest/gtest.h>
#include <QApplication>
#include "simulation/LawnSimulationView.h"
#include "StateSimulation.h"
#include "Lawn.h"
#include "Mover.h"
#include "Logger.h"
#include "FileLogger.h"
#include <mutex>
#include <thread>
#include <chrono>

using namespace std;

class LawnSimulationViewTests : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        int argc = 0;
        char** argv = nullptr;
        app_ = new QApplication(argc, argv);
    }
    
    static void TearDownTestSuite() {
        delete app_;
        app_ = nullptr;
    }
    
    void SetUp() override {
        // Create dependencies
        lawn_ = new Lawn(100, 80);
        mover_ = new Mover(30, 40, 15, 20);
        logger_ = new Logger();
        fileLogger_ = new FileLogger("test_view.log");
        
        simulation_ = new StateSimulation(*lawn_, *mover_, *logger_, *fileLogger_);
        simulation_mutex_ = new mutex();
        
        view_ = new LawnSimulationView(*simulation_, *simulation_mutex_);
    }
    
    void TearDown() override {
        delete view_;
        delete simulation_mutex_;
        delete simulation_;
        delete fileLogger_;
        delete logger_;
        delete mover_;
        delete lawn_;
    }
    
    static QApplication* app_;
    Lawn* lawn_;
    Mover* mover_;
    Logger* logger_;
    FileLogger* fileLogger_;
    StateSimulation* simulation_;
    mutex* simulation_mutex_;
    LawnSimulationView* view_;
};

QApplication* LawnSimulationViewTests::app_ = nullptr;

TEST_F(LawnSimulationViewTests, ConstructorCreatesValidWidget) {
    EXPECT_NE(nullptr, view_);
}

TEST_F(LawnSimulationViewTests, DefaultWindowSize) {
    constexpr int EXPECTED_WIDTH = 800;
    constexpr int EXPECTED_HEIGHT = 600;
    
    QSize hint = view_->sizeHint();
    
    EXPECT_EQ(EXPECTED_WIDTH, hint.width());
    EXPECT_EQ(EXPECTED_HEIGHT, hint.height());
}

TEST_F(LawnSimulationViewTests, MinimumWindowSize) {
    constexpr int EXPECTED_MIN_WIDTH = 400;
    constexpr int EXPECTED_MIN_HEIGHT = 300;
    
    QSize min_hint = view_->minimumSizeHint();
    
    EXPECT_EQ(EXPECTED_MIN_WIDTH, min_hint.width());
    EXPECT_EQ(EXPECTED_MIN_HEIGHT, min_hint.height());
}

TEST_F(LawnSimulationViewTests, TriggerRepaintDoesNotCrash) {
    EXPECT_NO_THROW(view_->triggerRepaint());
}

TEST_F(LawnSimulationViewTests, WidgetCanBeShownAndHidden) {
    EXPECT_NO_THROW(view_->show());
    EXPECT_TRUE(view_->isVisible());
    
    EXPECT_NO_THROW(view_->hide());
    EXPECT_FALSE(view_->isVisible());
}
