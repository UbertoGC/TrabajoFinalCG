#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    cv::Size patternSize(9, 6); // Esquinas internas del tablero
    float squareSize = 1.0f;

    std::vector<cv::Point3f> objp;
    for (int i = 0; i < patternSize.height; ++i)
        for (int j = 0; j < patternSize.width; ++j)
            objp.emplace_back(j * squareSize, i * squareSize, 0);

    std::vector<std::vector<cv::Point3f>> objpoints;
    std::vector<std::vector<cv::Point2f>> imgpoints;
    cv::Size imageSize;

    std::string path = "calib";
    std::string output_dir = "res_img";
    int img_index = 1;

    // Crear carpeta de salida si no existe
    if (!fs::exists(output_dir)) {
        fs::create_directory(output_dir);
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        std::string filename = entry.path().string();
        cv::Mat img = cv::imread(filename);
        if (img.empty()) {
            std::cerr << "No se pudo leer: " << filename << std::endl;
            continue;
        }

        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(gray, patternSize, corners);

        if (found) {
            cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1),
                cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));
            imgpoints.push_back(corners);
            objpoints.push_back(objp);
            imageSize = gray.size();

            cv::drawChessboardCorners(img, patternSize, corners, found);

            // Guardar imagen con esquinas detectadas
            std::string output_file = output_dir + "/img" + std::to_string(img_index) + "_res.jpg";
            cv::imwrite(output_file, img);
            std::cout << "Guardado: " << output_file << std::endl;

            img_index++;
        }
    }

    if (objpoints.empty()) {
        std::cerr << "No se detectaron esquinas en ninguna imagen." << std::endl;
        return -1;
    }

    // Calibrar cámara
    cv::Mat cameraMatrix, distCoeffs, rvecs, tvecs;
    double rms = cv::calibrateCamera(objpoints, imgpoints, imageSize,
                                     cameraMatrix, distCoeffs, rvecs, tvecs);

    std::cout << "Error RMS: " << rms << std::endl;
    std::cout << "Matriz de cámara:\n" << cameraMatrix << std::endl;
    std::cout << "Coeficientes de distorsión:\n" << distCoeffs << std::endl;

    // Guardar parámetros
    cv::FileStorage fs("calibration1.yml", cv::FileStorage::WRITE);
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs.release();

    std::cout << "Archivo 'calibration.yml' guardado correctamente." << std::endl;
    return 0;
}
