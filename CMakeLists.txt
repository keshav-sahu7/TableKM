cmake_minimum_required(VERSION 3.16)
project(TableKM VERSION 1.0 LANGUAGES CXX)

set(LANGUAGE CXX)
set(CXX_STANDARD_REQUIRED 17)

if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif(CMAKE_COMPILER_IS_GNUCXX)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS PrintSupport REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/KMTableLib/include   ${Qt6Widgets_INCLUDE_DIRS}   .)
link_directories(${CMAKE_SOURCE_DIR}/KMTableLib/lib)

set(
    K_BUILD_DIR
    ${CMAKE_BUILD_TYPE}
)


add_subdirectory(delegates)
add_subdirectory(datafmts)
add_subdirectory(manipulator)
add_subdirectory(misc)
add_subdirectory(rwview)

set(
    SRC_FILES

    AboutUs.cpp
    CreateTableDialog.cpp
    CustomErrMsgBox.cpp
    DockWidget.cpp
    DummyBasicView.cpp
    HelpPage.cpp
    IconProvider.cpp
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
    ViewDialog.cpp
    WelcomePage.cpp
    main.cpp
)

set(
    HDR_FILES

    AboutUs.hpp
    CreateTableDialog.hpp
    CustomErrMsgBox.hpp
    DockWidget.hpp
    DummyBasicView.hpp
    HelpPage.hpp
    IconProvider.hpp
    K2QtTableModel.hpp
    KErrorToHtml.hpp
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
    ViewDialog.hpp
    WelcomePage.hpp

)

set(
    UI_FILES

    AboutUs.ui
    HelpPage.ui
    MainWindow.ui
    OpenTableDialog.ui
    QueryDialog.ui
    TableInfoDialog.ui
    ViewDialog.ui
)

set(
    QRC_FILES

    resource.qrc
)

set(
    SOURCES
    
    ${SRC_FILES}
    ${HDR_FILES}
    ${UI_FILES}
    ${QRC_FILES}
)

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
    PRIVATE KMTableLib
    
)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES

    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${K_BUILD_DIR}/bin"
    WIN32_EXECUTABLE true
)

