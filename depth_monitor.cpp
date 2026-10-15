/*
 * SPDX-License-Identifier: MPL-2.0
 * Based on UnitreeCameraSDK example_getDepthFrame.cc (v1.1.0).
 * Original example: SunMingzhe, 2021.12.07.
 * Copyright (c) 2020-2021, Hangzhou Yushu Technology Stock CO.LTD.
 * All Rights Reserved. See LICENSE and THIRD_PARTY.md.
 * Coursework extension: Koroush Mobasserazad. Portfolio revision: 2026-10-15.
 */
#include <UnitreeCameraSDK.hpp>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <thread>
#include "depth_policy.hpp"

namespace {
volatile std::sig_atomic_t interrupted = 0;
void stopRequested(int) { interrupted = 1; }

struct CaptureSession {
    UnitreeCamera& camera;
    bool capture = false;
    bool stereo = false;
    ~CaptureSession() {
        if (stereo) { try { camera.stopStereoCompute(); } catch (...) {} }
        if (capture) { try { camera.stopCapture(); } catch (...) {} }
    }
};

cv::Mat visualize(const cv::Mat& meters) {
    cv::Mat valid = (meters >= 0.2) & (meters <= 4.0);
    cv::Mat clipped = meters.clone();
    clipped.setTo(0.2, ~valid);
    cv::Mat gray, color;
    clipped.convertTo(gray, CV_8U, 255.0 / 3.8, -0.2 * 255.0 / 3.8);
    cv::applyColorMap(gray, color, cv::COLORMAP_JET);
    color.setTo(cv::Scalar(0, 0, 0), ~valid);
    return color;
}
}

int main(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Usage: depth_monitor CONFIG METERS_PER_UNIT [THRESHOLD_M]\n"
                     "Verify the raw depth scale on your hardware before interpreting distances.\n";
        return 0;
    }
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: depth_monitor CONFIG METERS_PER_UNIT [THRESHOLD_M]\n";
        return 2;
    }
    try {
        const double scale = perception::positiveNumber(argv[2]);
        const double threshold = argc == 4 ? perception::positiveNumber(argv[3]) : 1.0;
        if (threshold <= 0.2 || threshold > 4.0)
            throw std::invalid_argument("Threshold must be greater than 0.2 and at most 4.0 meters");
        if (!std::ifstream(argv[1]).good())
            throw std::runtime_error("Configuration file cannot be read");

        std::signal(SIGINT, stopRequested);
        std::signal(SIGTERM, stopRequested);
        UnitreeCamera camera(argv[1]);
        if (!camera.isOpened()) throw std::runtime_error("Camera could not be opened");
        CaptureSession session{camera};
        camera.startCapture();
        session.capture = true;
        camera.startStereoCompute();
        session.stereo = true;
        cv::namedWindow("Go1 Depth Monitor", cv::WINDOW_NORMAL);
        auto lastFrame = std::chrono::steady_clock::now();
        auto lastLog = lastFrame - std::chrono::seconds(1);
        while (!interrupted && camera.isOpened()) {
            cv::Mat depth;
            std::chrono::microseconds timestamp;
            const bool received = camera.getDepthFrame(depth, false, timestamp);
            if (!received || depth.empty()) {
                cv::Mat unavailable(200, 640, CV_8UC3, cv::Scalar(0, 0, 0));
                cv::putText(unavailable, "NO FRAME - UNKNOWN DEPTH", cv::Point(12, 100),
                            cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 200, 255), 2);
                cv::imshow("Go1 Depth Monitor", unavailable);
                if (cv::waitKey(10) == 27) break;
                if (std::chrono::steady_clock::now() - lastFrame > std::chrono::seconds(5))
                    throw std::runtime_error("No depth frame received for five seconds");
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            lastFrame = std::chrono::steady_clock::now();
            if (depth.type() != CV_32FC1 && depth.type() != CV_16UC1)
                throw std::runtime_error("Unsupported depth format: " + std::to_string(depth.type()));

            const int u = depth.cols / 2, v = depth.rows / 2;
            const double raw = depth.type() == CV_32FC1 ?
                static_cast<double>(depth.at<float>(v, u)) :
                static_cast<double>(depth.at<unsigned short>(v, u));
            const auto state = perception::classify(raw, scale, threshold);
            cv::Mat meters;
            depth.convertTo(meters, CV_32F, scale);
            cv::Mat display = visualize(meters);
            cv::drawMarker(display, cv::Point(u, v), cv::Scalar(255, 255, 255), cv::MARKER_CROSS, 15);
            cv::putText(display, perception::label(state), cv::Point(8, 24),
                        cv::FONT_HERSHEY_SIMPLEX, 0.45, cv::Scalar(255, 255, 255), 1);
            cv::imshow("Go1 Depth Monitor", display);
            if (lastFrame - lastLog >= std::chrono::seconds(1)) {
                std::cout << "raw=" << raw << " scaled_m=" << raw * scale
                          << " type=" << depth.type() << " " << perception::label(state) << '\n';
                lastLog = lastFrame;
            }
            if (cv::waitKey(10) == 27) break;
        }
        cv::destroyAllWindows();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Depth monitor: " << error.what() << '\n';
        cv::destroyAllWindows();
        return 1;
    }
}
