
file(GLOB SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB INCS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

source_group("headers"            FILES ${INCS})
source_group("src"            FILES ${SOURCES})


# add executable
add_executable(${SOLUTION_NAME} ${INCS} ${SOURCES})

include(${CMAKE_CURRENT_LIST_DIR}/../Canvas/plotlib.cmake)

target_link_libraries(${SOLUTION_NAME}	debug ${MU_LIB_DEBUG}   optimized ${MU_LIB_RELEASE}
										debug ${NATGUI_LIB_DEBUG}  optimized ${NATGUI_LIB_RELEASE} 
										${plotlib_lib})

set(CMAKE_WIN32_EXECUTABLE TRUE)

set(APPWNDMENUANDTB_PLIST  ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)
setTargetPropertiesForGUIApp(${SOLUTION_NAME} $(APPWNDMENUANDTB_PLIST)) 


setIDEPropertiesForExecutable(${SOLUTION_NAME})

setPlatformDLLPath(${SOLUTION_NAME})



#MU_DEBUG, MU_RELEASE, MU_64BIT, MU_32BIT
addMUCompileDefinitions(${SOLUTION_NAME})

