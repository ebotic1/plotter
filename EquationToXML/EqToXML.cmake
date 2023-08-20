set(EqToXML_lib EquationToXML_library)
set(EqToXML_standalone modelMaker)

file(GLOB SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/lib/*.cpp)
file(GLOB INCLUDE  ${CMAKE_CURRENT_LIST_DIR}/inc/*.h)
file(GLOB MAIN  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

source_group("inc"        FILES ${INCLUDE})
source_group("src_lib"        FILES ${SOURCES})
source_group("src"        FILES ${MAIN})

add_library(${EqToXML_lib} STATIC ${INCLUDE} ${SOURCES})

add_executable(${EqToXML_standalone} ${MAIN})

target_link_libraries(${EqToXML_lib}	debug ${MU_LIB_DEBUG}   	optimized ${MU_LIB_RELEASE})

target_link_libraries(${EqToXML_standalone}	debug ${MU_LIB_DEBUG}   	optimized ${MU_LIB_RELEASE}  ${EqToXML_lib})


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