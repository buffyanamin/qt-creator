import qbs 1.0

QtcPlugin {
    name: "Git"

    Depends { name: "Qt"; submodules: ["widgets", "network"] }
    Depends { name: "Utils" }

    Depends { name: "Core" }
    Depends { name: "TextEditor" }
    Depends { name: "VcsBase" }
    Depends { name: "DiffEditor" }

    cpp.defines: base.concat(["QT_NO_FOREACH"])

    files: [
        "annotationhighlighter.cpp",
        "annotationhighlighter.h",
        "branchadddialog.cpp",
        "branchadddialog.h",
        "branchadddialog.ui",
        "branchcheckoutdialog.cpp",
        "branchcheckoutdialog.h",
        "branchcheckoutdialog.ui",
        "branchmodel.cpp",
        "branchmodel.h",
        "branchview.cpp",
        "branchview.h",
        "changeselectiondialog.cpp",
        "changeselectiondialog.h",
        "changeselectiondialog.ui",
        "commitdata.cpp",
        "commitdata.h",
        "git.qrc",
        "git_global.h", "gittr.h",
        "gitclient.cpp",
        "gitclient.h",
        "gitconstants.h",
        "giteditor.cpp",
        "giteditor.h",
        "gitgrep.cpp",
        "gitgrep.h",
        "githighlighters.cpp",
        "githighlighters.h",
        "gitplugin.cpp",
        "gitplugin.h",
        "gitsettings.cpp",
        "gitsettings.h",
        "gitsubmiteditor.cpp",
        "gitsubmiteditor.h",
        "gitsubmiteditorwidget.cpp",
        "gitsubmiteditorwidget.h",
        "gitsubmitpanel.ui",
        "gitutils.cpp",
        "gitutils.h",
        "logchangedialog.cpp",
        "logchangedialog.h",
        "mergetool.cpp",
        "mergetool.h",
        "remoteadditiondialog.ui",
        "remotedialog.cpp",
        "remotedialog.h",
        "remotedialog.ui",
        "remotemodel.cpp",
        "remotemodel.h",
        "stashdialog.cpp",
        "stashdialog.h",
        "stashdialog.ui",
    ]

    Group {
        name: "Gerrit"
        prefix: "gerrit/"
        files: [
            "authenticationdialog.cpp",
            "authenticationdialog.h",
            "authenticationdialog.ui",
            "branchcombobox.cpp",
            "branchcombobox.h",
            "gerritdialog.cpp",
            "gerritdialog.h",
            "gerritdialog.ui",
            "gerritmodel.cpp",
            "gerritmodel.h",
            "gerritoptionspage.cpp",
            "gerritoptionspage.h",
            "gerritparameters.cpp",
            "gerritparameters.h",
            "gerritplugin.cpp",
            "gerritplugin.h",
            "gerritremotechooser.cpp",
            "gerritremotechooser.h",
            "gerritserver.cpp",
            "gerritserver.h",
            "gerritpushdialog.cpp",
            "gerritpushdialog.h",
            "gerritpushdialog.ui",
        ]
    }
}
