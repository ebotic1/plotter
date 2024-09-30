cmake_minimum_required(VERSION 3.18)

set(SOLUTION_NAME modelMaker) #Unijeti naziv solution-a



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
file(GLOB HEADERS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)


file(GLOB SOURCES_GUI  ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*.cpp)
file(GLOB HEADERS_GUI  ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*.h)

file(GLOB SOURCES_TEXT  ${CMAKE_CURRENT_LIST_DIR}/src/textEditor/*.cpp)
file(GLOB HEADERS_TEXT  ${CMAKE_CURRENT_LIST_DIR}/src/textEditor/*.h)

file(GLOB INC ${CMAKE_CURRENT_LIST_DIR}/../common/*.cpp)

source_group("src_main"           FILES ${HEADERS} ${SOURCES})
source_group("src_gui"            FILES ${HEADERS_GUI} ${SOURCES_GUI})
source_group("src_text"           FILES ${HEADERS_TEXT} ${SOURCES_TEXT})

# add executable
add_executable(${SOLUTION_NAME} ${HEADERS} ${SOURCES} ${HEADERS_GUI} ${SOURCES_GUI} ${HEADERS_TEXT} ${SOURCES_TEXT} ${INC})

include("${SOURCE_ROOT}/../EquationToXML/EqToXML.cmake")
include("${SOURCE_ROOT}/../Canvas/plotlib.cmake")

target_link_libraries(${SOLUTION_NAME}	debug ${MU_LIB_DEBUG}   	optimized ${MU_LIB_RELEASE}
					debug ${NATGUI_LIB_DEBUG}   optimized ${NATGUI_LIB_RELEASE}
					${EqToXML_lib} ${plotlib_lib})


setTargetPropertiesForGUIApp(${SOLUTION_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist) 
setIDEPropertiesForGUIExecutable(${SOLUTION_NAME} ${CMAKE_CURRENT_LIST_DIR})
setIDEPropertiesForExecutable(${SOLUTION_NAME})


setPlatformDLLPath(${SOLUTION_NAME})




