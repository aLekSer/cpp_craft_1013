find_package( Boost ${SEARCH_PARAMETERS} COMPONENTS
	system
	unit_test_framework
	thread
	date_time
	chrono
	regex
)

compile_project( ${module_name} "*.cpp" "*.h" BINARY binaries multicast_communication Boost )

