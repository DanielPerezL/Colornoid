#ifndef BOLA_H
#define BOLA_H

#include <opencv2/opencv.hpp>

class Bola {
private:
    cv::Point2d posicion;
    cv::Point2d velocidad;
    int radio;
    cv::Scalar color;

public:
    // Constructor de la bola
    Bola(cv::Point2d pos, cv::Point2d vel, int r, cv::Scalar col);

    // Getters y Setters
    cv::Point2d getPosicion() const;
    void setPosicion(cv::Point2d pos);
    int getRadio() const;
    cv::Point2d getVelocidad() const;
    void setVelocidad(cv::Point2d vel);

    // Función para actualizar la posición de la bola
    void mover(double dt);

    // Función para comprobar colisión con los bordes
    //devuelve true si se ha salido por abajo
    bool comprobarBorde(int anchoVentana, int altoVentana);

    // Función para dibujar la bola
    void imprimeBola(cv::Mat &frame) const;
};
#endif
