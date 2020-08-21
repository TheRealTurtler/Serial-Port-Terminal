#pragma once

#include <QtWidgets/QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QList>
#include <QString>
#include <QMessageBox>
#include <QByteArray>
#include <QTextCursor>
#include <Qclipboard>
#include <QFileDialog>
#include <QTextStream>
#include <stdlib.h>
#include "ui_MainWindow.h"
#include "Terminal.h"
#include "Graph.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);

	void getPortSettings();
	void getPorts();
	void getBaudRates();
	void getDataBits();
	void getParity();
	void getStopBits();
	void getFlowControl();
	void getTrigger();

private slots:
	void openPort();
	void closePort();
	void writeData();
	void readData();
	void clearTerminal();
	void saveTerminal();
	void copyTerminal();
	void timeScaleChanged(int value);
	void showReceivedDataChanged(int checkState);
	void plotDataChanged(int checkState);
	void autoNewLinesChanged(int checkState);
	void triggerChanged(int trigger);

private:
	Ui::MainWindowClass ui;
	QSerialPort* serialPort;

	QClipboard* clipboard;

	Terminal* terminal;
	Graph* graph;

	bool showReceivedData;
	bool plotData;
};
