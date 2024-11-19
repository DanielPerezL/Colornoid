#include "bola.h"
#include "rectangulo.h"
#include "utils.h"
#include "QFileDialog"
#include "QColorDialog"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

//Declaracion de constantes del juego
const int NUM_VIDAS[]= {3,2,1};
int vidas=0;
string WINDOW_NAME = "Colornoid Gameplay";
const int RADIO_VIDAS = 15;
const int RADIO_BOLA = 12;
Scalar COLOR_BOLA = Scalar(255,0,0);
const int TECLA_ESC = 27;
const int ITERACIONES_INTERNAS = 10;
Point2d VELOCIDAD_BOLA = Point2d(0, 300);
const int FPS = 30; // FPS deseados
const double dt = 1.0 / FPS; // Valor de dt
const int FILAS_BLOQUES = 14;
const int COLUMNAS_BLOQUES = 10;
const String fases[] =
{
    {   //Fase 1
        "          "
        "RRRRRRRRRR"
        "IIIIIIIIII"
        "AAAAAAAAAA"
        "SSSSSSSSSS"
        "VVVVVVVVVV"
        "          "
        "          "
        "          "
        "          "
        "          "
        "          "
        "          "
        "          "
    },
    {   //Fase 2
        "          "
        "          "
        " RR AA MM "
        " RR AA MM "
        "          "
        "  SS  VV  "
        "  SS  VV  "
        "          "
        " II RR AA "
        " II RR AA "
        "          "
        "          "
        "          "
        "          "
    },
    {   //Fase 3
        "    V   V "
        "   VV  VV "
        "   VV VV  "
        "     V    "
        "    V     "
        "   RRRR   "
        "  RRRRRR  "
        " RRRRRRRR "
        " RRRRRRRR "
        " RRRRRRRR "
        "  RRRRRR  "
        "   RRRR   "
        "          "
        "          "
    },
    {   //Fase 4
        "NNNNNNNNNN"
        "RRRRRRRRRR"
        "IIIIIIIIII"
        "AAAAAAAAAA"
        "SSSSSSSSSS"
        "VVVVVVVVVV"
        "GGGGGGGGGG"
        "MMMMMMMMMM"
        "RRRRRRRRRR"
        "NNNNNNNNNN"
        "          "
        "          "
        "A        A"
        "NNNN  NNNN"
    }
};

//Propiedades encargadas de seleccionar el color
bool selectObject = false;
int trackObject = 0;
Rect selection;
Point origin;
int vmin = 10, vmax = 256, smin = 100;
Mat video_frame;

//El usuario dibuja un cuadro alrededor del objeto para rastrear. Esto activa CAMShift para comenzar el seguimiento
static void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);

        selection &= Rect(0, 0, video_frame.cols, video_frame.rows);

    }

    switch( event )
    {
    case EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selection = Rect(x,y,0,0);
        selectObject = true;
        break;
    case EVENT_LBUTTONUP:
        selectObject = false;
        if( selection.width > 0 && selection.height > 0 )
            trackObject = -1;   // Set up CAMShift properties in main() loop
        break;
    }
}

//En esta variable almacenamos la posicion Y minima para que se den colisiones bola-rectangulo
int limite_y_de_colisiones = 0;

/*Generamos la fase a partir del espacio disponible
    - devuelve la puntuación maxima posible (útil para finalizar el juego)
*/
int generarBloques(std::vector<std::vector<Rectangulo>> &todos, int faseIndex, int anchoPantalla, int altoPantalla) {
    //Limpiamos la matriz de rectangulos
    todos.clear();

    if(FILAS_BLOQUES == 0 || COLUMNAS_BLOQUES == 0){
        return 0;
    }

    double margin = 2*RADIO_VIDAS + 6;
    altoPantalla*=0.4;
    limite_y_de_colisiones = altoPantalla+RADIO_BOLA+2;

    altoPantalla-=margin;

    const int anchoBloque = anchoPantalla / COLUMNAS_BLOQUES; // Cálculo del ancho del bloque
    const int altoBloque = altoPantalla/ FILAS_BLOQUES; // Cálculo del alto del bloque

    todos.resize(FILAS_BLOQUES, std::vector<Rectangulo>(COLUMNAS_BLOQUES, Rectangulo(Point2d(0,0),Size2d(0,0),Scalar(0,0,0))));
    String fase = fases[faseIndex];
    int punt_max = 0;

    for (int i = 0; i < FILAS_BLOQUES; ++i) {
        for (int j = 0; j < COLUMNAS_BLOQUES; ++j) {
            char actual = fase[i*COLUMNAS_BLOQUES+j];
            Scalar color;
            switch (actual)  {
                case 'N': //Negro
                    color = Scalar(0, 0, 0);
                    break;
                case 'R': //Rojo
                    color = Scalar(0, 0, 255);
                    break;
                case 'G': //Gris
                    color = Scalar(158, 158, 158);
                    break;
                case 'A': //Azul cian
                    color = Scalar(255, 255, 0);
                    break;
                case 'I': //Amarillo
                    color = Scalar(0, 255, 255);
                    break;
                case 'M': //Marron
                    color = Scalar(57, 84, 84);
                    break;
                case 'V': //Verde
                    color = Scalar(0, 250, 15);
                    break;
                case 'S': //Rosa
                    color = Scalar(250, 0, 250);
                    break;
                default:
                    //Espacio sin rectangulo (e.d. rectangulo invalido)
                    todos[i][j] = Rectangulo(cv::Point2d(0, 0), cv::Size(anchoBloque, altoBloque), Scalar(0,0,0));
                    todos[i][j].setInvalid();
                    continue;
            }
            double x = j * anchoBloque;
            double y = i * altoBloque;
            // Crear un nuevo bloque y almacenarlo en la matriz
            todos[i][j] = Rectangulo(cv::Point2d(x, y+margin), cv::Size(anchoBloque, altoBloque), color);
            punt_max++;
        }
    }
    return punt_max;
}

Bola inicializarBola(int window_width){
    return Bola(Point2d(window_width*0.5, limite_y_de_colisiones+4), VELOCIDAD_BOLA, RADIO_BOLA, COLOR_BOLA);
}

void imprimirVidas(Mat frame){
    int margin = 3+RADIO_VIDAS;
    int offset = 40;
    for(int i=0;i<vidas;i++){
        Bola b(Point(margin + offset*i, margin), Point2d(0,0), RADIO_VIDAS, COLOR_BOLA);
        b.imprimeBola(frame);
    }
}

int jugar(int faseIndex, int dificultadIndex){
    srand(time(NULL));

    Mat video_stream;
    VideoCapture camera;
    camera.open(0);
    if (!camera.read(video_stream))
        return 0;
    flip(video_stream, video_frame, 1);
    namedWindow(WINDOW_NAME, 0);

    //Se activa el seleccionador de color
    setMouseCallback( WINDOW_NAME, onMouse, 0 );

    //inicializar juego
    int puntuacion = 0;
    assert(dificultadIndex>=0 && dificultadIndex<3);
    vidas = NUM_VIDAS[dificultadIndex];
    int window_width = video_frame.cols;
    int window_height = video_frame.rows;
    vector<vector<Rectangulo>> todos;

    int PUNT_MAX = generarBloques(todos, faseIndex,window_width, window_height);
    Bola bola = inicializarBola(window_width);
    int jugar_activo = 0;

    trackObject = 0;
    Mat hsv, mask, hist, hue, backproj, histimg = Mat::zeros(200, 320, CV_8UC3);
    Rect trackWindow;
    RotatedRect trackBox;


    //Parametros del histograma
    int hsize = 16;
    //UNICO PUNTO DONDE USAMOS FLOATS (lo exige openCV)
    float hranges[] = {0,180};
    const float* phranges = hranges;

    while(vidas>0 && puntuacion<PUNT_MAX && waitKey(1)!=TECLA_ESC){
        int64 inicio = getTickCount();
        if (!camera.read(video_stream))
            break;
        //Trabajar con video_frame para que sea mas intuitivo
        flip(video_stream, video_frame, 1);

        //Si se ha seleccionado el trackObject podemos aplicar algoritmo CAMshift
        cvtColor(video_frame, hsv, cv::COLOR_BGR2HSV);
        if( trackObject )
        {
            int _vmin = vmin, _vmax = vmax;
            inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
                    Scalar(180, 256, MAX(_vmin, _vmax)), mask);
            int ch[] = {0, 0};
            hue.create(hsv.size(), hsv.depth());
            mixChannels(&hsv, 1, &hue, 1, ch, 1);
            //Nuevo objeto seleccionado
            if( trackObject < 0 )
            {
                // El usuario ha seleccionado un objeto, configurar las propiedades de búsqueda de CAMShift una vez.
                Mat roi(hue, selection);
                Mat maskroi(mask, selection);
                calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
                normalize(hist, hist, 0, 255, NORM_MINMAX);

                trackWindow = selection;
                trackObject = 1; // Don't set up again, unless user selects new ROI

                histimg = Scalar::all(0);
                int binW = histimg.cols / hsize;
                Mat buf(1, hsize, CV_8UC3);
                for( int i = 0; i < hsize; i++ )
                    buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
                cvtColor(buf, buf, COLOR_HSV2BGR);

                for( int i = 0; i < hsize; i++ )
                {
                    int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows/255);
                    rectangle( histimg, Point(i*binW,histimg.rows),
                              Point((i+1)*binW,histimg.rows - val),
                              Scalar(buf.at<Vec3b>(i)), -1, 8 );
                }
                jugar_activo = 1;
            }

            // Perform CAMShift
            calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
            backproj &= mask;

            //Actualizamos la hitbox del jugador (cv::RotatedRect)
            trackBox = CamShift(backproj, trackWindow,
                                            TermCriteria( TermCriteria::EPS | TermCriteria::COUNT, 10, 1 ));
            if( trackWindow.area() <= 1 )
            {
                int cols = backproj.cols;
                int rows = backproj.rows;
                int r = (MIN(cols, rows) + 5)/6;
                trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
                                   trackWindow.x + r, trackWindow.y + r) &
                              Rect(0, 0, cols, rows);

            }
        }

        if(jugar_activo){
            //Por cada frame hacemos n ITERACIONES_INTERNAS de modo que ganamos precision
            for(int i=0; i<ITERACIONES_INTERNAS ;i++){
                //Actualizar estado de juego
                bola.mover(dt/ITERACIONES_INTERNAS);

                //Ver si la bola sale de la pantalla (perder vida)
                if(bola.comprobarBorde(window_width, window_height)){
                    vidas--;
                    bola = inicializarBola(window_width);
                    continue;
                }
                //Comprobacion colisiones Bola-Jugador
                manejarColision(bola, trackBox);

                //Comprobar colisiones Bola-Rectangulo
                if(manejarColisiones(bola, todos, FILAS_BLOQUES, COLUMNAS_BLOQUES, limite_y_de_colisiones))
                    puntuacion++;
            }

            //Impirmir elementos
            for(int i=0; i<FILAS_BLOQUES; i++){
                for(int j=0; j<COLUMNAS_BLOQUES; j++){
                    Rectangulo* rect = &todos[i][j];
                    if(rect->isValid()){
                        rect->imprimeRectangulo(video_frame);
                    }
                }
            }
            bola.imprimeBola(video_frame);
            imprimirVidas(video_frame);

            //Imprimir hitbox del jugador
            imprimirRotatedRect(video_frame, trackBox);
        }

        //Para cuando se selecciona un color
        if( selectObject && selection.width > 0 && selection.height > 0 )
        {
            Mat roi(video_frame, selection);
            bitwise_not(roi, roi);
        }

        imshow(WINDOW_NAME, video_frame);

        int64 fin = getTickCount();
        double tiempoTranscurrido = (fin - inicio) / getTickFrequency(); // Convertir a segundos
        // Ajustar el tiempo de espera para mantener el FPS constante
        if (tiempoTranscurrido < dt) {
            // Esperar el tiempo restante
            int espera = static_cast<int>((dt - tiempoTranscurrido) * 1000); // Convertir a milisegundos

            // Asegurarse de que el tiempo de espera no sea negativo
            if (espera > 0) {
                waitKey(espera);  // Esperar el tiempo calculado
            }
        }
        // Si el tiempo transcurrido ya es mayor al esperado, no esperar
    }

    destroyWindow(WINDOW_NAME);

    // Crear una ventana de felicitaciones SI Y SOLO SI GANA
    if (puntuacion == PUNT_MAX) {
        std::string ventana = "¡Felicidades!";
        cv::Mat imagenFelicidades(500, 800, CV_8UC3, cv::Scalar(24, 212, 59));

        // Agregar texto grande al centro de la imagen
        std::string mensaje = "Ganaste!";
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 2;
        int thickness = 3;
        int baseline = 0;

        // Calcular el tamaño del texto para centrarlo en la imagen
        cv::Size textSize = cv::getTextSize(mensaje, fontFace, fontScale, thickness, &baseline);
        cv::Point textOrg((imagenFelicidades.cols - textSize.width) / 2, (imagenFelicidades.rows + textSize.height) / 2);

        // Dibujar el texto en la imagen
        cv::putText(imagenFelicidades, mensaje, textOrg, fontFace, fontScale, cv::Scalar(255, 255, 255), thickness);

        // Mostrar la imagen
        cv::namedWindow(ventana, cv::WINDOW_NORMAL);
        cv::setWindowProperty(ventana, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

        // Mostrar la imagen
        cv::imshow(ventana, imagenFelicidades);

        // Esperar hasta que se presione una tecla
        cv::waitKey(-1);
        cv::destroyWindow(ventana);
    }

    return puntuacion;
}
