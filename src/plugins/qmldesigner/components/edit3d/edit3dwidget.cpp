/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
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

#include "designersettings.h"
#include "edit3dactions.h"
#include "edit3dcanvas.h"
#include "edit3dview.h"
#include "edit3dwidget.h"
#include "metainfo.h"
#include "qmldesignerconstants.h"
#include "qmldesignerplugin.h"
#include "qmlvisualnode.h"
#include "viewmanager.h"
#include <seekerslider.h>
#include <nodeinstanceview.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/icore.h>
#include <toolbox.h>
#include <utils/utilsicons.h>

#include <QActionGroup>
#include <QMimeData>
#include <QVBoxLayout>

namespace QmlDesigner {

Edit3DWidget::Edit3DWidget(Edit3DView *view) :
    m_view(view)
{
    setAcceptDrops(true);

    Core::Context context(Constants::C_QMLEDITOR3D);
    m_context = new Core::IContext(this);
    m_context->setContext(context);
    m_context->setWidget(this);

    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);

    auto fillLayout = new QVBoxLayout(this);
    fillLayout->setContentsMargins(0, 0, 0, 0);
    fillLayout->setSpacing(0);
    setLayout(fillLayout);

    SeekerSlider *seeker = new SeekerSlider(this);
    seeker->setEnabled(false);

    // Initialize toolbar
    m_toolBox = new ToolBox(seeker, this);
    fillLayout->addWidget(m_toolBox.data());

    // Iterate through view actions. A null action indicates a separator and a second null action
    // after separator indicates an exclusive group.
    auto addActionsToToolBox = [this, &context](const QVector<Edit3DAction *> &actions, bool left) {
        bool previousWasSeparator = true;
        QActionGroup *group = nullptr;
        for (auto action : actions) {
            if (action) {
                if (group)
                    group->addAction(action->action());
                addAction(action->action());
                if (left)
                    m_toolBox->addLeftSideAction(action->action());
                else
                    m_toolBox->addRightSideAction(action->action());
                previousWasSeparator = false;

                // Register action as creator command to make it configurable
                Core::Command *command = Core::ActionManager::registerAction(
                            action->action(), action->menuId().constData(), context);
                command->setDefaultKeySequence(action->action()->shortcut());
                command->augmentActionWithShortcutToolTip(action->action());
                // Clear action shortcut so it doesn't conflict with command's override action
                action->action()->setShortcut({});
            } else {
                if (previousWasSeparator) {
                    group = new QActionGroup(this);
                    previousWasSeparator = false;
                } else {
                    group = nullptr;
                    auto separator = new QAction(this);
                    separator->setSeparator(true);
                    addAction(separator);
                    if (left)
                        m_toolBox->addLeftSideAction(separator);
                    else
                        m_toolBox->addRightSideAction(separator);
                    previousWasSeparator = true;
                }
            }
        }
    };
    addActionsToToolBox(view->leftActions(), true);
    addActionsToToolBox(view->rightActions(), false);

    view->setSeeker(seeker);
    seeker->setToolTip(QLatin1String("Seek particle system time when paused."));

    QObject::connect(seeker, &SeekerSlider::positionChanged, [seeker](){
        QmlDesignerPlugin::instance()->viewManager().nodeInstanceView()
                ->view3DAction(View3DSeekActionCommand(seeker->position()));
    });

    // Onboarding label contains instructions for new users how to get 3D content into the project
    m_onboardingLabel = new QLabel(this);
    QString labelText =
            tr("Your file does not import Qt Quick 3D.<br><br>"
               "To create a 3D view, add the QtQuick3D module in the Library view. Or click"
               " <a href=\"#add_import\"><span style=\"text-decoration:none;color:%1\">here</span></a> "
               "here to add it immediately.<br><br>"
               "To import 3D assets from another tool, click the \"Add New Assets...\" button in the Assets tab of the Library view.");
    m_onboardingLabel->setText(labelText.arg(Utils::creatorTheme()->color(Utils::Theme::TextColorLink).name()));
    m_onboardingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    connect(m_onboardingLabel, &QLabel::linkActivated, this, &Edit3DWidget::linkActivated);
    fillLayout->addWidget(m_onboardingLabel.data());

    // Canvas is used to render the actual edit 3d view
    m_canvas = new Edit3DCanvas(this);
    fillLayout->addWidget(m_canvas.data());
    showCanvas(false);
}

void Edit3DWidget::contextHelp(const Core::IContext::HelpCallback &callback) const
{
    if (m_view)
        m_view->contextHelp(callback);

    callback({});
}

void Edit3DWidget::showCanvas(bool show)
{
    if (!show) {
        QImage emptyImage;
        m_canvas->updateRenderImage(emptyImage);
    }
    m_canvas->setVisible(show);
    m_onboardingLabel->setVisible(!show);
}

void Edit3DWidget::linkActivated(const QString &link)
{
    Q_UNUSED(link)
    if (m_view)
        m_view->addQuick3DImport();
}

Edit3DCanvas *Edit3DWidget::canvas() const
{
    return m_canvas.data();
}

Edit3DView *Edit3DWidget::view() const
{
    return m_view.data();
}

void Edit3DWidget::dragEnterEvent(QDragEnterEvent *dragEnterEvent)
{
    const DesignerActionManager &actionManager = QmlDesignerPlugin::instance()
                                                     ->viewManager().designerActionManager();
    if (actionManager.externalDragHasSupportedAssets(dragEnterEvent->mimeData()))
        dragEnterEvent->acceptProposedAction();
}

void Edit3DWidget::dropEvent(QDropEvent *dropEvent)
{
    const DesignerActionManager &actionManager = QmlDesignerPlugin::instance()
                                                     ->viewManager().designerActionManager();
    QHash<QString, QStringList> addedAssets = actionManager.handleExternalAssetsDrop(dropEvent->mimeData());

    view()->executeInTransaction("Edit3DWidget::dropEvent", [&] {
        // add 3D assets to 3d editor (QtQuick3D import will be added if missing)
        ItemLibraryInfo *itemLibInfo = m_view->model()->metaInfo().itemLibraryInfo();

        const QStringList added3DAssets = addedAssets.value(ComponentCoreConstants::add3DAssetsDisplayString);
        for (const QString &assetPath : added3DAssets) {
            QString fileName = QFileInfo(assetPath).baseName();
            fileName = fileName.at(0).toUpper() + fileName.mid(1); // capitalize first letter
            QString type = QString("Quick3DAssets.%1.%1").arg(fileName);
            QList<ItemLibraryEntry> entriesForType = itemLibInfo->entriesForType(type.toLatin1());
            if (!entriesForType.isEmpty()) { // should always be true, but just in case
                QmlVisualNode::createQml3DNode(view(), entriesForType.at(0),
                                               m_canvas->activeScene(), {}, false).modelNode();
            }
        }
    });
}

} // namespace QmlDesigner
