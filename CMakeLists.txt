# Preamble
cmake_minimum_required(VERSION 3.28)
list(APPEND CMAKE_MESSAGE_CONTEXT "augmented_containers")
set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # https://discourse.cmake.org/t/cmake-3-28-cmake-cxx-compiler-clang-scan-deps-notfound-not-found/9244
project(augmented_containers
    VERSION "0.0.0"
    DESCRIPTION "Maintain algorithm's output on changing input data"
    LANGUAGES CXX
)
enable_testing()

# Project wide setup
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Targets
add_custom_target(augmented_containers_administrative_files)
set_target_properties(augmented_containers_administrative_files PROPERTIES PROJECT_LABEL "administrative_files")
target_sources(augmented_containers_administrative_files PRIVATE "./README.md" "./LICENSE")
set_source_files_properties("./README.md" "./LICENSE" PROPERTIES HEADER_FILE_ONLY YES)
add_custom_target(augmented_containers_tooling_support)
target_sources(augmented_containers_tooling_support PRIVATE "./.clang-format")
set_source_files_properties("./.clang-format" PROPERTIES HEADER_FILE_ONLY YES)

add_library(augmented_containers_headers INTERFACE)
add_library(${PROJECT_NAME}::headers ALIAS augmented_containers_headers)
set_target_properties(augmented_containers_headers PROPERTIES
    EXPORT_NAME headers
)
target_sources(augmented_containers_headers
    PUBLIC
        FILE_SET headers
            TYPE HEADERS
            BASE_DIRS "./include"
            FILES
                "./include/augmented_containers/augmented_deque.hpp"
                "./include/augmented_containers/augmented_rb3p.hpp"
                "./include/augmented_containers/augmented_rb2p.hpp"
                "./include/augmented_containers/augmented_graph.hpp"
)

# Testing
option(${PROJECT_NAME}_ENABLE_TESTING "whether to enable testing for augmented_containers" ${PROJECT_IS_TOP_LEVEL})
if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(${PROJECT_NAME}_ENABLE_TESTING)
endif()
if(${PROJECT_NAME}_ENABLE_TESTING)
block()
    list(APPEND CMAKE_MESSAGE_CONTEXT test)

    if(NOT CMAKE_VERIFY_INTERFACE_HEADER_SETS)
        message([[testing enabled but CMAKE_VERIFY_INTERFACE_HEADER_SETS is not true, consider adding
    -DCMAKE_VERIFY_INTERFACE_HEADER_SETS=TRUE
to configure phase and using
    --target all_verify_interface_header_sets
at build phase, to also verify independent interface headers]])
    endif()

    add_executable(augmented_containers_augmented_deque_test)
    target_sources(augmented_containers_augmented_deque_test PRIVATE "./src/augmented_containers/augmented_deque.test.cpp")
    target_link_libraries(augmented_containers_augmented_deque_test PRIVATE ${PROJECT_NAME}::headers)
    add_test(NAME ${PROJECT_NAME}.augmented_deque COMMAND augmented_containers_augmented_deque_test)

    add_executable(augmented_containers_augmented_rb3p_test)
    target_sources(augmented_containers_augmented_rb3p_test PRIVATE "./src/augmented_containers/augmented_rb3p.test.cpp")
    target_link_libraries(augmented_containers_augmented_rb3p_test PRIVATE ${PROJECT_NAME}::headers)
    add_test(NAME ${PROJECT_NAME}.augmented_rb3p COMMAND augmented_containers_augmented_rb3p_test)

    add_executable(augmented_containers_augmented_rb2p_test)
    target_sources(augmented_containers_augmented_rb2p_test PRIVATE "./src/augmented_containers/augmented_rb2p.test.cpp")
    target_link_libraries(augmented_containers_augmented_rb2p_test PRIVATE ${PROJECT_NAME}::headers)
    add_test(NAME ${PROJECT_NAME}.augmented_rb2p COMMAND augmented_containers_augmented_rb2p_test)

    add_executable(augmented_containers_augmented_graph_test)
    target_sources(augmented_containers_augmented_graph_test PRIVATE "./src/augmented_containers/augmented_graph.test.cpp")
    target_link_libraries(augmented_containers_augmented_graph_test PRIVATE ${PROJECT_NAME}::headers)
    add_test(NAME ${PROJECT_NAME}.augmented_graph COMMAND augmented_containers_augmented_graph_test)
endblock()
endif()

# Packaging
block()
    list(APPEND CMAKE_MESSAGE_CONTEXT packaging)

    include(GNUInstallDirs)
    set(CMAKE_INSTALL_DOCDIR "${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME}")
    if(NOT WIN32 AND PROJECT_IS_TOP_LEVEL)
        set(CMAKE_INSTALL_PREFIX "/opt/jhcarl0814/${PROJECT_NAME}")
    endif()

    install(TARGETS augmented_containers_headers
        EXPORT ${PROJECT_NAME}_export
        FILE_SET headers
            DESTINATION "./${CMAKE_INSTALL_INCLUDEDIR}"
            COMPONENT ${PROJECT_NAME}_development
        INCLUDES DESTINATION "./${CMAKE_INSTALL_INCLUDEDIR}"
    )
    install(EXPORT ${PROJECT_NAME}_export
        DESTINATION "./cmake/${PROJECT_NAME}"
        FILE ${PROJECT_NAME}Config.cmake
        NAMESPACE ${PROJECT_NAME}::
        COMPONENT ${PROJECT_NAME}_development
    )
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION "./cmake/${PROJECT_NAME}"
        COMPONENT ${PROJECT_NAME}_development
    )

    get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
    message("CPACK_COMPONENTS_ALL: ${CPACK_COMPONENTS_ALL}")

    if(PROJECT_IS_TOP_LEVEL)
        set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
        set(CPACK_PACKAGE_VENDOR "jhcarl0814")
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
        set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
        set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
        set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
        set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
        set(CPACK_VERBATIM_VARIABLES YES)
        #set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_LIST_DIR}/Description.txt") #set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
        #set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_LIST_DIR}/Welcome.txt")
        set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/License")
        #set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_LIST_DIR}/Readme.txt")
        set(CPACK_SOURCE_IGNORE_FILES
            /\\..*
            /CMakeLists\\.txt\\.user
        )
        set(CPACK_IFW_PACKAGE_TITLE "Augmented Containers")
        set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_NAME "${PROJECT_NAME}_MaintenanceTool")
        set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_INI_FILE "${CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_NAME}.ini")
        include(CPack)
        include(CPackIFW)
        cpack_add_install_type(${PROJECT_NAME}_install_type_all DISPLAY_NAME "all")
        cpack_add_component_group(${PROJECT_NAME}_install_group_top_level
            DISPLAY_NAME "Augmented Containers"
            DESCRIPTION "${PROJECT_DESCRIPTION}"
            EXPANDED
            BOLD_TITLE
        )
        cpack_add_component(${PROJECT_NAME}_development
            DISPLAY_NAME "Headers"
            DESCRIPTION "Headers Component"
            GROUP ${PROJECT_NAME}_install_group_top_level
            INSTALL_TYPES ${PROJECT_NAME}_install_type_all
        )
        cpack_ifw_configure_component_group(${PROJECT_NAME}_install_group_top_level
            LICENSES "BSL-1.0 license (Boost Software License 1.0)" "${CPACK_RESOURCE_FILE_LICENSE}"
        )
    endif()
endblock()
