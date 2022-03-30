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

#include <model.h>
#include <modelnode.h>

#include <QObject>
#include <QUrl>
#include <QQmlPropertyMap>
#include <QQmlComponent>
#include <QColor>
#include <QPoint>
#include <QMouseEvent>

namespace QmlDesigner {

class PropertyEditorContextObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl specificsUrl READ specificsUrl WRITE setSpecificsUrl NOTIFY specificsUrlChanged)

    Q_PROPERTY(QString specificQmlData READ specificQmlData WRITE setSpecificQmlData NOTIFY specificQmlDataChanged)
    Q_PROPERTY(QString stateName READ stateName WRITE setStateName NOTIFY stateNameChanged)
    Q_PROPERTY(QStringList allStateNames READ allStateNames WRITE setAllStateNames NOTIFY allStateNamesChanged)

    Q_PROPERTY(bool isBaseState READ isBaseState WRITE setIsBaseState NOTIFY isBaseStateChanged)
    Q_PROPERTY(bool selectionChanged READ selectionChanged WRITE setSelectionChanged NOTIFY selectionChangedChanged)

    Q_PROPERTY(int majorVersion READ majorVersion WRITE setMajorVersion NOTIFY majorVersionChanged)
    Q_PROPERTY(int minorVersion READ minorVersion WRITE setMinorVersion NOTIFY minorVersionChanged)
    Q_PROPERTY(int majorQtQuickVersion READ majorQtQuickVersion WRITE setMajorQtQuickVersion NOTIFY majorQtQuickVersionChanged)
    Q_PROPERTY(int minorQtQuickVersion READ minorQtQuickVersion WRITE setMinorQtQuickVersion NOTIFY minorQtQuickVersionChanged)

    Q_PROPERTY(bool hasAliasExport READ hasAliasExport NOTIFY hasAliasExportChanged)

    Q_PROPERTY(bool hasActiveTimeline READ hasActiveTimeline NOTIFY hasActiveTimelineChanged)

    Q_PROPERTY(QQmlPropertyMap* backendValues READ backendValues WRITE setBackendValues NOTIFY backendValuesChanged)

    Q_PROPERTY(QQmlComponent* specificQmlComponent READ specificQmlComponent NOTIFY specificQmlComponentChanged)

public:
    PropertyEditorContextObject(QObject *parent = nullptr);

    QUrl specificsUrl() const {return m_specificsUrl; }
    QString specificQmlData() const {return m_specificQmlData; }
    QString stateName() const {return m_stateName; }
    QStringList allStateNames() const { return m_allStateNames; }

    bool isBaseState() const { return m_isBaseState; }
    bool selectionChanged() const { return m_selectionChanged; }

    QQmlPropertyMap* backendValues() const { return m_backendValues; }

    Q_INVOKABLE QString convertColorToString(const QVariant &color);
    Q_INVOKABLE QColor colorFromString(const QString &colorString);
    Q_INVOKABLE QString translateFunction();

    Q_INVOKABLE QStringList autoComplete(const QString &text, int pos, bool explicitComplete, bool filter);

    Q_INVOKABLE void toogleExportAlias();

    Q_INVOKABLE void goIntoComponent();

    Q_INVOKABLE void changeTypeName(const QString &typeName);
    Q_INVOKABLE void insertKeyframe(const QString &propertyName);

    Q_INVOKABLE void hideCursor();
    Q_INVOKABLE void restoreCursor();
    Q_INVOKABLE void holdCursorInPlace();

    Q_INVOKABLE int devicePixelRatio();

    Q_INVOKABLE QStringList styleNamesForFamily(const QString &family);

    Q_INVOKABLE QStringList allStatesForId(const QString &id);

    Q_INVOKABLE bool isBlocked(const QString &propName) const;

    int majorVersion() const;
    int majorQtQuickVersion() const;
    int minorQtQuickVersion() const;
    void setMajorVersion(int majorVersion);
    void setMajorQtQuickVersion(int majorVersion);
    void setMinorQtQuickVersion(int minorVersion);
    int minorVersion() const;
    void setMinorVersion(int minorVersion);

    bool hasActiveTimeline() const;
    void setHasActiveTimeline(bool b);

    void insertInQmlContext(QQmlContext *context);
    QQmlComponent *specificQmlComponent();

    bool hasAliasExport() const { return m_aliasExport; }

signals:
    void specificsUrlChanged();
    void specificQmlDataChanged();
    void stateNameChanged();
    void allStateNamesChanged();
    void isBaseStateChanged();
    void selectionChangedChanged();
    void backendValuesChanged();
    void majorVersionChanged();
    void minorVersionChanged();
    void majorQtQuickVersionChanged();
    void minorQtQuickVersionChanged();
    void specificQmlComponentChanged();
    void hasAliasExportChanged();
    void hasActiveTimelineChanged();

public slots:

     void setSpecificsUrl(const QUrl &newSpecificsUrl);

     void setSpecificQmlData(const QString &newSpecificQmlData);

     void setStateName(const QString &newStateName);

     void setAllStateNames(const QStringList &allStates);

     void setIsBaseState(bool newIsBaseState);

     void setSelectionChanged(bool newSelectionChanged);

     void setBackendValues(QQmlPropertyMap* newBackendValues);

     void setModel(Model *model);

     void triggerSelectionChanged();
     void setHasAliasExport(bool hasAliasExport);

private:
    QUrl m_specificsUrl;

    QString m_specificQmlData;
    QString m_stateName;
    QStringList m_allStateNames;

    bool m_isBaseState;
    bool m_selectionChanged;

    QQmlPropertyMap* m_backendValues;

    int m_majorVersion = 1;
    int m_minorVersion = 1;
    int m_majorQtQuickVersion = 1;
    int m_minorQtQuickVersion = -1;
    QQmlComponent *m_qmlComponent;
    QQmlContext *m_qmlContext;

    QPoint m_lastPos;

    Model *m_model = nullptr;

    bool m_aliasExport = false;

    bool m_setHasActiveTimeline = false;
};

class EasingCurveEditor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant modelNodeBackendProperty READ modelNodeBackend WRITE setModelNodeBackend NOTIFY modelNodeBackendChanged)

public:
    EasingCurveEditor(QObject *parent = nullptr) : QObject(parent)
    {}

    static void registerDeclarativeType();
    Q_INVOKABLE void runDialog();
    void setModelNodeBackend(const QVariant &modelNodeBackend);

signals:
    void modelNodeBackendChanged();

private:
    QVariant modelNodeBackend() const;

private:
    QVariant m_modelNodeBackend;
    QmlDesigner::ModelNode m_modelNode;
};

} //QmlDesigner {
