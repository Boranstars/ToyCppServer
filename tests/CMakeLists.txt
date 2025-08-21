add_executable(test_http_parser test_http_parser.cpp
${CMAKE_SOURCE_DIR}/http/http_parser.cpp
        ${CMAKE_SOURCE_DIR}/server/buffer.cpp)
target_include_directories(test_http_parser PRIVATE ${CMAKE_SOURCE_DIR}/http ${CMAKE_SOURCE_DIR}/server)


add_executable(test_response
        test_response.cpp
        ${CMAKE_SOURCE_DIR}/http/response.cpp
)

target_include_directories(test_response PRIVATE ${CMAKE_SOURCE_DIR}/http )

add_executable(test_fileloader_middleware
        test_middleware.cpp
        ${CMAKE_SOURCE_DIR}/middlewares/static_file_middleware.cpp
        ${CMAKE_SOURCE_DIR}/http/router.cpp
        ${CMAKE_SOURCE_DIR}/middlewares/router_middleware.cpp
)
target_include_directories(test_fileloader_middleware PRIVATE ${CMAKE_SOURCE_DIR}/http ${CMAKE_SOURCE_DIR}/middlewares ${CMAKE_SOURCE_DIR}/server)