PROJECT(libamqp C CXX)
# SET(CMAKE_VERBOSE_MAKEFILE ON)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_INSTALL_PREFIX}/share/scripts)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

FIND_PACKAGE(UnitTest++)
IF(UNITTEST++_FOUND)
INCLUDE_DIRECTORIES(${UNITTEST++_INCLUDE_DIR})
ENABLE_TESTING()
ENDIF(UNITTEST++_FOUND)

ADD_SUBDIRECTORY(src)

# $Id: CMakeLists.txt,v 1.14 2011/03/16 12:33:34 erk Exp $
