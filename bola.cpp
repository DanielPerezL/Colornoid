#include "bola.h"
double MIN_Y_VEL = 0.1;

// Constructor de la bola
Bola::Bola(cv::Point2d pos, cv::Point2d vel, int r, cv::Scalar col)
    : posicion(pos), velocidad(vel), radio(r), color(col) {}

// Getters y Setters
cv::Point2d Bola::getPosicion() const {
    return posicion;
}

void Bola::setPosicion(cv::Point2d pos) {
    posicion = pos;
}

int Bola::getRadio() const {
    return radio;
}

cv::Point2d Bola::getVelocidad() const {
    return velocidad;
}

void Bola::setVelocidad(cv::Point2d vel) {
    velocidad = vel;
    if(abs(velocidad.y) < MIN_Y_VEL){
        velocidad.y < 0 ? velocidad.y = -MIN_Y_VEL : velocidad.y = MIN_Y_VEL;
    }
}

// Función para mover la bola
void Bola::mover(double dt) {
    posicion += velocidad * dt;
}

/* Función para comprobar colisiones con los bordes
    - devuelve true cuando cae por abajo
*/
bool Bola::comprobarBorde(int anchoVentana, int altoVentana) {
    // Rebote lado izquierdo
    if (posicion.x - radio < 0) {
        velocidad.x *= -1;
        posicion.x = radio;
    }
    // Rebote lado derecho
    else if (posicion.x + radio > anchoVentana) {
        velocidad.x *= -1;
        posicion.x = anchoVentana - radio;
    }
    // Rebote techo
    else if (posicion.y - radio < 0) {
        velocidad.y *= -1;
        posicion.y = radio;
    }
    return posicion.y - radio > altoVentana;
}

// Función para dibujar la bola en la ventana con efectos estéticos
void Bola::imprimeBola(cv::Mat &frame) const {
    // Agregar una sombra debajo de la bola
    cv::Point sombraPosicion = posicion;
    cv::circle(frame, sombraPosicion, radio, cv::Scalar(50, 50, 50), -1);

    //Dibujar el borde de bola
    cv::circle(frame, posicion, radio, color, 2);
}


