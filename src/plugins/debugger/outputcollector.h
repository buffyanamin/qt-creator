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

#include <QObject>

QT_BEGIN_NAMESPACE
class QLocalServer;
class QLocalSocket;
class QSocketNotifier;
QT_END_NAMESPACE

namespace Debugger::Internal {

class OutputCollector : public QObject
{
    Q_OBJECT

public:
    OutputCollector() = default;
    ~OutputCollector() override;
    bool listen();
    void shutdown();
    QString serverName() const;
    QString errorString() const;

signals:
    void byteDelivery(const QByteArray &data);

private:
    void bytesAvailable();
#ifdef Q_OS_WIN
    void newConnectionAvailable();
    QLocalServer *m_server = nullptr;
    QLocalSocket *m_socket = nullptr;
#else
    QString m_serverPath;
    int m_serverFd;
    QSocketNotifier *m_serverNotifier = nullptr;
    QString m_errorString;
#endif
};

} // Debugger::Internal
