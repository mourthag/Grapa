#include "performancechart.h"

PerformanceChart::PerformanceChart()
{


    maxY = 500;
    xAxis = new QValueAxis();
    xAxis->setRange(0,10);

    valueSeries = new QLineSeries();
    axisSeries = new QLineSeries();

    valueSeries->append(0,0);
    axisSeries->append(0,0);

    areaSeries = new QAreaSeries(valueSeries, axisSeries);

    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(areaSeries);
    chart->setTheme(QChart::ChartThemeDark);
    chart->createDefaultAxes();
    chart->setAxisX(xAxis,areaSeries);
    chart->setTitle("Drawing Time");

    chart->axisY()->setRange(0, 500);
    chart->axisX()->setLabelsVisible(false);
    chart->axisY()->setLabelsVisible(false);

    QPen pen(Qt::red);
    areaSeries->setPen(pen);

    QLinearGradient fillGradient(QPointF(0,0), QPointF(0,1));
    fillGradient.setColorAt(0.0, QColor(255,0,0));
    fillGradient.setColorAt(0.5, QColor(255,255,0));
    fillGradient.setColorAt(1.0, QColor(255,150,0));
    fillGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    areaSeries->setBrush(fillGradient);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(300, 150);

}

void PerformanceChart::addData(float x, float y) {
    valueSeries->append(x, y);
    axisSeries->append(x, 0);

    if(y > maxY) {
        maxY = y;
        chart->axisY()->setRange(0, 1.1 * maxY);
    }
    chart->axisX()->setRange(x - 3300, x + 10);

}

QChartView* PerformanceChart::getChartView() {
    return chartView;
}
