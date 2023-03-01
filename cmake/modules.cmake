﻿#foreach(LIB_NAME IN ITEMS ${LIB_NAMES})
macro(add_plugin LIB_NAME LIB_DEPENDENCY)
    set(LIB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}")

    file(                                                                           
        GLOB_RECURSE LIB_SRC
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "${LIB_SRC_DIR}/*.c*"
        "${LIB_SRC_DIR}/*.h*"
    )

    if(WIN32)
        add_library(${LIB_NAME} SHARED ${LIB_SRC})
    elseif(UNIX)
        add_library(${LIB_NAME} SHARED ${LIB_SRC}) 
    endif()

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
    set_target_properties(${LIB_NAME} PROPERTIES FOLDER "Engine/Plugin")
    set_target_properties(${LIB_NAME} PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON)
    set_target_properties(${LIB_NAME} PROPERTIES CUDA_ARCHITECTURES ${CUDA_ARCH_FLAGS})
    set_target_properties(${LIB_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugin)
    

    set_target_properties(${LIB_NAME} PROPERTIES
        OUTPUT_NAME "dyno${LIB_NAME}-${PERIDYNO_LIBRARY_VERSION}")
    set_target_properties(${LIB_NAME} PROPERTIES
        CUDA_SEPARABLE_COMPILATION OFF)

    add_compile_definitions(PERIDYNO_API_EXPORTS)

    target_include_directories(${LIB_NAME} PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${PERIDYNO_INC_INSTALL_DIR}>)

    #To disable the warning "calling a constexpr __host__ function("***") from a __host__ __device__ function("***") is not allowed."
    target_compile_options(${LIB_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr;--expt-extended-lambda>)

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
    install(FILES ${FILE_DYNAMICS_HEADER}  DESTINATION ${PERIDYNO_INC_INSTALL_DIR}/Plugin/${LIB_NAME})
endmacro()


macro(add_example EXAMPLE_NAME GROUP_NAME LIB_DEPENDENCY)
    set(PROJECT_NAME ${EXAMPLE_NAME})

    file(                                                                                                       #利用glob命令读取所有源文件list
        GLOB_RECURSE SRC_LIST 
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h*"
    )

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_LIST})

    add_executable(${PROJECT_NAME} ${SRC_LIST})                                                                 #添加编译目标 可执行文件

    target_link_libraries(${PROJECT_NAME} 
        ${${LIB_DEPENDENCY}})

    file(RELATIVE_PATH PROJECT_PATH_REL "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")                  #判断当前project在根目录下的相对路径
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Examples/${GROUP_NAME}")                              #为project设定folder目录
    #    set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_BINARY_DIR}/bin/)

    if(WIN32)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    elseif(UNIX)
        if (CMAKE_BUILD_TYPE MATCHES Debug)
            set_target_properties(${PROJECT_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Debug")
        else()
            set_target_properties(${PROJECT_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Release")
        endif()
    endif()   
endmacro()