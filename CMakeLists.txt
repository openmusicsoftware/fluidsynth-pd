# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.14)

project(fluidsynth-pd)

set(DARWIN 0)
set(BUILD_JNI 0)
set(LINUX 0)
set(WINDOWS 0)
set(PICO 0)

message("CMAKE SYSTEM IS :${CMAKE_SYSTEM_NAME}.")
if(ANDROID)
 message("Android is active")
endif(ANDROID)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
 set(LINUX 1)
 message("Linux is active")
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
 set(WINDOWS 1)
 message("WINDOWS is active")
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

if(${CMAKE_SYSTEM_NAME} STREQUAL "PICO")
 set(PICO 1)
 message("PICO is active")
endif(${CMAKE_SYSTEM_NAME} STREQUAL "PICO")

if(ANDROID)
  set(CMAKE_EXE_LINKER_FLAGS "-fPIE -pie")
endif(ANDROID)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
 set(DARWIN 1)
 message("DARWIN is active")
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")




set (PROJECT_NAME "fluidsynth-pd")

include(FetchContent)
FetchContent_Declare(
        libpd
        GIT_REPOSITORY git@github.com:MaplePost/libpd.git
        GIT_TAG        origin/master
        GIT_SUBMODULES "pure-data"
)
FetchContent_Declare(
        fluidsynth
        GIT_REPOSITORY git@github.com:MaplePost/fluidsynth.git
        GIT_TAG        origin/master
)

FetchContent_MakeAvailable(libpd fluidsynth)


include_directories(
        ${libpd_SOURCE_DIR}/pure-data/src
        ${fluidsynth_DOURCE_DIR}/include
)


add_library(
        ${PROJECT_NAME}
        SHARED
        ${CMAKE_CURRENT_LIST_DIR}/fluidsynth-pd.c
)


#manipulate the filename output

target_compile_definitions(${PROJECT_NAME} PUBLIC PD)

set_target_properties(fluidsynth-pd PROPERTIES OUTPUT_NAME "fluidsynth~")

	# Defines platform specific suffix and the linking necessities.
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
	if(${APPLE})
		set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_darwin")
	elseif(${UNIX})
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
    elseif(${ANDROID})
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
	elseif(${WIN32})
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll")
	endif()

target_link_libraries(
        ${PROJECT_NAME}
        libfluidsynth
)
