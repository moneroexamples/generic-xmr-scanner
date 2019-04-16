macro(add_om_test _TEST_NAME)

    add_executable(${_TEST_NAME}_tests
            ${_TEST_NAME}_tests.cpp)

    target_link_libraries(${_TEST_NAME}_tests
            gtest gtest_main
            gmock gmock_main
            XMREG::core
            nlohmann_json::nlohmann_json
            Boost::fiber)

    add_test(NAME ${_TEST_NAME}_tests COMMAND ${_TEST_NAME}_tests)

endmacro()

resource_dir("res")

add_om_test(taskmanager)


