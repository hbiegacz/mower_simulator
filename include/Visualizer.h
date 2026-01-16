/* 
    Author: Hanna Biegacz
    
    This class handles the visualization layer using Qt. It reads the 
    current state from the StateInterpolator (read-only) and draws the lawn, mower and points
    on the screen. 

    It calculates coordinate scaling to fit the simulation world into the window 
    and make sure that all objects are proportionally scaled.
*/

#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPixmap>
#include <vector>
#include "StateInterpolator.h"
#include "SimulationSnapshot.h"

class Visualizer : public QWidget {
    Q_OBJECT

public:
    explicit Visualizer(StateInterpolator& render_context, QWidget* parent = nullptr);
    ~Visualizer() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    static const int DEFAULT_WINDOW_WIDTH = 800;
    static const int DEFAULT_WINDOW_HEIGHT = 600;
    static const int MIN_WINDOW_WIDTH = 400;
    static const int MIN_WINDOW_HEIGHT = 300;
    
    static const QColor UNMOWED_GRASS_COLOR;
    static const QColor MOWED_GRASS_COLOR;

    static constexpr double BASE_BUFFER_DELAY_MS = 200.0;  
    static constexpr double MAX_TIME_DRIFT_MS = 2000.0;    
    static constexpr double DRIFT_CORRECTION_FACTOR = 0.1; 

    StateInterpolator& state_interpolator_;
    SimulationSnapshot current_sim_snapshot_;
    
    QPixmap mower_image_;
    std::vector<QPixmap> point_pixmaps_;

    QElapsedTimer frame_timer_;
    double smoothed_render_time_ = 0.0;

    double scale_factor_ = 1.0;
    double lawn_length_cm_ = 0.0;
    QPointF map_offset_;

    void updateSmoothedRenderTime();
    void refreshStateAndLayout();
    
    double calculateIdealRenderTime(double actual_sim_time, double speed_multiplier) const;
    void advanceRenderTime(double dt_ms, double speed_multiplier);
    void syncWithSimulationClock(double actual_sim_time, double speed_multiplier);
    void applySoftTimeCorrection(double ideal_time);
    void performHardTimeReset(double ideal_time);
    bool hasSignificantTimeDrift(double target_render_time) const; // Możesz to usunąć jeśli używasz nowej logiki w syncWithSimulationClock, ale zostawiam dla kompatybilności wstecznej jeśli potrzebujesz

    void setupPainter(QPainter& painter);
    void updateLayout();
    void loadMowerImage();
    void loadPointImages();
    
    void renderLawn(QPainter& painter);
    void renderMower(QPainter& painter, const SimulationSnapshot& sim_snapshot);
    void renderPoints(QPainter& painter);
    
    QPointF mapToScreen(double x_cm, double y_cm);
    bool hasValidLawnDimensions() const;
    bool isLawnDataEmpty() const;
    void calculateMowerRenderSize(double mower_width, double mower_length, double blade_diameter, double& out_w_px, double& out_h_px) const;
};