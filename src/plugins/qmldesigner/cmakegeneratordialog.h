/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Design Tooling
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


#ifndef CMAKEGENERATORDIALOG_H
#define CMAKEGENERATORDIALOG_H

#include "checkablefilelistmodel.h"

#include <utils/fileutils.h>

#include <QDialog>


namespace QmlDesigner {
namespace GenerateCmake {

class CMakeGeneratorDialogModel : public CheckableFileListModel
{
public:
    CMakeGeneratorDialogModel(const Utils::FilePath &rootDir, const Utils::FilePaths &files, QObject *parent = nullptr);
protected:
    virtual bool checkedByDefault(const Utils::FilePath &file) const;
};

class CmakeGeneratorDialog : public QDialog
{
public:
    CmakeGeneratorDialog(const Utils::FilePath &rootDir, const Utils::FilePaths &files);
    Utils::FilePaths getFilePaths();

private:
    CheckableFileListModel *model;
};

}
}

#endif // CMAKEGENERATORDIALOG_H
