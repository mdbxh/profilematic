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
#include <QtCore/QCoreApplication>
#include <QDBusMetaType>
#include <QProcess>

#include "profileclient.h"
#include "configuration.h"
#include "preferences.h"
#include "model/rule.h"
#include "logic/rulesmanager.h"
#include "logic/conditionmanagerfactory.h"
#include "logic/actionfactory.h"
#include "interface/profilematicinterface.h"
#include "platform/platformutil.h"

#include <stdio.h>

#define CONVERSION_WARNING_CMDLINE "/opt/profilematic/bin/profilematic -conversionWarning"
#define CREDENTIAL_WARNING_CMDLINE "/opt/profilematic/bin/profilematic -credentialWarning"
#define MULTIRULE_WARNING_CMDLINE "/opt/profilematic/bin/profilematic -multiRuleWarning"

ConditionManager *
buildConditionManager() {
    qDebug("Building conditions");
    return ConditionManagerFactory::create();
}

Action *
buildAction(ProfileClient *profileClient) {
    return ActionFactory::create(profileClient);
}

void noLoggingSink(QtMsgType, const char *) {
    // NOP
}

void credentialsCheck(const QList<Rule> &rules) {
    bool hasRulesThatNeedDeviceModeCredential = false;
    foreach (const Rule &rule, rules) {
        if (rule.action().getFlightMode() >= 0 /* ||
            Seems like power saving mode works even without the credential?
            rule.action().getPowerSavingMode() >= 0 */) {
            hasRulesThatNeedDeviceModeCredential = true;
        }
    }

    if (hasRulesThatNeedDeviceModeCredential && !PlatformUtil::instance()->hasDeviceModeCredential()) {
        qDebug("Launching credential warning");
        QProcess::startDetached(CREDENTIAL_WARNING_CMDLINE);
        qDebug("Credential warning launched");
    }
}

int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);    

    if (argc >= 2 && QLatin1String("-noDebug") == argv[1]) {
        qDebug("Inhibiting logging");
        qInstallMsgHandler(noLoggingSink);
    }

    PlatformUtil::initialize();
    ProfileClient profileClient;
    Preferences preferences;
    int rules_version = -1;
    QList<Rule> rules;
    Configuration::readRules(rules, &rules_version);
    Configuration::readPreferences(preferences);
    QScopedPointer<ConditionManager> conditionManager(buildConditionManager());
    QScopedPointer<Action> action(buildAction(&profileClient));
    RulesManager rulesManager(&rules, conditionManager.data(), action.data(), &preferences);
    ProfileMaticInterface interface(&rulesManager, &rules, &preferences, &profileClient);

    if (interface.init()) {
        fprintf(stderr, "Exiting\n");
        PlatformUtil::deinitialize();
        return -1;
    }

    credentialsCheck(rules);

    qDebug("main: refresh");

    rulesManager.refresh();

    // LATER: this code can be removed a couple of versions down the road I think.
    qDebug("rules_version: %d", rules_version);
    // rules.size > 1 because default rule always exists
    if (rules.size() > 1) {
        switch (rules_version) {
        case 0:
            qDebug("Launching conversion warning");
            QProcess::startDetached(CONVERSION_WARNING_CMDLINE);
            qDebug("Conversion warning launched");
            break;
        case 1:
            qDebug("Launching multiRule warning");
            QProcess::startDetached(MULTIRULE_WARNING_CMDLINE);
            qDebug("Multi-rule warning launched");
            break;
        }
    }

    qDebug("Starting");
    int ret = a.exec();
    qDebug("Exiting\n");

    PlatformUtil::deinitialize();

    return ret;
}
