# Use this for installing AutobahnCpp into <prefix>/include.
cmake_minimum_required(VERSION 3.1)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.14.21.tar.gz"
    SHA1 "fd3234cfd4fed08ab142a3e244670d1ec70480e5"
)

project(autobahn-cpp VERSION 0.2.0)

option(AUTOBAHN_CPP_BUILD_EXAMPLES "Build examples" OFF)

hunter_add_package(Boost COMPONENTS program_options system thread random)
hunter_add_package(websocketpp)
hunter_add_package(msgpack)
hunter_add_package(OpenSSL)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-Wno-unused-local-typedefs -Wno-unknown-warning-option ${CMAKE_CXX_FLAGS}")
    if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
        set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
    endif()
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9.0") # i.e. if >= 4.9.0
        if (CMAKE_GENERATOR STREQUAL "Ninja")
            set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always ${CMAKE_CXX_FLAGS}")
        else()
            set(CMAKE_CXX_FLAGS "-fdiagnostics-color=auto ${CMAKE_CXX_FLAGS}")
        endif()
    endif()
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-unused-variable ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Boost CONFIG REQUIRED COMPONENTS program_options system thread random)
find_package(websocketpp CONFIG REQUIRED)
find_package(msgpack CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)


set(include_install_dir "include")
set(config_install_dir "lib/cmake/${PROJECT_NAME}")

set(PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/autobahn.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/boost_config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/exceptions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_arguments.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_auth_utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_authenticate.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_authenticate.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call_options.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call_options.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call_result.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_call_result.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_challenge.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_challenge.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_event.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_event.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_event_handler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_invocation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_invocation.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_message.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_message.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_message_type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_message_type.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_procedure.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_publication.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_publication.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_rawsocket_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_rawsocket_transport.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_register_request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_register_request.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_registration.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_registration.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_transport_handler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_session.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_session.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscribe_options.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscribe_options.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscribe_request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscribe_request.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscription.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_subscription.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_tcp_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_tcp_transport.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_transport_handler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_uds_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_unregister_request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_unregister_request.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_unsubscribe_request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_unsubscribe_request.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_websocket_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_websocket_transport.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_websocketpp_websocket_transport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/autobahn/wamp_websocketpp_websocket_transport.ipp
    )

add_library(autobahn-cpp INTERFACE)
target_link_libraries(autobahn-cpp INTERFACE
  Boost::program_options
  Boost::system
  Boost::thread
  Boost::random
  websocketpp::websocketpp
  msgpack::msgpack
  OpenSSL::Crypto
  OpenSSL::SSL
  Threads::Threads)

target_include_directories(autobahn-cpp INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${include_install_dir}>)

set(config_export_name "${PROJECT_NAME}Config")
set(targets_export_name "${PROJECT_NAME}Targets")
set(project_config "${CMAKE_CURRENT_BINARY_DIR}/${config_export_name}.cmake")
set(version_config "${CMAKE_CURRENT_BINARY_DIR}/${config_export_name}Version.cmake")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)
configure_package_config_file( # Uses target_exports_name
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)
write_basic_package_version_file(
  "${version_config}" COMPATIBILITY SameMajorVersion)

install(
  TARGETS autobahn-cpp
  EXPORT "${targets_export_name}"
  INCLUDES DESTINATION "${include_install_dir}")

install(
  EXPORT "${targets_export_name}"
  NAMESPACE "${namespace}"
  DESTINATION "${config_install_dir}")

install(
  FILES "${project_config}" "${version_config}"
  DESTINATION "${config_install_dir}")


foreach(h ${PUBLIC_HEADERS})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "include/" include "${h}")
    get_filename_component(HEADER_INCLUDE_DIRECTORY ${include} DIRECTORY)
    install(FILES ${h}
        DESTINATION ${HEADER_INCLUDE_DIRECTORY}
        COMPONENT "headers")
endforeach()


if(AUTOBAHN_CPP_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
