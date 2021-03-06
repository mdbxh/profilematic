#ifndef RULECONDITION_H
#define RULECONDITION_H

#include <QObject>
#include <QString>
#include <QSet>
#include <QTime>
#include <QMetaType>
#include <QDBusArgument>

#include "ruleconditionnfc.h"
#include "ruleconditionbatterylevel.h"
#include "ruleconditioncalendar.h"

class RuleCondition : public QObject
{
    Q_OBJECT
    Q_ENUMS(InternetConnectionMode)
    Q_ENUMS(ChargingState)

public:
    enum InternetConnectionMode {
        UndefinedInternetConnectionMode = 0,
        Wlan,
        Gsm,
        ConnectionAny,
        ConnectionNone
    };

    enum ChargingState {
        UndefinedChargingState = -1,
        NotCharging,
        Charging
    };

private:
    // Conditions
    QTime     _timeStart;
    QTime     _timeEnd;
    QSet<int> _days;
    QSet<int> _locationCells;
    int       _locationCellsTimeout;
    QSet<QString> _wlan;
    int       _wlanTimeout;
    int       _idleForSecs;
    RuleConditionNFC _nfc;
    InternetConnectionMode _internetConnectionMode;
    ChargingState _chargingState;
    RuleConditionBatteryLevel _batteryLevel;
    RuleConditionCalendar _calendar;

    // IMPROVE: maybe the QML specifics could be in inheriting class, keeping this
    // class "pure" plain Qt object?
    Q_PROPERTY(QString timeStart READ getTimeStartQml WRITE setTimeStartQml NOTIFY timeStartChanged)
    Q_PROPERTY(QString timeEnd READ getTimeEndQml WRITE setTimeEndQml NOTIFY timeEndChanged)
    Q_PROPERTY(QVariantList days READ getDaysQml WRITE setDaysQml NOTIFY daysChanged)
    Q_PROPERTY(QVariantList locationCells READ getLocationCellsQml WRITE setLocationCellsQml NOTIFY locationCellsChanged)
    Q_PROPERTY(int locationCellsTimeout READ getLocationCellsTimeout WRITE setLocationCellsTimeout NOTIFY locationCellsTimeoutChanged)
    Q_PROPERTY(QVariantList wlan READ getWlanQml WRITE setWlanQml NOTIFY wlanChanged)
    Q_PROPERTY(int wlanTimeout READ getWlanTimeout WRITE setWlanTimeout NOTIFY wlanTimeoutChanged)
    Q_PROPERTY(int idleForSecs READ getIdleForSecs WRITE setIdleForSecs NOTIFY idleForSecsChanged)
    Q_PROPERTY(RuleConditionNFC *nfc READ getNFCQml NOTIFY nfcChanged STORED false)
    Q_PROPERTY(enum InternetConnectionMode internetConnectionMode READ getInternetConnectionMode WRITE setInternetConnectionMode NOTIFY internetConnectionModeChanged)
    Q_PROPERTY(enum ChargingState chargingState READ getChargingState WRITE setChargingState NOTIFY chargingStateChanged)
    Q_PROPERTY(RuleConditionBatteryLevel *batteryLevel READ getBatteryLevelQml NOTIFY batteryLevelChanged STORED false)
    Q_PROPERTY(RuleConditionCalendar *calendar READ getCalendarQml NOTIFY calendarChanged STORED false)

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
    void locationCellsTimeoutChanged();
    void wlanChanged();
    void wlanTimeoutChanged();
    void idleForSecsChanged();
    void nfcChanged();
    void internetConnectionModeChanged();
    void chargingStateChanged();
    void batteryLevelChanged();
    void calendarChanged();

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

    int getLocationCellsTimeout() const;
    void setLocationCellsTimeout(int timeoutSecs);

    const QSet<QString> &getWlan() const;
    void setWlan(const QSet<QString> &wlan);
    // For QML
    QVariantList getWlanQml() const;
    void setWlanQml(const QVariantList &wlan);

    int getWlanTimeout() const;
    void setWlanTimeout(int timeoutSecs);

    int getIdleForSecs() const;
    // int getIdleForSecs() const { return _idleForSecs; }
    void setIdleForSecs(int idleForSecs);

    inline const RuleConditionNFC &nfc() const { return _nfc; }
    inline RuleConditionNFC &nfc() { return _nfc; }
    inline void setNFC(const RuleConditionNFC &nfc) { _nfc = nfc; }
    // For QML
    inline RuleConditionNFC *getNFCQml() { return &_nfc; }


    inline const RuleConditionBatteryLevel &batteryLevel() const { return _batteryLevel; }
    inline RuleConditionBatteryLevel &batteryLevel() { return _batteryLevel; }
    inline void setBatteryLevel(const RuleConditionBatteryLevel &batteryLevel) { _batteryLevel = batteryLevel; }
    // For QML
    inline RuleConditionBatteryLevel *getBatteryLevelQml() { return &_batteryLevel; }

    inline const RuleConditionCalendar &calendar() const { return _calendar; }
    inline RuleConditionCalendar &calendar() { return _calendar; }
    inline void setCalendar(const RuleConditionCalendar &calendar) { _calendar = calendar; }
    // For QML
    inline RuleConditionCalendar *getCalendarQml() { return &_calendar; }

    inline InternetConnectionMode getInternetConnectionMode() const { return _internetConnectionMode; }
    void setInternetConnectionMode(InternetConnectionMode);

    inline ChargingState getChargingState() const { return _chargingState; }
    void setChargingState(ChargingState);

    inline bool operator==(const RuleCondition &o) const {
        return this->_timeStart == o._timeStart
            && this->_timeEnd   == o._timeEnd
            && this->_days      == o._days
            && this->_locationCells == o._locationCells
            && this->_locationCellsTimeout == o._locationCellsTimeout
            && this->_wlan      == o._wlan
            && this->_wlanTimeout == o._wlanTimeout
            && this->_idleForSecs == o._idleForSecs
            && this->_nfc == o._nfc
            && this->_internetConnectionMode == o._internetConnectionMode
            && this->_chargingState == o._chargingState
            && this->_batteryLevel == o._batteryLevel
            && this->_calendar == o._calendar;
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
