#ifndef __PUNTOGL_H__
#define __PUNTOGL_H__
#include "Punto.h"
class PuntoGL : public Punto{
private:
    void Iniciar();
    float *normal;
public:
    PuntoGL(const float& = 0.0, const float& = 0.0, const float& = 0.0, const float& = 0.0, const float& = 0.0, const float& = 0.0);
    PuntoGL(const PuntoGL&);
    PuntoGL(const std::vector<float>&, const std::vector<float>& = std::vector<float>());
    float& NX() const;
    float& NY() const;
    float& NZ() const;
    PuntoGL& operator=(const PuntoGL&);
    ~PuntoGL();
};
PuntoGL::PuntoGL(const PuntoGL& p){
    Iniciar();
    this->posicion[0] = p.posicion[0];
    this->posicion[1] = p.posicion[1];
    this->posicion[2] = p.posicion[2];
    this->normal[0] = p.normal[0];
    this->normal[1] = p.normal[1];
    this->normal[2] = p.normal[2];
}
PuntoGL::PuntoGL(const std::vector<float>& p, const std::vector<float>& n){
    Iniciar();
    posicion[0] = p[0];
    posicion[1] = p[1];
    posicion[2] = p[2];
    if(n.size() == 3){
        normal[0] = n[0];
        normal[1] = n[1];
        normal[2] = n[2];
    }
}
PuntoGL::PuntoGL(const float& x, const float& y, const float& z, const float& nx, const float& ny, const float& nz){
    this->Iniciar();
    this->posicion[0] = x;
    this->posicion[1] = y;
    this->posicion[2] = z;
    this->normal[0] = nx;
    this->normal[1] = ny;
    this->normal[2] = nz;
}
void PuntoGL::Iniciar(){
    this->n_dimension = 3;
    this->posicion = new float[3];
    this->normal = new float[3];
}
float& PuntoGL::NX() const{
    return normal[0];
}
float& PuntoGL::NY() const{
    return normal[1];
}
float& PuntoGL::NZ() const{
    return normal[2];
}
PuntoGL& PuntoGL::operator=(const PuntoGL& p) {
    if (this != &p) {
        if(!(this->posicion)){
            this->posicion = new float[3];
        }
        if(!(this->normal)){
            this->normal = new float[3];
        }
        this->posicion[0] = p.posicion[0];
        this->posicion[1] = p.posicion[1];
        this->posicion[2] = p.posicion[2];
        this->normal[0] = p.normal[0];
        this->normal[1] = p.normal[1];
        this->normal[2] = p.normal[2];
    }
    return *this;
}
PuntoGL::~PuntoGL(){
    if (this->posicion) {
        delete[] this->posicion;
        this->posicion = nullptr;
    }
    if (this->normal) {
        delete[] this->normal;
        this->normal = nullptr;
    }
}
float Distancia(const PuntoGL& a, const PuntoGL& b) {
    return std::sqrt(   (a.X() - b.X())*(a.X() - b.X()) + 
                        (a.Y() - b.Y())*(a.Y() - b.Y()) + 
                        (a.Z() - b.Z())*(a.Z() - b.Z()));
}
struct Borde {
    PuntoGL* a;
    PuntoGL* b;
    Borde(PuntoGL* v1 = nullptr, PuntoGL* v2 = nullptr){
        a = v1;
        b = v2;
    }
    bool operator<(const Borde& otro) const {
        return std::tie(a, b) < std::tie(otro.a, otro.b);
    }
};
#endif