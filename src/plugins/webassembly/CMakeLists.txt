add_qtc_plugin(WebAssembly
  DEPENDS Qt5::Core
  PLUGIN_DEPENDS Core ProjectExplorer QtSupport
  SOURCES
    webassembly.qrc
    webassembly_global.h
    webassemblytr.h
    webassemblyconstants.h
    webassemblydevice.cpp webassemblydevice.h
    webassemblyemsdk.cpp webassemblyemsdk.h
    webassemblyoptionspage.cpp webassemblyoptionspage.h
    webassemblyplugin.cpp webassemblyplugin.h
    webassemblyqtversion.cpp webassemblyqtversion.h
    webassemblyrunconfigurationaspects.cpp webassemblyrunconfigurationaspects.h
    webassemblyrunconfiguration.cpp webassemblyrunconfiguration.h
    webassemblytoolchain.cpp webassemblytoolchain.h
)
