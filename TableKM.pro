QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/AboutUs.cpp \
    src/CreateTableDialog.cpp \
    src/CustomErrMsgBox.cpp \
    src/DockWidget.cpp \
    src/DummyBasicView.cpp \
    src/HelpPage.cpp \
    src/IconProvider.cpp \
    src/K2QtTableModel.cpp \
    src/KPairedLineEdit.cpp \
    src/KmtFuncPopupEditor.cpp \
    src/LineEdit.cpp \
    src/MainWindow.cpp \
    src/OpenTableDialog.cpp \
    src/ProjectView.cpp \
    src/QueryDialog.cpp \
    src/SyntaxHighlighter.cpp \
    src/TableInfoDialog.cpp \
    src/TableView.cpp \
    src/ViewDialog.cpp \
    src/WelcomePage.cpp \
    src/datafmts/CellStyler.cpp \
    src/datafmts/ColorUtilities.cpp \
    src/datafmts/CommonDataFormats.cpp \
    src/datafmts/StandardStylers.cpp \
    src/delegates/DateDelegate.cpp \
    src/delegates/DateTimeDelegate.cpp \
    src/delegates/NumericalDelegate.cpp \
    src/main.cpp \
    src/manipulator/AddColumnWidget.cpp \
    src/manipulator/AddRowWidget.cpp \
    src/manipulator/AutoValueGeneratorSettingsWindow.cpp \
    src/manipulator/DataEvaluator.cpp \
    src/manipulator/DropRowWidget.cpp \
    src/manipulator/ManipulatorHelperFncs_.cpp \
    src/manipulator/TableManipulator.cpp \
    src/manipulator/TransformColumnWidget.cpp \
    src/misc/MiscFunctions_.cpp \
    src/misc/ValidatorFactory.cpp \
    src/rwview/Kmx.cpp \
    src/rwview/KmxHelper.cpp \
    src/rwview/TablePreview.cpp \
    src/rwview/ViewReadWrite.cpp

HEADERS += \
    src/AboutUs.hpp \
    src/CreateTableDialog.hpp \
    src/CustomErrMsgBox.hpp \
    src/DockWidget.hpp \
    src/DummyBasicView.hpp \
    src/HelpPage.hpp \
    src/IconProvider.hpp \
    src/K2QtTableModel.hpp \
    src/KPairedLineEdit.hpp \
    src/KmtFuncPopupEditor.hpp \
    src/LineEdit.hpp \
    src/MainWindow.hpp \
    src/OpenTableDialog.hpp \
    src/ProjectView.hpp \
    src/QueryDialog.hpp \
    src/SyntaxHighlighter.hpp \
    src/TableInfoDialog.hpp \
    src/TableView.hpp \
    src/ViewDialog.hpp \
    src/WelcomePage.hpp \
    src/datafmts/CellStyler.hpp \
    src/datafmts/ColorUtilities.hpp \
    src/datafmts/CommonDataFormats.hpp \
    src/datafmts/StandardStylers.hpp \
    src/delegates/DateDelegate.hpp \
    src/delegates/DateTimeDelegate.hpp \
    src/delegates/Delegates.hpp \
    src/delegates/NumericalDelegate.hpp \
    src/manipulator/AddColumnWidget.hpp \
    src/manipulator/AddRowWidget.hpp \
    src/manipulator/AutoValueGeneratorSettingsWindow.hpp \
    src/manipulator/DataEvaluator.hpp \
    src/manipulator/DropRowWidget.hpp \
    src/manipulator/ManipulatorHelperFncs_.hpp \
    src/manipulator/TableManipulator.hpp \
    src/manipulator/TransformColumnWidget.hpp \
    src/misc/KDefines.h \
    src/misc/MiscFunctions_.hpp \
    src/misc/SwitchLogHandlerHelper_.hpp \
    src/misc/ValidatorFactory.hpp \
    src/rwview/Kmx.hpp \
    src/rwview/TablePreview.hpp \
    src/rwview/ViewReadWrite.hpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/KMTableLib/lib/ -lKMTableLib
else:win32:CONFIG(debug, debug|release){
    exists(KMTableLib/lib/libKMTableLib-d.dll.a){
        message(found debug version of KMTableLib)
        LIBS += -L$$PWD/KMTableLib/lib/ -lKMTableLib-d
    }
    else{
        message(debug version of KMTableLib not found! using the release version of KMTableLib)
        LIBS +=  -L$$PWD/KMTableLib/lib/ -lKMTableLib
    }
}
else:unix: LIBS += -L$$PWD/KMTableLib/lib/ -lKMTableLib

INCLUDEPATH += $$PWD/KMTableLib/include
INCLUDEPATH += $$PWD/src

DEPENDPATH += $$PWD/KMTableLib/include

RESOURCES += \
    src/resource.qrc

FORMS += \
    src/AboutUs.ui \
    src/HelpPage.ui \
    src/MainWindow.ui \
    src/OpenTableDialog.ui \
    src/QueryDialog.ui \
    src/TableInfoDialog.ui \
    src/ViewDialog.ui \
    src/manipulator/AddColumnWidget.ui \
    src/manipulator/AddRowWidget.ui \
    src/manipulator/AutoValueGeneratorSettingsWindow.ui \
    src/manipulator/DropRowWidget.ui \
    src/manipulator/TableManipulator.ui \
    src/manipulator/TransformColumnWidget.ui

