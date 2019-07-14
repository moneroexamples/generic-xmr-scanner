macro(add_om_test _TEST_NAME)

    add_executable(${_TEST_NAME}_tests
            ${_TEST_NAME}_tests.cpp
            ../ext/xmregcore/tests/JsonTx.cpp
            ../src/config/IConfigReader.cpp
            ../src/config/DefaultConfig.cpp)

    target_link_libraries(${_TEST_NAME}_tests
            gtest gtest_main
            gmock gmock_main
            XMREG::core
            Drogon::Drogon
            Boost::fiber
            stdc++fs)

    target_include_directories(${_TEST_NAME}_tests
        PRIVATE 
            ../ext/)

    add_test(NAME ${_TEST_NAME}_tests COMMAND ${_TEST_NAME}_tests)

endmacro()

resource_dir("../ext/xmregcore/tests/res/tx")

add_om_test(taskmanager)
add_om_test(outputsearchtask)


