
set(
    DELEGATES_SRC_FILES
    
    DateDelegate.cpp
    DateTimeDelegate.cpp
    NumericalDelegate.cpp

)

set(
    DELEGATES_HDR_FILES

    DateDelegate.hpp
    DateTimeDelegate.hpp
    Delegates.hpp
    NumericalDelegate.hpp

)

set(
    DELEGATES_UI_FILES
)

set(
    DELEGATES_SOURCES

    ${DELEGATES_SRC_FILES}
    ${DELEGATES_HDR_FILES}
    ${DELEGATES_UI_FILES}
)



set(DELEGATES_LIB_NAME delegates)
add_library(${DELEGATES_LIB_NAME} STATIC ${DELEGATES_SOURCES})
add_library(km::${DELEGATES_LIB_NAME} ALIAS ${DELEGATES_LIB_NAME})

