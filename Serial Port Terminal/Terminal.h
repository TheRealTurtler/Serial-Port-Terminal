#include <qplaintextedit.h>

class Terminal
{
public:
	explicit Terminal(QPlainTextEdit* textEdit = nullptr);

	void putData(const QByteArray& data);
	void addNewLines(QByteArray& data);

	void setMaxTextLength(int length);
	int getMaxTextLength();
	void setAutoNewLines(bool selection);
	bool getAutoNewLines();

private:
	QPlainTextEdit* terminal;
	int maxTextLength;
	bool autoNewLines;
};
