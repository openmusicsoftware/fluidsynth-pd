# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.14)

project(fluidsynth-pd)

set(DARWIN 0)
set(LINUX 0)
set(WINDOWS 0)

message("CMAKE SYSTEM IS :${CMAKE_SYSTEM_NAME}.")
if(ANDROID)
 message("Android is active")
 set(CMAKE_EXE_LINKER_FLAGS "-fPIE -pie")
endif(ANDROID)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
 set(LINUX 1)
 message("Linux is active")
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
 set(WINDOWS 1)
 message("WINDOWS is active")
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
 set(DARWIN 1)
 message("DARWIN is active")
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

set (PROJECT_NAME "fluidsynth-pd")

include(FetchContent)

set(CMAKE_TLS_VERIFY true)

FetchContent_Declare(
        libpd
        GIT_REPOSITORY git@github.com:libpd/libpd.git
        GIT_TAG        origin/master
        GIT_SUBMODULES "pure-data"
)

FetchContent_MakeAvailable(libpd)

if (ANDROID)
    if(NOT DEFINED JNI_FOLDER_DIR)
        message(FATAL_ERROR "error JNI_FOLDER_DIR must be defined")
    endif(NOT DEFINED JNI_FOLDER_DIR)
    set (ADDITIONAL_CLEAN_FILES "${JNI_FOLDER_DIR}")
    find_package(PkgConfig REQUIRED)

    set(FS_URL "https://github.com/FluidSynth/fluidsynth/releases/download/v2.2.4/fluidsynth-2.2.4-android24.zip")
    set(FS_SHA256 "dfc232369e9ce552b1903ef294357d5c39dc3b45c0b16bde891247f4178704dd")

    FetchContent_Declare(
            fluidsynth
            URL ${FS_URL}
            URL_HASH SHA256=${FS_SHA256}
    )

    FetchContent_MakeAvailable(fluidsynth)

    add_library(FLUIDSYNTH SHARED IMPORTED)
    set_target_properties(FLUIDSYNTH PROPERTIES
        IMPORTED_LOCATION ${fluidsynth_SOURCE_DIR}/lib/${ANDROID_ABI}/libfluidsynth.so
            )

    include_directories(
            ${libpd_SOURCE_DIR}/pure-data/src
            ${fluidsynth_SOURCE_DIR}/include
    )

    file(COPY "${fluidsynth_SOURCE_DIR}/lib"
            DESTINATION "${JNI_FOLDER_DIR}"
            PATTERN "libfluidsynth.so" EXCLUDE)

elseif(DARWIN)

    FetchContent_Declare(
            fluidsynth
            GIT_REPOSITORY git@github.com:MaplePost/fluidsynth.git
            GIT_TAG        origin/master
    )

    FetchContent_MakeAvailable(fluidsynth)

    include_directories(
            ${libpd_SOURCE_DIR}/pure-data/src
            ${fluidsynth_SOURCE_DIR}/include
    )


endif(ANDROID)




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
	if(DARWIN)
		set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_darwin")
	elseif(LINUX)
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
    elseif(ANDROID)
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".so")
	elseif(WINDOWS)
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll")
	endif()

if(ANDROID)
    target_link_libraries(
            ${PROJECT_NAME}
            FLUIDSYNTH
            libpd
    )

elseif(DARWIN)
    target_link_libraries(
        ${PROJECT_NAME}
        libfluidsynth
    )
endif(ANDROID)
