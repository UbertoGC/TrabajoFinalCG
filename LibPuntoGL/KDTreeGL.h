#ifndef __KDTREEGL_H__
#define __KDTREEGL_H__
#include "PuntoGL.h"

class NodoKDTreeGL{
private:
    PuntoGL* valor;
    NodoKDTreeGL* der;
    NodoKDTreeGL* izq;
    int dim;
public:
    NodoKDTreeGL(PuntoGL&, int d);
    void Insertar(PuntoGL&);
    void BuscarPorRango(PuntoGL&, std::vector<PuntoGL*>&, float);
    ~NodoKDTreeGL();
};
NodoKDTreeGL::NodoKDTreeGL(PuntoGL& v, int d){
    valor = &v;
    der = nullptr;
    izq = nullptr;
    dim = d;
}
void NodoKDTreeGL::Insertar(PuntoGL& v){
    if(v[dim] > (*valor)[dim]){
        if(der){
            der->Insertar(v);
        }else{
            der = new NodoKDTreeGL(v, (dim + 1) % 3);
        }
    }else{
        if(izq){
            izq->Insertar(v);
        }else{
            izq = new NodoKDTreeGL(v, (dim + 1) % 3);
        }
    }
}
void NodoKDTreeGL::BuscarPorRango(PuntoGL& punto, std::vector<PuntoGL*>& resultado, float radio){
    if (!this->valor){
        return;
    }
    if(Distancia(*this->valor,punto) <= radio){
        resultado.push_back(this->valor);
    }
    NodoKDTreeGL * select;
    float dist_dim = punto[dim] - (*this->valor)[dim];
    if(dist_dim <= 0 || std::abs(dist_dim) <= radio){
        if(izq){
            this->izq->BuscarPorRango(punto,resultado,radio);
        }
    }
    if(dist_dim >= 0 || std::abs(dist_dim) <= radio){
        if(der){
            this->der->BuscarPorRango(punto,resultado,radio);
        }
    }
}
NodoKDTreeGL::~NodoKDTreeGL(){
    delete der;
    delete izq;
}
class KDTreeGL{
private:
    NodoKDTreeGL* raiz;
public:
    KDTreeGL();
    void Insertar(PuntoGL&);
    void BuscarPorRango(PuntoGL&, std::vector<PuntoGL*>&, float);
    ~KDTreeGL();
};
KDTreeGL::KDTreeGL(){
    raiz = nullptr;
}
void KDTreeGL::Insertar(PuntoGL& v){
    if(this->raiz == nullptr){
        this->raiz = new NodoKDTreeGL(v,0);
    }else{
        this->raiz->Insertar(v);
    }
}
void KDTreeGL::BuscarPorRango(PuntoGL& punto, std::vector<PuntoGL*>& resultado, float radio){
    if(raiz){
        raiz->BuscarPorRango(punto, resultado, radio);
    }
}
KDTreeGL::~KDTreeGL(){
    delete raiz;
}

#endif