cmake_minimum_required(VERSION 2.8)

#Déclaration du projet
project(raspberry)

function(replaceTag tag value filePath)
	FILE(READ ${filePath} FILE_CONTENT)
	MESSAGE("contenu du fichier ${filePath} : ${FILE_CONTENT}")
	STRING(REPLACE "${tag})" "${value}" 
		MODIFIED_FILE_CONTENT "${FILE_CONTENT}")
	FILE(WRITE ${filePath} "${MODIFIED_FILE_CONTENT}")
endfunction(replaceTag)

set(TARGET "raspbian" CACHE STRING "qemu or fedora")
SET(INSTALL_PREFIX "." CACHE PATH "Where sandbox will be created")
set(CMAKE_INSTALL_PREFIX "${INSTALL_PREFIX}" CACHE INTERNAL "where sandbox will be created" FORCE)

include_directories(include)

###########################
# for qemu compilation
###########################
if ( ${TARGET} STREQUAL "raspbian" )
	add_definitions(-std=c++0x)
	add_library( wiiuse SHARED IMPORTED)
	MESSAGE("put your libwiiuse.so in ${CMAKE_BINARY_DIR} for correct link")
	set_target_properties( wiiuse PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libwiiuse.so )
	add_definitions( -DRASPBERRY )
	set(INSTALL_DIR "sandbox")
#	install(TARGETS wiiuse
#		LIBRARY DESTINATION ${INSTALL_DIR})
	install(PROGRAMS ${CMAKE_BINARY_DIR}/libwiiuse.so
		DESTINATION ${INSTALL_DIR})
########################
# for fedora compilation
########################
elseif( ${TARGET} STREQUAL "fedora" )
	add_definitions(-std=gnu++11)
	set(INSTALL_DIR "sandbox")
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
        src/ConfigManager.cpp
)

target_link_libraries(
        radio
        wiiuse
        pthread
        mpdclient
)

##################
## INSTALL PART ##
##################
install(PROGRAMS scripts/startWiimote.sh
	DESTINATION ${INSTALL_DIR})

#load the function replaceTag
install(SCRIPT "replaceTag.cmake")
#use the function loaded
install(CODE "replaceTag( \"##TARGET##\" ${TARGET} ${INSTALL_DIR}/startWiimote.sh)")
install(FILES misc/webradio.m3u
	DESTINATION ${INSTALL_DIR})

set( soundFiles misc/byebye.wav misc/comeonthen.wav misc/hello_uk.wav misc/uh-oh.wav misc/youllregretthat.wav)
install(FILES ${soundFiles} DESTINATION ${INSTALL_DIR}/sound)
install(TARGETS radio
	RUNTIME DESTINATION ${INSTALL_DIR})

