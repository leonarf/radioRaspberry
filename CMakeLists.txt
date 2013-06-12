cmake_minimum_required(VERSION 2.8)

#Déclaration du projet
project(raspberry)

set(TARGET ON CACHE STRING "raspbian or fedora")

include_directories(include)

###########################
# for raspberry compilation
###########################
if ( ${TARGET} STREQUAL "raspbian" )
	add_definitions(-std=c++0x)
	add_library( wiiuse SHARED IMPORTED )
	MESSAGE("cherche wiiuse.so dans ${CMAKE_BINARY_DIR}")
	set_target_properties( wiiuse PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libwiiuse.so )
	add_definitions( -DRASPBERRY )

########################
# for fedora compilation
########################
elseif( ${TARGET} STREQUAL "fedora" )
	add_definitions(-std=gnu++11)
else()
	MESSAGE("TARGET ${TARGET} not supported")
endif ()

add_definitions(-O3)
#Déclaration de l'exécutable
add_executable(
        radio
        src/main.cpp
        src/RadioManager.cpp
        src/Utils.cpp
        src/WiimoteManager.cpp
)

target_link_libraries(
        radio
        wiiuse
        pthread
        mpdclient
)

INSTALL(FILES scripts/startWiimote.sh misc/webradio.m3u DESTINATION install_directory)
