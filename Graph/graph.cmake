set(GRAPH_NAME Graph)

file(GLOB GRAPH_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB GRAPH_INCLUDE  ${CMAKE_CURRENT_LIST_DIR}/inc/*.h)
file(GLOB GRAPH_HEADERS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB GRAPH_COMMON_SRC ${CMAKE_CURRENT_LIST_DIR}/../common/*.cpp)
file(GLOB GRAPH_COMMON_H ${CMAKE_CURRENT_LIST_DIR}/../common/*.h)


source_group("inc"        FILES ${GRAPH_INCLUDE})
source_group("inc\\src"     FILES ${GRAPH_HEADERS})
source_group("inc\\common"     FILES ${GRAPH_COMMON_H})
source_group("src"         FILES ${GRAPH_SOURCES})
source_group("src\\common"     FILES ${GRAPH_COMMON_SRC})


add_library(${GRAPH_NAME} SHARED  ${GRAPH_INCLUDE} ${GRAPH_SOURCES} ${GRAPH_HEADERS} 
									${GRAPH_COMMON_SRC} ${GRAPH_COMMON_H})

target_link_libraries(${GRAPH_NAME} 		debug ${MU_LIB_DEBUG} 		optimized ${MU_LIB_RELEASE} 
										    debug ${NATGUI_LIB_DEBUG}   optimized ${NATGUI_LIB_RELEASE})

target_compile_definitions(${GRAPH_NAME} PRIVATE GRAPH_EXPORTS)

setIDEPropertiesForLib(${GRAPH_NAME})

