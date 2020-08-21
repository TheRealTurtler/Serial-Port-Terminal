#pragma once

#include <QtCharts>

class Graph
{
public:
	explicit Graph(QChartView* chartView = nullptr, QString name = NULL);

	void plotData(const QByteArray& data);
	void addPoint(QPoint point);

	void setTimeScale(int points);
	int getTimeScale();
	void setTrigger(int trigger);
	int getTrigger();

private:
	QChartView* graph;
	QLineSeries* seriesPrev;
	QLineSeries* series;
	//QScatterSeries* series;
	QChart* chart;

	QValueAxis* xAxis;
	QValueAxis* yAxis;

	QColor* blue;
	QColor* green;

	int timeScale;
	int maxPoints;
	int trigger;
	int triggerLevel;

	void redrawGraph();
	void restartSeries();

	enum Triggered
	{
		untriggered,
		risingEdge,
		fallingEdge
	};
};
