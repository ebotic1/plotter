cmake_minimum_required(VERSION 3.18)

set(SOLUTION_NAME tanfser_editor)

#BEGIN dio koji ide u sve solutions
project(${SOLUTION_NAME})
set(SOURCE_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(HOME_ROOT $ENV{HOME})
if (WIN32)
	string(REPLACE "\\" "/" HOME_ROOT "${HOME_ROOT}")
endif()
set(WORK_ROOT ${HOME_ROOT}/Work)
include(${WORK_ROOT}/DevEnv/Common.cmake) 
#END dio koji ide u sve solutions

include(${WORK_ROOT}/DevEnv/natGUI.cmake)

file(GLOB SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

source_group("src"            FILES ${SOURCES})
source_group("header"            FILES ${HEADERS})



add_executable(${SOLUTION_NAME} ${HEADERS} ${SOURCES})


target_link_libraries(${SOLUTION_NAME}	debug ${MU_LIB_DEBUG}   	optimized ${MU_LIB_RELEASE}
					debug ${NATGUI_LIB_DEBUG}   optimized ${NATGUI_LIB_RELEASE})

set(CMAKE_WIN32_EXECUTABLE TRUE)

set(APPWNDMENUANDTB_PLIST  ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)

setTargetPropertiesForGUIApp(${SOLUTION_NAME} $(APPWNDMENUANDTB_PLIST))

setIDEPropertiesForExecutable(${SOLUTION_NAME})

setPlatformDLLPath(${SOLUTION_NAME})



#MU_DEBUG, MU_RELEASE, MU_64BIT, MU_32BIT
addMUCompileDefinitions(${SOLUTION_NAME})