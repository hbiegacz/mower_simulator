#include "LawnVisualizer.h"
#include "Lawn.h"
#include <QPainter>
#include <QPaintEvent>
#include <iostream>

const QColor LawnVisualizer::UNMOWED_GRASS_COLOR = QColor(34, 139, 34);
const QColor LawnVisualizer::MOWED_GRASS_COLOR = QColor(139, 90, 43);
const QColor LawnVisualizer::GRID_LINE_COLOR = QColor(200, 200, 200, 100);

LawnVisualizer::LawnVisualizer(Lawn* lawn, QWidget* parent)
    : QWidget(parent), lawn_(lawn) {
    
    if (!lawn_) {
        std::cerr << "[LawnVisualizer] ERROR: Lawn pointer is null!" << std::endl;
        throw std::invalid_argument("Lawn pointer cannot be null");
    }
    
              << lawn_->getWidth() << "cm x " 
              << lawn_->getLength() << "cm" << std::endl;
    
    auto fields = lawn_->getFields();
              << fields[0].size() << " cols x " 
              << fields.size() << " rows" << std::endl;
              << (fields[0].size() * fields.size()) << std::endl;
    
    setMinimumSize(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
    resize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    
              << DEFAULT_WINDOW_WIDTH << "x" << DEFAULT_WINDOW_HEIGHT << std::endl;
}

void LawnVisualizer::refresh() {
    update();
}

QSize LawnVisualizer::sizeHint() const {
    return QSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
}

QSize LawnVisualizer::minimumSizeHint() const {
    return QSize(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
}

void LawnVisualizer::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    
    drawLawnGrid(painter);
}

void LawnVisualizer::drawLawnGrid(QPainter& painter) {
    auto fields = lawn_->getFields();
    
    unsigned int num_rows = fields.size();
    unsigned int num_cols = fields[0].size();
    
    double field_width_px = static_cast<double>(width()) / num_cols;
    double field_height_px = static_cast<double>(height()) / num_rows;
    
    bool draw_grid_lines = (field_width_px >= 2.0 && field_height_px >= 2.0);
    unsigned int mowed_count = 0;
    unsigned int unmowed_count = 0;
    
    for (unsigned int row = 0; row < num_rows; ++row) {
        for (unsigned int col = 0; col < num_cols; ++col) {
            bool is_mowed = fields[row][col];
            
            double x = col * field_width_px;
            double y = height() - (row + 1) * field_height_px;
            
            QColor fill_color = is_mowed ? MOWED_GRASS_COLOR : UNMOWED_GRASS_COLOR;
            
            painter.fillRect(QRectF(x, y, field_width_px, field_height_px), fill_color);
            
            if (draw_grid_lines) {
                painter.setPen(GRID_LINE_COLOR);
                painter.drawRect(QRectF(x, y, field_width_px, field_height_px));
            }
            
            if (is_mowed) {
                mowed_count++;
            } else {
                unmowed_count++;
            }
        }
    }
}

void LawnVisualizer::drawFieldAt(QPainter& painter, unsigned int row, unsigned int col, bool is_mowed) {
    double field_width_px = calculateFieldPixelWidth();
    double field_height_px = calculateFieldPixelHeight();
    
    double x = col * field_width_px;
    double y = height() - (row + 1) * field_height_px;
    
    QColor fill_color = is_mowed ? MOWED_GRASS_COLOR : UNMOWED_GRASS_COLOR;
    
    painter.fillRect(QRectF(x, y, field_width_px, field_height_px), fill_color);
    painter.setPen(GRID_LINE_COLOR);
    painter.drawRect(QRectF(x, y, field_width_px, field_height_px));
}

double LawnVisualizer::calculateFieldPixelWidth() const {
    auto fields = lawn_->getFields();
    if (fields.empty() || fields[0].empty()) {
        return 0.0;
    }
    
    unsigned int num_cols = fields[0].size();
    return static_cast<double>(width()) / num_cols;
}

double LawnVisualizer::calculateFieldPixelHeight() const {
    auto fields = lawn_->getFields();
    if (fields.empty()) {
        return 0.0;
    }
    
    unsigned int num_rows = fields.size();
    return static_cast<double>(height()) / num_rows;
}
