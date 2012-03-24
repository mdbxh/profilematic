#include "rulecondition.h"

RuleCondition::RuleCondition(QObject *parent)
    : QObject(parent), _wlanTimeout(0)
{
    _init();
}

RuleCondition::RuleCondition(const RuleCondition &o)
    : QObject(NULL),
      _timeStart(o._timeStart),
      _timeEnd(o._timeEnd),
      _days(o._days),
      _locationCells(o._locationCells),
      _wlan(o._wlan),
      _wlanTimeout(o._wlanTimeout)
{
    _init();
}

void
RuleCondition::_init() {
    connect(this, SIGNAL(timeStartChanged()),     this, SIGNAL(changed()));
    connect(this, SIGNAL(timeEndChanged()),       this, SIGNAL(changed()));
    connect(this, SIGNAL(daysChanged()),          this, SIGNAL(changed()));
    connect(this, SIGNAL(locationCellsChanged()), this, SIGNAL(changed()));
    connect(this, SIGNAL(wlanChanged()),          this, SIGNAL(changed()));
    connect(this, SIGNAL(wlanTimeoutChanged()),   this, SIGNAL(changed()));
}


RuleCondition &
RuleCondition::operator=(const RuleCondition &o)
{
    _timeStart = o._timeStart;
    _timeEnd = o._timeEnd;
    _days = o._days;
    _locationCells = o._locationCells;
    _wlan = o._wlan;
    _wlanTimeout = o._wlanTimeout;

    return *this;
}

RuleCondition::~RuleCondition() {}

QString
RuleCondition::_getTimeQml(const QTime &time) const {
    if (time.isNull()) {
        return "";
    }
    int hour = time.hour();
    int min  = time.minute();
    QString timeStr;
    if (hour < 10) {
        timeStr += "0";
    }
    timeStr += QString::number(hour);
    timeStr += ":";
    if (min < 10) {
        timeStr += "0";
    }
    timeStr += QString::number(min);
    return timeStr;
}

QTime
RuleCondition::getTimeStart() const {
    return _timeStart;
}

void
RuleCondition::setTimeStart(const QTime &timeStart) {
    // Normalize to always have 00 seconds
    QTime useTime = _timeWithSecs(timeStart, 0);
    if (_timeStart != useTime) {
        _timeStart = useTime;
        emit timeStartChanged();
    }
}

QString
RuleCondition::getTimeStartQml() const {
    return _getTimeQml(getTimeStart());
}

void
RuleCondition::setTimeStartQml(const QString &timeStart) {
    if (timeStart.isEmpty()) {
        setTimeStart(QTime());
    } else {
        setTimeStart(QTime::fromString(timeStart, "hh:mm"));
    }
}

QTime
RuleCondition::getTimeEnd() const {
    return _timeEnd;
}

void
RuleCondition::setTimeEnd(const QTime &timeEnd) {
    QTime useTime = _timeWithSecs(timeEnd, 0);
    if (_timeEnd != useTime) {
        _timeEnd = useTime;
        emit timeEndChanged();
    }
}

QString
RuleCondition::getTimeEndQml() const {
    return _getTimeQml(getTimeEnd());
}

void
RuleCondition::setTimeEndQml(const QString &timeEnd) {
    setTimeEnd(QTime::fromString(timeEnd, "hh:mm"));
}

const QSet<int> &
RuleCondition::getDays() const {
    return _days;
}

void
RuleCondition::setDays(const QSet<int> &days) {
    if (_days != days) {
        _days = days;
        emit daysChanged();
    }
}

QVariantList
RuleCondition::getDaysQml() const {
    QVariantList days;
    for (QSet<int>::const_iterator i = _days.constBegin(); i != _days.constEnd(); ++i) {
        days << QVariant::fromValue(*i);
    }

    return days;

}
void
RuleCondition::setDaysQml(const QVariantList &days) {
    QSet<int> daysSet;
    for (QVariantList::const_iterator i = days.constBegin(); i != days.constEnd(); ++i) {
        daysSet << i->toInt();
    }
    setDays(daysSet);
}

const QSet<int> &
RuleCondition::getLocationCells() const {
    return _locationCells;
}

void
RuleCondition::setLocationCells(const QSet<int> &cells) {
    if (_locationCells != cells) {
        _locationCells = cells;
        emit locationCellsChanged();
    }
}

bool variantIntLessThan(const QVariant &s1, const QVariant &s2)
{
    return s1.toInt() < s2.toInt();
}

QVariantList
RuleCondition::getLocationCellsQml() const {
    QVariantList cells;
    for (QSet<int>::const_iterator i = _locationCells.constBegin(); i != _locationCells.constEnd(); ++i) {
        cells << QVariant::fromValue(*i);
    }
    qSort(cells.begin(), cells.end(), variantIntLessThan);
    return cells;

}
void
RuleCondition::setLocationCellsQml(const QVariantList &cells) {
    QSet<int> cellsSet;
    for (QVariantList::const_iterator i = cells.constBegin(); i != cells.constEnd(); ++i) {
        cellsSet << i->toInt();
    }
    setLocationCells(cellsSet);
}

const QSet<QString> &
RuleCondition::getWlan() const {
    return _wlan;
}

void
RuleCondition::setWlan(const QSet<QString> &wlan) {
    if (_wlan != wlan) {
        _wlan = wlan;
        emit wlanChanged();
    }
}

bool variantQStringLessThan(const QVariant &s1, const QVariant &s2)
{
    return s1.toString() < s2.toString();
}

QVariantList
RuleCondition::getWlanQml() const {
    QVariantList wlan;
    for (QSet<QString>::const_iterator i = _wlan.constBegin(); i != _wlan.constEnd(); ++i) {
        wlan << QVariant::fromValue(*i);
    }
    qSort(wlan.begin(), wlan.end(), variantIntLessThan);
    return wlan;

}
void
RuleCondition::setWlanQml(const QVariantList &wlan) {
    QSet<QString> wlanSet;
    for (QVariantList::const_iterator i = wlan.constBegin(); i != wlan.constEnd(); ++i) {
        wlanSet << i->toString();
    }
    setWlan(wlanSet);
}

int
RuleCondition::getWlanTimeout() const {
    return _wlanTimeout;
}

void
RuleCondition::setWlanTimeout(int timeoutSecs) {
    if (_wlanTimeout != timeoutSecs) {
        _wlanTimeout = timeoutSecs;
        emit wlanTimeoutChanged();
    }
}

QDBusArgument &operator<<(QDBusArgument &argument, const RuleCondition &rule)
{
    argument.beginStructure();
    argument << rule.getTimeStart();
    argument << rule.getTimeEnd();
    argument << rule.getDays();
    argument << rule.getLocationCells();
    argument << rule.getWlan();
    argument << rule.getWlanTimeout();
    argument.endStructure();
    return argument;
}

// Retrieve the MyStructure data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, RuleCondition &rule)
{
    QTime     timeStart = rule.getTimeStart();
    QTime     timeEnd = rule.getTimeEnd();
    QList<int> days = rule.getDays().toList();
    QList<int> cells = rule.getLocationCells().toList();
    QList<QString> wlan = rule.getWlan().toList();
    int     wlanTimeout = rule.getWlanTimeout();

    argument.beginStructure();
    argument >> timeStart;
    argument >> timeEnd;
    argument >> days;
    argument >> cells;
    argument >> wlan;
    argument >> wlanTimeout;
    argument.endStructure();

    rule.setTimeStart(timeStart);
    rule.setTimeEnd(timeEnd);
    rule.setDays(QSet<int>::fromList(days));
    rule.setLocationCells(QSet<int>::fromList(cells));
    rule.setWlan(QSet<QString>::fromList(wlan));
    rule.setWlanTimeout(wlanTimeout);

    return argument;
}