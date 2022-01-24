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

#include "utils_global.h"

#include <QProcess>

namespace Utils {

class CommandLine;
class Environment;
class FilePath;

class QTCREATOR_UTILS_EXPORT ConsoleProcess : public QObject
{
    Q_OBJECT

public:
    enum Mode { Run, Debug, Suspend };

    explicit ConsoleProcess(QObject *parent = nullptr);
    ~ConsoleProcess() override;

    void setCommand(const Utils::CommandLine &command);
    const Utils::CommandLine &commandLine() const;

    void setAbortOnMetaChars(bool abort);

    void setWorkingDirectory(const Utils::FilePath &dir);
    Utils::FilePath workingDirectory() const;

    void setEnvironment(const Environment &env);
    const Environment &environment() const;

    void setRunAsRoot(bool on);

    QProcess::ProcessError error() const;
    QString errorString() const;

    void start();
    void stopProcess();

public:
    void setMode(Mode m);
    Mode mode() const;

    bool isRunning() const; // This reflects the state of the console+stub
    qint64 processId() const;

    void kickoffProcess();
    void interruptProcess();

    qint64 applicationMainThreadID() const;

    int exitCode() const;
    QProcess::ExitStatus exitStatus() const;

    static bool startTerminalEmulator(const QString &workingDir, const Utils::Environment &env);

signals:
    void errorOccurred(QProcess::ProcessError error);

    // These reflect the state of the actual client process
    void started();
    void finished();

private:
    void stubConnectionAvailable();
    void readStubOutput();
    void stubExited();
    void cleanupAfterStartFailure(const QString &errorMessage);
    void finish(int exitCode, QProcess::ExitStatus exitStatus);
    void killProcess();
    void killStub();
    void emitError(QProcess::ProcessError err, const QString &errorString);
    QString stubServerListen();
    void stubServerShutdown();
    void cleanupStub();
    void cleanupInferior();

    class ConsoleProcessPrivate *d;
};

} // Utils
