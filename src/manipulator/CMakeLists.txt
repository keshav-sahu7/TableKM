set(
    MANIPULATOR_SRC_FILES

    AddColumnWidget.cpp
    AddRowWidget.cpp
    AutoValueGeneratorSettingsWindow.cpp
    DataEvaluator.cpp
    DropRowWidget.cpp
    ManipulatorHelperFncs_.cpp
    TableManipulator.cpp
    TransformColumnWidget.cpp
)


set(
    MANIPULATOR_HDR_FILES

    AddColumnWidget.hpp
    AddRowWidget.hpp
    AutoValueGeneratorSettingsWindow.hpp
    DataEvaluator.hpp
    DropRowWidget.hpp
    ManipulatorHelperFncs_.hpp
    TableManipulator.hpp
    TransformColumnWidget.hpp
)


set(
    DATAFTMS_UI_FILES
    
    AddColumnWidget.ui
    AddRowWidget.ui
    AutoValueGeneratorSettingsWindow.ui
    DropRowWidget.ui
    TableManipulator.ui
    TransformColumnWidget.ui
)



set(
    MANIPULATOR_SOURCES

    ${MANIPULATOR_SRC_FILES}
    ${MANIPULATOR_HDR_FILES}
    ${DATAFTMS_UI_FILES}
)


set(MANIPULATOR_LIB_NAME manipulator)
add_library(${MANIPULATOR_LIB_NAME} STATIC ${MANIPULATOR_SOURCES})
add_library(km::${MANIPULATOR_LIB_NAME} ALIAS ${MANIPULATOR_LIB_NAME})

