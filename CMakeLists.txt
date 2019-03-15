project(i3status)
cmake_minimum_required(VERSION 2.8)

set(VERSION 2.10)
add_definitions(-DVERSION="${VERSION}")

OPTION(USE_PULSE "Enable PulseAudio volume" OFF)
OPTION(MODULE_RUN_WATCH "Enable run_watch module" OFF)
OPTION(MODULE_BRIGHTNESS "Enable brightness module" ON)
OPTION(MODULE_LANGUAGE "Enable language module" ON)
OPTION(MODULE_TEMPERATURE "Enable cpu temperature module" OFF)
OPTION(MODULE_DDATE "Enable ddate module" OFF)
OPTION(MODULE_DISK_INFO "Enable disk module" OFF)
OPTION(MODULE_IPV6 "Enable ipv6 module" OFF)
OPTION(MODULE_CPU_USAGE "Enable cpu usage module" OFF)

OPTION(I3BAR_CLICKS_EVENTS "Enable i3bar click events listener" ON)
OPTION(OUTPUT_ONLY_I3BAR "Output format is only i3bar" ON)

include_directories(include)

FIND_PACKAGE(PkgConfig REQUIRED)
pkg_check_modules(LIBCONFUSE libconfuse REQUIRED)
include_directories(${LIBCONFUSE_INCLUDE_DIRS})
link_directories(${LIBCONFUSET_LIBRARY_DIRS})

set(LIBS
    -l${LIBCONFUSE_LIBRARIES}
    -lyajl
    -lpthread
)

set(HDR
    include/i3status.h
    include/queue.h
)

set(SRC
    i3status.c
    src/auto_detect_format.c
    src/first_network_device.c
    src/general.c
    src/output.c
    src/print_battery_info.c
    src/print_eth_info.c
    src/print_ip_addr.c
    src/print_load.c
    src/print_path_exists.c
    src/print_time.c
    src/print_volume.c
    src/print_wireless_info.c
)


function(USE_SRC)
    set(oneValueArgs FLAG)
    set(multiValueArgs LIBS SRCS DEFINES PKGCONFIG)
    cmake_parse_arguments(USE_SRC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    if(${USE_SRC_FLAG})
        if(USE_SRC_PKGCONFIG)
            STRING(RANDOM LENGTH 10 NAME)
            PKG_CHECK_MODULES(${NAME} ${USE_SRC_PKGCONFIG} REQUIRED)
            include_directories(${${NAME}_INCLUDE_DIRS})
            list(APPEND LIBS ${${NAME}_LIBRARIES})
        endif()

        list(APPEND SRC ${USE_SRC_SRCS})
        set(SRC ${SRC} PARENT_SCOPE)

        add_definitions(${USE_SRC_DEFINES})

        list(APPEND LIBS ${USE_SRC_LIBS})
        set(LIBS ${LIBS} PARENT_SCOPE)
    endif()

endfunction()

USE_SRC(FLAG ${USE_PULSE} SRCS src/pulse.c LIBS -lpulse -lm DEFINES -DPULSE)

USE_SRC(FLAG ${I3BAR_CLICKS_EVENTS} SRCS src/click_events.c DEFINES -DI3BAR_CLICKS)
USE_SRC(FLAG ${OUTPUT_ONLY_I3BAR} DEFINES -DOUTPUT_ONLY_I3BAR)

USE_SRC(FLAG ${MODULE_RUN_WATCH} SRCS src/print_run_watch.c src/process_runs.c DEFINES -DRUN_WATCH)
USE_SRC(FLAG ${MODULE_BRIGHTNESS} SRCS src/print_brightness.c DEFINES -DBRIGHTNESS PKGCONFIG xcb-randr xcb-atom xcb-aux xcb)
USE_SRC(FLAG ${MODULE_LANGUAGE} SRCS src/print_language.c DEFINES -DLANGUAGE PKGCONFIG x11)
USE_SRC(FLAG ${MODULE_TEMPERATURE} SRCS src/print_cpu_temperature.c DEFINES -DTEMPERATURE)
USE_SRC(FLAG ${MODULE_DDATE} SRCS src/print_ddate.c DEFINES -DDDATE)
USE_SRC(FLAG ${MODULE_DISK_INFO} SRCS src/print_disk_info.c DEFINES -DDISK_INFO)
USE_SRC(FLAG ${MODULE_IPV6} SRCS src/print_ipv6_addr.c DEFINES -DMOD_IPV6)
USE_SRC(FLAG ${MODULE_CPU_USAGE} SRCS src/print_cpu_usage.c DEFINES -DCPU_USAGE)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    PKG_CHECK_MODULES(LIBNL libnl-genl-3.0 REQUIRED)
    include_directories(${LIBNL_INCLUDE_DIRS})
    list(APPEND LIBS ${LIBNL_LIBRARIES} -lasound)
    add_definitions(-DLINUX -D_GNU_SOURCE)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
        add_compile_options(-fvisibility=hidden)
    endif()
endif()

add_compile_options(-std=gnu99 -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare -pedantic)

add_executable(${PROJECT_NAME} ${SRC} ${HDR} i3status.conf)
target_link_libraries(${PROJECT_NAME} ${LIBS})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES i3status.conf DESTINATION ${DESTDIR}/etc COMPONENT config)
