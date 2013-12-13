find_package( Boost ${SEARCH_PARAMETERS} COMPONENTS
    system
    unit_test_framework
    filesystem
    thread
    date_time
    chrono
    regex
)

find_package( multicast_communication ${SEARCH_PARAMETERS} COMPONENTS
	multicast_communication
	thread_safe_queue
)

compile_project( ${tests_name} "*.cpp" "*.h" BINARY tests ${module_name} Boost multicast_communication minute_market_calculator )
register_test( ${tests_name} 15 14 )

