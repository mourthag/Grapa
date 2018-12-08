#include "performancechart.h"

PerformanceChart::PerformanceChart()
{
    maxY = 500;
    xAxis = new QValueAxis();
    xAxis->setRange(0,10);

    chart = new QChart();
    chart->addSeries(new QLineSeries);
    chart->setTheme(QChart::ChartThemeDark);
    chart->createDefaultAxes();
    chart->setTitle("Frame Time");

    chart->axisY()->setRange(0, 500);
    chart->removeAllSeries();

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(300, 250);

}
void PerformanceChart::setName(int index, QString name) {
    series.at(index)->setName(name);
}

void PerformanceChart::addSeries(int amount) {
    for(int i=0; i < amount; ++i) {
        QLineSeries *newSeries = (QLineSeries*)malloc(sizeof(QLineSeries));
        newSeries = new QLineSeries();
        chart->addSeries(newSeries);
        series.push_back(newSeries);
    }
    chart->createDefaultAxes();
}
void PerformanceChart::addData(float x, float y, int index) {
    series.at(index)->append(x,y);
    maxY = std::max(maxY, y);
    chart->axisY()->setRange(0, 1.1 * maxY);
    chart->axisX()->setRange(std::max(x - 3300.0, 0.0), x);

}

QChartView* PerformanceChart::getChartView() {
    return chartView;
}
