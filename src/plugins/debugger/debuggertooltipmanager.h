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

#include "debuggerconstants.h"

#include <utils/filepath.h>

#include <QCoreApplication>
#include <QDate>
#include <QPoint>

namespace Debugger::Internal {

class DebuggerEngine;
class StackFrame;

class DebuggerToolTipContext
{
public:
    DebuggerToolTipContext();
    bool isValid() const { return !expression.isEmpty(); }
    bool matchesFrame(const StackFrame &frame) const;
    bool isSame(const DebuggerToolTipContext &other) const;
    QString toolTip() const;

    Utils::FilePath fileName;
    int position;
    int line;
    int column;
    int scopeFromLine;
    int scopeToLine;
    QString function; //!< Optional, informational only.
    QString engineType;
    QDate creationDate;

    QPoint mousePosition;
    QString expression;
    QString iname;
    bool isCppEditor;
};

using DebuggerToolTipContexts = QList<DebuggerToolTipContext>;

class DebuggerToolTipManager
{
    Q_DISABLE_COPY_MOVE(DebuggerToolTipManager)

public:
    explicit DebuggerToolTipManager(DebuggerEngine *engine);
    ~DebuggerToolTipManager();

    void deregisterEngine();
    void updateToolTips();
    bool hasToolTips() const;

    DebuggerToolTipContexts pendingTooltips() const;

    void closeAllToolTips();
    void resetLocation();

private:
    class DebuggerToolTipManagerPrivate *d;
};

} // Debugger::Internal
