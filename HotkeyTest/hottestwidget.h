#ifndef HOTTESTWIDGET_H
#define HOTTESTWIDGET_H

#include <QWidget>

namespace Ui {
	class HotTestWidget;
}

class HotTestWidget : public QWidget
{
	Q_OBJECT

public:
	explicit HotTestWidget(QWidget *parent = 0);
	~HotTestWidget();

private:
	Ui::HotTestWidget *ui;
};

#endif // HOTTESTWIDGET_H
