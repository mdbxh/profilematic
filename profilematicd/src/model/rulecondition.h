#ifndef RULECONDITION_H
#define RULECONDITION_H

#include <QObject>
#include <QString>
#include <QSet>
#include <QTime>
#include <QMetaType>
#include <QDBusArgument>

class RuleCondition : public QObject
{
    Q_OBJECT

    // Conditions
    QTime     _timeStart;
    QTime     _timeEnd;
    QSet<int> _days;
    QSet<int> _locationCells;
    QSet<QString> _wlan;
    int       _wlanTimeout;

    // IMPROVE: maybe the QML specifics could be in inheriting class, keeping this
    // class "pure" plain Qt object?
    Q_PROPERTY(QString timeStart READ getTimeStartQml WRITE setTimeStartQml NOTIFY timeStartChanged)
    Q_PROPERTY(QString timeEnd READ getTimeEndQml WRITE setTimeEndQml NOTIFY timeEndChanged)
    Q_PROPERTY(QVariantList days READ getDaysQml WRITE setDaysQml NOTIFY daysChanged)
    Q_PROPERTY(QVariantList locationCells READ getLocationCellsQml WRITE setLocationCellsQml NOTIFY locationCellsChanged)
    Q_PROPERTY(QVariantList wlan READ getWlanQml WRITE setWlanQml NOTIFY wlanChanged)
    Q_PROPERTY(int wlanTimeout READ getWlanTimeout WRITE setWlanTimeout NOTIFY wlanTimeoutChanged)

    QString _getTimeQml(const QTime &time) const;

    inline QTime _timeWithSecs(const QTime time, int secs) const {
        return time.isNull() ? time : QTime(time.hour(), time.minute(), secs);
    }

    void _init();
signals:
    void changed();

    void timeStartChanged();
    void timeEndChanged();
    void daysChanged();
    void locationCellsChanged();
    void wlanChanged();
    void wlanTimeoutChanged();

public:
    RuleCondition(QObject *parent = 0);
    RuleCondition(const RuleCondition &cond);
    virtual ~RuleCondition();

    RuleCondition &operator=(const RuleCondition &o);

    QTime getTimeStart() const;
    void setTimeStart(const QTime &timeStart);
    // For QML
    QString getTimeStartQml() const;
    void setTimeStartQml(const QString &timeEnd);
    QTime getTimeEnd() const;
    void setTimeEnd(const QTime &timeEnd);
    // For QML
    QString getTimeEndQml() const;
    void setTimeEndQml(const QString &timeEnd);
    inline QTime getTimeDuration() const {
        int secsToEnd = _timeStart.secsTo(_timeEnd);
        qDebug("secsToEnd %d", secsToEnd);
        if (secsToEnd <= 0) {
            secsToEnd += 24 * 60 * 60;
        }
        return QTime().addSecs(secsToEnd);
    }
    // 0 = monday, 6 = sunday
    const QSet<int> &getDays() const;
    void setDays(const QSet<int> &days);
    // For QML
    QVariantList getDaysQml() const;
    void setDaysQml(const QVariantList &days);

    const QSet<int> &getLocationCells() const;
    void setLocationCells(const QSet<int> &cells);
    // For QML
    QVariantList getLocationCellsQml() const;
    void setLocationCellsQml(const QVariantList &cells);

    const QSet<QString> &getWlan() const;
    void setWlan(const QSet<QString> &wlan);
    // For QML
    QVariantList getWlanQml() const;
    void setWlanQml(const QVariantList &wlan);

    int getWlanTimeout() const;
    void setWlanTimeout(int timeoutSecs);

    inline bool operator==(const RuleCondition &o) const {
        return this->_timeStart == o._timeStart
            && this->_timeEnd   == o._timeEnd
            && this->_days      == o._days
            && this->_locationCells == o._locationCells
            && this->_wlan      == o._wlan;
    }

    inline bool isTimeStartRuleUsable() const {
        return getTimeStart().isValid();
    }
    inline bool isTimeEndRuleUsable() const {
        return getTimeEnd().isValid();
    }
    inline bool isDaysRuleUsable() const {
        return !getDays().isEmpty();
    }
    inline bool isLocationCellsRuleUsable() const {
        return !getLocationCells().isEmpty();
    }
};

Q_DECLARE_METATYPE(RuleCondition)
Q_DECLARE_METATYPE(RuleCondition *)

// Marshall the Rule data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const RuleCondition &mystruct);

// Retrieve the Rule data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, RuleCondition &mystruct);

#endif // RULECONDITION_H
