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

#include "baseeditordocumentprocessor.h"
#include "cppcompletionassistprovider.h"
#include "cppmodelmanager.h"
#include "cppparsecontext.h"
#include "cppsemanticinfo.h"
#include "editordocumenthandle.h"

#include <texteditor/textdocument.h>

#include <QMutex>
#include <QTimer>

namespace CppEditor {
namespace Internal {

class CppEditorDocument : public TextEditor::TextDocument
{
    Q_OBJECT

    friend class CppEditorDocumentHandleImpl;

public:
    explicit CppEditorDocument();

    bool isObjCEnabled() const;
    void setCompletionAssistProvider(TextEditor::CompletionAssistProvider *provider) override;
    TextEditor::CompletionAssistProvider *completionAssistProvider() const override;
    TextEditor::IAssistProvider *quickFixAssistProvider() const override;

    void recalculateSemanticInfoDetached();
    SemanticInfo recalculateSemanticInfo(); // TODO: Remove me

    void setPreferredParseContext(const QString &parseContextId);
    void setExtraPreprocessorDirectives(const QByteArray &directives);

    void scheduleProcessDocument();

    ParseContextModel &parseContextModel();

    QFuture<CursorInfo> cursorInfo(const CursorInfoParams &params);
    TextEditor::TabSettings tabSettings() const override;

    bool save(QString *errorString,
              const Utils::FilePath &filePath = Utils::FilePath(),
              bool autoSave = false) override;

signals:
    void codeWarningsUpdated(unsigned contentsRevision,
                             const QList<QTextEdit::ExtraSelection> selections,
                             const TextEditor::RefactorMarkers &refactorMarkers);

    void ifdefedOutBlocksUpdated(unsigned contentsRevision,
                                 const QList<TextEditor::BlockRange> ifdefedOutBlocks);

    void cppDocumentUpdated(const CPlusPlus::Document::Ptr document);    // TODO: Remove me
    void semanticInfoUpdated(const SemanticInfo semanticInfo); // TODO: Remove me

    void preprocessorSettingsChanged(bool customSettings);

protected:
    void applyFontSettings() override;

private:
    void invalidateFormatterCache();
    void onFilePathChanged(const Utils::FilePath &oldPath, const Utils::FilePath &newPath);
    void onMimeTypeChanged();

    void onAboutToReload();
    void onReloadFinished();
    void onDiagnosticsChanged(const QString &fileName, const QString &kind);


    void reparseWithPreferredParseContext(const QString &id);

    void processDocument();

    QByteArray contentsText() const;
    unsigned contentsRevision() const;

    BaseEditorDocumentProcessor *processor();
    void resetProcessor();
    void applyPreferredParseContextFromSettings();
    void applyExtraPreprocessorDirectivesFromSettings();
    void releaseResources();

    void showHideInfoBarAboutMultipleParseContexts(bool show);

    void initializeTimer();

private:
    bool m_fileIsBeingReloaded = false;
    bool m_isObjCEnabled = false;

    // Caching contents
    mutable QMutex m_cachedContentsLock;
    mutable QByteArray m_cachedContents;
    mutable int m_cachedContentsRevision = -1;

    unsigned m_processorRevision = 0;
    QTimer m_processorTimer;
    QScopedPointer<BaseEditorDocumentProcessor> m_processor;

    CppCompletionAssistProvider *m_completionAssistProvider = nullptr;

    // (Un)Registration in CppModelManager
    QScopedPointer<CppEditorDocumentHandle> m_editorDocumentHandle;

    ParseContextModel m_parseContextModel;
};

} // namespace Internal
} // namespace CppEditor
