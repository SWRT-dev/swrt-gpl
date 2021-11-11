cmake_minimum_required(VERSION 2.6)

PROJECT(uci C)

OPTION(CMAKE_INSTALL_PREFIX "Install prefix" /usr)
OPTION(UCI_CONFDIR "Global directory" "/etc/config")
OPTION(UCI_SAVEDIR "Temporary save directory" "/tmp/.uci")

ADD_DEFINITIONS(-Os -Wall -Werror --std=gnu99 -g3 -I. -DUCI_PREFIX="${CMAKE_INSTALL_PREFIX}")

OPTION(UCI_PLUGIN_SUPPORT "plugin support" ON)
OPTION(UCI_DEBUG "debugging support" OFF)
OPTION(UCI_DEBUG_TYPECAST "typecast debugging support" OFF)
OPTION(BUILD_LUA "build Lua plugin" ON)

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/uci_config.h.in ${CMAKE_SOURCE_DIR}/uci_config.h )

SET(LIB_SOURCES libuci.c file.c util.c delta.c parse.c)

ADD_LIBRARY(uci-shared SHARED ${LIB_SOURCES})
SET_TARGET_PROPERTIES(uci-shared PROPERTIES OUTPUT_NAME uci)
TARGET_LINK_LIBRARIES(uci-shared dl)

ADD_LIBRARY(uci-static STATIC ${LIB_SOURCES})
SET_TARGET_PROPERTIES(uci-static PROPERTIES OUTPUT_NAME uci)

ADD_EXECUTABLE(cli cli.c)
SET_TARGET_PROPERTIES(cli PROPERTIES OUTPUT_NAME uci)
TARGET_LINK_LIBRARIES(cli uci-shared dl)

ADD_EXECUTABLE(cli-static cli.c)
SET_TARGET_PROPERTIES(cli-static PROPERTIES OUTPUT_NAME uci-static)
TARGET_LINK_LIBRARIES(cli-static uci-static dl)

ADD_LIBRARY(ucimap STATIC ucimap.c)

#ADD_EXECUTABLE(ucimap-example ucimap-example.c)
#TARGET_LINK_LIBRARIES(ucimap-example uci-static ucimap dl)

#ADD_SUBDIRECTORY(lua)

INSTALL(FILES uci.h uci_config.h ucimap.h
	DESTINATION include
)

INSTALL(TARGETS uci-shared uci-static cli cli-static
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

