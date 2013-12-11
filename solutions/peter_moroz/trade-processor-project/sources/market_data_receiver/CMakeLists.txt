
find_package( Boost ${SEARCH_PARAMETERS} COMPONENTS
	regex 
	system
	thread
	date_time
	chrono
)

compile_project( ${module_name} "*.cpp" "*.h" BINARY binaries multicast_communication Boost )

