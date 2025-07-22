#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <eigen3/Eigen/Dense>
#include <opencv2/opencv.hpp>
#include "LibPuntoGL/Triangulo.h"
#include "LibPuntoGL/PuntoGL.h"
#include "LibPuntoGL/KDTreeGL.h"
#include "Controles.h"
using namespace std;
using namespace cv;
int windowWidth = 640;
int windowHeight = 480;
GLuint cameraTexture;
User usuario;
void leerArchivoASC(const string& rutaArchivo, vector<PuntoGL>& datos) {
    cout<<"Leyendo...\n";
    ifstream archivo(rutaArchivo);
    string linea;
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return;
    }
    while (getline(archivo, linea)) {
        istringstream ss(linea);
        PuntoGL p;
        if (ss >> p[0] >> p[1] >> p[2] >> p.NX() >> p.NY() >> p.NZ() ) {
            datos.push_back(p);
        }
    }
    archivo.close();
}
bool CalcularCentroEsfera(PuntoGL* a, PuntoGL* b, PuntoGL* c, PuntoGL& centro) {
    Eigen::Vector3f A(a->X(), a->Y(), a->Z());
    Eigen::Vector3f B(b->X(), b->Y(), b->Z());
    Eigen::Vector3f C(c->X(), c->Y(), c->Z());

    Eigen::Vector3f AB = B - A;
    Eigen::Vector3f AC = C - A;

    Eigen::Vector3f N = AB.cross(AC);
    float area2 = N.norm();
    if (area2 < 1e-6f) return false;

    Eigen::Vector3f AB_mid = (A + B) / 2.0f;
    Eigen::Vector3f AC_mid = (A + C) / 2.0f;
    Eigen::Vector3f AB_dir = AB.cross(N);
    Eigen::Vector3f AC_dir = AC.cross(N);

    Eigen::Matrix3f M;
    M.row(0) = AB_dir;
    M.row(1) = AC_dir;
    M.row(2) = N.normalized();

    Eigen::Vector3f rhs;
    rhs[0] = AB_dir.dot(AB_mid);
    rhs[1] = AC_dir.dot(AC_mid);
    rhs[2] = N.normalized().dot(A);

    Eigen::Vector3f O = M.colPivHouseholderQr().solve(rhs);
    centro = PuntoGL(O.x(), O.y(), O.z(), 0, 0, 0);
    return true;
}
bool VerificarTriangulo(PuntoGL* a, PuntoGL* b, PuntoGL* c, KDTreeGL& arbol) {
    PuntoGL centro;
    if (!CalcularCentroEsfera(a, b, c, centro)){
        return false;
    }
    //La variable limit_dist determina el rango en el cual no deben encontrarse, ningún punto
    //alrededor de la CentroEsfera para validar el triangulo.
    //float limit_dist = min({Distancia(centro, *a), Distancia(centro, *b), Distancia(centro, *c)});
    float limit_dist = (Distancia(centro, *a) + Distancia(centro, *b) + Distancia(centro, *c)) / 3.0;
    vector<PuntoGL*> vecinos;
    arbol.BuscarPorRango(centro, vecinos, limit_dist);
    for (int i = 0; i < vecinos.size(); i++) {
        if (!vecinos[i]){
            continue;
        }
        if (vecinos[i] == a || vecinos[i] == b || vecinos[i] == c){
            continue;
        }
        if (Distancia(centro, *vecinos[i]) < limit_dist){
            return false;
        }
    }
    return true;
}
bool BuscarTrianguloInicial(vector<PuntoGL*>& puntos_punteros, float radio, KDTreeGL& arbol,
    vector<Triangulo>& malla, set<Borde>& bordesActivos, set<PuntoGL*>& usados){
    for (PuntoGL* p : puntos_punteros) {
        vector<PuntoGL*> vecinos;
        arbol.BuscarPorRango(*p, vecinos, radio * 1.1f);
        for (int i = 0; i < vecinos.size(); i++) {
            for (int j = i + 1; j < vecinos.size(); j++) {
                PuntoGL* a = vecinos[i];
                PuntoGL* b = vecinos[j];
                if (p == a) {
                    break;
                }
                if (p == b) {
                    continue;
                }
                if (VerificarTriangulo(p, a, b, arbol)) {
                    malla.push_back(Triangulo(*p, *a, *b));
                    usados.insert(p);
                    usados.insert(a);
                    usados.insert(b);
                    bordesActivos.insert({p, a});
                    bordesActivos.insert({a, b});
                    bordesActivos.insert({b, p});
                }
            }
        }
    }
    return false;
}
void ExpandirBorde(const Borde& borde, float radio, KDTreeGL& arbol,
                   vector<Triangulo>& malla, set<Borde>& bordesActivos,
                   set<PuntoGL*>& usados) {
    PuntoGL* v1 = borde.a;
    PuntoGL* v2 = borde.b;

    PuntoGL centro_busqueda((v1->X() + v2->X()) / 2,
                            (v1->Y() + v2->Y()) / 2,
                            (v1->Z() + v2->Z()) / 2, 0, 0, 0);

    vector<PuntoGL*> candidatos;
    arbol.BuscarPorRango(centro_busqueda, candidatos, radio * 1.1f);

    for (PuntoGL* v3 : candidatos) {
        if (v3 == v1 || v3 == v2 || usados.count(v3)){
            continue;
        }

        if (VerificarTriangulo(v1, v2, v3, arbol)) {
            malla.push_back(Triangulo(*v1, *v2, *v3));
            usados.insert(v3);

            Borde b1(v2, v3), b2(v3, v1);

            if (bordesActivos.count(b1)) bordesActivos.erase(b1); else bordesActivos.insert(b1);
            if (bordesActivos.count(b2)) bordesActivos.erase(b2); else bordesActivos.insert(b2);

            return;
        }
    }
}
void BPA(vector<PuntoGL*>& puntos, vector<Triangulo>& malla){
    KDTreeGL arbol;
    set<Borde> bordesActivos;
    set<PuntoGL*> usados;
    for (int i = 0; i < puntos.size(); i++){
        arbol.Insertar(*puntos[i]);
    }
    float radio = 2.5f;
    float radio_bordes = 5.0f;
    cout<<"hola1\n";
    if (!BuscarTrianguloInicial(puntos, radio, arbol, malla, bordesActivos, usados)){
        return;
    }
    cout<<"hola2\n";
    while (!bordesActivos.empty()) {
        Borde borde = *bordesActivos.begin();
        bordesActivos.erase(borde);
        ExpandirBorde(borde, radio_bordes, arbol, malla, bordesActivos, usados);
    }
    cout<<"hola3\n";
}
void ReducirPuntos(vector<PuntoGL>& puntos, vector<PuntoGL*>& puntos_filtrados){
    KDTreeGL arbol;
    vector<PuntoGL*> puntos_originales(puntos.size());
    set<PuntoGL*> ya_agregados;
    for (int i = 0; i < puntos.size(); i++){
        arbol.Insertar(puntos[i]);
        puntos_originales[i] = &puntos[i];
    }
    cout<<"Eliminando Puntos...\n";
    for (PuntoGL* p : puntos_originales) {
        if (ya_agregados.count(p)){
            continue;
        }
        vector<PuntoGL*> vecinos;
        arbol.BuscarPorRango(*p, vecinos, 2.0f);
        for (PuntoGL* vecino : vecinos) {
            ya_agregados.insert(vecino);
        }
        puntos_filtrados.push_back(p);
    }
}
void drawBackgroundQuad() {
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cameraTexture);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(1, 0); glVertex2f(windowWidth, 0);
        glTexCoord2f(1, 1); glVertex2f(windowWidth, windowHeight);
        glTexCoord2f(0, 1); glVertex2f(0, windowHeight);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}
int main(){
    string ruta = "Prueba2/Escanear_Project1_scan_";
    vector<PuntoGL> puntos;
    for (int i = 1; i < 9; i++){
        string ruta_final = ruta + to_string(i)  + ".asc";
        leerArchivoASC(ruta_final, puntos);
    }
    cout<<"Puntos totales: "<<puntos.size()<<endl;
    vector<Triangulo> malla;
    vector<PuntoGL*> puntos_filtrados;
    ReducirPuntos(puntos, puntos_filtrados);
    cout<<"Puntos filtrados: "<<puntos_filtrados.size()<<endl;
    BPA(puntos_filtrados, malla);
    cout<<"Triangulos creados: "<<malla.size()<<endl;

    Mat cameraMatrix, distCoeffs;
    FileStorage fs("calibration.yml", FileStorage::READ);
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    fs.release();

    VideoCapture cam(0);
    cam.set(CAP_PROP_FRAME_WIDTH, windowWidth);
    cam.set(CAP_PROP_FRAME_HEIGHT, windowHeight);
    if (!cam.isOpened()) {
        std::cerr << "No se pudo abrir la cámara.\n";
        return -1;
    }
    Mat frameBGR;
    cam >> frameBGR;
    if (frameBGR.empty()) {
        cerr << "La cámara no devuelve imagen\n";
        return -1;
    }

    windowWidth = frameBGR.cols;
    windowHeight = frameBGR.rows;

    if (!glfwInit()){
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL + OpenCV", nullptr, nullptr);
    if (!window) {
        cerr << "No se pudo crear la ventana\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &cameraTexture);
    glBindTexture(GL_TEXTURE_2D, cameraTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Usar formato GL_BGR directamente
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 frameBGR.cols, frameBGR.rows,
                 0, GL_BGR, GL_UNSIGNED_BYTE, frameBGR.data);
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        cam >> frameBGR;
        if (frameBGR.empty()) continue;

        Mat frameUndistorted;
        undistort(frameBGR, frameUndistorted, cameraMatrix, distCoeffs);

        // Actualizar textura cada frame
        glBindTexture(GL_TEXTURE_2D, cameraTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        frameUndistorted.cols, frameUndistorted.rows,
                        GL_BGR, GL_UNSIGNED_BYTE, frameUndistorted.data);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawBackgroundQuad();
        float aspecto = (float)windowWidth / (float)windowHeight;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspecto, 0.1f, 2000.0f);
        glm::mat4 view = usuario.process_vista();
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&projection[0][0]);

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(&view[0][0]);
        for (int i = 0; i < malla.size(); i++){
            malla[i].Dibujar();
        }
        /*
        glPointSize(5.0f);
        glBegin(GL_POINTS);
        for (int i = 0; i < puntos_filtrados.size(); i++){
            glColor3f(1.0, 0.0, 0.0); 
            glVertex3f(puntos_filtrados[i]->X(), puntos_filtrados[i]->Y(), puntos_filtrados[i]->Z());
        }
        glEnd();
        */
        glColor3f(1.0f, 1.0f, 1.0f);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &cameraTexture);
    glfwTerminate();
    return 0;
}