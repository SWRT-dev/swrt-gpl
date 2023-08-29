cmake_minimum_required (VERSION 2.6)

project (roxml C)

set (VERSION "2")
set (SOVERSION "3.0.2")

option(CONFIG_XML_FILE   "Can the target library parse file documents. When set to OFF, this result in smaller binary." ON)
option(CONFIG_XML_EDIT   "Can the target library modify XML tree. When set to OFF, this result in smaller binary (this saves about 3kb)" ON)
option(CONFIG_XML_COMMIT "Can the target library commit a tree. When set to OFF, this result in smaller binary (this saves about 5kb)." ON)
option(CONFIG_XML_XPATH  "Can the target library perform xpath queries. When set to 0, this result in smaller binary (this saves about 20kb)." ON)

# Enable UnitTest and CDash support
# This also adds the option BUILD_TESTING
include(CTest)
if (BUILD_TESTING)
    add_subdirectory(unittest)
endif()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DLINUX)
endif()

include_directories(src)

add_definitions (-DIGNORE_EMPTY_TEXT_NODES -DCONFIG_XML_CONTENT -DCONFIG_XML_NAV -DCONFIG_XML_BUFF)

set(ROXML_SOURCES
	src/roxml_core.c
	src/roxml_parser.c
	src/roxml_buff.c
	src/roxml_nav.c
	src/roxml_content.c
	src/roxml_mem.c
	src/roxml_stub.c
)

set(ROXML_HEADERS
	src/roxml.h
	src/roxml_buff.h
	src/roxml_core.h
	src/roxml_defines.h
	src/roxml_internal.h
	src/roxml_mem.h
	src/roxml_parser.h
	src/roxml_tune.h
	src/roxml_types.h
	src/roxml_xpath.h
)

if (CONFIG_XML_FILE)
    add_definitions(-DCONFIG_XML_FILE)
    set(ROXML_SOURCES ${ROXML_SOURCES} src/roxml_file.c)
    set(ROXML_HEADERS ${ROXML_HEADERS} src/roxml_file.h)
endif()

if (CONFIG_XML_EDIT)
    add_definitions(-DCONFIG_XML_EDIT)
    set(ROXML_SOURCES ${ROXML_SOURCES} src/roxml_edit.c)
endif()

if (CONFIG_XML_COMMIT)
    add_definitions(-DCONFIG_XML_COMMIT)
    set(ROXML_SOURCES ${ROXML_SOURCES} src/roxml_commit.c)
endif()

if (CONFIG_XML_XPATH)
    add_definitions(-DCONFIG_XML_XPATH)
    set(ROXML_SOURCES ${ROXML_SOURCES} src/roxml_xpath.c)
    set(ROXML_HEADERS ${ROXML_HEADERS} src/roxml_xpath.h)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(ROXML_HEADERS ${ROXML_HEADERS} src/roxml_win32_native.h)
endif()

# Note: we need to add the headers to create Visual Studio Projects
# with working intellisense. For building the project this would not be
# necessary
add_library(roxml-dynamic SHARED ${ROXML_SOURCES} ${ROXML_HEADERS})
		
add_library(roxml-static STATIC ${ROXML_SOURCES} ${ROXML_HEADERS})
		
add_executable(roxml-tool src/roxml_shell.c)

target_link_libraries(roxml-tool roxml-dynamic)

set_target_properties (roxml-dynamic 
	PROPERTIES 
		OUTPUT_NAME roxml 
		SOVERSION ${SOVERSION} 
		VERSION ${VERSION}
	)
	
set_target_properties (roxml-static 
	PROPERTIES 
		OUTPUT_NAME roxml
	)
	
set_target_properties (roxml-tool 
	PROPERTIES 
		OUTPUT_NAME roxml
	)

install (TARGETS roxml-dynamic roxml-static DESTINATION lib)
install (TARGETS roxml-tool DESTINATION bin)
install (FILES "src/roxml.h" DESTINATION include)
