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

#include "clangeditordocumentprocessor.h"

#include "clangbackendcommunicator.h"
#include "clangprojectsettings.h"
#include "clangdiagnostictooltipwidget.h"
#include "clangfixitoperation.h"
#include "clangfixitoperationsextractor.h"
#include "clangmodelmanagersupport.h"
#include "clangutils.h"

#include <diagnosticcontainer.h>
#include <sourcelocationcontainer.h>

#include <clangsupport/tokeninfocontainer.h>

#include <cppeditor/builtincursorinfo.h>
#include <cppeditor/clangdiagnosticconfigsmodel.h>
#include <cppeditor/compileroptionsbuilder.h>
#include <cppeditor/cppcodemodelsettings.h>
#include <cppeditor/cppmodelmanager.h>
#include <cppeditor/cpptoolsreuse.h>
#include <cppeditor/cppworkingcopy.h>
#include <cppeditor/editordocumenthandle.h>

#include <texteditor/fontsettings.h>
#include <texteditor/texteditor.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditorsettings.h>

#include <cplusplus/CppDocument.h>

#include <utils/algorithm.h>
#include <utils/textutils.h>
#include <utils/qtcassert.h>
#include <utils/runextensions.h>

#include <QTextBlock>
#include <QVBoxLayout>
#include <QWidget>

namespace ClangCodeModel {
namespace Internal {

ClangEditorDocumentProcessor::ClangEditorDocumentProcessor(TextEditor::TextDocument *document)
    : BaseEditorDocumentProcessor(document->document(), document->filePath().toString())
    , m_document(*document)
    , m_parser(new ClangEditorDocumentParser(document->filePath().toString()))
    , m_builtinProcessor(document)
{
    connect(m_parser.data(), &ClangEditorDocumentParser::projectPartInfoUpdated,
            this, &BaseEditorDocumentProcessor::projectPartInfoUpdated);

    // Forwarding the semantic info from the builtin processor enables us to provide all
    // editor (widget) related features that are not yet implemented by the clang plugin.
    connect(&m_builtinProcessor, &CppEditor::BuiltinEditorDocumentProcessor::cppDocumentUpdated,
            this, &ClangEditorDocumentProcessor::cppDocumentUpdated);
    connect(&m_builtinProcessor, &CppEditor::BuiltinEditorDocumentProcessor::semanticInfoUpdated,
            this, &ClangEditorDocumentProcessor::semanticInfoUpdated);
    connect(&m_builtinProcessor, &CppEditor::BuiltinEditorDocumentProcessor::codeWarningsUpdated,
            this, &ClangEditorDocumentProcessor::codeWarningsUpdated);
    m_builtinProcessor.setSemanticHighlightingChecker([this] {
        return !ClangModelManagerSupport::instance()->clientForFile(m_document.filePath());
    });

    m_parserSynchronizer.setCancelOnWait(true);
}

void ClangEditorDocumentProcessor::runImpl(
        const CppEditor::BaseEditorDocumentParser::UpdateParams &updateParams)
{
    m_builtinProcessor.runImpl(updateParams);
}

void ClangEditorDocumentProcessor::recalculateSemanticInfoDetached(bool force)
{
    m_builtinProcessor.recalculateSemanticInfoDetached(force);
}

void ClangEditorDocumentProcessor::semanticRehighlight()
{
    const auto matchesEditor = [this](const Core::IEditor *editor) {
        return editor->document()->filePath() == m_document.filePath();
    };
    if (!Utils::contains(Core::EditorManager::visibleEditors(), matchesEditor))
        return;
    if (ClangModelManagerSupport::instance()->clientForFile(m_document.filePath()))
        return;
    m_builtinProcessor.semanticRehighlight();
}

CppEditor::SemanticInfo ClangEditorDocumentProcessor::recalculateSemanticInfo()
{
    return m_builtinProcessor.recalculateSemanticInfo();
}

CppEditor::BaseEditorDocumentParser::Ptr ClangEditorDocumentProcessor::parser()
{
    return m_builtinProcessor.parser();
}

CPlusPlus::Snapshot ClangEditorDocumentProcessor::snapshot()
{
   return m_builtinProcessor.snapshot();
}

bool ClangEditorDocumentProcessor::isParserRunning() const
{
    return m_builtinProcessor.isParserRunning();
}

bool ClangEditorDocumentProcessor::hasProjectPart() const
{
    return !m_projectPart.isNull();
}

CppEditor::ProjectPart::ConstPtr ClangEditorDocumentProcessor::projectPart() const
{
    return m_projectPart;
}

void ClangEditorDocumentProcessor::clearProjectPart()
{
    m_projectPart.clear();
}

::Utils::Id ClangEditorDocumentProcessor::diagnosticConfigId() const
{
    return m_diagnosticConfigId;
}

void ClangEditorDocumentProcessor::setParserConfig(
        const CppEditor::BaseEditorDocumentParser::Configuration &config)
{
    m_parser->setConfiguration(config);
    m_builtinProcessor.parser()->setConfiguration(config);
    emit parserConfigChanged(Utils::FilePath::fromString(filePath()), config);
}

CppEditor::BaseEditorDocumentParser::Configuration ClangEditorDocumentProcessor::parserConfig() const
{
    return m_parser->configuration();
}

QFuture<CppEditor::CursorInfo>
ClangEditorDocumentProcessor::cursorInfo(const CppEditor::CursorInfoParams &params)
{
    return m_builtinProcessor.cursorInfo(params);
}

ClangEditorDocumentProcessor *ClangEditorDocumentProcessor::get(const QString &filePath)
{
    return qobject_cast<ClangEditorDocumentProcessor*>(
                CppEditor::CppModelManager::cppEditorDocumentProcessor(filePath));
}

} // namespace Internal
} // namespace ClangCodeModel
