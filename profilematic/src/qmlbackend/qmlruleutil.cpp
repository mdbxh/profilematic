/**********************************************************************
 * Copyright 2011-2012 Arto Jalkanen
 *
 * This file is part of ProfileMatic.
 *
 * ProfileMatic is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ProfileMatic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ProfileMatic.  If not, see <http://www.gnu.org/licenses/>
**/
#include <cstdio>
#include <cstdlib>
#include "qmlruleutil.h"

QmlRuleUtil *QmlRuleUtil::_instance = 0;

void
QmlRuleUtil::initialize(QmlProfilesModel *profilesModel) {
    if (_instance != 0) {
        fprintf(stderr, "QmlRuleUtil::initialize called more than once");
        exit(1);
    }

    _instance = new QmlRuleUtil(profilesModel);
}

void
QmlRuleUtil::deinitialize() {
    delete _instance;
    _instance = 0;
}

QmlRuleUtil::QmlRuleUtil(QmlProfilesModel *profilesModel) : QObject(0) {
    _profilesModel = profilesModel;
}

QString
QmlRuleUtil::timeSummary(const RuleCondition *cond, const QString &nonUsableTimeString, bool /*inListing*/) {
    // qDebug("QmlRulesModel::getTimeSummaryText()");
    // Rule rule = ruleVariant.value<Rule>();
    if (cond == 0) {
        qDebug("QmlRulesModel::getTimeSummaryText() null rule");
        return nonUsableTimeString;
    }

    if (cond->getDays().isEmpty()
            || !cond->getTimeStart().isValid()
            || !cond->getTimeEnd().isValid()) {
//        qDebug("QmlRulesModel::getTimeSummaryText(): nonUsable, getDays.isEmpty/!validTimeStart/!validTimeEnd %d, %d, %d",
//               rule->getDays().isEmpty(), !rule->getTimeStart().isValid(), !rule->getTimeEnd().isValid());
        return nonUsableTimeString;
    }

    QString summary;
    summary += cond->getTimeStartQml();
    summary += " - ";
    summary += cond->getTimeEndQml();
    if (cond->getTimeStart() == cond->getTimeEnd()) {
        summary += " (";
        summary += "24h";
        summary += ")";
    }
    // I don't think this length clarification is needed otherwise
//    else {
//        QTime duration = rule->getTimeDuration();
//        qDebug("QmlRulesModel::getTimeSummaryText() 3" );
//        if (duration.hour() != 0) {
//            summary += QString::number(duration.hour());
//            summary += "h";
//        }
//        qDebug("QmlRulesModel::getTimeSummaryText() 3");
//        if (duration.minute() != 0) {
//            summary += QString::number(duration.minute());
//            summary += "min";
//        }
//    }

    summary += " ";
//    qDebug("QmlRulesModel::getTimeSummaryText() getDaysSummaryText");
    summary += daysSummaryText(cond->getDays());
//    qDebug("/QmlRulesModel::getTimeSummaryText() returning %s", qPrintable(summary));

    return summary;
}

void
QmlRuleUtil::timeClear(RuleCondition *cond) {
    if (cond == 0) return;

    QSet<int> days;
    cond->setDays(days);
    cond->setTimeStart(QTime());
    cond->setTimeEnd(QTime());
}

QString
QmlRuleUtil::daysSummaryText(const QSet<int> &days, bool /*inListing*/) {
    // qDebug("QmlRulesModel::getDaysSummaryText(), days size %d", days.size());

    if (days.size() == 7) {
        return "All days";
    }
    if (days.size() == 0) {
        return "No days";
    }
    // If only one day selected, display full name
    if (days.size() == 1) {
        int dayId = *(days.constBegin());
        return QDate::longDayName(dayId + 1, QDate::StandaloneFormat);
    }

    int rangeStart = -1;

    QString daysStr = "";
    // The loop goes up-to 7 so that Sunday is considered as last in range.
    // Days.contains for 7 should always contain false as days are in range of 0-6.
    for (int i = 0; i < 8; ++i) {
        if (days.contains(i)) {
            if (rangeStart < 0) {
                rangeStart = i;

            }
        } else {
            if (rangeStart > -1) {
                if (!daysStr.isEmpty()) {
                    daysStr += ", ";
                }

                daysStr += QDate::shortDayName(rangeStart + 1, QDate::StandaloneFormat);
                if (rangeStart < i - 1) {
                    daysStr += " - ";
                    daysStr += QDate::shortDayName((i - 1) + 1, QDate::StandaloneFormat);
                }
                rangeStart = -1;
            }
        }
    }
    return daysStr;
}

QString
QmlRuleUtil::cellLocationSummary(const RuleCondition *cond, const QString &nonUsable, bool inListing) {
    if (cond == 0) return nonUsable;

    int numCellIds = cond->getLocationCells().size();
    QString s;
    if (numCellIds == 0) {
        s.append(nonUsable);
    } else {
        if (inListing) {
            s.append("on location");
        } else {
            s.append("Cell ids set");
            if (cond->getLocationCellsTimeout() > 0) {
                s.append(QString(" (%1 timeout)").arg(cond->getLocationCellsTimeout()));
            }
        }
    }
    return s;
}

void
QmlRuleUtil::cellLocationClear(RuleCondition *cond) {
    if (cond == 0) return;

    QSet<int> cells;
    cond->setLocationCells(cells);
    cond->setLocationCellsTimeout(0);
}

QString
QmlRuleUtil::internetConnectionModeSummary(const RuleCondition *cond, const QString &nonUsable, bool /* inListing */) {
    if (cond == 0) return nonUsable;

    switch (cond->getInternetConnectionMode()) {
    case RuleCondition::Gsm:
        return "Mobile net connection";
    case RuleCondition::Wlan:
        return "WLAN net connection";
    case RuleCondition::ConnectionAny:
        return "Any net connection";
    case RuleCondition::ConnectionNone:
        return "No net connection";
    default:
        return nonUsable;
    }
    return nonUsable;
}

void
QmlRuleUtil::internetConnectionModeClear(RuleCondition *cond) {
    if (cond == 0) return;

    cond->setInternetConnectionMode(RuleCondition::UndefinedInternetConnectionMode);
}

QString
QmlRuleUtil::wlanSummary(const RuleCondition *cond, const QString &nonUsable, bool inListing) {
    if (cond == 0) return nonUsable;

    int numWlans = cond->getWlan().size();
    QString s;
    if (numWlans == 0) {
        s.append(nonUsable);
    } else {
        if (inListing) {
            s.append("on WLAN");
        } else {
            s.append("In use");
            if (cond->getWlanTimeout() > 0) {
                s.append(QString(" (%1 timeout)").arg(cond->getWlanTimeout()));
            }
        }
    }
    return s;
}

void
QmlRuleUtil::wlanClear(RuleCondition *cond) {
    if (cond == 0) return;

    QSet<QString> numWlans;
    cond->setWlan(numWlans);
    cond->setWlanTimeout(0);
}

QString
QmlRuleUtil::idleSummary(const RuleCondition *cond, const QString &nonUsable, bool inListing) {
    if (cond == 0) return nonUsable;

    int idleForSecs = cond->getIdleForSecs();
    QString s;
    if (idleForSecs < 0) {
        s.append(nonUsable);
    } else {
        if (inListing) {
            s.append("Idle");
        } else {
            s.append(QString("At least for %1 minutes").arg(idleForSecs / 60));
        }
    }
    return s;
}

void
QmlRuleUtil::idleClear(RuleCondition *cond) {
    if (cond == 0) return;

    cond->setIdleForSecs(-1);
}

QString
QmlRuleUtil::nfcSummary(const RuleCondition *cond, const QString &nonUsable, bool inListing) {
    if (cond == 0) return nonUsable;

    int nfcUids = cond->nfc().getUids().size();
    QString s;
    if (nfcUids == 0) {
        s.append(nonUsable);
    } else {
        if (inListing) {
            s.append("NFC");
        } else {
            s.append("NFC detections set");
            if (cond->nfc().getToggleCondition()) {
                s.append(" (toggles)");
            }
        }
    }
    return s;
}

void
QmlRuleUtil::nfcClear(RuleCondition *cond) {
    if (cond == 0) return;

    QSet<QByteArray> uids;
    cond->nfc().setUids(uids);
    cond->nfc().setToggleCondition(false);
}


QString
QmlRuleUtil::chargingSummary(const RuleCondition *cond, const QString &nonUsable, bool inListing) {
    if (cond == 0) return nonUsable;

    RuleCondition::ChargingState chargingState = cond->getChargingState();
    QString s;
    switch (chargingState) {
    case RuleCondition::NotCharging:
        s.append(inListing ? "not charging" : "Not charging power");
        break;
    case RuleCondition::Charging:
        s.append(inListing ? "charging" : "Charging power");
        break;
    default:
        s.append(nonUsable);
        break;
    }

    return s;
}

void
QmlRuleUtil::chargingClear(RuleCondition *cond) {
    if (cond == 0) return;

    cond->setChargingState(RuleCondition::UndefinedChargingState);
}

QString
QmlRuleUtil::batteryLevelSummary(RuleCondition *cond, const QString &nonUsable, bool inListing) {
    qDebug("batteryLevelSummary QML version");
    return batteryLevelSummary(const_cast<const RuleCondition *>(cond), nonUsable, inListing);
}

QString
QmlRuleUtil::batteryLevelSummary(const RuleCondition *cond, const QString &nonUsable, bool /*inListing*/) {
    if (cond == 0) return nonUsable;

    const RuleConditionBatteryLevel &bl = cond->batteryLevel();
    if (!bl.isValid()) {
        return nonUsable;
    }
    qDebug("QmlRuleUtil::Battery level %d - %d", bl.getLevelMin(), bl.getLevelMax());

    if (bl.getLevelMax() == bl.getLevelMin()) {
        return QString("Battery level exactly %1%").arg(bl.getLevelMin());
    }
    else if (bl.getLevelMax() == 100) {
        return QString("Battery level at least %1%").arg(bl.getLevelMin());
    } else if (bl.getLevelMin() == 0) {
        return QString("Battery level at most %1%").arg(bl.getLevelMax());
    }
    return QString("Battery level between %1% - %2%").arg(bl.getLevelMin()).arg(bl.getLevelMax());
}

void
QmlRuleUtil::batteryLevelClear(RuleCondition *cond) {
    if (cond == 0) return;

    cond->batteryLevel().clear();
}

QString
QmlRuleUtil::calendarSummary(const RuleCondition *cond, const QString &nonUsable, bool inListing) {
    Q_UNUSED(inListing)

    if (cond == 0) return nonUsable;

    const RuleConditionCalendar &calendar = cond->calendar();
    if (calendar.isValid()) {
        return "Calendar entries";
    }
    return nonUsable;
}

void
QmlRuleUtil::calendarClear(RuleCondition *cond) {
    if (cond == 0) return;

    cond->calendar().clear();
}

QString
QmlRuleUtil::profileSummary(const RuleAction *action, const QString &nonUsable, bool inListing) {
    if (action == 0) return nonUsable;

    QString profile = action->getProfile();
    if (!profile.isEmpty()) {
        QString profileName = _profilesModel->getProfileToName(profile);
        QString summary = profileName;

        if (!inListing) {
            if (_profilesModel->profileHasVolume(profile) && action->getProfileVolume() > -1) {
                summary += QString(" (%1%)").arg(action->getProfileVolume());
            }
            if (action->getRestoreProfile()) {
                summary += ". Restores previous profile.";
            }
        }
        return summary;
    }
    return nonUsable;
}

void
QmlRuleUtil::profileClear(RuleAction *action) {
    if (action == 0) return;

    action->setProfile(QString());
    action->setProfileVolume(-1);
    action->setRestoreProfile(false);
}

QString
QmlRuleUtil::presenceSummary(const RuleAction *action, const QString &nonUsable, bool inListing) {
    if (action == 0) return nonUsable;

    QString summary = nonUsable;
    bool atLeastOneChange = false;

    foreach (PresenceRule *presenceRule, action->presenceRules()) {
        if (presenceRule->action() != PresenceRule::Retain) {
            atLeastOneChange = true;
            break;
        }
    }

    if (atLeastOneChange)
        summary = "Availability change";

    if (!inListing && action->getRestorePresence())
        summary += ". Restores previous availability";

    return summary;
}

void
QmlRuleUtil::presenceClear(RuleAction *action) {
    if (action == 0) return;

    QList<PresenceRule *> presenceRules;
    action->setPresenceRules(presenceRules);
}

QString
QmlRuleUtil::flightModeSummary(const RuleAction *action, const QString &nonUsable, bool inListing) {
    if (action == 0) return nonUsable;

    QString summary;
    switch (action->getFlightMode()) {
    case 0:
        summary = "Flight mode off"; break;
    case 1:
        summary = "Flight mode on"; break;
    default:
        return nonUsable;
    }

    if (!inListing && action->getRestoreFlightMode()) {
        summary += ". Restores previous mode.";
    }
    return summary;
}

void
QmlRuleUtil::flightModeClear(RuleAction *action) {
    if (action == 0) return;

    action->setFlightMode(-1);
    action->setRestoreFlightMode(false);
}

QString
QmlRuleUtil::powerSavingModeSummary(const RuleAction *action, const QString &nonUsable, bool inListing) {
    if (action == 0) return nonUsable;

    QString summary;
    switch (action->getPowerSavingMode()) {
    case 0:
        summary = "Power saving off"; break;
    case 1:
        summary = "Power saving on"; break;
    default:
        return nonUsable;
    }

    if (!inListing && action->getRestorePowerSavingMode()) {
        summary += ". Restores previous mode.";
    }
    return summary;
}

void
QmlRuleUtil::powerSavingModeClear(RuleAction *action) {
    if (action == 0) return;

    action->setPowerSavingMode(-1);
    action->setRestorePowerSavingMode(false);
}

QString
QmlRuleUtil::bluetoothModeSummary(const RuleAction *action, const QString &nonUsable, bool inListing) {
    if (action == 0) return nonUsable;

    QString summary;
    switch (action->getBlueToothMode()) {
    case 0:
        summary = "Bluetooth off"; break;
    case 1:
        summary = "Bluetooth on"; break;
    case 2:
        summary = inListing ? "Bluetooth on" : "Bluetooth on and visible"; break;
    default:
        return nonUsable;
    }

    if (!inListing && action->getRestoreBlueToothMode()) {
        summary += ". Restores previous mode.";
    }
    return summary;
}

void
QmlRuleUtil::bluetoothModeClear(RuleAction *action) {
    if (action == 0) return;

    action->setBlueToothMode(-1);
    action->setRestoreBlueToothMode(false);
}

QString
QmlRuleUtil::cellularModeSummary(const RuleAction *action, const QString &nonUsable, bool inListing) {
    if (action == 0) return nonUsable;

    QString summary;
    switch (action->getCellularMode()) {
    case 0:
        summary = "Dual GSM/3G"; break;
    case 1:
        summary = "GSM"; break;
    case 2:
        summary = "3G"; break;
    default:
        return nonUsable;
    }

    if (!inListing && action->getRestoreCellularMode()) {
        summary += ". Restores previous mode.";
    }

    return summary;
}

void
QmlRuleUtil::cellularModeClear(RuleAction *action) {
    if (action == 0) return;

    action->setCellularMode(-1);
}

QString
QmlRuleUtil::standByScreenModeSummary(const RuleAction *action, const QString &nonUsable, bool inListing) {
    if (action == 0) return nonUsable;

    QString summary;
    switch (action->getStandByScreenMode()) {
    case 0:
        summary = "Stand-by screen off"; break;
    case 1:
        summary = "Stand-by screen on"; break;
    default:
        return nonUsable;
    }

    if (!inListing && action->getRestoreStandByScreenMode()) {
        summary += ". Restores previous mode.";
    }

    return summary;
}

void
QmlRuleUtil::standByScreenModeClear(RuleAction *action) {
    if (action == 0) return;

    action->setStandByScreenMode(-1);
    action->setRestoreStandByScreenMode(false);
}

QString
QmlRuleUtil::customActionSummary(const RuleAction *action, const QString &nonUsable, bool inListing) {
    if (action == 0) return nonUsable;

    QString commandLine = action->getCommandLine().trimmed();
    QString commandLineExit = action->getCommandLineExit().trimmed();
    if (inListing) {
        return (!commandLine.isEmpty() || !commandLineExit.isEmpty() ? "Command line" : nonUsable);
    } else {
        if (!commandLine.isEmpty() &&
                !commandLineExit.isEmpty()) {
            return "Custom action on activate and deactivate has been set";
        }
        else if (!commandLine.isEmpty()) {
            return "Custom action on activate has been set";
        }
        else if (!commandLineExit.isEmpty()) {
            return "Custom action on deactivate has been set";
        }
    }
    return nonUsable;
}

void
QmlRuleUtil::customActionClear(RuleAction *action) {
    if (action == 0) return;

    action->setCommandLine(QString());
    action->setCommandLineExit(QString());
}
