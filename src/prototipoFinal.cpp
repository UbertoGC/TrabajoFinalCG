// main.cpp
#include <opencv2/opencv.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

cv::Mat cameraMatrix, distCoeffs;
cv::VideoCapture cap;

bool loadCameraCalibration(const std::string& filename) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) return false;
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    return true;
}

void drawAxis(const cv::Mat& frame, const cv::Mat& rvec, const cv::Mat& tvec) {
    std::vector<cv::Point3f> axisPoints = {{0,0,0}, {1,0,0}, {0,1,0}, {0,0,-1}};
    std::vector<cv::Point2f> imagePoints;
    projectPoints(axisPoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

    cv::line(frame, imagePoints[0], imagePoints[1], cv::Scalar(0,0,255), 2); // X - red
    cv::line(frame, imagePoints[0], imagePoints[2], cv::Scalar(0,255,0), 2); // Y - green
    cv::line(frame, imagePoints[0], imagePoints[3], cv::Scalar(255,0,0), 2); // Z - blue
}

int main() {
    if (!loadCameraCalibration("calibration.yml")) {
        std::cerr << "Error loading calibration." << std::endl;
        return -1;
    }

    cap.open(0);
    if (!cap.isOpened()) {
        std::cerr << "No se puede abrir la camara" << std::endl;
        return -1;
    }

    cv::Size patternSize(6, 9); // tablero de ajedrez 6x9
    std::vector<cv::Point3f> objPoints;
    for (int i = 0; i < patternSize.height; ++i)
        for (int j = 0; j < patternSize.width; ++j)
            objPoints.emplace_back(j, i, 0);

    while (true) {
        cv::Mat frame, gray;
        cap >> frame;
        if (frame.empty()) break;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(gray, patternSize, corners);

        if (found) {
            cv::Mat rvec, tvec;
            cv::solvePnP(objPoints, corners, cameraMatrix, distCoeffs, rvec, tvec);
            drawAxis(frame, rvec, tvec);
        }

        cv::imshow("AR", frame);
        if (cv::waitKey(1) == 27) break;
    }

    return 0;
}
