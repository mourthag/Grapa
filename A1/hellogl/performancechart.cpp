#include "performancechart.h"

PerformanceChart::PerformanceChart()
{


    maxY = 500;
    xAxis = new QValueAxis();
    xAxis->setRange(0,10);

    valueSeriesA = new QLineSeries();
    valueSeriesB = new QLineSeries();
    axisSeries = new QLineSeries();

    valueSeriesA->append(0,0);
    valueSeriesB->append(0,0);
    axisSeries->append(0,0);

    areaSeriesA = new QAreaSeries(valueSeriesA, axisSeries);
    areaSeriesB = new QAreaSeries(valueSeriesB, axisSeries);

    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(areaSeriesA);
    chart->addSeries(areaSeriesB);
    chart->setTheme(QChart::ChartThemeDark);
    chart->createDefaultAxes();
    chart->setTitle("Drawing Time");

    chart->axisY()->setRange(0, 500);
    chart->axisX()->setLabelsVisible(false);
    chart->axisY()->setLabelsVisible(false);

    QPen penA(Qt::red);
    areaSeriesA->setPen(penA);

    QLinearGradient fillGradientA(QPointF(0,0), QPointF(0,1));
    fillGradientA.setColorAt(0.0, QColor(255, 0, 0, 125));
    fillGradientA.setColorAt(1.0, QColor(255, 50, 0, 125));
    fillGradientA.setCoordinateMode(QGradient::ObjectBoundingMode);
    areaSeriesA->setBrush(fillGradientA);

    QPen penB(Qt::yellow);
    areaSeriesB->setPen(penB);

    QLinearGradient fillGradientB(QPointF(0,0), QPointF(0,1));
    fillGradientB.setColorAt(0.0, QColor(0, 255, 0, 125));
    fillGradientB.setColorAt(1.0, QColor(50, 255, 0, 125));
    fillGradientB.setCoordinateMode(QGradient::ObjectBoundingMode);
    areaSeriesB->setBrush(fillGradientB);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(300, 150);

}

void PerformanceChart::addData(float x, float y1, float y2) {
    valueSeriesA->append(x, y1);
    valueSeriesB->append(x, y2);
    axisSeries->append(x, 0);

    maxY = std::max(maxY, std::max(y1, y2));
    chart->axisY()->setRange(0, 1.1 * maxY);
    chart->axisX()->setRange(std::max(x - 3300.0, 0.0), x);

}

QChartView* PerformanceChart::getChartView() {
    return chartView;
}
