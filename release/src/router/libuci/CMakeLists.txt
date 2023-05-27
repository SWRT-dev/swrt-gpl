cmake_minimum_required(VERSION 2.6)
INCLUDE(GNUInstallDirs)

PROJECT(uci C)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

ADD_DEFINITIONS(-Wall -Werror)
IF(CMAKE_C_COMPILER_VERSION VERSION_GREATER 6)
	ADD_DEFINITIONS(-Wextra -Werror=implicit-function-declaration)
	ADD_DEFINITIONS(-Wformat -Werror=format-security -Werror=format-nonliteral)
ENDIF()
ADD_DEFINITIONS(-Os -std=gnu99 -g3 -Wmissing-declarations -Wno-unused-parameter)
ADD_DEFINITIONS(-DUCI_PREFIX="${CMAKE_INSTALL_PREFIX}")


OPTION(UCI_DEBUG "debugging support" OFF)
OPTION(UCI_DEBUG_TYPECAST "typecast debugging support" OFF)
OPTION(BUILD_LUA "build Lua binding" OFF)
OPTION(BUILD_STATIC "statically linking uci" OFF)

FIND_PATH(ubox_include_dir libubox/usock.h)

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/uci_config.h.in ${CMAKE_SOURCE_DIR}/uci_config.h )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${ubox_include_dir})

SET(LIB_SOURCES libuci.c file.c util.c delta.c parse.c blob.c)

FIND_LIBRARY(ubox NAMES ubox)
IF(BUILD_STATIC)
  FIND_LIBRARY(ubox-static NAMES libubox.a)
ENDIF(BUILD_STATIC)

ADD_LIBRARY(uci SHARED ${LIB_SOURCES})
SET_TARGET_PROPERTIES(uci PROPERTIES OUTPUT_NAME uci)
TARGET_LINK_LIBRARIES(uci ${ubox})

ADD_LIBRARY(uci-static STATIC ${LIB_SOURCES})
SET_TARGET_PROPERTIES(uci-static PROPERTIES OUTPUT_NAME uci)
TARGET_LINK_LIBRARIES(uci-static ${ubox-static})

ADD_EXECUTABLE(cli cli.c)
SET_TARGET_PROPERTIES(cli PROPERTIES OUTPUT_NAME uci)
IF(BUILD_STATIC)
  TARGET_LINK_LIBRARIES(cli uci-static ${ubox-static})
ELSE(BUILD_STATIC)
  TARGET_LINK_LIBRARIES(cli uci ${ubox})
ENDIF(BUILD_STATIC)

ADD_LIBRARY(ucimap STATIC ucimap.c)

#IF(BUILD_LUA)
#  ADD_SUBDIRECTORY(lua)
#ENDIF()

#IF(UNIT_TESTING)
#  ENABLE_TESTING()
#  ADD_SUBDIRECTORY(tests)
#
#  IF(CMAKE_C_COMPILER_ID STREQUAL "Clang")
#    ADD_LIBRARY(uci-san SHARED ${LIB_SOURCES})
#    SET_TARGET_PROPERTIES(uci-san PROPERTIES OUTPUT_NAME uci-san)
#    TARGET_COMPILE_OPTIONS(uci-san PRIVATE -g -fno-omit-frame-pointer -fsanitize=undefined,address,leak -fno-sanitize-recover=all)
#    TARGET_LINK_OPTIONS(uci-san PRIVATE -fsanitize=undefined,address,leak)
#    TARGET_LINK_LIBRARIES(uci-san ${ubox})
#
#    ADD_EXECUTABLE(cli-san cli.c)
#    SET_TARGET_PROPERTIES(cli-san PROPERTIES OUTPUT_NAME uci-san)
#    TARGET_COMPILE_OPTIONS(cli-san PRIVATE -g -fno-omit-frame-pointer -fsanitize=undefined,address,leak -fno-sanitize-recover=all)
#    TARGET_LINK_OPTIONS(cli-san PRIVATE -fsanitize=undefined,address,leak)
#    TARGET_LINK_LIBRARIES(cli-san uci-san ${ubox})
#  ENDIF()
#
#ENDIF()

INSTALL(FILES uci.h uci_config.h uci_blob.h ucimap.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

INSTALL(TARGETS uci cli
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

