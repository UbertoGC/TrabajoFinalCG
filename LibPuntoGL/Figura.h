#ifndef __FIGURA_H__
#define __FIGURA_H__
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <omp.h>
#include "Punto.h"
class Figura{
private:
    void Iniciar();
protected:
    Punto* puntos;
    bool** conexion;
    int n_puntos;
public:
    Figura();
    Figura(const Figura&);
    Figura(int);
    Figura(std::vector<Punto>&);
    Figura(std::vector<std::vector<float>>&, std::vector<std::vector<bool>>&);
    virtual void Dibujar();
    void AgregarPunto(const std::vector<float>&);
    void ConectarTodos();
    void resize(int);
    int size();
    Punto& operator[](int) const;
    Figura& operator=(const Figura&);
    ~Figura();
};
Figura::Figura(){
    puntos = nullptr;
    conexion = nullptr;
    n_puntos = 0;
}
Figura::Figura(const Figura& B){
    this->n_puntos = B.n_puntos;
    this->Iniciar();
    #pragma omp parallel for
    for (int i = 0; i < n_puntos; i++){
        #pragma omp simd
        for (int j = 0; j < n_puntos; j++){
            conexion[i][j] = B.conexion[i][j];
        }
    }
}
Figura::Figura(int n_p){
    this->n_puntos = n_p;
    this->Iniciar();
    #pragma omp parallel for
    for (int i = 0; i < this->n_puntos; i++){
        this->conexion[i] = new bool[this->n_puntos]{false};
    }
}
Figura::Figura(std::vector<Punto>& pu){
    this->n_puntos = pu.size();
    this->Iniciar();
    #pragma omp parallel for
    for (int i = 0; i < this->n_puntos; i++){
        this->puntos[i] = pu[i];
    }
}
Figura::Figura(std::vector<std::vector<float>>& pu, std::vector<std::vector<bool>>& ar){
    n_puntos = pu.size();
    this->Iniciar();
    #pragma omp parallel for
    for (int i = 0; i < n_puntos; i++){
        puntos[i].asignar(pu[i]);
    }
    #pragma omp parallel for
    for (int i = 0; i < n_puntos; i++){
        #pragma omp simd
        for (int j = 0; j < n_puntos; j++){
            conexion[i][j] = ar[i][j];
        }
    }
}
void Figura::Iniciar(){
    puntos = new Punto[n_puntos];
    conexion = new bool*[n_puntos];
    #pragma omp parallel for
    for (int i = 0; i < n_puntos; i++){
        conexion[i] = new bool[n_puntos]{false};
    }
}
void Figura::Dibujar(){
    glColor3f(0.0f, 1.0f, 1.0f);  
    glBegin(GL_POINTS);
    for (int i = 0; i < n_puntos; i++){
        glVertex3f(this->puntos[i].X(), this->puntos[i].Y(), this->puntos[i].Z());
    }
    glEnd();
    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 0.0f);  
    glBegin(GL_LINES);
    for (int i = 0; i < n_puntos; i++){
        for (int j = i+1; j < n_puntos; j++){
            if(conexion[i][j]){
                glVertex3f(this->puntos[i].X(), this->puntos[i].Y(), this->puntos[i].Z());
                glVertex3f(this->puntos[j].X(), this->puntos[j].Y(), this->puntos[j].Z());
            }
        }
    }
    glEnd();
}
void Figura::AgregarPunto(const std::vector<float>& add){
    Punto* puntos_nuevo = new Punto[n_puntos + 1];
    for (int i = 0; i < n_puntos; i++){
        puntos_nuevo[i] = this->puntos[i];
    }
    puntos_nuevo[n_puntos].asignar(add);
    delete[] this->puntos;
    this->puntos = puntos_nuevo;
    n_puntos += 1;
}
void Figura::resize(int n_p){
    if(n_p != n_puntos){
        if(puntos != nullptr){
            delete[] puntos;
        }
        if(conexion != nullptr){
            #pragma omp parallel for
            for (int i = 0; i < n_puntos; i++){
                delete[] conexion[i];
            }
            delete[] conexion;
        }
        n_puntos = n_p;
        this->Iniciar();
    }
}
void Figura::ConectarTodos(){
    #pragma omp parallel for
    for (int i = 0; i < n_puntos; i++){
        #pragma omp simd
        for (int j = i+1; j < n_puntos; j++){
            if(i != j){
                conexion[i][j] = true;
            }
        }
    }
}
int Figura::size(){
    return n_puntos;
}
Punto& Figura::operator[](int i) const {
    return puntos[i];
}
Figura& Figura::operator=(const Figura& B){
    if(this != &B){
        if(this->n_puntos != B.n_puntos){
            delete[] this->puntos;
            for (int i = 0; i < this->n_puntos; i++){
                delete[] this->conexion[i];
            }
            delete[] this->conexion;
            this->n_puntos = B.n_puntos;
            this->Iniciar();
        }
        #pragma omp parallel for
        for (int i = 0; i < this->n_puntos; i++){
            this->puntos[i] = B.puntos[i];
        }
        #pragma omp parallel for
        for (int i = 0; i < this->n_puntos; i++){
            #pragma omp simd
            for (int j = 0; j < this->n_puntos; j++){
                this->conexion[i][j] = B.conexion[i][j];
            }
        }
    }
    return *this;
}
Figura::~Figura(){
    if (puntos) {
        delete[] puntos;
        puntos = nullptr;
    }
    if (conexion) {
        for (int i = 0; i < n_puntos; i++) {
            if(conexion[i]){
                delete[] conexion[i];
            }
        }
        delete[] conexion;
        conexion = nullptr;
    }
}
#endif