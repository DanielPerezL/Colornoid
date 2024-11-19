#ifndef RECTANGULO_H
#define RECTANGULO_H

#include <opencv2/opencv.hpp>
#include "bola.h"

class Rectangulo {
private:
    cv::Point2d esquinaSuperiorIzquierda;
    cv::Size2d tamaño;
    cv::Scalar color;
public:
    // Constructor del rectángulo
    Rectangulo(cv::Point2d esquina, cv::Size2d tam, cv::Scalar col);

    // Getters y setters
    bool esControlJugador() const;
    cv::Size2d getTamaño() const;
    cv::Point2d getPosicion() const;
    bool isValid() const;
    void setInvalid();

    // Función para dibujar el rectángulo
    void imprimeRectangulo(cv::Mat &frame) const;

    // Función para comprobar colisión con la bola
    bool comprobarColision(const Bola &bola) const;
};

#endif
