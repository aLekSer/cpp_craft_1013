
find_package( Boost ${SEARCH_PARAMETERS} COMPONENTS
    system
    thread
    date_time
    chrono
    regex
)

find_package( multicast_communication ${SEARCH_PARAMETERS} COMPONENTS
    multicast_communication
    thread_safe_queue
)

compile_project( ${module_name} "*.cpp" "*.h" BINARY binaries multicast_communication Boost minute_market_calculator minute_market_data)

