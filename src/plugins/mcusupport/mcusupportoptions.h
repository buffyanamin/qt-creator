/****************************************************************************
**
** Copyright (C) 2021
**  The Qt Company Ltd.
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

#include "mcusupport_global.h"

#include <QObject>
#include <QVector>
#include <QVersionNumber>

QT_FORWARD_DECLARE_CLASS(QWidget)

namespace Utils {
class FilePath;
class PathChooser;
class InfoLabel;
} // namespace Utils

namespace ProjectExplorer {
class Kit;
class ToolChain;
} // namespace ProjectExplorer

namespace McuSupport {
namespace Internal {

class McuPackage;
class McuToolChainPackage;

void printMessage(const QString &message, bool important);

class McuTarget : public QObject
{
    Q_OBJECT

public:
    enum class OS { Desktop, BareMetal, FreeRTOS };

    struct Platform
    {
        QString name;
        QString displayName;
        QString vendor;
    };

    enum { UnspecifiedColorDepth = -1 };

    McuTarget(const QVersionNumber &qulVersion,
              const Platform &platform,
              OS os,
              const QVector<McuPackage *> &packages,
              const McuToolChainPackage *toolChainPackage,
              int colorDepth = UnspecifiedColorDepth);

    const QVersionNumber &qulVersion() const;
    const QVector<McuPackage *> &packages() const;
    const McuToolChainPackage *toolChainPackage() const;
    const Platform &platform() const;
    OS os() const;
    int colorDepth() const;
    bool isValid() const;
    void printPackageProblems() const;

private:
    const QVersionNumber m_qulVersion;
    const Platform m_platform;
    const OS m_os;
    const QVector<McuPackage *> m_packages;
    const McuToolChainPackage *m_toolChainPackage;
    const int m_colorDepth;
};

class McuSdkRepository
{
public:
    QVector<McuPackage *> packages;
    QVector<McuTarget *> mcuTargets;

    void deletePackagesAndTargets();
};

class McuSupportOptions : public QObject
{
    Q_OBJECT

public:
    enum UpgradeOption { Ignore, Keep, Replace };

    McuSupportOptions(QObject *parent = nullptr);
    ~McuSupportOptions() override;

    McuPackage *qtForMCUsSdkPackage = nullptr;
    McuSdkRepository sdkRepository;

    void setQulDir(const Utils::FilePath &dir);
    static Utils::FilePath qulDirFromSettings();

    static QString kitName(const McuTarget *mcuTarget);

    static QList<ProjectExplorer::Kit *> existingKits(const McuTarget *mcuTarget);
    static QList<ProjectExplorer::Kit *> matchingKits(const McuTarget *mcuTarget,
                                                      const McuPackage *qtForMCUsSdkPackage);
    static QList<ProjectExplorer::Kit *> upgradeableKits(const McuTarget *mcuTarget,
                                                         const McuPackage *qtForMCUsSdkPackage);
    static QList<ProjectExplorer::Kit *> kitsWithMismatchedDependencies(const McuTarget *mcuTarget);
    static QList<ProjectExplorer::Kit *> outdatedKits();
    static void removeOutdatedKits();
    static ProjectExplorer::Kit *newKit(const McuTarget *mcuTarget, const McuPackage *qtForMCUsSdk);
    static void createAutomaticKits();
    static UpgradeOption askForKitUpgrades();
    static void upgradeKits(UpgradeOption upgradeOption);
    static void upgradeKitInPlace(ProjectExplorer::Kit *kit,
                                  const McuTarget *mcuTarget,
                                  const McuPackage *qtForMCUsSdk);
    static void fixKitsDependencies();
    void checkUpgradeableKits();
    static void fixExistingKits();
    void populatePackagesAndTargets();
    static void registerQchFiles();
    static void registerExamples();

    static const QVersionNumber &minimalQulVersion();

    static QVersionNumber kitQulVersion(const ProjectExplorer::Kit *kit);
    static bool kitUpToDate(const ProjectExplorer::Kit *kit,
                            const McuTarget *mcuTarget,
                            const McuPackage *qtForMCUsSdkPackage);

private:
    void deletePackagesAndTargets();

signals:
    void changed();
};

} // namespace Internal
} // namespace McuSupport
