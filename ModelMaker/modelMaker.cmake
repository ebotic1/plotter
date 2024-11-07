set(MODELMAKER_NAME modelMaker)

file(GLOB MODELMAKER_SRC  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB MODELMAKER_INC  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
include(${WORK_ROOT}/DevEnv/natGUI.cmake)

#include(${WORK_ROOT}/DevEnv/MatrixLib.cmake)
include(${WORK_ROOT}/DevEnv/SymbComp.cmake)

file(GLOB MODELMAKER_SRC_GUI  ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*/*.cpp ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*.cpp)
file(GLOB MODELMAKER_INC_GUI  ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*/*.h ${CMAKE_CURRENT_LIST_DIR}/src/guiEditor/*.h)

file(GLOB MODELMAKER_SRC_TEXT  ${CMAKE_CURRENT_LIST_DIR}/src/textEditor/*.cpp)
file(GLOB MODELMAKER_INC_TEXT  ${CMAKE_CURRENT_LIST_DIR}/src/textEditor/*.h)

file(GLOB INC ${CMAKE_CURRENT_LIST_DIR}/../common/*.cpp)

source_group("inc"           	FILES ${MODELMAKER_INC} ${MODELMAKER_INC})
source_group("src\\gui"           FILES ${MODELMAKER_INC} ${MODELMAKER_INC_GUI})
source_group("src\\text"           FILES ${MODELMAKER_INC} ${MODELMAKER_INC_TEXT})

source_group("src\\main"           FILES ${MODELMAKER_INC} ${MODELMAKER_SRC})
source_group("src\\gui"            FILES ${MODELMAKER_INC_GUI} ${MODELMAKER_SRC_GUI})
source_group("src\\text"           FILES ${MODELMAKER_INC_TEXT} ${MODELMAKER_SRC_TEXT})

# add executable
add_executable(${MODELMAKER_NAME} ${MODELMAKER_INC} ${MODELMAKER_SRC} ${MODELMAKER_INC_GUI} ${MODELMAKER_SRC_GUI} ${MODELMAKER_INC_TEXT} ${MODELMAKER_SRC_TEXT} ${INC})

include(${CMAKE_CURRENT_LIST_DIR}/../EquationToXML/EqToXML.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../Graph/graph.cmake)

target_link_libraries(${MODELMAKER_NAME}	
					debug ${MU_LIB_DEBUG}
					debug ${NATGUI_LIB_DEBUG} 
					debug ${SYMBCOMP_LIB_DEBUG}
					optimized ${NATGUI_LIB_RELEASE}
					optimized ${SYMBCOMP_LIB_RELEASE}
					optimized ${MU_LIB_RELEASE}
					${EqToXML_LIB_NAME} ${GRAPH_NAME})

setTargetPropertiesForGUIApp(${MODELMAKER_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist) 
setIDEPropertiesForGUIExecutable(${MODELMAKER_NAME} ${CMAKE_CURRENT_LIST_DIR})
setIDEPropertiesForExecutable(${MODELMAKER_NAME})

setPlatformDLLPath(${MODELMAKER_NAME})




