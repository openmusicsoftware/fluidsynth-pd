# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_SUPPRESS_REGENERATION true)
#set(CMAKE_MACOSX_RPATH Off)
#set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")


set (PROJECT_NAME "fluidsynthpd")

include_directories( ${CMAKE_CURRENT_LIST_DIR})

add_library(
        ${PROJECT_NAME}
        SHARED
        ${CMAKE_CURRENT_LIST_DIR}/fluidsynth-pd.c
)

add_library( fluidsynthlib
        SHARED
        IMPORTED )

add_library(
        pdlib
        SHARED
        IMPORTED
)

set_target_properties( # Specifies the target library.
        pdlib

        # Specifies the parameter you want to define.
        PROPERTIES IMPORTED_LOCATION

        # Provides the path to the library you want to import.
        ${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI}/libpd.so

        )



set_target_properties( # Specifies the target library.
        fluidsynthlib

        # Specifies the parameter you want to define.
        PROPERTIES IMPORTED_LOCATION

        # Provides the path to the library you want to import.
       ${CMAKE_CURRENT_LIST_DIR}/../../app/src/main/jniLibs/${ANDROID_ABI}/libfluidsynth.so )


#manipulate the filename output
#set_target_properties(fluidsynth-pd PROPERTIES PREFIX "")
#set_target_properties(fluidsynth-pd PROPERTIES OUTPUT_NAME "fluidsynth~")
#set_target_properties(fluidsynth-pd PROPERTIES SUFFIX ".pd_darwin")
#set_target_properties(fluidsynth-pd PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")

target_compile_definitions(${PROJECT_NAME} PUBLIC PD)

	# Defines plateform specifix suffix and the linking necessities.
#	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
#	if(${APPLE})
#		set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
#		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_darwin")
#	elseif(${UNIX})
#		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
#	elseif(${WIN32})
#		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll")
#		find_library(PD_LIBRARY NAMES pd HINTS ${PD_LIB_PATH})
#		target_link_libraries(${PROJECT_NAME} ${PD_LIBRARY})
#	endif()



target_link_libraries(
        ${PROJECT_NAME}
        fluidsynthlib
        pdlib
)
