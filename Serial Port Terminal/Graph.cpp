#include "Graph.h"

Graph::Graph(QChartView* chartView, QString name)
{
	graph = chartView;
	seriesPrev = new QLineSeries();
	series = new QLineSeries();
	//series = new QScatterSeries();
	chart = new QChart();

	timeScale = 100;
	//maxPoints = 300;	// max 300 without OpenGL
	maxPoints = 5000;	// more with OpenGL
	trigger = untriggered;
	triggerLevel = 20;

	//blue = new QColor(0, 51, 128);
	blue = new QColor(0, 106, 206);
	green = new QColor(64, 195, 0);

	QPen pen = series->pen();
	pen.setWidth(2);

	series->setPen(pen);
	seriesPrev->setPen(pen);

	seriesPrev->setColor(*green);
	series->setColor(*blue);

	seriesPrev->setUseOpenGL(true);
	series->setUseOpenGL(true);

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

	chart->addSeries(seriesPrev);
	chart->addSeries(series);

	seriesPrev->attachAxis(xAxis);
	seriesPrev->attachAxis(yAxis);
	series->attachAxis(xAxis);
	series->attachAxis(yAxis);

	chart->setTitle(name);

	graph->setChart(chart);
	graph->setRenderHint(QPainter::Antialiasing);
}

void Graph::plotData(const QByteArray& data)
{
	int x = 0;
	static int xPrev = 0;
	int y = 0;
	int dataPos = 1;
	int bufferNumPos = 0;
	char bufferNum[16] = { 0 };

	QList<QByteArray>* dataPoints = new QList<QByteArray>();

	for (int i = 0; i < data.size(); i++)
	{
		if (data.at(i) == '[')
		{
			QByteArray* dataPoint = new QByteArray();

			dataPoint->append(data.at(i));

			while (data.at(i) != ']' && i < data.size() - 1)
			{
				i++;

				dataPoint->append(data.at(i));
			}

			if (dataPoint->endsWith(']'))
			{
				dataPoints->append(*dataPoint);
			}

			dataPoint->~QByteArray();
		}
	}

	for (int i = 0; i < dataPoints->size(); i++)
	{
		dataPos = 1;
		strcpy(bufferNum, "");
		bufferNumPos = 0;

		while (QChar::isDigit(dataPoints->at(i).at(dataPos)))
		{
			bufferNum[bufferNumPos] = dataPoints->at(i).at(dataPos);

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

		while (QChar::isDigit(dataPoints->at(i).at(dataPos)))
		{
			bufferNum[bufferNumPos] = dataPoints->at(i).at(dataPos);

			dataPos++;
			bufferNumPos++;
		}

		bufferNum[bufferNumPos] = '\0';

		y = atoi(bufferNum);

		addPoint(QPoint(x, y));
	}

	dataPoints->~QList();
}

void Graph::addPoint(QPoint point)
{
	static int yPrev = 0;
	static int xTriggerOffset = 0;

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
				xTriggerOffset = point.x();
				point.setX(point.x() - xTriggerOffset);
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
				xTriggerOffset = point.x();
				point.setX(point.x() - xTriggerOffset);
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
		point.setX(point.x() - xTriggerOffset);
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