#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QDebug>
#include <QObject>
#include <QFuture>
#include <QRandomGenerator>
#include <QThread>
#include <QThreadPool>

QT_CHARTS_USE_NAMESPACE

class TaskControl
{
public:
    TaskControl(QFutureInterfaceBase *f) : fu(f) { }
    bool shouldRun() const { return !fu->isCanceled(); }
    bool shouldPause() const { return !fu->isPaused(); }

private:
    QFutureInterfaceBase *fu;
};

template <class T>
class ControllableTask : public QObject
{
public:
    virtual ~ControllableTask() {}
    virtual T run(TaskControl& control) = 0;
};

template <typename T>
class RunControllableTask : public QFutureInterface<T> , public QRunnable
{
public:
    RunControllableTask(ControllableTask<T>* tsk) : task(tsk) { }
    virtual ~RunControllableTask() { delete task; }

    QFuture<T> start() {
        this->setRunnable(this);
        this->reportStarted();
        QFuture<T> future = this->future();
        QThreadPool::globalInstance()->start(this, /*m_priority*/ 0);
        return future;
    }

    void run() {
        if (this->isCanceled()) {
            this->reportFinished();
            return;
        }
        TaskControl control(this);
        result = this->task->run(control);
        if (!this->isCanceled()) {
            this->reportResult(result);
        }        
        this->reportFinished();
    }

    T result;
    ControllableTask<T> *task;
};

class TaskExecutor {
public:
    template <class T>
    static QFuture<T> run(ControllableTask<T>* task) {
        return (new RunControllableTask<T>(task))->start();
    }
};

class RandomGenerator : public ControllableTask<QPointF>
{
    Q_OBJECT
public:
    RandomGenerator(QObject *parent = nullptr) { Q_UNUSED(parent) qInfo() << "Constructed" << QThread::currentThread();};

    virtual QPointF run(TaskControl &control) {
        QPointF point;
        while (control.shouldRun() && control.shouldPause()) {
            point = QPointF(QRandomGenerator::global()->bounded(0, 10), QRandomGenerator::global()->bounded(0, 10));
            QThread::currentThread()->setObjectName("Other thread");
            qInfo() << "Starting" << QThread::currentThread() << "generated: " << point;
            QThread::currentThread()->sleep(1);
            emit generated(point);
        }
        return point;
    }

signals:
    void generated(QPointF point);
};

class ChartView : public QChartView
{
    Q_OBJECT
public:
    explicit ChartView(QWidget *parent = 0);
    ~ChartView() {};

public slots:
    void start();
    void pause();
    void stop();
    void addValue(QPointF point);

private:
    QScatterSeries *series;
    QFuture<QPointF> future;
    RandomGenerator *randomGenerator;
};

#endif // CHARTVIEW_H
