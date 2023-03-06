cmake_minimum_required(VERSION 3.10)

#foreach(LIB_NAME IN ITEMS ${LIB_NAMES})
macro(build_library LIB_NAME LIB_DEPENDENCY)
 set(LIB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}")
  file(                                                                           
        GLOB_RECURSE LIB_SRC
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "${LIB_SRC_DIR}/*.c*"
        "${LIB_SRC_DIR}/*.h*"
        "${LIB_SRC_DIR}/*.inl"
    )
    add_library(${LIB_NAME} SHARED ${LIB_SRC}) 

    foreach(SRC IN ITEMS ${LIB_SRC}) 
        get_filename_component(SRC_PATH "${SRC}" PATH)
        file(RELATIVE_PATH SRC_PATH_REL "${LIB_SRC_DIR}" "${SRC_PATH}")
        string(REPLACE "/" "\\" GROUP_PATH "${SRC_PATH_REL}")
        source_group("${GROUP_PATH}" FILES "${SRC}")
    endforeach()

    if(WIN32)
        target_compile_options(${LIB_NAME} PRIVATE -Xcompiler "/wd 4819") 
    endif()
    
    file(RELATIVE_PATH PROJECT_PATH_REL "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    set_target_properties(${LIB_NAME} PROPERTIES FOLDER "Engine/Dynamics")
    
    set_target_properties(${LIB_NAME} PROPERTIES
        OUTPUT_NAME "dyno${LIB_NAME}-${PERIDYNO_LIBRARY_VERSION}")

    add_compile_definitions(PERIDYNO_API_EXPORTS)

    if(WIN32)
        set_target_properties(${LIB_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    elseif(UNIX)
        if (CMAKE_BUILD_TYPE MATCHES Debug)
            set_target_properties(${LIB_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Debug")
        else()
            set_target_properties(${LIB_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Release")
        endif()
    endif()

    target_include_directories(${LIB_NAME} PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/Dynamics>
        $<INSTALL_INTERFACE:${PERIDYNO_INC_INSTALL_DIR}/Dynamics>)

    target_link_libraries(${LIB_NAME} PUBLIC ${${LIB_DEPENDENCY}})

    install(TARGETS ${LIB_NAME}
        EXPORT ${LIB_NAME}Targets
        RUNTIME  DESTINATION  ${PERIDYNO_RUNTIME_INSTALL_DIR}
        LIBRARY  DESTINATION  ${PERIDYNO_LIBRARY_INSTALL_DIR}
        ARCHIVE  DESTINATION  ${PERIDYNO_ARCHIVE_INSTALL_DIR}
        )

    install(EXPORT ${LIB_NAME}Targets DESTINATION ${PERIDYNO_CMAKE_CONFIG_INSTALL_DIR}
        FILE ${LIB_NAME}Targets.cmake)

    #Append ${LIB_NAME}Targets.cmake to the global list, which will be include in PeridynoConfig.cmake
    get_property(LOCAL_CMAKES_NAMES GLOBAL PROPERTY "GLOBAL_CMAKES_NAMES")
    list(APPEND LOCAL_CMAKES_NAMES "${LIB_NAME}Targets.cmake")    
    set_property(GLOBAL PROPERTY GLOBAL_CMAKES_NAMES ${LOCAL_CMAKES_NAMES})

    file(GLOB FILE_DYNAMICS_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}/*.h")
    install(FILES ${FILE_DYNAMICS_HEADER}  DESTINATION ${PERIDYNO_INC_INSTALL_DIR}/Dynamics/${LIB_NAME})
endmacro()
     
#set no lib for now
    set(LIB_DEPENDENCY Core Framework Topology)
    build_library(FEM LIB_DEPENDENCY)