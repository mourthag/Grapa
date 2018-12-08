#ifndef PERFORMANCECHART_H
#define PERFORMANCECHART_H

#include <QLineSeries>
#include <QAreaSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QDebug>

using namespace QtCharts;

class PerformanceChart
{
public:
    PerformanceChart();

    QChartView *getChartView();

    void addData(float x, float y, int index);
    void addSeries(int amount);
    void setName(int index, QString name);

private:
    QValueAxis *xAxis;

    QChartView *chartView;
    QChart *chart;
    std::vector<QLineSeries*> series;

    float maxY;
};

#endif // PERFORMANCECHART_H
