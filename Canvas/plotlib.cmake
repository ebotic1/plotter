set(plotlib_lib canvas_prozor)

file(GLOB SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB HEADERS  ${CMAKE_CURRENT_LIST_DIR}/inc/*.h)

add_library(${plotlib_lib} SHARED  ${HEADERS} ${SOURCES})

target_link_libraries(${plotlib_lib} 		debug ${MU_LIB_DEBUG} 		optimized ${MU_LIB_RELEASE} 
										    debug ${NATGUI_LIB_DEBUG}   optimized ${NATGUI_LIB_RELEASE})

message("canvas lib included")

set(APPWNDMENUANDwST  ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)

setTargetPropertiesForGUIApp(${plotlib_lib} $(APPWNDMENUANDwST))

target_compile_definitions(${plotlib_lib} PRIVATE EXPORT)

if (WIN32)
 target_compile_definitions(${plotlib_lib} PUBLIC   MU_WINDOWS)
elseif(APPLE)                                             
target_compile_definitions(${plotlib_lib} PUBLIC   MU_MACOS)
else()                                                    
target_compile_definitions(${plotlib_lib} PUBLIC   MU_LINUX)
endif()