if (VIKKI_LOAD_AVERAGE_SENSOR)
    add_subdirectory(load_average_sensor)
endif (VIKKI_LOAD_AVERAGE_SENSOR)

if (VIKKI_MEMORY_USAGE_SENSOR)
    add_subdirectory(memory_usage_sensor)
endif (VIKKI_MEMORY_USAGE_SENSOR)

if (VIKKI_FILE_SYSTEM_USAGE_SENSOR)
	add_subdirectory(file_system_usage_sensor)
endif (VIKKI_FILE_SYSTEM_USAGE_SENSOR)
