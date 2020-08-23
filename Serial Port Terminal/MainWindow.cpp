#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	serialPort = new QSerialPort(this);

	connect(ui.pushButton_Connect, &QPushButton::clicked, this, &MainWindow::openPort);
	connect(ui.pushButton_Clear, &QPushButton::clicked, this, &MainWindow::clearTerminal);
	connect(ui.pushButton_Save, &QPushButton::clicked, this, &MainWindow::saveTerminal);
	connect(ui.pushButton_CopyAll, &QPushButton::clicked, this, &MainWindow::copyTerminal);
	connect(ui.horizontalSlider_ShownPoints, &QSlider::valueChanged, this, &MainWindow::timeScaleChanged);
	connect(ui.spinBox_ShownPoints, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::timeScaleChanged);
	connect(ui.horizontalSlider_ShownPoints, QOverload<int>::of(&QSlider::valueChanged), ui.spinBox_ShownPoints, QOverload<int>::of(&QSpinBox::setValue));
	connect(ui.spinBox_ShownPoints, QOverload<int>::of(&QSpinBox::valueChanged), ui.horizontalSlider_ShownPoints, QOverload<int>::of(&QSlider::setValue));
	connect(ui.checkBox_ShowReceivedData, &QCheckBox::stateChanged, this, &MainWindow::showReceivedDataChanged);
	connect(ui.checkBox_PlotData, &QCheckBox::stateChanged, this, &MainWindow::plotDataChanged);
	connect(ui.checkBox_AutoNewLine, &QCheckBox::stateChanged, this, &MainWindow::autoNewLinesChanged);
	connect(ui.comboBox_Trigger, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::triggerChanged);

	connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);

	ui.plainTextEdit_Terminal->setReadOnly(true);
	ui.plainTextEdit_Terminal->setTabStopDistance(QFontMetricsF(ui.plainTextEdit_Terminal->font()).horizontalAdvance(' ') * 6);
	ui.plainTextEdit_Terminal->setMaximumBlockCount(100);

	clipboard = QGuiApplication::clipboard();

	terminal = new Terminal(ui.plainTextEdit_Terminal);
	graph = new Graph(ui.graphicsView_Graph, QString::QString("Graph"));

	graph->statusBar = statusBar();

	showReceivedData = ui.checkBox_ShowReceivedData->isChecked();
	plotData = ui.checkBox_PlotData->isChecked();

	getPortSettings();

	getTrigger();
}

void MainWindow::getPortSettings()
{
	getPorts();
	getBaudRates();
	getDataBits();
	getParity();
	getStopBits();
	getFlowControl();
}

void MainWindow::getPorts()
{
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

	for (int i = 0; i < ports.size(); i++)
	{
		ui.comboBox_Port->addItem(ports[i].portName(), NULL);
	}

	int index = ui.comboBox_Port->findText("COM8");

	if (index != -1)
	{
		ui.comboBox_Port->setCurrentIndex(index);
	}
}

void MainWindow::getBaudRates()
{
	QList<qint32> baudRates = QSerialPortInfo::standardBaudRates();

	for (int i = 0; i < baudRates.size(); i++)
	{
		ui.comboBox_Baud->addItem(QString::number(baudRates[i]), NULL);
	}

	int index = ui.comboBox_Baud->findText("115200");

	if (index != -1)
	{
		ui.comboBox_Baud->setCurrentIndex(index);
	}
}

void MainWindow::getDataBits()
{
	for (int i = 5; i <= 8; i++)
	{
		ui.comboBox_DataBits->addItem(QString::number(i), NULL);
	}

	int index = ui.comboBox_DataBits->findText("8");

	if (index != -1)
	{
		ui.comboBox_DataBits->setCurrentIndex(index);
	}
}

void MainWindow::getParity()
{
	ui.comboBox_Parity->addItem(QString::QString("Gerade"), NULL);
	ui.comboBox_Parity->addItem(QString::QString("Ungerade"), NULL);
	ui.comboBox_Parity->addItem(QString::QString("Keine"), NULL);
	ui.comboBox_Parity->addItem(QString::QString("Markierung"), NULL);
	ui.comboBox_Parity->addItem(QString::QString("Leerzeichen"), NULL);

	int index = ui.comboBox_Parity->findText("Keine");

	if (index != -1)
	{
		ui.comboBox_Parity->setCurrentIndex(index);
	}
}

void MainWindow::getStopBits()
{
	ui.comboBox_StopBits->addItem(QString::QString("1"), NULL);
	ui.comboBox_StopBits->addItem(QString::QString("1.5"), NULL);
	ui.comboBox_StopBits->addItem(QString::QString("2"), NULL);

	int index = ui.comboBox_StopBits->findText("1");

	if (index != -1)
	{
		ui.comboBox_StopBits->setCurrentIndex(index);
	}
}

void MainWindow::getFlowControl()
{
	ui.comboBox_FlowControl->addItem(QString::QString("Software"), NULL);
	ui.comboBox_FlowControl->addItem(QString::QString("Hardware"), NULL);
	ui.comboBox_FlowControl->addItem(QString::QString("Keine"), NULL);

	int index = ui.comboBox_FlowControl->findText("Keine");

	if (index != -1)
	{
		ui.comboBox_FlowControl->setCurrentIndex(index);
	}
}

void MainWindow::getTrigger()
{
	ui.comboBox_Trigger->addItem(QString("ungetriggert"), NULL);
	ui.comboBox_Trigger->addItem(QString("steigend"), NULL);
	ui.comboBox_Trigger->addItem(QString("fallend"), NULL);
}

void MainWindow::openPort()
{
	serialPort->setPortName(ui.comboBox_Port->currentText());
	serialPort->setBaudRate(ui.comboBox_Baud->currentText().toInt());
	serialPort->setDataBits(QSerialPort::DataBits(ui.comboBox_DataBits->currentText().toInt()));
	serialPort->setParity(QSerialPort::NoParity);				// TODO
	serialPort->setStopBits(QSerialPort::OneStop);				// TODO
	serialPort->setFlowControl(QSerialPort::NoFlowControl);		// TODO

	if (serialPort->open(QIODevice::ReadWrite))
	{
		QString status = QString::QString("Successfully connected to ")
			+ ui.comboBox_Port->currentText();

		statusBar()->showMessage(status, 5000);

		disconnect(ui.pushButton_Connect, nullptr, nullptr, nullptr);
		connect(ui.pushButton_Connect, &QPushButton::clicked, this, &MainWindow::closePort);
		ui.pushButton_Connect->setText(QString::QString("Verbindung trennen"));
	}
	else
	{
		QMessageBox::critical(this, tr("Error"), serialPort->errorString());
	}
}

void MainWindow::closePort()
{
	if (serialPort->isOpen())
	{
		serialPort->close();

		statusBar()->showMessage("Disconnected", 5000);
	}

	disconnect(ui.pushButton_Connect, nullptr, nullptr, nullptr);
	connect(ui.pushButton_Connect, &QPushButton::clicked, this, &MainWindow::openPort);
	ui.pushButton_Connect->setText(QString::QString("Verbinden"));
}

void MainWindow::writeData()
{
	// TODO
}


void MainWindow::readData()
{
	const QByteArray data = serialPort->readAll();

	if (showReceivedData)
	{
		terminal->putData(data);
	}

	if (plotData)
	{
		graph->plotData(data);
	}
}

void MainWindow::clearTerminal()
{
	ui.plainTextEdit_Terminal->clear();
}

void MainWindow::saveTerminal()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save as", QDir::currentPath(), ("Text file (*.txt)"));
	QFile file(fileName);

	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
		return;
	}

	QTextStream out(&file);
	QString text = ui.plainTextEdit_Terminal->toPlainText();
	out << text;

	file.close();

	statusBar()->showMessage("File saved at " + fileName, 5000);
}

void MainWindow::copyTerminal()
{
	clipboard->setText(ui.plainTextEdit_Terminal->toPlainText());
}

void MainWindow::timeScaleChanged(int value)
{
	graph->setTimeScale(value);
}

void MainWindow::showReceivedDataChanged(int checkState)
{
	showReceivedData = (bool)checkState;
}

void MainWindow::plotDataChanged(int checkState)
{
	plotData = (bool)checkState;
}

void MainWindow::autoNewLinesChanged(int checkState)
{
	terminal->setAutoNewLines((bool)checkState);
}

void MainWindow::triggerChanged(int trigger)
{
	graph->setTrigger(trigger);
}