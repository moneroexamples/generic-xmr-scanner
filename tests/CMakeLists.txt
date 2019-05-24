macro(add_om_test _TEST_NAME)

    add_executable(${_TEST_NAME}_tests
            ${_TEST_NAME}_tests.cpp)

    target_link_libraries(${_TEST_NAME}_tests
            gtest gtest_main
            gmock gmock_main
            XMREG::core
            Drogon::Drogon
            Boost::fiber)

    target_include_directories(${_TEST_NAME}_tests
        PRIVATE 
            ../ext/)

    add_test(NAME ${_TEST_NAME}_tests COMMAND ${_TEST_NAME}_tests)

endmacro()

resource_dir("res")

add_om_test(taskmanager)
add_om_test(outputsearchtask)


