/**********************************************************************
 * Copyright 2011 Arto Jalkanen
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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QList>
#include <QSettings>

#include "model/rule.h"
#include "preferences.h"

/**
  * 0 <= 1.1.1, before timeEnd
  * 1 = 1.2.0, endTime, default rule, cell tower based location
  * 2 = 1.6.0, multi-rule
  */
#define RULES_VERSION 2

class Configuration
{
    static void _writeIntList(QSettings &s, const QString &prefix, const QString &key, const QList<int> &values);
    static void _writeStringList(QSettings &s, const QString &prefix, const QString &key, const QList<QString> &values);
    static void _readIntList(QSettings &s, const QString &prefix, const QString &key, QList<int> &values);
    static void _readStringList(QSettings &s, const QString &prefix, const QString &key, QList<QString> &values);
    static void _assignRuleId(Rule &r, const QVariant &ruleIdVar);
    static void _writePresenceRuleList(QSettings &s, const QList<PresenceRule *> &rules);
    static void _readPresenceRuleList(QSettings &s, QList<PresenceRule *> &rules);
    static void _writeNfcCondition(QSettings &s, const RuleConditionNFC &condNfc);
    static void _readNfcCondition(QSettings &s, RuleConditionNFC &condNfc);
    static void _writeBatteryLevelCondition(QSettings &s, const RuleConditionBatteryLevel &cond);
    static void _readBatteryLevelCondition(QSettings &s, RuleConditionBatteryLevel &cond);
    static void _writeCalendarCondition(QSettings &s, const RuleConditionCalendar &cond);
    static void _readCalendarCondition(QSettings &s, RuleConditionCalendar &cond);

public:
    Configuration();

    static void writeRules(const QList<Rule> &rules);
    static void readRules(QList<Rule> &rules, int *rules_version = 0);

    static void writePreferences(const Preferences &preferences);
    static void readPreferences(Preferences &preferences);

};

#endif // CONFIGURATION_H
