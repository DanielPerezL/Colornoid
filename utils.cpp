#include "utils.h"
#include <opencv2/opencv.hpp>
using namespace cv;

//Definicion de casos al colisionar con la esquina de un rectangulo
const int INFERIOR_IZQUIERDA = 0;
const int INFERIOR_DERECHA = 1;
const int SUPERIOR_IZQUIERDA = 2;
const int SUPERIOR_DERECHA = 3;


int aleatorio(int minimum_number, int max_number){
    return rand() % (max_number + 1 - minimum_number) + minimum_number;
}

int aleatorio(int n){
    return rand()%n;
}

// Función auxiliar para calcular el punto más cercano de una línea a un punto
cv::Point2d calcularPuntoMasCercano(const cv::Point2d &punto, const cv::Point2d &a, const cv::Point2d &b) {
    cv::Point2d ab = b - a;
    double t = ((punto.x - a.x) * ab.x + (punto.y - a.y) * ab.y) / (ab.x * ab.x + ab.y * ab.y);
    t = std::max(0.0, std::min(1.0, t));  // Limitar t al rango [0, 1] para estar dentro del segmento de línea
    return a + t * ab;
}

// Función que obtiene los puntos de un RotatedRect en precisión double
void getRotatedRectPoints(const cv::RotatedRect& rect, cv::Point2d points[4]) {
    // Array temporal para almacenar los puntos en precisión float
    cv::Point2f points_f[4];
    rect.points(points_f);  // Obtener los puntos en float

    // Convertir cada Point2f a Point2d y guardarlos en el array `points`
    for (int i = 0; i < 4; ++i) {
        points[i] = cv::Point2d(static_cast<double>(points_f[i].x), static_cast<double>(points_f[i].y));
    }
}

void imprimirRotatedRect(Mat frame, RotatedRect r){
    Point2d vertices[4];
    getRotatedRectPoints(r, vertices);
    for (int i = 0; i < 4; i++)
        line(frame, vertices[i], vertices[(i+1)%4], Scalar(0,255,0), 2);

    /*Descomentar si se quiere visualizar el rectangulo que contiene
      al rotatedRect
    */
    //Rect brect = r.boundingRect();
    //rectangle(frame, brect, Scalar(255,0,0), 2);
}

// Funciones para definir la orientación de la colisión basándonos en la posición de la bola y la dirección de su velocidad
bool colisionEnLadoIzquierdo(Point2d posicionBola, Point2d velocidadBola, Rectangulo* rect){
    return (posicionBola.x < rect->getPosicion().x && velocidadBola.x > 0);
}
bool colisionEnLadoDerecho(Point2d posicionBola, Point2d velocidadBola, Rectangulo* rect){
    return (posicionBola.x > rect->getPosicion().x + rect->getTamaño().width && velocidadBola.x < 0);
}
bool colisionEnLadoSuperior(Point2d posicionBola, Point2d velocidadBola, Rectangulo* rect){
    return (posicionBola.y < rect->getPosicion().y && velocidadBola.y > 0);
}
bool colisionEnLadoInferior(Point2d posicionBola, Point2d velocidadBola, Rectangulo* rect){
    return (posicionBola.y > rect->getPosicion().y + rect->getTamaño().height && velocidadBola.y < 0);
}

//Funcion auxiliar para detectar si la bola debe rebotar en esquina o en plano
bool esRectanguloAdyacente(Bola bola, std::vector<std::vector<Rectangulo>>& rectangulos, int fila, int columna, int caso) {
    Rectangulo* rect = &rectangulos[fila][columna];
    Point2d posicionBola = bola.getPosicion();
    Point2d velocidadBola = bola.getVelocidad();

    switch(caso){
    case INFERIOR_IZQUIERDA:
        if(colisionEnLadoIzquierdo(posicionBola, velocidadBola, rect) && fila < (int)rectangulos.size() - 1){
            return rectangulos[fila + 1][columna].isValid();  // Bloque abajo
        }
        if(colisionEnLadoInferior(posicionBola, velocidadBola, rect) && columna > 0){
            return rectangulos[fila][columna - 1].isValid();  // Bloque a la izquierda
        }
        break;
    case INFERIOR_DERECHA:
        if(colisionEnLadoDerecho(posicionBola, velocidadBola, rect) && fila < (int)rectangulos.size() - 1){
            return rectangulos[fila + 1][columna].isValid();  // Bloque abajo
        }
        if(colisionEnLadoInferior(posicionBola, velocidadBola, rect) && columna < (int)rectangulos[0].size() - 1){
            return rectangulos[fila][columna + 1].isValid();  // Bloque a la derecha
        }
        break;
    case SUPERIOR_IZQUIERDA:
        if(colisionEnLadoIzquierdo(posicionBola, velocidadBola, rect) && fila > 0){
            return rectangulos[fila - 1][columna].isValid();  // Bloque arriba
        }
        if(colisionEnLadoSuperior(posicionBola, velocidadBola, rect) && columna > 0){
            return rectangulos[fila][columna - 1].isValid();  // Bloque a la izquierda
        }
        break;
    case SUPERIOR_DERECHA:
        if(colisionEnLadoDerecho(posicionBola, velocidadBola, rect) && fila > 0){
            return rectangulos[fila - 1][columna].isValid();  // Bloque arriba
        }
        if(colisionEnLadoSuperior(posicionBola, velocidadBola, rect) && columna < (int)rectangulos[0].size() - 1){
            return rectangulos[fila][columna + 1].isValid();  // Bloque a la derecha
        }
        break;
    default:
        return false;
    }
    return false;
}

bool manejarColision(Bola &bola, int i, int j, std::vector<std::vector<Rectangulo>>&rectangulos) {
    Rectangulo* rectangulo = &rectangulos[i][j];
    if (!rectangulo->isValid()) return false;

    // Obtener la posición de la bola y su radio
    cv::Point2d posicionBola = bola.getPosicion();
    int radioBola = bola.getRadio();

    // Obtener la posición y el tamaño del rectángulo
    cv::Point2d esquinaSuperiorIzquierda = rectangulo->getPosicion();
    cv::Size2d tamañoRectangulo = rectangulo->getTamaño();

    // Determinar los límites del rectángulo
    double izquierda = esquinaSuperiorIzquierda.x;
    double derecha = esquinaSuperiorIzquierda.x + tamañoRectangulo.width;
    double arriba = esquinaSuperiorIzquierda.y;
    double abajo = esquinaSuperiorIzquierda.y + tamañoRectangulo.height;

    // Calcular el punto más cercano en el interior del rectángulo
    double puntoCercanoX = std::max(izquierda, std::min(posicionBola.x, derecha));
    double puntoCercanoY = std::max(arriba, std::min(posicionBola.y, abajo));

    // Calcular la distancia entre el centro de la bola y el punto más cercano
    double distanciaX = posicionBola.x - puntoCercanoX;
    double distanciaY = posicionBola.y - puntoCercanoY;
    double distanciaCuadrada = distanciaX * distanciaX + distanciaY * distanciaY; //Mas eficiente que sqrt

    // Si la distancia es menor o igual al cuadrado del radio de la bola, hay colisión
    if (distanciaCuadrada > radioBola * radioBola)
        return false; //No hubo colision

    bool colisionLadoHorizontal = puntoCercanoY == arriba || puntoCercanoY == abajo;
    bool colisionLadoVertical = puntoCercanoX == izquierda || puntoCercanoX == derecha;

    //No aplicamos la normal del vector porque sabemos que este rectangulo NO puede rotar
    if (colisionLadoHorizontal && !colisionLadoVertical) {
        // Rebote en el lado superior o inferior
        bola.setVelocidad(cv::Point2d(bola.getVelocidad().x, -bola.getVelocidad().y));
        // Marcar el rectángulo como destruido o inválido
        rectangulo->setInvalid();
        return true;  // Hubo colisión
    }
    if (colisionLadoVertical && !colisionLadoHorizontal) {
        // Rebote en el lado izquierdo o derecho
        bola.setVelocidad(cv::Point2d(-bola.getVelocidad().x, bola.getVelocidad().y));
        // Marcar el rectángulo como destruido o inválido
        rectangulo->setInvalid();
        return true;  // Hubo colisión
    }
    // Colisión en esquina
    cv::Point2d esquinaMasCercana(puntoCercanoX, puntoCercanoY);
    int caso;
    if(esquinaMasCercana.x == rectangulo->getPosicion().x){
        if(esquinaMasCercana.y == rectangulo->getPosicion().y){
            caso = SUPERIOR_IZQUIERDA;
        }else{
            caso = INFERIOR_IZQUIERDA;
        }
    }else{
        if(esquinaMasCercana.y == rectangulo->getPosicion().y){
            caso = SUPERIOR_DERECHA;
        }else{
            caso = INFERIOR_DERECHA;
        }
    }

    // Verificar si hay un rectángulo adyacente en la posición de la esquina
    if (esRectanguloAdyacente(bola, rectangulos, i, j, caso)) {
        // Si hay un rectángulo adyacente, tratamos la colisión como un rebote en lado
        if (std::abs(distanciaX) > std::abs(distanciaY)) {
            bola.setVelocidad(cv::Point2d(-bola.getVelocidad().x, bola.getVelocidad().y));
            // Marcar el rectángulo como destruido o inválido
            rectangulo->setInvalid();
            return true;  // Hubo colisión
        }
        bola.setVelocidad(cv::Point2d(bola.getVelocidad().x, -bola.getVelocidad().y));
        // Marcar el rectángulo como destruido o inválido
        rectangulo->setInvalid();
        return true;  // Hubo colisión
    }

    // Si no hay rectángulo adyacente, se trata como colisión en esquina
    cv::Point2d normal(0, 0);
    switch (caso) {
    case INFERIOR_IZQUIERDA:
        normal = Point2d(-1.0, 1.0); // Hacia arriba y hacia la izquierda
        break;
    case INFERIOR_DERECHA:
        normal = Point2d(1.0, 1.0);  // Hacia arriba y hacia la derecha
        break;
    case SUPERIOR_IZQUIERDA:
        normal = Point2d(-1.0, -1.0); // Hacia abajo y hacia la izquierda
        break;
    case SUPERIOR_DERECHA:
        normal = Point2d(1.0, -1.0);  // Hacia abajo y hacia la derecha
        break;
    }
    normal /= cv::norm(normal);
    cv::Point2d velocidadBola = bola.getVelocidad();
    double productoPunto = velocidadBola.dot(normal);

    if(productoPunto > 0) return false; //No hubo colision

    // Aplicar la fórmula de reflexión: v_reflejado = v - 2 * (v · n) * n
    cv::Point2d velocidadReflejada = velocidadBola - 2 * productoPunto * normal;
    bola.setVelocidad(velocidadReflejada);
    // Marcar el rectángulo como destruido o inválido
    rectangulo->setInvalid();
    return true;  // Hubo colisión
}

//Devuelve el numero de colisiones en el frame
bool manejarColisiones(Bola &bola, std::vector<std::vector<Rectangulo>> &rectangulos , int filas, int columnas, int limite_y){
    if(bola.getPosicion().y > limite_y)
        return false;

    for(int i=0; i<filas; i++){
        for(int j=0; j<columnas; j++){
            if(manejarColision(bola, i, j, rectangulos)){
                //Cada frame solo buscamos 1 colision
                return true;
            }
        }
    }
    return false;
}

bool manejarColision(Bola &bola, cv::RotatedRect &rectangulo) {
    // Obtener la posición de la bola y su radio
    cv::Point2d posicionBola = bola.getPosicion();
    int radioBola = bola.getRadio();

    // Obtener los vértices del RotatedRect
    Point2d vertices[4];
    getRotatedRectPoints(rectangulo, vertices);

    // Variable para determinar si se produce una colisión con una esquina
    bool colisionConEsquina = false;
    cv::Point2d esquinaColisionada;
    int indiceEsquina;

    // Verificar si la colisión es con alguna esquina
    for (int i = 0; i < 4; i++) {
        double distanciaEsquina = cv::norm(posicionBola - vertices[i]);
        if (distanciaEsquina <= radioBola) {
            colisionConEsquina = true;
            esquinaColisionada = vertices[i];
            indiceEsquina = i;
            break;
        }
    }

    if (colisionConEsquina) {
        // Obtener los dos vectores adyacentes a la esquina
        cv::Point2d vectorA = vertices[(indiceEsquina - 1+4) % 4] - vertices[indiceEsquina];
        cv::Point2d vectorB = vertices[indiceEsquina] - vertices[(indiceEsquina + 1) % 4];

        // Calcular las normales perpendiculares de cada vector
        cv::Point2d normalA(-vectorA.y, vectorA.x);
        cv::Point2d normalB(-vectorB.y, vectorB.x);

        // Normalizar ambas normales
        normalA /= cv::norm(normalA);
        normalB /= cv::norm(normalB);

        // Calcular la normal de la esquina (que apunta hacia afuera)
        cv::Point2d normal = normalA + normalB;
        normal /= cv::norm(normal);  // Normalizar la normal promedio

        // Obtener la velocidad actual de la bola
        cv::Point2d velocidadBola = bola.getVelocidad();
        // Aplicar la fórmula de reflexión: v_reflejado = v - 2 * (v · n) * n
        double productoPunto = velocidadBola.dot(normal);

        //Si hay colision los vectores tienen que apuntar en direcciones opuestas
        if(productoPunto > 0) return false;

        cv::Point2d velocidadReflejada = velocidadBola - 2 * productoPunto * normal;
        // Establecer la nueva velocidad de la bola
        bola.setVelocidad(velocidadReflejada);
        return true;  // Hubo colisión con la esquina
    }

    // Si no hay colisión con la esquina, seguimos con la detección de colisión con los lados
    double distanciaMinima = std::numeric_limits<double>::max();
    cv::Point2d puntoMasCercano;
    cv::Point2d puntoA, puntoB;

    for (int i = 0; i < 4; i++) {
        // Vértices adyacentes forman los lados del rectángulo
        cv::Point2d verticeA = vertices[i];
        cv::Point2d verticeB = vertices[(i + 1) % 4];

        // Calcular la distancia desde el centro de la bola a la línea formada por (verticeA, verticeB)
        cv::Point2d puntoCercanoTemp = calcularPuntoMasCercano(posicionBola, verticeA, verticeB);

        // Calcular la distancia al punto más cercano
        double distancia = cv::norm(posicionBola - puntoCercanoTemp);
        if (distancia < distanciaMinima) {
            distanciaMinima = distancia;
            puntoMasCercano = puntoCercanoTemp;
            puntoA = verticeA;
            puntoB = verticeB;
        }
    }

    // Verificar si la distancia mínima es menor o igual al radio de la bola (colisión con un lado)
    if (distanciaMinima <= radioBola) {
        // Calcular el vector normal al lado más cercano
        cv::Point2d vectorLado = puntoA - puntoB;
        cv::Point2d normal(-vectorLado.y, vectorLado.x);  // Normal perpendicular al lado

        // Normalizar el vector normal
        normal /= cv::norm(normal);

        // Obtener la velocidad actual de la bola
        cv::Point2d velocidadBola = bola.getVelocidad();

        // Calcular el producto escalar para verificar la dirección
        double productoPunto = velocidadBola.dot(normal);

        //Si hay colision los vectores tienen que apuntar en direcciones opuestas
        if(productoPunto > 0) return false; //En este caso la bola ya se mueve hacia afuera del RotaredRect

        // Aplicar la fórmula de reflexión: v_reflejado = v - 2 * (v · n) * n
        cv::Point2d velocidadReflejada = velocidadBola - 2 * productoPunto * normal;

        // Establecer la nueva velocidad de la bola
        bola.setVelocidad(velocidadReflejada);

        return true;  // Hubo colisión con un lado
    }

    return false;  // No hubo colisión
}

