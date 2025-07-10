#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "No se pudo abrir la cámara.\n";
        return -1;
    }

    cv::Size patternSize(6, 9); // columnas, filas internas
    std::vector<std::vector<cv::Point2f>> imagePoints;
    std::vector<std::vector<cv::Point3f>> objectPoints;

    std::vector<cv::Point3f> obj;
    for (int i = 0; i < patternSize.height; ++i)
        for (int j = 0; j < patternSize.width; ++j)
            obj.push_back(cv::Point3f(j, i, 0.0f));

    std::cout << "Presiona espacio para capturar una imagen.\n";

    while (true) {
        cv::Mat frame, gray;
        cap >> frame;
        if (frame.empty()) break;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(gray, patternSize, corners);

        if (found)
            cv::drawChessboardCorners(frame, patternSize, corners, found);

        cv::imshow("Captura", frame);
        char key = cv::waitKey(30);

        if (key == 32 && found) { // espacio
            std::cout << "Imagen capturada\n";
            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
        } else if (key == 27) break; // ESC
    }

    if (imagePoints.size() < 5) {
        std::cerr << "No hay suficientes imágenes para calibrar.\n";
        return -1;
    }

    cv::Mat cameraMatrix, distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;

    cv::calibrateCamera(objectPoints, imagePoints, patternSize, cameraMatrix, distCoeffs, rvecs, tvecs);

    cv::FileStorage fs("calibration.yml", cv::FileStorage::WRITE);
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs.release();

    std::cout << "Calibración guardada en calibration.yml\n";
    return 0;
}
