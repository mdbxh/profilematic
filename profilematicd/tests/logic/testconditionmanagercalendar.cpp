#include "testconditionmanagercalendar.h"

#include "../../src/platform/platformutil.h"

class MockCalendarManager : public CalendarManager {
public:
    QList<CalendarEntry> returnEntries;
    int loadCalendarEntriesCalls;
    QDate calledStartDate;
    QDate calledEndDate;

    MockCalendarManager(QObject *parent = 0) : CalendarManager(parent) {
        loadCalendarEntriesCalls = 0;
    }

    QList<CalendarEntry> loadCalendarEntries(const QDate &startDate, const QDate &endDate) {
        qDebug("MockCalendarManager::loadCalendarEntries");
        ++loadCalendarEntriesCalls;
        calledStartDate = startDate;
        calledEndDate = endDate;

        return returnEntries;
    }

    virtual void closeCalendar() {}

};

class MockPlatformUtil : public PlatformUtil {
public:
    QList<Rule::IdType> matchingRules;
    QList<Rule::IdType> refreshCalledWithRules;
    bool refreshCalled;
    bool emitRefreshNeeded;
    MockCalendarManager *cm;

    MockPlatformUtil(MockCalendarManager *cm) {
        this->cm = cm;
    }

    virtual CalendarManager *createCalendarManager(QObject *parent = 0) {
        Q_UNUSED(parent)

        return cm;
    }
};

TestConditionManagerCalendar::TestConditionManagerCalendar()
{
}

void
TestConditionManagerCalendar::init() {
    qDebug("TestConditionManagerCalendar::init");
    _cm = new MockCalendarManager;
    _pu = new MockPlatformUtil(_cm);
    PlatformUtil::initialize(_pu);
}

void
TestConditionManagerCalendar::conditionSetForMatching() {
    Q_ASSERT(PlatformUtil::instance() == _pu);

    ConditionManagerCalendar cmc;

    RuleConditionCalendar condCalNotSet;
    RuleConditionCalendar condCalSummarySet;
    condCalSummarySet.setSummaryMatch("something");

    RuleCondition cond;

    cond.setCalendar(condCalNotSet);
    QCOMPARE(cmc.conditionSetForMatching(cond), false);
    cond.setCalendar(condCalSummarySet);
    QCOMPARE(cmc.conditionSetForMatching(cond), true);
}

void
TestConditionManagerCalendar::match_testCalendarLoading() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    condCal.setSummaryMatch("something");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now = dateTimeBase;

    QCOMPARE(_cm->loadCalendarEntriesCalls, 0);

    cmc.match(dateTimeBase, condCal);
    QCOMPARE(_cm->loadCalendarEntriesCalls, 1);
    QCOMPARE(_cm->calledStartDate, now.date());
    QCOMPARE(_cm->calledEndDate, now.addDays(1).date());

    // Should not reload for same day as it's already loaded
    cmc.match(dateTimeBase.addSecs(60), condCal);
    QCOMPARE(_cm->loadCalendarEntriesCalls, 1);
    QCOMPARE(_cm->calledStartDate, now.date());
    QCOMPARE(_cm->calledEndDate, now.addDays(1).date());

    // Should not reload the range for same day as it's already loaded
    cmc.match(now.addSecs(60), condCal);
    QCOMPARE(_cm->loadCalendarEntriesCalls, 1);
    QCOMPARE(_cm->calledStartDate, now.date());
    QCOMPARE(_cm->calledEndDate, now.addDays(1).date());

    // Should reload as day has changed
    now = now.addDays(1);
    cmc.match(now, condCal);
    QCOMPARE(_cm->loadCalendarEntriesCalls, 2);
    QCOMPARE(_cm->calledStartDate, now.date());
    QCOMPARE(_cm->calledEndDate, now.addDays(1).date());
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_emptyCalendar_nextSetToNextDay() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now = dateTimeBase;

    QList<CalendarEntry> entries;

    _cm->returnEntries = entries;

    // Initial state for invalid
    QCOMPARE(cmc.minimumIntervalSec(), 0);

    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    QCOMPARE(cmc.minimumIntervalSec(), (14*60/*mins/h*/)*60+1);

    // Next day set for reload when reached
    now = now.addSecs((14*60/*mins/h*/)*60+1);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    QCOMPARE(cmc.minimumIntervalSec(), (24*60*60-1)+1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_afterNoMoreConditionsMatch_nextSetToNextDay() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now = dateTimeBase;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs((5*60)), dateTimeBase.addSecs(5*60+120), "the match2", "");

    _cm->returnEntries = entries;

    // Initial state for invalid
    QCOMPARE(cmc.minimumIntervalSec(), 0);

    // Match at start time, next interval at end time
    now = now.addSecs(5*60);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    QCOMPARE(cmc.minimumIntervalSec(), 120+1);

    // Trigger at end time (not matching anymore), next interval in the next day
    now = now.addSecs(120+1);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    // Time now 10:07:01, time until next day 13:52:59 (round up + 1)
    QCOMPARE(cmc.minimumIntervalSec(), (13*60*60) + 52*60 + 59 + 1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_oneConditionBefore() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now = dateTimeBase;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match", "");
    entries << CalendarEntry(dateTimeBase.addSecs((5*60)), dateTimeBase.addSecs(5*60+120), "the match2", "");

    _cm->returnEntries = entries;

    // Initial state for invalid
    QCOMPARE(cmc.minimumIntervalSec(), 0);

    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    QCOMPARE(cmc.minimumIntervalSec(), 5*60+1);

    // When reaching calendar start time, the next timer is to endTime
    now = now.addSecs(5*60);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    QCOMPARE(cmc.minimumIntervalSec(), 120+1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_oneConditionBeforeAndThenAtEnd() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now = dateTimeBase;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match", "");
    entries << CalendarEntry(dateTimeBase.addSecs((5*60)), dateTimeBase.addSecs(5*60+120), "the match2", "");

    _cm->returnEntries = entries;

    // Initial state for invalid
    QCOMPARE(cmc.minimumIntervalSec(), 0);

    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    QCOMPARE(cmc.minimumIntervalSec(), 5*60+1);

    // The old timer is kept if time advances within the current watched range
    now = now.addSecs(5*60 - 1);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    QCOMPARE(cmc.minimumIntervalSec(), 5*60+1);

    // After the timeout trigger has been called, the interval will be updated. Since now = startTime, next stop is endTime
    now = now.addSecs(1);
    cmc.onTimeoutForNextMatchCheck();
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    // Since rounding is 1 second
    QCOMPARE(cmc.minimumIntervalSec(), 120+1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_oneConditionBeforeRoundToSecond() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now = dateTimeBase.addMSecs(-1000);;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match", "");
    entries << CalendarEntry(dateTimeBase.addMSecs((5*60*1000 + 10)), dateTimeBase.addSecs(5*60+120), "the match2", "");

    _cm->returnEntries = entries;

    // Initial state for invalid
    QCOMPARE(cmc.minimumIntervalSec(), 0);

    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    QCOMPARE(cmc.minimumIntervalSec(), 5*60 + 2);

    // Timer is updated even if it has not been triggered, if time advances past it
    now = dateTimeBase.addSecs(5*60 + 2);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    QCOMPARE(cmc.minimumIntervalSec(), 120 - 2 + 1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_oneConditionAtStart() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match", "");
    entries << CalendarEntry(dateTimeBase.addSecs((5*60)), dateTimeBase.addSecs(5*60+120), "the match2", "");

    _cm->returnEntries = entries;

    now = dateTimeBase.addSecs(5*60);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    QCOMPARE(cmc.minimumIntervalSec(), 120 + 1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_oneConditionAtEnd() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match", "");
    entries << CalendarEntry(dateTimeBase.addSecs((5*60)), dateTimeBase.addSecs(5*60+120), "the match2", "");

    _cm->returnEntries = entries;

    now = dateTimeBase.addSecs(5*60+120);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    QCOMPARE(cmc.minimumIntervalSec(), (10*60 - (5*60+120)) + 1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_oneConditionInMiddle() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match", "");
    entries << CalendarEntry(dateTimeBase.addSecs((5*60)), dateTimeBase.addSecs(5*60+120), "the match2", "");

    _cm->returnEntries = entries;

    now = dateTimeBase.addSecs(5*60+120+1);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    QCOMPARE(cmc.minimumIntervalSec(), (10*60 - (5*60+120+1)) + 1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_oneConditionBothMatchLaterEndTimeCloser() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match", "");
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+60), "the match2", "");

    _cm->returnEntries = entries;

    now = dateTimeBase.addSecs(10*60);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    QCOMPARE(cmc.minimumIntervalSec(), 60 + 1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_oneConditionBothMatchEarlierEndTimeCloser() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal;
    ConditionManagerCacheable::MatchStatus match;

    condCal.setSummaryMatch("the match, the match 2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+60), "the match", "");
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match2", "");

    _cm->returnEntries = entries;

    now = dateTimeBase.addSecs(10*60);
    match = cmc.match(now, condCal);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    QCOMPARE(cmc.minimumIntervalSec(), 60 + 1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_multiConditionFirstMatchSecondNearest() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal1;
    RuleConditionCalendar condCal2;
    ConditionManagerCacheable::MatchStatus match;

    condCal1.setSummaryMatch("the match1");
    condCal2.setSummaryMatch("the match2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs((10*60+60)), dateTimeBase.addSecs(10*60+180), "the match2", "");
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match1", "");

    _cm->returnEntries = entries;

    now = dateTimeBase.addSecs(10*60);
    match = cmc.match(now, condCal1);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    QCOMPARE(cmc.minimumIntervalSec(), 120 + 1);

    match = cmc.match(now, condCal2);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    QCOMPARE(cmc.minimumIntervalSec(), 60 + 1);
}

void
TestConditionManagerCalendar::match_nextNearestDateTime_multiConditionSecondMatchFirstNearest() {
    ConditionManagerCalendar cmc;
    RuleConditionCalendar condCal1;
    RuleConditionCalendar condCal2;
    ConditionManagerCacheable::MatchStatus match;

    condCal1.setSummaryMatch("the match1");
    condCal2.setSummaryMatch("the match2");

    // Tuesday
    QDateTime dateTimeBase = QDateTime::fromString("27.09.2011 10:00", "dd.MM.yyyy hh:mm");
    QDateTime now;

    QList<CalendarEntry> entries;
    entries << CalendarEntry(dateTimeBase.addSecs(10*60), dateTimeBase.addSecs(10*60+120), "the match2", "");
    entries << CalendarEntry(dateTimeBase.addSecs(10*60+60), dateTimeBase.addSecs(10*60+180), "the match1", "");

    _cm->returnEntries = entries;

    now = dateTimeBase.addSecs(10*60);
    match = cmc.match(now, condCal1);
    QCOMPARE(match, ConditionManagerCacheable::NotMatched);
    QCOMPARE(cmc.minimumIntervalSec(), 60 + 1);

    match = cmc.match(now, condCal2);
    QCOMPARE(match, ConditionManagerCacheable::Matched);
    QCOMPARE(cmc.minimumIntervalSec(), 60 + 1);
}
