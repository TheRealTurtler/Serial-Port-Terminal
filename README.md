# Serial Port Terminal
Serial port terminal with visual data logging. Works great as oscilloscope for arduino or any other device that sends data over serial port.

# How to use
Simply plug in a device that communicates over the serial port, configure the port settings and click "Verbinden" (connect).

For text output check the check box above the terminal
that says "Empfangene Daten anzeigen" (show received data). To turn on automatic line breaks after each data point check "Automatische Zeilenumbrüche" (automatic line breaks).

If you want to visualize the received data, check the "Daten plotten" (plot data) checkbox above the graph and adjust the timescale with the "Zeitskala [ms]" (time scale [ms])
slider below the graph.

To trigger the data like on an oscilloscope, select a trigger mode in the "Trigger" selection above the graph.

# Data Formating
As of now, data that is supposed to be plotted has to be formated as "[x,y]" (without quotation marks) and must not be separated by anything else (meaning the data stream has to
look like this: [x1,y1][x2,y2][x3,y3]...

# About
This project was created with Qt.

Icons used: https://www.deviantart.com/danrabbit/art/elementary-Icons-65437279
