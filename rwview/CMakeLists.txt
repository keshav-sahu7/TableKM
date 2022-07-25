
set(
    RWVIEW_SRC_FILES

    Kmx.cpp
    KmxHelper.cpp
    TablePreview.cpp
    ViewReadWrite.cpp
)

set(
    RWVIEW_HDR_FILES

    Kmx.hpp
    TablePreview.hpp
    ViewReadWrite.hpp
)

set(
    RWVIEW_UI_FILES
)

set(
    RWVIEW_SOURCES

    ${RWVIEW_SRC_FILES}
    ${RWVIEW_HDR_FILES}
    ${RWVIEW_UI_FILES}
)

set(RWVIEW_LIB_NAME rwview)
add_library(${RWVIEW_LIB_NAME} STATIC ${RWVIEW_SOURCES})
add_library(km::${RWVIEW_LIB_NAME} ALIAS ${RWVIEW_LIB_NAME})

