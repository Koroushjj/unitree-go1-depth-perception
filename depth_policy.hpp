// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <cmath>
#include <stdexcept>
#include <string>

namespace perception {
enum class State { Unknown, Obstacle, BeyondThreshold };

inline double positiveNumber(const std::string& text) {
    std::size_t consumed = 0;
    const double value = std::stod(text, &consumed);
    if (consumed != text.size() || !std::isfinite(value) || value <= 0.0)
        throw std::invalid_argument("Expected a finite positive number");
    return value;
}

inline State classify(double raw, double metersPerUnit, double threshold,
                      double minimum = 0.2, double maximum = 4.0) {
    if (!std::isfinite(raw) || !std::isfinite(metersPerUnit) || metersPerUnit <= 0.0 ||
        !std::isfinite(threshold) || !std::isfinite(minimum) || !std::isfinite(maximum) ||
        minimum <= 0.0 || maximum <= minimum || threshold <= minimum || threshold > maximum)
        return State::Unknown;
    const double meters = raw * metersPerUnit;
    if (!std::isfinite(meters) || meters < minimum || meters > maximum)
        return State::Unknown;
    return meters < threshold ? State::Obstacle : State::BeyondThreshold;
}

inline const char* label(State state) {
    switch (state) {
        case State::Obstacle: return "OBSTACLE AT CENTER";
        case State::BeyondThreshold: return "CENTER BEYOND THRESHOLD";
        default: return "UNKNOWN DEPTH";
    }
}
} // namespace perception
