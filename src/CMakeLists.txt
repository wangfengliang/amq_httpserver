add_executable(server main.cc)

target_link_libraries(server
	lwan
	dl
	${ADDITIONAL_LIBRARIES}
)

