project(postgresql_facade)

set(target postgresql_facade)

find_package(PostgreSQL REQUIRED)

set(sources
    ${CMAKE_CURRENT_SOURCE_DIR}/psql_connection.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/psql_connection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/psql_facade.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/psql_facade.h
    ${CMAKE_CURRENT_SOURCE_DIR}/psql_row.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/psql_row.h)

add_library(${target} SHARED ${sources})

target_link_libraries(${target}
    PUBLIC
      PostgreSQL::PostgreSQL)

target_include_directories(${target}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties( ${target}
        PROPERTIES
            CXX_STANDARD 17)
