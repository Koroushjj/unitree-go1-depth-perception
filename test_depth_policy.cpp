// SPDX-License-Identifier: MPL-2.0
#include "depth_policy.hpp"
#include <iostream>
#include <limits>

int main() {
    using perception::State;
    int checks = 0, failures = 0;
    auto check = [&](bool result, const char* name) {
        ++checks;
        if (!result) { ++failures; std::cerr << "FAIL: " << name << '\n'; }
    };
    check(perception::classify(0.5, 1, 1) == State::Obstacle, "float meters");
    check(perception::classify(500, 0.001, 1) == State::Obstacle, "millimeter conversion");
    check(perception::classify(1, 1, 1) == State::BeyondThreshold, "threshold equality");
    check(perception::classify(0, 1, 1) == State::Unknown, "zero depth");
    check(perception::classify(-1, 1, 1) == State::Unknown, "negative depth");
    check(perception::classify(2.28772e-05, 1, 1) == State::Unknown, "observed tiny depth");
    check(perception::classify(std::numeric_limits<double>::quiet_NaN(), 1, 1) == State::Unknown, "NaN");
    check(perception::classify(std::numeric_limits<double>::infinity(), 1, 1) == State::Unknown, "infinity");
    check(perception::classify(5, 1, 1) == State::Unknown, "beyond display range");
    check(perception::classify(0.2, 1, 1) == State::Obstacle, "minimum boundary");
    check(perception::classify(4, 1, 1) == State::BeyondThreshold, "maximum boundary");
    check(perception::classify(1, 0, 1) == State::Unknown, "invalid scale");
    check(perception::classify(1, 1, 0.1) == State::Unknown, "invalid threshold");
    check(perception::classify(1e308, 1e308, 1) == State::Unknown, "overflow");
    check(perception::positiveNumber("0.001") == 0.001, "parse scale");
    for (const char* value : {"0", "-1", "nan", "inf", "1garbage", ""}) {
        bool rejected = false;
        try { (void)perception::positiveNumber(value); } catch (const std::exception&) { rejected = true; }
        check(rejected, value);
    }
    std::cout << checks << " checks, " << failures << " failures\n";
    return failures == 0 ? 0 : 1;
}
