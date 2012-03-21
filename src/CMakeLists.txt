CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#SET( CMAKE_BUILD_TYPE Debug )
#
SET(LogoSources Logo_Common Logo_Core Logo_Logger Logo_Utils )
#INCLUDE
LINK_DIRECTORIES("/usr/local/lib")
#
FIND_PACKAGE(OpenCV REQUIRED)
#
ADD_LIBRARY(logorec_dynamic SHARED ${LogoSources})
SET_TARGET_PROPERTIES(logorec_dynamic PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(logorec_dynamic ${OpenCV_LIBS})

#
ADD_LIBRARY(logorec_static STATIC ${LogoSources})
SET_TARGET_PROPERTIES(logorec_static PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(logorec_static ${OpenCV_LIBS})
#
#
ADD_EXECUTABLE(logorec Logo_main.cpp)
TARGET_LINK_LIBRARIES(logorec logorec_dynamic ${OpenCV_LIBS})
