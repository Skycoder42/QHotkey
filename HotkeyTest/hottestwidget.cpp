#include "hottestwidget.h"
#include "ui_hottestwidget.h"

HotTestWidget::HotTestWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::HotTestWidget)
{
	ui->setupUi(this);
}

HotTestWidget::~HotTestWidget()
{
	delete ui;
}
