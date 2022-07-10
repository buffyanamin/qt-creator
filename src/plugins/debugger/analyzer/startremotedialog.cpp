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

#include "startremotedialog.h"

#include "debuggertr.h"

#include <coreplugin/icore.h>

#include <projectexplorer/devicesupport/idevice.h>
#include <projectexplorer/devicesupport/sshparameters.h>
#include <projectexplorer/kitchooser.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/runcontrol.h>

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

using namespace ProjectExplorer;
using namespace Utils;

namespace Debugger {
namespace Internal {

class StartRemoteDialogPrivate
{
public:
    KitChooser *kitChooser;
    QLineEdit *executable;
    QLineEdit *arguments;
    QLineEdit *workingDirectory;
    QDialogButtonBox *buttonBox;
};

} // namespace Internal

StartRemoteDialog::StartRemoteDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Internal::StartRemoteDialogPrivate)
{
    setWindowTitle(Tr::tr("Start Remote Analysis"));

    d->kitChooser = new KitChooser(this);
    d->kitChooser->setKitPredicate([](const Kit *kit) {
        const IDevice::ConstPtr device = DeviceKitAspect::device(kit);
        return kit->isValid() && device && !device->sshParameters().host().isEmpty();
    });
    d->executable = new QLineEdit(this);
    d->arguments = new QLineEdit(this);
    d->workingDirectory = new QLineEdit(this);

    d->buttonBox = new QDialogButtonBox(this);
    d->buttonBox->setOrientation(Qt::Horizontal);
    d->buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    auto formLayout = new QFormLayout;
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->addRow(Tr::tr("Kit:"), d->kitChooser);
    formLayout->addRow(Tr::tr("Executable:"), d->executable);
    formLayout->addRow(Tr::tr("Arguments:"), d->arguments);
    formLayout->addRow(Tr::tr("Working directory:"), d->workingDirectory);

    auto verticalLayout = new QVBoxLayout(this);
    verticalLayout->addLayout(formLayout);
    verticalLayout->addWidget(d->buttonBox);

    QSettings *settings = Core::ICore::settings();
    settings->beginGroup("AnalyzerStartRemoteDialog");
    d->kitChooser->populate();
    d->kitChooser->setCurrentKitId(Id::fromSetting(settings->value("profile")));
    d->executable->setText(settings->value("executable").toString());
    d->workingDirectory->setText(settings->value("workingDirectory").toString());
    d->arguments->setText(settings->value("arguments").toString());
    settings->endGroup();

    connect(d->kitChooser, &KitChooser::activated, this, &StartRemoteDialog::validate);
    connect(d->executable, &QLineEdit::textChanged, this, &StartRemoteDialog::validate);
    connect(d->workingDirectory, &QLineEdit::textChanged, this, &StartRemoteDialog::validate);
    connect(d->arguments, &QLineEdit::textChanged, this, &StartRemoteDialog::validate);
    connect(d->buttonBox, &QDialogButtonBox::accepted, this, &StartRemoteDialog::accept);
    connect(d->buttonBox, &QDialogButtonBox::rejected, this, &StartRemoteDialog::reject);

    validate();
}

StartRemoteDialog::~StartRemoteDialog()
{
    delete d;
}

void StartRemoteDialog::accept()
{
    QSettings *settings = Core::ICore::settings();
    settings->beginGroup("AnalyzerStartRemoteDialog");
    settings->setValue("profile", d->kitChooser->currentKitId().toString());
    settings->setValue("executable", d->executable->text());
    settings->setValue("workingDirectory", d->workingDirectory->text());
    settings->setValue("arguments", d->arguments->text());
    settings->endGroup();

    QDialog::accept();
}

void StartRemoteDialog::validate()
{
    bool valid = !d->executable->text().isEmpty();
    d->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}

CommandLine StartRemoteDialog::commandLine() const
{
    const Kit *kit = d->kitChooser->currentKit();
    const FilePath filePath = DeviceKitAspect::deviceFilePath(kit, d->executable->text());
    return {filePath, d->arguments->text(), CommandLine::Raw};
}

FilePath StartRemoteDialog::workingDirectory() const
{
    return FilePath::fromString(d->workingDirectory->text());
}

} // Debugger
