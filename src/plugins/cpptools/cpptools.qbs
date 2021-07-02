import qbs 1.0
import qbs.FileInfo

Project {
    name: "CppTools"

    QtcPlugin {
        Depends { name: "Qt.widgets" }
        Depends { name: "Qt.testlib"; condition: project.withAutotests }
        Depends { name: "CPlusPlus" }
        Depends { name: "ClangSupport" }
        Depends { name: "Utils" }

        Depends { name: "Core" }
        Depends { name: "TextEditor" }
        Depends { name: "ProjectExplorer" }
        Depends { name: "app_version_header" }

        pluginTestDepends: [
            "CppEditor",
            "QmakeProjectManager",
        ]

        cpp.defines: base
        Properties {
            condition: qbs.toolchain.contains("msvc")
            cpp.defines: base.concat("_SCL_SECURE_NO_WARNINGS")
        }

        files: [
            "abstracteditorsupport.cpp",
            "abstracteditorsupport.h",
            "abstractoverviewmodel.h",
            "baseeditordocumentparser.cpp",
            "baseeditordocumentparser.h",
            "baseeditordocumentprocessor.cpp",
            "baseeditordocumentprocessor.h",
            "builtineditordocumentparser.cpp",
            "builtineditordocumentparser.h",
            "builtineditordocumentprocessor.cpp",
            "builtineditordocumentprocessor.h",
            "builtinindexingsupport.cpp",
            "builtinindexingsupport.h",
            "builtincursorinfo.cpp",
            "builtincursorinfo.h",
            "clangbasechecks.ui",
            "clangdiagnosticconfig.cpp",
            "clangdiagnosticconfig.h",
            "clangdiagnosticconfigsmodel.cpp",
            "clangdiagnosticconfigsmodel.h",
            "clangdiagnosticconfigsselectionwidget.cpp",
            "clangdiagnosticconfigsselectionwidget.h",
            "clangdiagnosticconfigswidget.cpp",
            "clangdiagnosticconfigswidget.h",
            "clangdiagnosticconfigswidget.ui",
            "compileroptionsbuilder.cpp",
            "compileroptionsbuilder.h",
            "cppbuiltinmodelmanagersupport.cpp",
            "cppbuiltinmodelmanagersupport.h",
            "cppcanonicalsymbol.cpp",
            "cppcanonicalsymbol.h",
            "cppchecksymbols.cpp",
            "cppchecksymbols.h",
            "cppclassesfilter.cpp",
            "cppclassesfilter.h",
            "cppcodeformatter.cpp",
            "cppcodeformatter.h",
            "cppcodemodelinspectordumper.cpp",
            "cppcodemodelinspectordumper.h",
            "cppcodemodelsettings.cpp",
            "cppcodemodelsettings.h",
            "cppcodemodelsettingspage.cpp",
            "cppcodemodelsettingspage.h",
            "cppcodemodelsettingspage.ui",
            "cppcodestylepreferences.cpp",
            "cppcodestylepreferences.h",
            "cppcodestylepreferencesfactory.cpp",
            "cppcodestylepreferencesfactory.h",
            "cppcodestylesettings.cpp",
            "cppcodestylesettings.h",
            "cppcodestylesettingspage.cpp",
            "cppcodestylesettingspage.h",
            "cppcodestylesettingspage.ui",
            "cppcodestylesnippets.h",
            "cppcompletionassist.cpp",
            "cppcompletionassist.h",
            "cppcompletionassistprocessor.cpp",
            "cppcompletionassistprocessor.h",
            "cppcompletionassistprovider.cpp",
            "cppcompletionassistprovider.h",
            "cppcursorinfo.h",
            "cppcurrentdocumentfilter.cpp",
            "cppcurrentdocumentfilter.h",
            "cppdoxygen.cpp",
            "cppdoxygen.h",
            "cppeditoroutline.cpp",
            "cppeditoroutline.h",
            "cppeditorwidgetinterface.h",
            "cppelementevaluator.cpp",
            "cppelementevaluator.h",
            "cppfileiterationorder.cpp",
            "cppfileiterationorder.h",
            "cppfilesettingspage.cpp",
            "cppfilesettingspage.h",
            "cppfilesettingspage.ui",
            "cppfindreferences.cpp",
            "cppfindreferences.h",
            "cppfollowsymbolundercursor.cpp",
            "cppfollowsymbolundercursor.h",
            "cppfunctionsfilter.cpp",
            "cppfunctionsfilter.h",
            "cpphoverhandler.cpp",
            "cpphoverhandler.h",
            "cppincludesfilter.cpp",
            "cppincludesfilter.h",
            "cppindexingsupport.cpp",
            "cppindexingsupport.h",
            "cpplocalsymbols.cpp",
            "cpplocalsymbols.h",
            "cpplocatordata.cpp",
            "cpplocatordata.h",
            "cpplocatorfilter.cpp",
            "cpplocatorfilter.h",
            "cppmodelmanager.cpp",
            "cppmodelmanager.h",
            "cppmodelmanagersupport.cpp",
            "cppmodelmanagersupport.h",
            "cppoverviewmodel.cpp",
            "cppoverviewmodel.h",
            "cpppointerdeclarationformatter.cpp",
            "cpppointerdeclarationformatter.h",
            "cppprojectfile.cpp",
            "cppprojectfile.h",
            "cppprojectfilecategorizer.cpp",
            "cppprojectfilecategorizer.h",
            "cppprojectinfogenerator.cpp",
            "cppprojectinfogenerator.h",
            "cppprojectpartchooser.cpp",
            "cppprojectpartchooser.h",
            "cppprojectupdater.cpp",
            "cppprojectupdater.h",
            "cppprojectupdaterinterface.h",
            "cppqtstyleindenter.cpp",
            "cppqtstyleindenter.h",
            "cpprefactoringchanges.cpp",
            "cpprefactoringchanges.h",
            "cpprefactoringengine.cpp",
            "cpprefactoringengine.h",
            "cppselectionchanger.cpp",
            "cppselectionchanger.h",
            "cppsemanticinfo.h",
            "cppsemanticinfoupdater.cpp",
            "cppsemanticinfoupdater.h",
            "cppsourceprocessor.cpp",
            "cppsourceprocessor.h",
            "cpptools_global.h",
            "cpptools_utils.h",
            "cpptoolsbridge.cpp",
            "cpptoolsbridge.h",
            "cpptoolsbridgeinterface.h",
            "cpptoolsbridgeqtcreatorimplementation.cpp",
            "cpptoolsbridgeqtcreatorimplementation.h",
            "cpptoolsconstants.h",
            "cpptoolsjsextension.cpp",
            "cpptoolsjsextension.h",
            "cpptoolsplugin.cpp",
            "cpptoolsplugin.h",
            "cpptoolsreuse.cpp",
            "cpptoolsreuse.h",
            "cpptoolssettings.cpp",
            "cpptoolssettings.h",
            "cppvirtualfunctionassistprovider.cpp",
            "cppvirtualfunctionassistprovider.h",
            "cppvirtualfunctionproposalitem.cpp",
            "cppvirtualfunctionproposalitem.h",
            "cppworkingcopy.cpp",
            "cppworkingcopy.h",
            "cursorineditor.h",
            "doxygengenerator.cpp",
            "doxygengenerator.h",
            "editordocumenthandle.cpp",
            "editordocumenthandle.h",
            "followsymbolinterface.h",
            "functionutils.cpp",
            "functionutils.h",
            "generatedcodemodelsupport.cpp",
            "generatedcodemodelsupport.h",
            "headerpathfilter.cpp",
            "headerpathfilter.h",
            "includeutils.cpp",
            "includeutils.h",
            "indexitem.cpp",
            "indexitem.h",
            "insertionpointlocator.cpp",
            "insertionpointlocator.h",
            "wrappablelineedit.cpp",
            "wrappablelineedit.h",
            "projectinfo.cpp",
            "projectinfo.h",
            "projectpart.cpp",
            "projectpart.h",
            "searchsymbols.cpp",
            "searchsymbols.h",
            "semantichighlighter.cpp",
            "semantichighlighter.h",
            "senddocumenttracker.cpp",
            "senddocumenttracker.h",
            "stringtable.cpp",
            "stringtable.h",
            "symbolfinder.cpp",
            "symbolfinder.h",
            "symbolsfindfilter.cpp",
            "symbolsfindfilter.h",
            "typehierarchybuilder.cpp",
            "typehierarchybuilder.h",
            "usages.h",
        ]

        Group {
            name: "TestCase"
            condition: qtc.testsEnabled || project.withAutotests
            files: [
                "cpptoolstestcase.cpp",
                "cpptoolstestcase.h",
            ]
        }

        Group {
            name: "Tests"
            condition: qtc.testsEnabled
            files: [
                "compileroptionsbuilder_test.cpp",
                "cppcodegen_test.cpp",
                "cppcompletion_test.cpp",
                "cppheadersource_test.cpp",
                "cpplocalsymbols_test.cpp",
                "cpplocatorfilter_test.cpp",
                "cppmodelmanager_test.cpp",
                "cpppointerdeclarationformatter_test.cpp",
                "cppsourceprocessertesthelper.cpp",
                "cppsourceprocessertesthelper.h",
                "cppsourceprocessor_test.cpp",
                "modelmanagertesthelper.cpp",
                "modelmanagertesthelper.h",
                "projectinfo_test.cpp",
                "symbolsearcher_test.cpp",
                "typehierarchybuilder_test.cpp",
            ]

            cpp.defines: outer.concat(['SRCDIR="' + FileInfo.path(filePath) + '"'])
        }

        Export {
            Depends { name: "CPlusPlus" }
            Depends { name: "Qt.concurrent" }
        }
    }
}
