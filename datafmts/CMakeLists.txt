
set(
    DATAFMTS_SRC_FILES

    CellStyler.cpp
    ColorUtilities.cpp
    CommonDataFormats.cpp
    StandardStylers.cpp

)


set(
    DATAFMTS_HDR_FILES

    CellStyler.hpp
    ColorUtilities.hpp
    CommonDataFormats.hpp
    StandardStylers.hpp

)

set(
    DATAFTMS_UI_FILES
    
)



set(
    DATAFMTS_SOURCES

    ${DATAFMTS_SRC_FILES}
    ${DATAFMTS_HDR_FILES}
    ${DATAFTMS_UI_FILES}
)


set(DATAFMTS_LIB_NAME datafmts)
add_library(${DATAFMTS_LIB_NAME} STATIC ${DATAFMTS_SOURCES})
add_library(km::${DATAFMTS_LIB_NAME} ALIAS ${DATAFMTS_LIB_NAME})

