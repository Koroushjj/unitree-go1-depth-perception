/**
 * @file example_getDepthFrame.cc
 */

#include <UnitreeCameraSDK.hpp>
#include <unistd.h>
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[]) {

    // Initialisierung des Kameraobjekts mit der YAML-Konfigurationsdatei
    UnitreeCamera cam("stereo_camera_config.yaml");

    // Prüfung, ob die Kamera erfolgreich geöffnet wurde
    if (!cam.isOpened())
        exit(EXIT_FAILURE);

    // Start der Bildaufnahme
    cam.startCapture();

    // Aktivierung der Stereo-Tiefenberechnung
    cam.startStereoCompute();

    // Hauptschleife für die kontinuierliche Verarbeitung
    while (cam.isOpened()) {

        cv::Mat depth;
        std::chrono::microseconds t;

        // Abruf eines neuen Tiefenbildes
        if (!cam.getDepthFrame(depth, false, t)) {
            usleep(1000);
            continue;
        }

        // Verarbeitung nur bei gültigem Tiefenbild
        if (!depth.empty()) {

            std::cout << "type = " << depth.type()
                << ", depth() = " << depth.depth()
                << ", channels = " << depth.channels()
                << std::endl;

            // Bestimmung des zentralen Bildpunkts
            int u = depth.cols / 2;
            int v = depth.rows / 2;

            // Hinderniserkennung bei Float-Tiefendaten
            if (depth.type() == CV_32FC1) {

                float d_m = depth.at<float>(v, u);

                std::cout << "Distance at center: " << d_m << " m" << std::endl;

                // Schwellenwertbasierte Hinderniserkennung
                if (d_m > 0.0 && d_m < 1) {
                    std::cout << "Obstacle detected!" << std::endl;
                }
                else {
                    std::cout << "Path is clear." << std::endl;
                }

            }
            else {
                std::cout << "Unexpected depth type: " << depth.type() << std::endl;
            }

            cv::Mat depthShow;

            // Verarbeitung von 16-Bit-Tiefenbildern (Millimeter)
            if (depth.type() == CV_16UC1) {

                double minDist_mm = 100.0;
                double maxDist_mm = 4000.0;

                cv::Mat depthClipped = depth.clone();

                // Begrenzung des darstellbaren Tiefenbereichs
                depthClipped.setTo(maxDist_mm, depth > maxDist_mm);
                depthClipped.setTo(minDist_mm, depth < minDist_mm);

                cv::Mat depth8u;

                // Umwandlung in 8-Bit-Darstellung
                depthClipped.convertTo(
                    depth8u,
                    CV_8U,
                    255.0 / (maxDist_mm - minDist_mm),
                    -minDist_mm * 255.0 / (maxDist_mm - minDist_mm)
                );

                // Farbliche Visualisierung
                cv::applyColorMap(depth8u, depthShow, cv::COLORMAP_JET);
            }

            // Verarbeitung von Float-Tiefenbildern (Meter)
            else if (depth.type() == CV_32FC1) {

                double minDist_m = 0.2;
                double maxDist_m = 4.0;

                cv::Mat depthClipped = depth.clone();

                depthClipped.setTo(maxDist_m, depth > maxDist_m);
                depthClipped.setTo(minDist_m, depth < minDist_m);

                cv::Mat depth8u;

                depthClipped.convertTo(
                    depth8u,
                    CV_8U,
                    255.0 / (maxDist_m - minDist_m),
                    -minDist_m * 255.0 / (maxDist_m - minDist_m)
                );

                cv::applyColorMap(depth8u, depthShow, cv::COLORMAP_JET);
            }

            // Fallback bei unbekanntem Datenformat
            else {
                depthShow = depth;
            }

            // Anzeige des Tiefenbilds
            cv::imshow("UnitreeCamera-Depth", depthShow);
        }

        // Programmbeendigung mit ESC
        char key = cv::waitKey(10);

        if (key == 27)
            break;
    }

    // Kontrolliertes Stoppen der Tiefenberechnung und Kameraaufnahme
    cam.stopStereoCompute();
    cam.stopCapture();

    return 0;
}