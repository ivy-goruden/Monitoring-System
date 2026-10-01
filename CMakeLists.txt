cmake_minimum_required(VERSION 3.20)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

project(MonitoringSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Boost REQUIRED COMPONENTS thread)
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(TgBot CONFIG REQUIRED)

pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
find_path(MAILIO_INCLUDE_DIR mailio/message.hpp REQUIRED)
find_library(MAILIO_LIBRARY NAMES mailio REQUIRED)
find_path(TOMLPLUSPLUS_INCLUDE_DIR toml++/toml.hpp REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/agents")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_config.hpp.in"
    "${CMAKE_BINARY_DIR}/generated/build_config.hpp"
    @ONLY
)

foreach(agent IN ITEMS cpu_check mem_check swap_check)
    set(config_file "${CMAKE_CURRENT_SOURCE_DIR}/agents/lib${agent}.conf")
    set(build_config_file "${CMAKE_BINARY_DIR}/agents/lib${agent}.conf")
    if(EXISTS "${config_file}" AND NOT EXISTS "${build_config_file}")
        configure_file("${config_file}" "${build_config_file}" COPYONLY)
    endif()

    add_library(${agent} SHARED "${CMAKE_CURRENT_SOURCE_DIR}/agents/${agent}.cpp")
    target_include_directories(${agent} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/agents")
    target_link_libraries(${agent} PRIVATE Threads::Threads nlohmann_json::nlohmann_json)
    target_compile_options(${agent} PRIVATE -Wall)
    set_target_properties(${agent} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/agents"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/agents"
    )
endforeach()

add_executable(monitoring
    src/main.cpp
    src/Core/core.cpp
    src/Gui/gui.cpp
    src/Controller/controller.cpp
    src/include/chrono_helpers.cpp
    src/include/config_parser.cpp
    src/include/notificator.cpp
    src/include/subscription.cpp
)

target_include_directories(monitoring PRIVATE
    "${CMAKE_BINARY_DIR}/generated"
    "${MAILIO_INCLUDE_DIR}"
    "${TOMLPLUSPLUS_INCLUDE_DIR}"
)
target_link_libraries(monitoring PRIVATE
    PkgConfig::GTKMM
    "${MAILIO_LIBRARY}"
    TgBot::TgBot
    Boost::thread
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
    nlohmann_json::nlohmann_json
    "${CMAKE_DL_LIBS}"
)
target_compile_options(monitoring PRIVATE -Wall)