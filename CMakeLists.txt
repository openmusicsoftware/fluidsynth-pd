# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-undefined dynamic_lookup")
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH Off)
#set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/libpd)

add_library(
        fluidsynth-pd
        SHARED
        ${CMAKE_CURRENT_LIST_DIR}/fluidsynth-pd.c
)

add_library( fluidsynth
        SHARED
        IMPORTED )

set_target_properties( # Specifies the target library.
        fluidsynth

        # Specifies the parameter you want to define.
        PROPERTIES IMPORTED_LOCATION

        # Provides the path to the library you want to import.
       ${CMAKE_CURRENT_LIST_DIR}/libs/libfluidsynth.dylib )


#manipulate the filename output
set_target_properties(fluidsynth-pd PROPERTIES PREFIX "")
set_target_properties(fluidsynth-pd PROPERTIES OUTPUT_NAME "fluidsynth~")
set_target_properties(fluidsynth-pd PROPERTIES SUFFIX ".pd_darwin")
set_target_properties(fluidsynth-pd PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")


	# Defines plateform specifix suffix and the linking necessities.
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
	if(${APPLE})
		set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_darwin")
	elseif(${UNIX})
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
	elseif(${WIN32})
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll")
		find_library(PD_LIBRARY NAMES pd HINTS ${PD_LIB_PATH})
		target_link_libraries(${PROJECT_NAME} ${PD_LIBRARY})
	endif()



target_link_libraries(
        fluidsynth-pd
        fluidsynth
)
