
add_subdirectory(delegates)
add_subdirectory(datafmts)
add_subdirectory(manipulator)
add_subdirectory(misc)
add_subdirectory(rwview)

set(
    SRC_FILES

    AboutUs.cpp
    CreateTableDialog.cpp
    CreateViewDialog.cpp
    CustomErrMsgBox.cpp
    DockWidget.cpp
    DummyBasicView.cpp
    HelpPage.cpp
    K2QtTableModel.cpp
    KPairedLineEdit.cpp
    KmtFuncPopupEditor.cpp
    LineEdit.cpp
    MainWindow.cpp
    OpenTableDialog.cpp
    ProjectView.cpp
    QueryDialog.cpp
    SyntaxHighlighter.cpp
    TableInfoDialog.cpp
    TableView.cpp
    ThemeHandler2.cpp
    WelcomePage.cpp
    main.cpp
)

set(
    HDR_FILES

    AboutUs.hpp
    CreateTableDialog.hpp
    CreateViewDialog.hpp
    CustomErrMsgBox.hpp
    DockWidget.hpp
    DummyBasicView.hpp
    HelpPage.hpp
    K2QtTableModel.hpp
    KPairedLineEdit.hpp
    KmtFuncPopupEditor.hpp
    LineEdit.hpp
    MainWindow.hpp
    OpenTableDialog.hpp
    ProjectView.hpp
    QueryDialog.hpp
    SyntaxHighlighter.hpp
    TableInfoDialog.hpp
    TableView.hpp
    ThemeHandler2.hpp
    WelcomePage.hpp

)

set(
    UI_FILES

    AboutUs.ui
    CreateViewDialog.ui
    HelpPage.ui
    MainWindow.ui
    OpenTableDialog.ui
    QueryDialog.ui
    TableInfoDialog.ui

)

set(
    QRC_FILES

    resource.qrc
    ${EXTERNAL_THEME_RESOURCE}
)

set(
    SOURCES
    
    ${SRC_FILES}
    ${HDR_FILES}
    ${UI_FILES}
    ${QRC_FILES}
)

set( KM_TABLE_LIB KMTableLib)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/KMTableLib/lib/libKMTableLib-d.dll.a")
		message(STATUS "Building with KMTableLib-d")
		set( KM_TABLE_LIB KMTableLib-d)
	endif()
endif()

add_executable(
    ${PROJECT_NAME}     #binary name
    ${SOURCES}
)



target_link_libraries(
    ${PROJECT_NAME}
    
    PRIVATE km::delegates
    PRIVATE km::datafmts
    PRIVATE km::manipulator
    PRIVATE km::misc
    PRIVATE km::rwview
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::PrintSupport
    PRIVATE ${KM_TABLE_LIB}
    
)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES

    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/bin"
    WIN32_EXECUTABLE true
)
