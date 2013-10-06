cmake_minimum_required(VERSION 2.8)

#Déclaration du projet
project(mqtt_mpd_module)

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

add_definitions(-O0)
#Déclaration des libraries
add_library(
	utils
	SHARED
        src/Utils.cpp
	src/ConfigManager.cpp	
)

#Déclaration des éxécutables
add_executable(
	IRRemote
	src/RemoteIRManager.cpp
        src/mainIR.cpp
)
target_link_libraries(
	IRRemote
	lirc_client
	mosquittopp
	utils
)

add_executable(
	Radio
	src/RadioManager.cpp
        src/mainRadio.cpp
)
target_link_libraries(
	Radio
        mpdclient
	mosquittopp
	utils
	jsoncpp
)

##################
## INSTALL PART ##
##################
#install(PROGRAMS scripts/startWiimote.sh
#	DESTINATION ${INSTALL_DIR})

#load the function replaceTag
install(SCRIPT "replaceTag.cmake")
#use the function loaded
#install(CODE "replaceTag( \"##TARGET##\" ${TARGET} ${INSTALL_DIR}/startWiimote.sh)")
install(FILES misc/webradio.m3u misc/lircrcd
	DESTINATION ${INSTALL_DIR})

set( soundFiles misc/byebye.wav misc/comeonthen.wav misc/hello_uk.wav misc/uh-oh.wav misc/youllregretthat.wav)
install(FILES ${soundFiles} DESTINATION ${INSTALL_DIR}/sound)
install(TARGETS Radio IRRemote utils
	RUNTIME DESTINATION ${INSTALL_DIR}
	LIBRARY DESTINATION ${INSTALL_DIR})

######################
## PACKAGE CREATION ##
######################
SET(MAJOR_VERSION 0)
SET(MINOR_VERSION 1)
SET(PATCH_VERSION 0)

IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
	INCLUDE(InstallRequiredSystemLibraries)

	SET(CPACK_SET_DESTDIR "on")
	SET(CPACK_PACKAGING_INSTALL_PREFIX ".")
	SET(CPACK_GENERATOR "DEB")

	SET(CPACK_PACKAGE_DESCRIPTION "radio and remote modules")
	SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "modules talking with mosquitto to transmit command from a remote control to MPD ")
	SET(CPACK_PACKAGE_VENDOR "Vendor")
	SET(CPACK_PACKAGE_CONTACT "leonard.michelet@gmail.com ")
	SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
	SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
	SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
	SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
	SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")

	SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libmosquittopp1 (>= 1), libmpdclient2 (>=2.3), liblircclient0 (>= 0.9)")

	SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
	SET(CPACK_DEBIAN_PACKAGE_SECTION "raspberry")
	SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

	#SET(CPACK_COMPONENTS_ALL Libraries ApplicationData)
	INCLUDE(CPack)
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
