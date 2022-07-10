/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator Squish plugin.
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

#include <utils/treemodel.h>

#include <QSortFilterProxyModel>

namespace Squish {
namespace Internal {

class ObjectsMapTreeItem;

enum PropertyType { Equals, RegularExpression, Wildcard };

class Property
{
public:
    Property();
    Property(const QByteArray &data);
    bool set(const QString &propName, const QString &oper, const QString &propValue);

    const QStringList toStringList() const;
    bool isContainer() const;
    bool isRelativeWidget() const;

    static PropertyType typeFromString(const QString &typeString);
    const QString toString() const;

    QString m_name;
    PropertyType m_type = Equals;
    QString m_value;

    static const QString OPERATOR_IS;
    static const QString OPERATOR_EQUALS;
    static const QString OPERATOR_REGEX;
    static const QString OPERATOR_WILDCARD;
};

typedef QList<Property> PropertyList;

class PropertyTreeItem : public Utils::TreeItem
{
public:
    explicit PropertyTreeItem(const Property &property,
                              Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsEditable
                                                    | Qt::ItemIsSelectable);

    QVariant data(int column, int role) const override;
    bool setData(int column, const QVariant &data, int role) override;
    Qt::ItemFlags flags(int column) const override;
    Property property() const { return m_property; }

private:
    Property m_property;
    Qt::ItemFlags m_flags = Qt::NoItemFlags;
};

class PropertiesModel : public Utils::TreeModel<PropertyTreeItem>
{
    Q_OBJECT
public:
    PropertiesModel(ObjectsMapTreeItem *parentItem);

    bool setData(const QModelIndex &idx, const QVariant &data, int role) override;
    ObjectsMapTreeItem *parentItem() const { return m_parentItem; }
    void addNewProperty(PropertyTreeItem *item);
    void removeProperty(PropertyTreeItem *item);
    void modifySpecialProperty(const QString &oldValue, const QString &newValue);

    QStringList allPropertyNames() const;

signals:
    void propertyChanged(
        ObjectsMapTreeItem *item, const QString &old, const QString &modified, int row, int column);
    void propertyRemoved(ObjectsMapTreeItem *item, const Property &property);
    void propertyAdded(ObjectsMapTreeItem *item);

private:
    ObjectsMapTreeItem *m_parentItem; // not owned
};

class PropertiesSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    PropertiesSortModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

} // namespace Internal
} // namespace Squish
