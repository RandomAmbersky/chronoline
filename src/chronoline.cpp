#include <iostream>
#include <QMessageBox>
#include "chronoline.h"

ChronoLine::ChronoLine(QWidget *parent) :
    QGraphicsView(parent),
    _lockAutoUpdate(true),
    idSequencer(0)
{
    scene = new QGraphicsScene;
    setScene(scene);
    setBackgroundColor(Qt::cyan);
    // Time Line
    timeLine = new CLTimeLine();
    scene->addItem(timeLine);
    _lockAutoUpdate = false;
}

void ChronoLine::updateAll()
{
    if (timeLine->calcScale(childrenRect()))
        scene->update();
    else
        QMessageBox::critical(0, QString::fromUtf8("Ошибка"),
            QString::fromUtf8("Слишком большая единица измерения для выбранного периода либо неверная дата окончания"));
}

void ChronoLine::setBackgroundColor(QColor c)
{
    setBackgroundBrush(QBrush(c, Qt::SolidPattern));
    if (!_lockAutoUpdate) updateAll();
}

void ChronoLine::lockAutoUpdate()
{
    _lockAutoUpdate = true;
}

void ChronoLine::unLockAutoUpdate()
{
    _lockAutoUpdate = false;
    updateAll();
}

bool ChronoLine::isAutoUpdateLocked()
{
    return _lockAutoUpdate;
}

void ChronoLine::setUnit(const ChronoLineUnit& unit)
{
    timeLine->setUnit(unit);
    if (!_lockAutoUpdate) updateAll();
}

void ChronoLine::setMinDate(const QDateTime date)
{
    timeLine->setMinDate(date);
    if (!_lockAutoUpdate) updateAll();
}

void ChronoLine::setMaxDate(const QDateTime date)
{
    timeLine->setMaxDate(date);
    if (!_lockAutoUpdate) updateAll();
}

ChronoLineUnit ChronoLine::unit()
{
    return timeLine->unit();
}

QDateTime ChronoLine::minDate()
{
    return timeLine->minDate();
}

QDateTime ChronoLine::maxDate()
{
    return timeLine->maxDate();
}

long ChronoLine::addPeriod(const QDateTime& minDate, const QDateTime& maxDate, const QColor& color)
{
    long idPeriod = ++idSequencer; // first ID is 1
    if (minDate>=maxDate) return 0;
    periods[idPeriod] = new CLPeriod(idPeriod, minDate, maxDate, color, timeLine);
    periods[idPeriod]->setParentItem(timeLine);
    if (!_lockAutoUpdate) updateAll();
    return idPeriod;
}

bool ChronoLine::editPeriod(long idPeriod, const QDateTime& minDate, const QDateTime& maxDate)
{
    if (minDate>=maxDate) return false;
    CLPeriod* p = periods[idPeriod];
    if (!p) return false;
    p->setMinDate(minDate);
    p->setMaxDate(maxDate);
    return true;
}

bool ChronoLine::removePeriod(long idPeriod)
{
    CLPeriod* p = periods[idPeriod];
    if (!p) return false;
    periods.remove(idPeriod);
    delete p;
    return true;
}

bool ChronoLine::readPeriod(long idPeriod, QDateTime& minDate, QDateTime& maxDate)
{
    CLPeriod* p = periods[idPeriod];
    if (!p) return false;
    minDate = p->minDate();
    maxDate = p->maxDate();
    return true;
}

long ChronoLine::addEventFlag(const QDateTime& date, const QColor& color)
{
    QRect r = childrenRect();
    timeLine->calcScale(r);
    long idFlag = ++idSequencer; // first ID is 1
    evFlags[idFlag] = new CLFlag(idFlag, date, clftEvent, color, timeLine);
    evFlags[idFlag]->setParentItem(timeLine);
    evFlags[idFlag]->setPos(timeLine->xForDate(date, r), 1);
    if (!_lockAutoUpdate) updateAll();
    return idFlag;
}

bool ChronoLine::editEventFlag(long idFlag, const QDateTime& date)
{
    CLFlag* f = evFlags[idFlag];
    if (!f) return false;
    f->setDate(date);
    return true;
}

bool ChronoLine::removeEventFlag(long idFlag)
{
    CLFlag* f = evFlags[idFlag];
    if (!f) return false;
    evFlags.remove(idFlag);
    delete f;
    return true;
}

bool ChronoLine::readEventFlag(long idFlag, QDateTime& date)
{
    CLFlag* f = evFlags[idFlag];
    if (!f) return false;
    date = f->date();
    return true;
}

void ChronoLine::resizeEvent(QResizeEvent* event)
{
    QRect r = childrenRect();
    timeLine->calcScale(r);
    foreach (const long id, evFlags.keys()) {
        CLFlag* p = evFlags.value(id);
        p->setPos(timeLine->xForDate(p->date(), r), 1);
    }
    QGraphicsView::resizeEvent(event);
}

