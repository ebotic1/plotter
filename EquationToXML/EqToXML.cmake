set(EqToXML_lib EquationToXML)

file(GLOB SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB HEADERS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

source_group("headers"        FILES ${HEADERS})
source_group("src"            FILES ${SOURCES})

add_library(${EqToXML_lib} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${EqToXML_lib}	debug ${MU_LIB_DEBUG}   	optimized ${MU_LIB_RELEASE})


#MU_DEBUG, MU_RELEASE, MU_64BIT, MU_32BIT
addMUCompileDefinitions(${EqToXML_lib})

message(INFO "EqToXML is included")

target_compile_definitions(${EqToXML_lib} PRIVATE EXPORT)

if (WIN32)
    target_compile_definitions(${EqToXML_lib} PUBLIC   MU_WINDOWS)
elseif(APPLE)                                             
    target_compile_definitions(${EqToXML_lib} PUBLIC   MU_MACOS)
else()                                                    
    target_compile_definitions(${EqToXML_lib} PUBLIC   MU_LINUX)
endif()