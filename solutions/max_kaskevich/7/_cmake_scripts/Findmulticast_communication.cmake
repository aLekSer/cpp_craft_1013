include( utils )

if (multicast_communication_ROOT)
	set(multicast_communication_root ${multicast_communication_ROOT} )
elseif(NOT "$ENV{MULTICAST_COMMUNICATION_ROOT}" STREQUAL "")
	set(multicast_communication_root $ENV{MULTICAST_COMMUNICATION_ROOT} )
else()
	message(FATAL_ERROR "[ERROR]: No MULTICAST_COMMUNICATION_ROOT environment variable found")
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE Debug)
	if (!multicast_communication_FIND_QUIETLY)
		message(STATUS "CMAKE_BUILD_TYPE was not tuned, so we install there 'Debug'")
	endif()
else()
endif()

if (multicast_communication_DEBUG)
	message(STATUS "We going to look into '${multicast_communication_root}' (MULTICAST_COMMUNICATION_ROOT environment variable) folder.")
	create_string_from_list( component_list ${multicast_communication_FIND_COMPONENTS} )
	message(STATUS "We are looking for: '" ${component_list} "' components.")
endif(multicast_communication_DEBUG)

set(multicast_communication_FOUND FALSE)
set(multicast_communication_INCLUDE_DIRS "")
set(multicast_communication_LIBRARIES "")

if (UNIX)
	set(binary_search_folder ${multicast_communication_root}/_build_${CMAKE_BUILD_TYPE}_${CMAKE_ADDRESS_MODEL}/bin_${CMAKE_ADDRESS_MODEL}/${CMAKE_BUILD_TYPE})
else(UNIX)
	set(binary_search_folder ${multicast_communication_root}/_build_${CMAKE_BUILD_TYPE}_${CMAKE_ADDRESS_MODEL}/bin_${CMAKE_ADDRESS_MODEL}/${CMAKE_BUILD_TYPE})
endif(UNIX)
if (NOT multicast_communication_FIND_QUIETLY)
	message(STATUS "Looking for libraries at " ${binary_search_folder})
endif()

if (multicast_communication_DEBUG)
	message(STATUS "Binary search folder: " ${binary_search_folder} )
endif(multicast_communication_DEBUG)

set(components multicast_communication)

foreach(component ${multicast_communication_FIND_COMPONENTS})

list_contains(we_should_find_${component} ${component} ${components})
if(we_should_find_${component})

	if (multicast_communication_DEBUG)
		message(STATUS "Searching for ${component} library. ")
	endif()
	set(multicast_communication_${component}_FOUND FALSE)
	set(multicast_communication_${component}_INCLUDE_DIRS NOTFOUND)
	set(multicast_communication_${component}_LIBRARIES NOTFOUND)

	find_path( multicast_communication_${component}_INCLUDE_DIR
			NAMES "CMakeLists.txt"
			HINTS "${multicast_communication_root}/sources/${component}/")
	set(multicast_communication_${component}_INCLUDE_DIRS ${multicast_communication_${component}_INCLUDE_DIR})

	find_library( multicast_communication_${component}_LIBRARY
		NAMES "${component}" 
		HINTS ${binary_search_folder} )
	set(multicast_communication_${component}_LIBRARIES ${multicast_communication_${component}_LIBRARY})

	if(NOT multicast_communication_FIND_QUIETLY)
		message(STATUS "multicast_communication ${component} component: ")
		create_string_from_list(headers ${multicast_communication_${component}_INCLUDE_DIRS})
		message(STATUS "   headers  :  " ${headers})
		create_string_from_list(libraries ${multicast_communication_${component}_LIBRARIES})
		message(STATUS "   libraries:  " ${libraries})
	endif()	

	if(NOT multicast_communication_${component}_INCLUDE_DIR OR NOT multicast_communication_${component}_LIBRARY)
		message(FATAL_ERROR " We can't find multicast_communication ${component} component")
	else()
		set(multicast_communication_INCLUDE_DIRS ${multicast_communication_INCLUDE_DIRS} ${multicast_communication_${component}_INCLUDE_DIRS})
		set(multicast_communication_LIBRARIES ${multicast_communication_LIBRARIES} ${multicast_communication_${component}_LIBRARIES})
	endif()
else()
	message(FATAL_ERROR "Unknown component: ${component}")
endif(we_should_find_${component})

endforeach( component )


if (multicast_communication_DEBUG)
	create_string_from_list(headers ${multicast_communication_INCLUDE_DIRS})
	message(STATUS "multicast_communication_INCLUDE_DIRS: " ${headers})
	create_string_from_list(libraries ${multicast_communication_LIBRARIES})
	message(STATUS "multicast_communication_LIBRARIES: " ${libraries})
endif(multicast_communication_DEBUG)

