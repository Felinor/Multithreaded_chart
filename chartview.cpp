#include "chartview.h"
#include <QFutureWatcher>

ChartView::ChartView(QWidget *parent) :
    QChartView(new QChart(), parent)
{
    series = new QScatterSeries();
    series->setName("scatter series");
    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series->setMarkerSize(20.0);

    setRenderHint(QPainter::Antialiasing);
    chart()->addSeries(series);

    chart()->setTitle("Multithreaded chart");
    chart()->createDefaultAxes();
    chart()->axisY()->setMax(11);
    chart()->axisY()->setMin(-1);
    chart()->axisX()->setMax(11);
    chart()->axisX()->setMin(-1);
    chart()->setDropShadowEnabled(true);    

    chart()->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);
    chart()->setTheme(QChart::ChartThemeBlueCerulean);

    QThread::currentThread()->setObjectName("ChartView thread");
    qInfo() << "Starting" << QThread::currentThread();
}

void ChartView::start() {
    qDebug() << "Start generation";
    future = TaskExecutor::run(randomGenerator = new RandomGenerator(this));
    connect(randomGenerator, &RandomGenerator::generated, this, &ChartView::addValue);
}

void ChartView::pause() {
    qDebug() << "Pause generation";
    disconnect(randomGenerator);
    future.pause();
}

void ChartView::stop() {
    qDebug() << "Stop generation";
    future.cancel();
    disconnect(randomGenerator);
    series->clear();
}

void ChartView::addValue(QPointF point) {
    series->append(point);
}
