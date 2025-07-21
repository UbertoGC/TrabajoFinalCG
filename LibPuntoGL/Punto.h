#ifndef __PUNTO_H__
#define __PUNTO_H__
#include <cmath>
#include <vector>
class Punto{
protected:
    float *posicion;
    int n_dimension;
public:
    Punto();
    Punto(const Punto&);
    Punto(const std::vector<float>&);
    Punto(int);
    void asignar(const std::vector<float>&);
    void limpiar();
    float& X() const;
    float& Y() const;
    float& Z() const;
    int dim() const;
    void imprimir() const;
    float& operator[](int) const;
    Punto& operator=(const Punto&);
    bool operator<(const Punto&) const;
    bool operator==(const Punto&) const;
    bool operator!=(const Punto&) const;
    friend float Distancia(const Punto&,const Punto&);
    ~Punto();
};
Punto::Punto(){
    this->posicion = nullptr;
    this->n_dimension = 0;
}
Punto::Punto(const Punto& B){
    this->n_dimension = B.n_dimension;
    posicion = new float[this->n_dimension];
    for (int i = 0; i < this->n_dimension; i++){
        this->posicion[i] = B.posicion[i];
    }
}
Punto::Punto(const std::vector<float>& val){
    this->n_dimension = val.size();
    this->posicion = new float[this->n_dimension];
    for (int i = 0; i < this->n_dimension; i++){
        this->posicion[i] = val[i];
    }
}
Punto::Punto(int n_d){
    this->n_dimension = n_d;
    this->posicion = new float[this->n_dimension]{0.0};
}
void Punto::asignar(const std::vector<float>& v){
    if(n_dimension != v.size()){
        this->limpiar();
        n_dimension = v.size();
        posicion = new float[n_dimension]; 
    }
    for (int i = 0; i < n_dimension; i++){
        posicion[i] = v[i];
    }
}
void Punto::limpiar(){
    if(posicion != nullptr){
        delete posicion;
    }
    n_dimension = 0;
}
float& Punto::X() const{
    return posicion[0];
}
float& Punto::Y() const{
    return posicion[1];
}
float& Punto::Z() const{
    return posicion[2];
}
int Punto::dim() const{
    return n_dimension;
}
void Punto::imprimir() const{
    std::cout<<"["<<this->posicion[0]<<", "<<this->posicion[1]<<", "<<this->posicion[2]<<"]\n";
}
float& Punto::operator[](int i) const{
    return posicion[i];
}
Punto& Punto::operator=(const Punto& B) {
    if (this != &B){
        if (this->posicion) delete[] this->posicion;
        this->n_dimension = B.n_dimension;
        this->posicion = new float[this->n_dimension];
        for (int i = 0; i < this->n_dimension; i++){
            this->posicion[i] = B.posicion[i];
        }
    }
    return *this;
}
bool Punto::operator<(const Punto& B) const{
    int minimo = std::min(this->n_dimension, B.n_dimension);
    bool flag = false;
    for (int i = 0; i < minimo; i++){
        if(B.posicion[i] != this->posicion[i]){
            flag = (this->posicion[i] < B.posicion[i]);
            break;
        }
    }
    return flag;
}
bool Punto::operator==(const Punto& B) const{
    if(this->n_dimension != B.n_dimension){
        return false;
    }else{
        float EPSILON = 1e-5;
        for (int i = 0; i < this->n_dimension; i++) {
            if (std::fabs(this->posicion[i] - B.posicion[i]) > EPSILON){
                return false;
            }
        }
        return true;
    }
}
bool Punto::operator!=(const Punto& B) const{
    return !(*this == B);
}
Punto::~Punto(){
    if (this->posicion) {
        delete[] this->posicion;
        this->posicion = nullptr;
    }
}
float Distancia(const Punto& a, const Punto& b) {
    return std::sqrt(   (a.X() - b.X())*(a.X() - b.X()) + 
                        (a.Y() - b.Y())*(a.Y() - b.Y()) + 
                        (a.Z() - b.Z())*(a.Z() - b.Z()));
}
#endif