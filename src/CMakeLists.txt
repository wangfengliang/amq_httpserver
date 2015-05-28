
include_directories("/usr/local/include/activemq-cpp-3.8.4/"
"/usr/local/apr/include/apr-1/" "/usr/local/activemq-cpp-3.8.4/include")
set(amp_SOURCES 
    amq_factory.cc
)

add_library(amp STATIC ${amp_SOURCES})
target_link_libraries(amp
	activemq-cpp
)

add_executable(amp_httpserver main.cc)
target_link_libraries(amp_httpserver
	lwan
	amp
)
add_executable(amq_test amq_consumer_test.cc)
target_link_libraries(amq_test
	amp
)

