#include "rectangulo.h"

cv::Point2d INVALID_POS(-1.0,-1.0);

// Constructor del rectángulo
Rectangulo::Rectangulo(cv::Point2d esquina, cv::Size2d tam, cv::Scalar col)
    : esquinaSuperiorIzquierda(esquina), tamaño(tam), color(col) {}

// Getters y setters
cv::Size2d Rectangulo::getTamaño() const {
    return tamaño;
}

cv::Point2d Rectangulo::getPosicion() const {
    return esquinaSuperiorIzquierda;
}

void Rectangulo::setInvalid() {
    esquinaSuperiorIzquierda = INVALID_POS;
}

bool Rectangulo::isValid() const {
    return esquinaSuperiorIzquierda.x != INVALID_POS.x && esquinaSuperiorIzquierda.y != INVALID_POS.y;
}

// Función para dibujar el rectángulo en la ventana
void Rectangulo::imprimeRectangulo(cv::Mat &frame) const {
    // Definir el grosor del marco y del borde
    int grosorMarco = 4; // Grosor del marco en píxeles
    int grosorBorde = 2; // Grosor del borde en píxeles

    // Calcular las esquinas internas para el marco (dentro del rectángulo original)
    cv::Point2d esquinaMarcoIzquierda = esquinaSuperiorIzquierda + cv::Point2d(grosorMarco, grosorMarco);
    cv::Point2d esquinaMarcoDerecha = esquinaSuperiorIzquierda + cv::Point2d(tamaño.width, tamaño.height) - cv::Point2d(grosorMarco, grosorMarco);

    // Calcular las esquinas internas para el borde negro
    cv::Point2d esquinaBordeIzquierda = esquinaSuperiorIzquierda + cv::Point2d(grosorBorde, grosorBorde);
    cv::Point2d esquinaBordeDerecha = esquinaSuperiorIzquierda + cv::Point2d(tamaño.width, tamaño.height) - cv::Point2d(grosorBorde, grosorBorde);

    // Dibujar el marco resaltante
    cv::Scalar colorMarco(100, 100, 100);
    cv::Scalar colorBorde(0, 0, 0);
    cv::rectangle(frame, esquinaSuperiorIzquierda, esquinaSuperiorIzquierda + cv::Point2d(tamaño.width, tamaño.height), colorBorde, -1);

    // Dibujar el borde negro dentro del marco
    cv::rectangle(frame, esquinaBordeIzquierda, esquinaBordeDerecha, colorMarco, -1);

    // Dibujar el rectángulo principal aún más pequeño dentro del borde negro
    cv::rectangle(frame, esquinaMarcoIzquierda, esquinaMarcoDerecha, color, -1);
}


// Función para comprobar colisión con la bola
bool Rectangulo::comprobarColision(const Bola &bola) const {
    cv::Point2d posBola = bola.getPosicion();
    int radioBola = bola.getRadio();

    bool colisionX = posBola.x + radioBola >= esquinaSuperiorIzquierda.x && posBola.x - radioBola <= esquinaSuperiorIzquierda.x + tamaño.width;
    bool colisionY = posBola.y + radioBola >= esquinaSuperiorIzquierda.y && posBola.y - radioBola <= esquinaSuperiorIzquierda.y + tamaño.height;

    return colisionX && colisionY;
}
