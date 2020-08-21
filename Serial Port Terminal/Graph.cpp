#include "Graph.h"

Graph::Graph(QChartView* chartView, QString name)
{
	graph = chartView;
	seriesPrev = new QLineSeries();
	series = new QLineSeries();
	//series = new QScatterSeries();
	chart = new QChart();

	timeScale = 100;
	maxPoints = 2000;
	trigger = risingEdge;
	triggerLevel = 20;

	blue = new QColor(0, 51, 128);
	green = new QColor(64, 195, 0);

	seriesPrev->setColor(*green);
	series->setColor(*blue);

	//seriesPrev->setUseOpenGL(true);
	//series->setUseOpenGL(true);

	chart->legend()->hide();
	//chart->addSeries(seriesPrev);
	//chart->addSeries(series);

	xAxis = new QValueAxis();
	yAxis = new QValueAxis();

	xAxis->setMin(0);
	xAxis->setMax(timeScale);
	yAxis->setMin(0);
	yAxis->setMax(1023);

	chart->addAxis(xAxis, Qt::AlignBottom);
	chart->addAxis(yAxis, Qt::AlignLeft);

	/*
	seriesPrev->attachAxis(xAxis);
	seriesPrev->attachAxis(yAxis);
	series->attachAxis(xAxis);
	series->attachAxis(yAxis);
	*/

	chart->setTitle(name);

	graph->setChart(chart);
	graph->setRenderHint(QPainter::Antialiasing);
}

void Graph::plotData(const QByteArray& data)
{
	int x = 0;
	static int xPrev = 0;
	int y = 0;
	int dataPos = 0;
	int bufferNumPos = 0;
	char bufferNum[16] = { 0 };

	if (data.front() == '[' && data.back() == ']')
	{
		while (dataPos <= data.size() - 2)
		{
			while (data.at(dataPos) == '[' || data.at(dataPos) == ']')
			{
				dataPos++;
			}

			while (data.at(dataPos) != ',')
			{
				bufferNum[bufferNumPos] = data.at(dataPos);

				dataPos++;
				bufferNumPos++;
			}

			bufferNum[bufferNumPos] = '\0';

			x = atoi(bufferNum) % timeScale;

			if (x < xPrev)
			{
				restartSeries();
			}

			xPrev = x;

			strcpy(bufferNum, "");
			bufferNumPos = 0;
			dataPos++;

			while (data.at(dataPos) != ']')
			{
				bufferNum[bufferNumPos] = data.at(dataPos);

				dataPos++;
				bufferNumPos++;
			}

			bufferNum[bufferNumPos] = '\0';

			y = atoi(bufferNum);

			strcpy(bufferNum, "");
			bufferNumPos = 0;

			addPoint(QPoint(x, y));
		}
	}
}

void Graph::addPoint(QPoint point)
{
	static int yPrev = 0;
	static int xOffset = 0;

	if (series->count() > maxPoints)
	{
		series->removePoints(0, series->count() - maxPoints);
	}

	if (series->count() == 0)
	{
		switch (trigger)
		{
		case untriggered:
			series->append(point);
			yPrev = point.y();
			break;

		case risingEdge:
			if (yPrev < point.y() && point.y() - yPrev > triggerLevel)
			{
				xOffset = point.x();
				point.setX(point.x() - xOffset);
				series->append(point);
				yPrev = point.y();
			}
			else
			{
				yPrev = point.y();
				return;
			}
			break;

		case fallingEdge:
			if (yPrev > point.y() && yPrev - point.y() > triggerLevel)
			{
				xOffset = point.x();
				point.setX(point.x() - xOffset);
				series->append(point);
				yPrev = point.y();
			}
			else
			{
				yPrev = point.y();
				return;
			}
			break;
		}
	}
	else
	{
		point.setX(point.x() - xOffset);
		series->append(point);
		yPrev = point.y();
	}

	if (seriesPrev->count() > 1)
	{
		for (int i = 0; i < seriesPrev->count() - 1; i++)
		{
			if (seriesPrev->at(0).x() <= point.x())
			{
				seriesPrev->remove(0);
			}
			else
			{
				break;
			}
		}
	}

	redrawGraph();
}

void Graph::redrawGraph()
{
	chart->removeSeries(seriesPrev);
	chart->removeSeries(series);

	chart->addSeries(seriesPrev);
	chart->addSeries(series);

	seriesPrev->attachAxis(xAxis);
	seriesPrev->attachAxis(yAxis);
	series->attachAxis(xAxis);
	series->attachAxis(yAxis);

	graph->setChart(chart);
}

void Graph::restartSeries()
{
	QLineSeries* temp = seriesPrev;
	seriesPrev = series;
	series = temp;

	series->clear();

	seriesPrev->setColor(*green);
	series->setColor(*blue);
}

void Graph::setTimeScale(int time)
{
	timeScale = time;

	xAxis->setMax(time);
	//series->detachAxis(series->attachedAxes().first());
	//series->attachAxis(xAxis);
}

int Graph::getTimeScale()
{
	return timeScale;
}

void Graph::setTrigger(int trigger)
{
	this->trigger = trigger;
}

int Graph::getTrigger()
{
	return trigger;
}