add_qtc_test(tst_qml_testcore
  EXCLUDE_FROM_PRECHECK
  CONDITION TARGET QmlProjectManager AND Qt5_VERSION VERSION_GREATER_EQUAL 6.2.0
  DEFINES
    QT_CREATOR
    QMLDESIGNER_TEST
    IDE_PLUGIN_PATH="${PROJECT_BINARY_DIR}/${IDE_PLUGIN_PATH}"
    IDE_DATA_PATH="${PROJECT_BINARY_DIR}/${IDE_DATA_PATH}"
    TESTSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
  SOURCES
    ../data/testfiles.qrc
    ../testview.cpp
    ../testview.h
    testrewriterview.cpp
    testrewriterview.h
    tst_testcore.cpp
    tst_testcore.h
    ../testconnectionmanager.cpp
    ../testconnectionmanager.h
)

include(../../../../../src/plugins/qmldesigner/qmldesignercore.cmake)
extend_with_qmldesigner_core(tst_qml_testcore)
