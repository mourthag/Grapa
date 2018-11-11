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

    void addData(float x, float y);

private:
    QValueAxis *xAxis;

    QChartView *chartView;
    QChart *chart;
    QLineSeries *valueSeries;
    QLineSeries *axisSeries;
    QAreaSeries *areaSeries;

    float maxY;
};

#endif // PERFORMANCECHART_H
