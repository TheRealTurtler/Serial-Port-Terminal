#include "Terminal.h"

Terminal::Terminal(QPlainTextEdit* textEdit)
{
	terminal = textEdit;
	maxTextLength = 2000;
	autoNewLines = false;
}

void Terminal::putData(const QByteArray& data)
{
	QTextCursor cursor = terminal->textCursor();
	QString text = terminal->toPlainText();

	if (text.length() > maxTextLength)
	{
		text.remove(0, text.length() - maxTextLength);

		terminal->setPlainText(text);

		cursor.movePosition(QTextCursor::End);
		terminal->setTextCursor(cursor);
	}
	else
	{
		cursor.movePosition(QTextCursor::End);
		terminal->setTextCursor(cursor);

		if (autoNewLines)
		{
			QByteArray dataNewLines = data;
			addNewLines(dataNewLines);
			terminal->insertPlainText(dataNewLines);
		}
		else
		{
			terminal->insertPlainText(data);
			//terminal->appendPlainText(data);
		}
	}
}

void Terminal::addNewLines(QByteArray& data)
{
	for (int i = 0; i < data.size(); i++)
	{
		if (data.at(i) == ']')
		{
			data.insert(i + 1, "\r\n");
		}
	}
}

void Terminal::setMaxTextLength(int length)
{
	maxTextLength = length;
}

int Terminal::getMaxTextLength()
{
	return maxTextLength;
}

void Terminal::setAutoNewLines(bool selection)
{
	autoNewLines = selection;
}

bool Terminal::getAutoNewLines()
{
	return autoNewLines;
}