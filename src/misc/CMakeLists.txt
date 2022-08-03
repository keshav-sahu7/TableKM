
set(
    MISC_SRC_FILES
    
    MiscFunctions_.cpp
    ValidatorFactory.cpp

)


set(
    MISC_HDR_FILES
    
    KDefines.h
    MiscFunctions_.hpp
    SwitchLogHandlerHelper_.hpp
    ValidatorFactory.hpp

)

set(
    MISC_UI_FILES
)

set(
    MISC_SOURCES

    ${MISC_SRC_FILES}
    ${MISC_HDR_FILES}
    ${MISC_UI_FILES}
)

set(MISC_LIB_NAME misc)
add_library(${MISC_LIB_NAME} STATIC ${MISC_SOURCES})
add_library(km::${MISC_LIB_NAME} ALIAS ${MISC_LIB_NAME})
