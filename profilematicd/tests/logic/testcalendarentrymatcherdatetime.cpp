/**********************************************************************
 * Copyright 2012 Arto Jalkanen
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
#include "testcalendarentrymatcherdatetime.h"

TestCalendarEntryMatcherDateTime::TestCalendarEntryMatcherDateTime()
{
}

void
TestCalendarEntryMatcherDateTime::matchDateTime() {

    RuleConditionCalendar cond;
    QDateTime now = QDateTime::fromString("27.09.2011 22:00", "dd.MM.yyyy hh:mm");

    CalendarEntryMatcherDateTime matcher(cond, now);
    CalendarEntry matchStartLimit(now, now.addSecs(60*10), QString(), QString());
    CalendarEntry matchEndLimit(now.addSecs(-60*10), now, QString(), QString());
    CalendarEntry nomatchStartAfterNow(now.addSecs(1), now.addSecs(2), QString(), QString());
    CalendarEntry nomatchEndBeforeNow(now.addSecs(-2), now.addSecs(-1), QString(), QString());
    // If start and end are same, end is used as start + 1min, so this should match
    CalendarEntry matchStartAndEndSame(now.addSecs(-1), now.addSecs(-1), QString(), QString());

    QCOMPARE(matcher.match(matchStartLimit), true);
    QCOMPARE(matcher.match(matchEndLimit), true);
    QCOMPARE(matcher.match(nomatchStartAfterNow), false);
    QCOMPARE(matcher.match(nomatchEndBeforeNow), false);
    QCOMPARE(matcher.match(matchStartAndEndSame), true);

    // Check nextNearest values
    QCOMPARE(matcher.nextNearestStartOrEnd(matchStartLimit), matchStartLimit.end());
    QCOMPARE(matcher.nextNearestStartOrEnd(matchEndLimit), QDateTime());
    QCOMPARE(matcher.nextNearestStartOrEnd(nomatchStartAfterNow), nomatchStartAfterNow.start());
    QCOMPARE(matcher.nextNearestStartOrEnd(nomatchEndBeforeNow), QDateTime());
    QCOMPARE(matcher.nextNearestStartOrEnd(matchStartAndEndSame), matchStartAndEndSame.start().addSecs(60));
}

void
TestCalendarEntryMatcherDateTime::matchDateTime_append() {
    int appendSecs = 10;
    RuleConditionCalendar cond;
    cond.setTimeAppend(appendSecs);

    QDateTime now = QDateTime::fromString("27.09.2011 22:00", "dd.MM.yyyy hh:mm");

    CalendarEntryMatcherDateTime matcher(cond, now);
    CalendarEntry matchStartLimit(now, now.addSecs(60*10), QString(), QString());
    CalendarEntry matchEndLimit(now.addSecs(-60*10), now, QString(), QString());
    CalendarEntry nomatchStartAfterNow(now.addSecs(1), now.addSecs(2), QString(), QString());
    CalendarEntry nomatchEndBeforeNow(now.addSecs(-200), now.addSecs(-100), QString(), QString());
    // If start and end are same, end is used as start + 1min, so this should match
    CalendarEntry matchStartAndEndSame(now.addSecs(-1), now.addSecs(-1), QString(), QString());

    QCOMPARE(matcher.match(matchStartLimit), true);
    QCOMPARE(matcher.match(matchEndLimit), true);
    QCOMPARE(matcher.match(nomatchStartAfterNow), false);
    QCOMPARE(matcher.match(nomatchEndBeforeNow), false);
    QCOMPARE(matcher.match(matchStartAndEndSame), true);

    // Check nextNearest values
    QCOMPARE(matcher.nextNearestStartOrEnd(matchStartLimit), matchStartLimit.end().addSecs(appendSecs));
    QCOMPARE(matcher.nextNearestStartOrEnd(matchEndLimit), now.addSecs(appendSecs));
    QCOMPARE(matcher.nextNearestStartOrEnd(nomatchStartAfterNow), nomatchStartAfterNow.start());
    QCOMPARE(matcher.nextNearestStartOrEnd(nomatchEndBeforeNow), QDateTime());
    QCOMPARE(matcher.nextNearestStartOrEnd(matchStartAndEndSame), matchStartAndEndSame.start().addSecs(60).addSecs(appendSecs));
}

void
TestCalendarEntryMatcherDateTime::matchDateTime_prepend() {
    // TODO
//    int prependSecs = 10;
//    RuleConditionCalendar cond;
//    cond.setTimeAppend(prependSecs);

//    QDateTime now = QDateTime::fromString("27.09.2011 22:00", "dd.MM.yyyy hh:mm");

//    CalendarEntryMatcherDateTime matcher(cond, now);
//    CalendarEntry matchStartLimit(now, now.addSecs(60*10), QString(), QString());
//    CalendarEntry matchEndLimit(now.addSecs(-60*10), now, QString(), QString());
//    CalendarEntry nomatchStartAfterNow(now.addSecs(1), now.addSecs(2), QString(), QString());
//    CalendarEntry nomatchEndBeforeNow(now.addSecs(-200), now.addSecs(-100), QString(), QString());
//    // If start and end are same, end is used as start + 1min, so this should match
//    CalendarEntry matchStartAndEndSame(now.addSecs(-1), now.addSecs(-1), QString(), QString());

//    QCOMPARE(matcher.match(matchStartLimit), true);
//    QCOMPARE(matcher.match(matchStartLimit), true);
//    QCOMPARE(matcher.match(matchEndLimit), true);
//    QCOMPARE(matcher.match(nomatchStartAfterNow), false);
//    QCOMPARE(matcher.match(nomatchEndBeforeNow), false);
//    QCOMPARE(matcher.match(matchStartAndEndSame), true);

//    // Check nextNearest values
//    QCOMPARE(matcher.nextNearestStartOrEnd(matchStartLimit), matchStartLimit.end());
//    QCOMPARE(matcher.nextNearestStartOrEnd(matchEndLimit), now);
//    QCOMPARE(matcher.nextNearestStartOrEnd(nomatchStartAfterNow), nomatchStartAfterNow.start());
//    QCOMPARE(matcher.nextNearestStartOrEnd(nomatchEndBeforeNow), QDateTime());
//    QCOMPARE(matcher.nextNearestStartOrEnd(matchStartAndEndSame), matchStartAndEndSame.start().addSecs(60).addSecs(prependSecs));
}
