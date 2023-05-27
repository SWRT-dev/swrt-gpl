cmake_minimum_required(VERSION 2.6)

PROJECT(uci C)

SET(CMAKE_INSTALL_PREFIX /)

IF(NOT LUA_CFLAGS)
	INCLUDE(FindPkgConfig)
	pkg_search_module(LUA lua5.1 lua-5.1)
ENDIF()

ADD_DEFINITIONS(-Os -Wall -Werror --std=gnu99 -g3 -I.. ${LUA_CFLAGS})
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..)

IF(APPLE)
  INCLUDE_DIRECTORIES(/opt/local/include)
  LINK_DIRECTORIES(/opt/local/lib)
  SET(CMAKE_SHARED_MODULE_CREATE_C_FLAGS "${CMAKE_SHARED_MODULE_CREATE_C_FLAGS} -undefined dynamic_lookup")
ENDIF()

IF(NOT LUAPATH)
	EXECUTE_PROCESS(
		COMMAND  lua -e "for k in string.gmatch(package.cpath .. \";\", \"([^;]+)/..so;\") do if k:sub(1,1) == \"/\" then print(k) break end end"
		OUTPUT_VARIABLE LUAPATH
		RESULT_VARIABLE LUA_CHECK_RES
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	IF(NOT ${LUA_CHECK_RES} EQUAL 0 OR "${LUAPATH}" EQUAL "")
		MESSAGE(SEND_ERROR "Lua was not found on your system")
	ENDIF()
ENDIF()

ADD_LIBRARY(uci_lua MODULE uci.c)
SET_TARGET_PROPERTIES(uci_lua PROPERTIES
	OUTPUT_NAME uci
	PREFIX ""
)
TARGET_LINK_LIBRARIES(uci_lua uci dl)

INSTALL(TARGETS uci_lua
	LIBRARY DESTINATION ${LUAPATH}
)
