#include "changehierarchybutton.h"

#include "settingsprivate.h"
#include <QApplication>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QStylePainter>

#include <QtDebug>

ChangeHierarchyButton::ChangeHierarchyButton(QWidget *parent) :
	QPushButton(parent)
{}

void ChangeHierarchyButton::paintEvent(QPaintEvent *)
{
	QStylePainter p(this);
	QColor base = QApplication::palette().base().color();

	p.save();
	if (rect().contains(mapFromGlobal(QCursor::pos()))) {
		p.setPen(Qt::NoPen);
		p.setBrush(QApplication::palette().highlight());
		p.drawRect(this->rect());
		p.setCompositionMode(QPainter::CompositionMode_DestinationAtop);
		p.drawPixmap(this->rect(), QPixmap(":/icons/hierarchy"));
		p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	}

	// Gradient
	QLinearGradient g(rect().topLeft(), rect().bottomLeft());
	if (SettingsPrivate::instance()->isCustomColors()) {
		g.setColorAt(0, base.lighter(110));
		g.setColorAt(1, base);
	} else {
		g.setColorAt(0, base);
		g.setColorAt(1, QApplication::palette().window().color());
	}
	p.fillRect(rect(), g);
	p.restore();

	if (!rect().contains(mapFromGlobal(QCursor::pos()))) {
		p.drawPixmap(this->rect(), QPixmap(":/icons/hierarchy"));
	}

	// Draw a thin line before the treeview
	p.save();
	p.setPen(base.darker(115));
	p.drawLine(rect().bottomLeft(), rect().bottomRight());
	p.restore();

	// Border
	p.setPen(QApplication::palette().mid().color());
	if (isLeftToRight()) {
		p.drawLine(rect().topRight(), rect().bottomRight());
	} else {
		p.drawLine(rect().topLeft(), rect().bottomLeft());
	}
}

void ChangeHierarchyButton::resizeEvent(QResizeEvent *event)
{
	if (event->size().height() != this->width()) {
		this->setMinimumWidth(event->size().height());
	}
}
