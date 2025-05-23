cmake_minimum_required(VERSION 2.6)

PROJECT(ubus C)

ADD_DEFINITIONS(-Wall -Werror)
IF(CMAKE_C_COMPILER_VERSION VERSION_GREATER 6)
	ADD_DEFINITIONS(-Wextra -Werror=implicit-function-declaration)
	ADD_DEFINITIONS(-Wformat -Werror=format-security -Werror=format-nonliteral)
ENDIF()
ADD_DEFINITIONS(-Os -std=gnu99 -g3 -Wmissing-declarations -Wno-unused-parameter)

OPTION(BUILD_LUA "build Lua plugin" ON)
OPTION(BUILD_EXAMPLES "build examples" ON)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(UBUS_UNIX_SOCKET "/var/run/ubus/ubus.sock")
SET(UBUS_MAX_MSGLEN 1048576)

ADD_DEFINITIONS( -DUBUS_UNIX_SOCKET="${UBUS_UNIX_SOCKET}")
ADD_DEFINITIONS( -DUBUS_MAX_MSGLEN=${UBUS_MAX_MSGLEN})

IF(BUILD_STATIC)
  FIND_LIBRARY(ubox_library NAMES libubox.a)
  FIND_LIBRARY(blob_library NAMES libblobmsg_json.a)
ELSE(BUILD_STATIC)
  FIND_LIBRARY(ubox_library NAMES ubox)
  FIND_LIBRARY(blob_library NAMES blobmsg_json)
ENDIF(BUILD_STATIC)

FIND_PATH(ubox_include_dir libubox/usock.h)
INCLUDE_DIRECTORIES(${ubox_include_dir})

SET(LIB_SOURCES libubus.c libubus-io.c libubus-obj.c libubus-sub.c libubus-req.c libubus-acl.c)

IF(BUILD_STATIC)
  ADD_LIBRARY(ubus STATIC ${LIB_SOURCES})
ELSE(BUILD_STATIC)
  ADD_LIBRARY(ubus SHARED ${LIB_SOURCES})
ENDIF(BUILD_STATIC)
TARGET_LINK_LIBRARIES(ubus ${ubox_library})

find_library(json NAMES json-c json)

MACRO(ADD_UNIT_TEST_SAN name output_name)
  ADD_EXECUTABLE(${name}-san ${name}.c)
  TARGET_COMPILE_OPTIONS(${name}-san PRIVATE -g -fno-omit-frame-pointer -fsanitize=undefined,address,leak -fno-sanitize-recover=all)
  TARGET_LINK_OPTIONS(${name}-san PRIVATE -fsanitize=undefined,address,leak)
  TARGET_LINK_LIBRARIES(${name}-san ubus ubusd_library ${ubox_library} ${blob_library} ${json})
  TARGET_INCLUDE_DIRECTORIES(${name}-san PRIVATE ${PROJECT_SOURCE_DIR})
  SET_TARGET_PROPERTIES(${name}-san PROPERTIES OUTPUT_NAME ${output_name})
ENDMACRO(ADD_UNIT_TEST_SAN)

ADD_LIBRARY(ubusd_library STATIC ubusd.c ubusd_proto.c ubusd_id.c ubusd_obj.c ubusd_event.c ubusd_acl.c ubusd_monitor.c)
ADD_EXECUTABLE(ubusd ubusd_main.c)
TARGET_LINK_LIBRARIES(ubusd ubusd_library ${ubox_library} ${blob_library} ${json})

ADD_EXECUTABLE(cli cli.c)
SET_TARGET_PROPERTIES(cli PROPERTIES OUTPUT_NAME ubus)
TARGET_LINK_LIBRARIES(cli ubus ${ubox_library} ${blob_library} ${json})

ADD_SUBDIRECTORY(lua)
ADD_SUBDIRECTORY(examples)

IF(UNIT_TESTING)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY(tests)
  ADD_UNIT_TEST_SAN(cli ubus-san)
  ADD_UNIT_TEST_SAN(ubusd_main ubusd-san)
ENDIF()

IF(ABIVERSION)
	SET_TARGET_PROPERTIES(ubus PROPERTIES VERSION ${ABIVERSION})
ENDIF()

INSTALL(TARGETS ubus cli
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)
INSTALL(TARGETS ubusd
	RUNTIME DESTINATION sbin
)

INSTALL(FILES libubus.so.2021-06-03 libubus.so DESTINATION lib)
INSTALL(FILES ubusmsg.h ubus_common.h libubus.h DESTINATION include)
