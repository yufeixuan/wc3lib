set(wc3lib_EDITOR_QBLP_H
	blpiohandler.hpp
	blpioplugin.hpp
)
set(wc3lib_EDITOR_QBLP_SRC
	blpiohandler.cpp
	blpioplugin.cpp
)
set(wc3lib_EDITOR_QBLP_SRC ${wc3lib_EDITOR_QBLP_SRC} PARENT_SCOPE)

# BLP Qt plugin
# allows you to read and write BLP image files by using class QImage and image type "BLP"
add_library(qblp SHARED ${wc3lib_EDITOR_QBLP_SRC})
target_link_libraries(qblp wc3libcore wc3libblp Qt5::Core Qt5::Widgets)

if (WIN32)
	add_library(qblp_static ${wc3lib_EDITOR_QBLP_SRC})
	target_link_libraries(qblp_static wc3libcore wc3libblp Qt5::Core Qt5::Widgets)
endif ()

# wc3libcore is necessary because of catching Exception

if (DEBUG)
	add_subdirectory(test)
endif ()

# Detect the Qt5 plugin directory, source: https://github.com/lxde/lxqt-qtplugin/blob/master/src/CMakeLists.txt
get_target_property(QT_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE} IMPORTED_LOCATION)

if(NOT QT_QMAKE_EXECUTABLE)
	message(FATAL_ERROR "qmake is not found.")
endif()

# execute the command "qmake -query QT_INSTALL_PLUGINS" to get the path of plugins dir.
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
	OUTPUT_VARIABLE QT_PLUGINS_DIR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (QT_PLUGINS_DIR)
	message(STATUS "Qt5 plugin directory:" "${QT_PLUGINS_DIR}")
else ()
	message(FATAL_ERROR "Qt5 plugin directory cannot be detected.")
endif ()

# install
install(TARGETS qblp DESTINATION lib)
# TODO fix absolute file path
if (NOT WIN32)
	install(TARGETS qblp DESTINATION ${QT_PLUGINS_DIR}/imageformats) #/usr/lib64/qt5/plugins/imageformats/
endif ()
install(FILES ${wc3lib_EDITOR_QBLP_H} DESTINATION include/wc3lib/editor/qblp)
