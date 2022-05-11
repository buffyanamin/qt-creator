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

#include "deviceshell.h"
#include "processinterface.h"

#include <qtcassert.h>
#include <qtcprocess.h>

#include <QLoggingCategory>
#include <QScopeGuard>

Q_LOGGING_CATEGORY(deviceShellLog, "qtc.utils.deviceshell", QtWarningMsg)

namespace Utils {

DeviceShell::DeviceShell()
{
    m_thread.setObjectName("Shell Thread");
    m_thread.start();
}

DeviceShell::~DeviceShell()
{
    if (m_thread.isRunning()) {
        m_thread.quit();
        m_thread.wait();
    }
}

bool DeviceShell::waitForStarted()
{
    QTC_ASSERT(m_shellProcess, return false);
    Q_ASSERT(QThread::currentThread() != &m_thread);

    bool result;
    QMetaObject::invokeMethod(
        m_shellProcess,
        [this] { return m_shellProcess->waitForStarted(); },
        Qt::BlockingQueuedConnection,
        &result);
    return result;
}

/*!
 * \brief DeviceShell::runInShell
 * \param cmd The command to run
 * \param stdInData Data to send to the stdin of the command
 * \return true if the command finished with EXIT_SUCCESS(0)
 *
 * Runs the cmd inside the internal shell process and return whether it exited with EXIT_SUCCESS
 *
 * Will automatically defer to the internal thread
 */
bool DeviceShell::runInShell(const CommandLine &cmd, const QByteArray &stdInData)
{
    QTC_ASSERT(m_shellProcess, return false);
    Q_ASSERT(QThread::currentThread() != &m_thread);

    bool result = false;
    QMetaObject::invokeMethod(
        m_shellProcess,
        [this, &cmd, &stdInData] { return runInShellImpl(cmd, stdInData); },
        Qt::BlockingQueuedConnection,
        &result);
    return result;
}

bool DeviceShell::runInShellImpl(const CommandLine &cmd, const QByteArray &stdInData)
{
    QTC_ASSERT(QThread::currentThread() == &m_thread, return false);

    QTC_ASSERT(m_shellProcess->isRunning(), return false);
    QTC_ASSERT(m_shellProcess, return false);
    QTC_CHECK(m_shellProcess->readAllStandardOutput().isNull()); // clean possible left-overs
    QTC_CHECK(m_shellProcess->readAllStandardError().isNull());  // clean possible left-overs
    auto cleanup = qScopeGuard(
        [this] { m_shellProcess->readAllStandardOutput(); }); // clean on assert

    QString prefix;
    if (!stdInData.isEmpty())
        prefix = "echo '" + QString::fromUtf8(stdInData.toBase64()) + "' | base64 -d | ";

    const QString suffix = " > /dev/null 2>&1\necho $?\n";
    const QString command = prefix + cmd.toUserOutput() + suffix;

    qCDebug(deviceShellLog) << "Running:" << command;

    m_shellProcess->write(command);
    m_shellProcess->waitForReadyRead();

    const QByteArray output = m_shellProcess->readAllStandardOutput();

    bool ok = false;
    const int result = output.toInt(&ok);

    qCInfo(deviceShellLog) << "Run command in shell:" << cmd.toUserOutput() << "result: " << output
                           << " ==>" << result;
    QTC_ASSERT(ok, return false);

    return result == EXIT_SUCCESS;
}

/*!
 * \brief DeviceShell::outputForRunInShell
 * \param cmd The command to run
 * \param stdInData Data to send to the stdin of the command
 * \return The stdout of the command
 *
 * Runs a command inside the running shell and returns the stdout that was generated by it.
 *
 * Will automatically defer to the internal thread
 */
DeviceShell::RunResult DeviceShell::outputForRunInShell(const CommandLine &cmd,
                                                        const QByteArray &stdInData)
{
    QTC_ASSERT(m_shellProcess, return {});
    Q_ASSERT(QThread::currentThread() != &m_thread);

    RunResult result;
    QMetaObject::invokeMethod(
        m_shellProcess,
        [this, &cmd, &stdInData] { return outputForRunInShellImpl(cmd, stdInData); },
        Qt::BlockingQueuedConnection,
        &result);
    return result;
}

DeviceShell::RunResult DeviceShell::outputForRunInShellImpl(const CommandLine &cmd,
                                                            const QByteArray &stdInData)
{
    QTC_ASSERT(QThread::currentThread() == &m_thread, return {});

    QTC_ASSERT(m_shellProcess, return {});
    QTC_CHECK(m_shellProcess->readAllStandardOutput().isNull()); // clean possible left-overs
    QTC_CHECK(m_shellProcess->readAllStandardError().isNull());  // clean possible left-overs
    auto cleanup = qScopeGuard(
        [this] { m_shellProcess->readAllStandardOutput(); }); // clean on assert

    QString prefix;
    if (!stdInData.isEmpty())
        prefix = "echo '" + QString::fromUtf8(stdInData.toBase64()) + "' | base64 -d | ";

    const QString markerCmd = "echo __qtc$?qtc__ 1>&2\n";
    const QString suffix = "\n" + markerCmd;
    const QString command = prefix + cmd.toUserOutput() + suffix;

    qCDebug(deviceShellLog) << "Running:" << command;
    m_shellProcess->write(command);

    RunResult result;

    while (true) {
        m_shellProcess->waitForReadyRead();
        QByteArray stdErr = m_shellProcess->readAllStandardError();
        if (stdErr.endsWith("qtc__\n")) {
            QByteArray marker = stdErr.right(stdErr.length() - stdErr.lastIndexOf("__qtc"));
            QByteArray exitCodeStr = marker.mid(5, marker.length() - 11);
            bool ok = false;
            const int exitCode = exitCodeStr.toInt(&ok);

            result.stdOutput = m_shellProcess->readAllStandardOutput();
            result.exitCode = ok ? exitCode : -1;
            break;
        }
    }

    //const QByteArray output = m_shellProcess->readAllStandardOutput();
    qCDebug(deviceShellLog) << "Received output:" << result.stdOutput;
    qCInfo(deviceShellLog) << "Run command in shell:" << cmd.toUserOutput()
                           << "output size:" << result.stdOutput.size()
                           << "exit code:" << result.exitCode;
    return result;
}

void DeviceShell::close()
{
    QTC_ASSERT(QThread::currentThread() == thread(), return );
    QTC_ASSERT(m_thread.isRunning(), return );

    m_thread.quit();
    m_thread.wait();
}

/*!
 * \brief DeviceShell::setupShellProcess
 *
 * Override this function to setup the shell process.
 * The default implementation just sets the command line to "bash"
 */
void DeviceShell::setupShellProcess(QtcProcess *shellProcess)
{
    shellProcess->setCommand(CommandLine{"bash"});
}

/*!
 * \brief DeviceShell::startupFailed
 *
 * Override to display custom error messages
 */
void DeviceShell::startupFailed(const CommandLine &cmdLine)
{
    qCDebug(deviceShellLog) << "Failed to start shell via:" << cmdLine.toUserOutput();
}

/*!
 * \brief DeviceShell::start
 * \return Returns true if starting the Shell process succeeded
 *
 * \note You have to call this function when deriving from DeviceShell. Current implementations call the function from their constructor.
 */
bool DeviceShell::start()
{
    m_shellProcess = new QtcProcess();
    connect(m_shellProcess, &QtcProcess::done, this, [this] { emit done(); });
    connect(m_shellProcess, &QtcProcess::errorOccurred, this, &DeviceShell::errorOccurred);
    connect(m_shellProcess, &QObject::destroyed, this, [this] { m_shellProcess = nullptr; });
    connect(&m_thread, &QThread::finished, m_shellProcess, [this] { closeShellProcess(); });

    setupShellProcess(m_shellProcess);

    m_shellProcess->setProcessMode(ProcessMode::Writer);
    m_shellProcess->setWriteData("echo\n");

    // Moving the process into its own thread ...
    m_shellProcess->moveToThread(&m_thread);

    bool result = false;
    QMetaObject::invokeMethod(
        m_shellProcess,
        [this] {
            m_shellProcess->start();

            if (!m_shellProcess->waitForStarted() || !m_shellProcess->waitForReadyRead()
                || m_shellProcess->readAllStandardOutput() != "\n") {
                closeShellProcess();
                return false;
            }
            // TODO: Check if necessary tools are available ( e.g. base64, /bin/sh etc. )
            return true;
        },
        Qt::BlockingQueuedConnection,
        &result);

    if (!result) {
        startupFailed(m_shellProcess->commandLine());
    }

    return result;
}

void DeviceShell::closeShellProcess()
{
    if (m_shellProcess) {
        if (m_shellProcess->isRunning()) {
            m_shellProcess->write("exit\n");
            if (!m_shellProcess->waitForFinished(2000))
                m_shellProcess->terminate();
        }
        m_shellProcess->deleteLater();
    }
}

} // namespace Utils
