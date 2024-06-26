/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include "debugger_global.h"

#include <extensionsystem/iplugin.h>
#include <utils/filepath.h>

namespace ProjectExplorer { class RunControl; }

namespace Debugger::Internal {

class DebuggerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Debugger.json")

public:
    DebuggerPlugin();
    ~DebuggerPlugin() override;

private:
    // IPlugin implementation.
    bool initialize(const QStringList &arguments, QString *errorMessage) override;
    QObject *remoteCommand(const QStringList &options,
                           const QString &workingDirectory,
                           const QStringList &arguments) override;
    ShutdownFlag aboutToShutdown() override;
    void extensionsInitialized() override;

    // Called from AppOutputPane::attachToRunControl().
    Q_SLOT void attachExternalApplication(ProjectExplorer::RunControl *rc);

    // Called from GammaRayIntegration
    Q_SLOT void getEnginesState(QByteArray *json) const;

    // Called from DockerDevice
    Q_SLOT void autoDetectDebuggersForDevice(const Utils::FilePaths &searchPaths,
                                             const QString &detectionId,
                                             QString *logMessage);
    Q_SLOT void removeDetectedDebuggers(const QString &detectionId, QString *logMessage);
    Q_SLOT void listDetectedDebuggers(const QString &detectionId, QString *logMessage);

    QVector<QObject *> createTestObjects() const override;
};

} // Debugger::Internal

Q_DECLARE_METATYPE(QString *)
