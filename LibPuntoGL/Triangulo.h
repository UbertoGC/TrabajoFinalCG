#ifndef __TRIANGULO_H__
#define __TRIANGULO_H__
#include <eigen3/Eigen/Dense>
#include "Figura.h"
class Triangulo : public Figura{
private:
    void Iniciar();
public:
    Triangulo();
    Triangulo(const Triangulo&);
    Triangulo(const Punto&, const Punto&, const Punto&);
    Triangulo(const std::vector<Punto>&);
    friend struct TrianguloHash;
    Punto& operator[](int) const;
    Triangulo& operator=(const Triangulo&);
    void Dibujar(float = 0.0f, float = 0.0f, float = 1.0f, float = 1.0f, float = 1.0f, float = 0.0f);
    bool operator<(const Triangulo&) const;
    bool operator==(const Triangulo&) const;
    ~Triangulo();
};
Triangulo::Triangulo(){
    this->Iniciar();
}
Triangulo::Triangulo(const Triangulo& B){
    this->Iniciar();
    for (int i = 0; i < 3; i++){
        this->puntos[i] = B.puntos[i]; 
    }
}
Triangulo::Triangulo(const Punto& A,const Punto& B,const Punto& C){
    this->Iniciar();
    this->puntos[0] = A;
    this->puntos[1] = B;
    this->puntos[2] = C;
}
Triangulo::Triangulo(const std::vector<Punto>& p){
    this->Iniciar();
    if(p.size() == 3){
        for (int i = 0; i < 3; i++){
            this->puntos[i] = p[i]; 
        }
    }
}
void Triangulo::Iniciar(){
    this->n_puntos = 3;
    this->puntos = new Punto[this->n_puntos];
    this->conexion = new bool*[this->n_puntos];
    for (int i = 0; i < n_puntos; i++){
        this->conexion[i] = new bool[this->n_puntos]{false};
        for (int j = 0; j < this->n_puntos; j++){
            if(i != j){
                this->conexion[i][j] = true;
            }
        }
    }
}
Punto& Triangulo::operator[](int i) const{
    return this->puntos[i];
}
Triangulo& Triangulo::operator=(const Triangulo& B){
    if (this != &B) {
        if(this->puntos == nullptr){
            this->puntos = new Punto[this->n_puntos];
            this->conexion = new bool*[this->n_puntos];
        }
        for (int i = 0; i < this->n_puntos; i++) {
            this->puntos[i] = B.puntos[i];
            this->conexion[i] = new bool[n_puntos];
            for (int j = 0; j < this->n_puntos; j++) {
                this->conexion[i][j] = B.conexion[i][j];
            }
        }
    }
    return *this;

}
void Triangulo::Dibujar(float r_tri, float g_tri, float b_tri, float r_lin, float g_lin, float b_lin){
    glColor3f(r_tri, g_tri, b_tri);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 3; i++) {
        glVertex3f(puntos[i].X(), puntos[i].Y(), puntos[i].Z());
    }
    glEnd();
    glLineWidth(1.0f);
    glColor3f(r_lin, g_lin, b_lin);
    glBegin(GL_LINES);
    for (int i = 0; i < n_puntos; i++) {
        for (int j = i + 1; j < n_puntos; j++) {
            if (conexion[i][j]) {
                glVertex3f(puntos[i].X(), puntos[i].Y(), puntos[i].Z());
                glVertex3f(puntos[j].X(), puntos[j].Y(), puntos[j].Z());
            }
        }
    }
    glEnd();
}
bool Triangulo::operator<(const Triangulo& B) const{
    std::vector<Punto> T1;
    std::vector<Punto> T2;
    for (int i = 0; i < 3; i++){
        T1.push_back(this->puntos[i]);
        T2.push_back(B.puntos[i]);
    }
    

    std::sort(T1.begin(), T1.end());
    std::sort(T2.begin(), T2.end());

    return T1 < T2;
}
bool Triangulo::operator==(const Triangulo& b) const{
    std::vector<Punto> pa = { (*this)[0], (*this)[1], (*this)[2] };
    std::vector<Punto> pb = { b[0], b[1], b[2] };

    std::sort(pa.begin(), pa.end(), [](const Punto& a, const Punto& b) {
        for (int i = 0; i < a.dim(); ++i) {
            if (std::abs(a[i] - b[i]) > 1e-5)
                return a[i] < b[i];
        }
        return false;
    });
    std::sort(pb.begin(), pb.end(), [](const Punto& a, const Punto& b) {
        for (int i = 0; i < a.dim(); ++i) {
            if (std::abs(a[i] - b[i]) > 1e-5)
                return a[i] < b[i];
        }
        return false;
    });

    return pa[0] == pb[0] && pa[1] == pb[1] && pa[2] == pb[2];
}
Triangulo::~Triangulo(){
    if (puntos) {
        delete[] puntos;
        puntos = nullptr;
    }
    if (conexion) {
        for (int i = 0; i < n_puntos; i++) {
            if( conexion[i]){
                delete[] conexion[i];
            }
        }
        delete[] conexion;
        conexion = nullptr;
    }
}
struct TrianguloHash {
    std::size_t operator()(const Triangulo& t) const {
        std::vector<Punto> puntos = { t[0], t[1], t[2] };
        std::sort(puntos.begin(), puntos.end(), [](const Punto& a, const Punto& b) {
            for (int i = 0; i < a.dim(); ++i) {
                if (std::abs(a[i] - b[i]) > 1e-5)
                    return a[i] < b[i];
            }
            return false;
        });
        size_t h = 0;
        for (const auto& p : puntos) {
            h ^= std::hash<float>()(p.X()) ^ std::hash<float>()(p.Y()) ^ std::hash<float>()(p.Z());
        }
        return h;
    }
};
#endif