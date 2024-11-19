#ifndef UTILS_H
#define UTILS_H
#include <opencv2/opencv.hpp>
#include "bola.h"
#include "rectangulo.h"
using namespace cv;

int aleatorio(int minimum_number, int max_number);

int aleatorio(int n);

void imprimirRotatedRect(Mat frame, RotatedRect r);

bool manejarColisiones(Bola &bola, std::vector<std::vector<Rectangulo>> &rectangulos, int filas, int columnas, int limite_y);

bool manejarColision(Bola &bola, RotatedRect &rectangulo);

#endif // UTILS_H
