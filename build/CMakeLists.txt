cmake_minimum_required(VERSION 3.10)

# Define xdelegate as an interface library
if(NOT TARGET xdelegate)
    add_library(xdelegate INTERFACE)
   
    #set the root for this project
    set(ROOT "${CMAKE_SOURCE_DIR}/dependencies/xdelegate")

    # path to xdelegate for the include files
    set(xdelegate_INCLUDE_LIST "${ROOT}"
    )
    
    # Define file list
    set(xdelegate_FILES_LIST "${ROOT}/source/xdelegate.h"
                             "${ROOT}/readme.md"
    )

    # Set global properties
    set_property(GLOBAL PROPERTY xdelegate_GROUP "dependencies/xcore/xdelegate")
    set_property(GLOBAL PROPERTY xdelegate_FILES "${xdelegate_FILES_LIST}")
    set_property(GLOBAL PROPERTY xdelegate_INCLUDES "${xdelegate_INCLUDE_LIST}")
    set_property(GLOBAL APPEND PROPERTY COMPONENT_REGISTRY "xdelegate")
endif()