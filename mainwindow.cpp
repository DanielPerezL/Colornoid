#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QColorDialog"
#include <opencv2/opencv.hpp>
#include "juego.h"
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playButton_clicked()
{
    int puntuacion = jugar(this->ui->comboBox_Fase->currentIndex(), this->ui->comboBox_Dificultad->currentIndex());
    this->ui->score->setText(QString::number(puntuacion));
}
