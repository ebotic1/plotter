set(CANVAS_DLL canvas_prozor)

file(GLOB SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB HEADERS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

add_library(${CANVAS_DLL} SHARED  ${HEADERS} ${SOURCES})

target_link_libraries(${CANVAS_DLL} 		debug ${MU_LIB_DEBUG} 		optimized ${MU_LIB_RELEASE} 
										    debug ${NATGUI_LIB_DEBUG}   optimized ${NATGUI_LIB_RELEASE})



set(APPWNDMENUANDwST  ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)

setTargetPropertiesForGUIApp(${CANVAS_DLL} $(APPWNDMENUANDwST))

target_compile_definitions(${CANVAS_DLL} PRIVATE EXPORT)

if (WIN32)
 target_compile_definitions(${CANVAS_DLL} PUBLIC   MU_WINDOWS)
elseif(APPLE)                                             
target_compile_definitions(${CANVAS_DLL} PUBLIC   MU_MACOS)
else()                                                    
target_compile_definitions(${CANVAS_DLL} PUBLIC   MU_LINUX)
endif()