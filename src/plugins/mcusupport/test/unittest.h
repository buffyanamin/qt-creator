/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "mcupackage.h"
#include "mcusupportoptions.h"
#include "mcusupportplugin.h"
#include "mcusupportsdk.h"
#include "mcutarget.h"
#include "packagemock.h"

#include <projectexplorer/kit.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/projectexplorer.h>
#include <utils/filepath.h>
#include <utils/fileutils.h>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

namespace McuSupport::Internal::Test {

class McuSupportTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void test_addNewKit();
    void test_parseBasicInfoFromJson();
    void test_createPackagesWithCorrespondingSettings();
    void test_createPackagesWithCorrespondingSettings_data();
    void test_createFreeRtosPackageWithCorrectSetting_data();
    void test_createFreeRtosPackageWithCorrectSetting();
    void test_createTargetsTheNewWay_data();
    void test_createTargetsTheNewWay();
    void test_createPackages();
    void test_parseCmakeEntries();
    void test_removeRtosSuffix_data();
    void test_removeRtosSuffix();

private:
    QVersionNumber currentQulVersion{2, 0};

    const QString id{"target_id"};
    const QString name{"target_name"};
    const QString vendor{"target_vendor"};

    const QString freeRtosEnvVar{"EVK_MIMXRT1170_FREERTOS_PATH"};
    const QString freeRtosCmakeVar{"FREERTOS_DIR"};
    const QString defaultfreeRtosPath{"/opt/freertos/default"};
}; // class McuSupportTest

} // namespace McuSupport::Internal::Test
