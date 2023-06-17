
file(GLOB SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB INCS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)




source_group("inc"            FILES ${INCS})
source_group("src"            FILES ${SOURCES})


# add executable
add_executable(${SOLUTION_NAME} ${INCS} ${SOURCES})

include(${SOURCE_ROOT}/../Canvas/plotlib.cmake)

target_link_libraries(${SOLUTION_NAME}	debug ${MU_LIB_DEBUG}   	optimized ${MU_LIB_RELEASE}
					debug ${NATGUI_LIB_DEBUG}   optimized ${NATGUI_LIB_RELEASE} ${CANVAS_DLL})

set(CMAKE_WIN32_EXECUTABLE TRUE)

set(APPWNDMENUANDTB_PLIST  ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)

setTargetPropertiesForGUIApp(${SOLUTION_NAME} $(APPWNDMENUANDTB_PLIST)) #sranje stvar


setIDEPropertiesForExecutable(${SOLUTION_NAME})

setPlatformDLLPath(${SOLUTION_NAME})



#MU_DEBUG, MU_RELEASE, MU_64BIT, MU_32BIT
addMUCompileDefinitions(${SOLUTION_NAME})

