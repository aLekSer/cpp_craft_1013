
find_package( Boost ${SEARCH_PARAMETERS} COMPONENTS
    system
    thread
    date_time
    chrono
)


find_package( multicast_communication ${SEARCH_PARAMETERS} COMPONENTS
	multicast_communication
	thread_safe_queue
)


compile_project( ${module_name} "*.cpp" "*.h" STATIC libraries Boost multicast_communication minute_market_calculator )
