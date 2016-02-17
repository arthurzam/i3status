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

set(LIBS
    -lconfuse
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

FIND_PACKAGE(PkgConfig REQUIRED)

if(USE_PULSE)
    list(APPEND LIBS -lpulse -lm)
    list(APPEND SRC src/pulse.c)
    add_definitions(-DPULSE)
endif()

if(I3BAR_CLICKS_EVENTS)
    list(APPEND SRC src/click_events.c)
    add_definitions(-DI3BAR_CLICKS)
endif()

if(OUTPUT_ONLY_I3BAR)
    add_definitions(-DOUTPUT_ONLY_I3BAR)
endif()

if(MODULE_RUN_WATCH)
    list(APPEND SRC src/print_run_watch.c src/process_runs.c)
    add_definitions(-DRUN_WATCH)
endif()

if(MODULE_BRIGHTNESS)
    PKG_CHECK_MODULES(RANDR xcb-randr xcb-atom xcb-aux xcb REQUIRED)

    include_directories(${RANDR_INCLUDE_DIRS})
    list(APPEND LIBS ${RANDR_LIBRARIES})
    list(APPEND SRC src/print_brightness.c)
    add_definitions(-DBRIGHTNESS)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare -pedantic")

if(MODULE_LANGUAGE)
    PKG_CHECK_MODULES(X11 x11 REQUIRED)

    include_directories(${X11_INCLUDE_DIRS})
    list(APPEND LIBS ${X11_LIBRARIES})
    list(APPEND SRC src/print_language.c)
    add_definitions(-DLANGUAGE)
endif()

if(MODULE_TEMPERATURE)
    list(APPEND SRC src/print_cpu_temperature.c)
    add_definitions(-DTEMPERATURE)
endif()

if(MODULE_DDATE)
    list(APPEND SRC src/print_ddate.c)
    add_definitions(-DDDATE)
endif()

if(MODULE_DISK_INFO)
    list(APPEND SRC src/print_disk_info.c)
    add_definitions(-DDISK_INFO)
endif()

if(MODULE_IPV6)
    list(APPEND SRC src/print_ipv6_addr.c)
    add_definitions(-DMOD_IPV6)
endif()

if(MODULE_CPU_USAGE)
    list(APPEND SRC src/print_cpu_usage.c)
    add_definitions(-DCPU_USAGE)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    PKG_CHECK_MODULES(LIBNL libnl-genl-3.0 REQUIRED)
    include_directories(${LIBNL_INCLUDE_DIRS})
    list(APPEND LIBS ${LIBNL_LIBRARIES} -lasound)
    add_definitions(-DLINUX -D_GNU_SOURCE)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
        add_definitions(-fvisibility=hidden)
    endif()
endif()

add_executable(${PROJECT_NAME} ${SRC} ${HDR} i3status.conf)
target_link_libraries(${PROJECT_NAME} ${LIBS})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES i3status.conf DESTINATION ${DESTDIR}/etc COMPONENT config)
