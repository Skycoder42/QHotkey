#include "hottestwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HotTestWidget w;
	w.show();

	return a.exec();
}
