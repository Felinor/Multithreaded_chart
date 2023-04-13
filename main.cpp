#include <QApplication>
#include <QGraphicsScene>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QPushButton>
#include <QToolBar>
#include "chartview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ChartView *chartView = new ChartView;
    QMainWindow window;

    QPushButton startBtn;
    QPushButton pauseBtn;
    QPushButton stopBtn;

    startBtn.setText("Start");
    pauseBtn.setText("Pause");
    stopBtn.setText("Stop");

    QObject::connect(&startBtn, &QPushButton::clicked, chartView, &ChartView::start);
    QObject::connect(&pauseBtn, &QPushButton::clicked, chartView, &ChartView::pause);
    QObject::connect(&stopBtn, &QPushButton::clicked, chartView, &ChartView::stop);

    QToolBar *toolBar = new QToolBar;
    toolBar->addWidget(&startBtn);
    toolBar->addSeparator();
    toolBar->addWidget(&pauseBtn);
    toolBar->addSeparator();
    toolBar->addWidget(&stopBtn);
    window.addToolBar(Qt::BottomToolBarArea, toolBar);

    window.setCentralWidget(chartView);
    window.resize(800, 600);
    window.show();

    QThread::currentThread()->setObjectName("Main thread");
    qInfo() << "Starting" << QThread::currentThread();

    return a.exec();
}
