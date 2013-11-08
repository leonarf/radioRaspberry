cmake_minimum_required(VERSION 2.8)

#Déclaration du projet
project(LeoModules)

set(TARGET "raspbian" CACHE STRING "qemu or fedora")

include_directories(include)

###########################
# for qemu compilation
###########################
if ( ${TARGET} STREQUAL "raspbian" )
	add_definitions(-std=c++0x)
	#add_library( wiiuse SHARED IMPORTED)
	#MESSAGE("put your libwiiuse.so in ${CMAKE_BINARY_DIR} for correct link")
	#set_target_properties( wiiuse PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libwiiuse.so )
	#add_definitions( -DRASPBERRY )
	#install(PROGRAMS ${CMAKE_BINARY_DIR}/libwiiuse.so
	#	DESTINATION ${INSTALL_DIR})
########################
# for fedora compilation
########################
elseif( ${TARGET} STREQUAL "fedora" )
	add_definitions(-std=gnu++11)
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
		src/LCDTexts.cpp	
		src/mqttTopics.cpp
)
target_link_libraries(
	utils
		jsoncpp
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
set_target_properties(IRRemote PROPERTIES COMPILE_DEFINITIONS "PID_FILE=\"/var/run/${PROJECT_NAME}_IRRemote.pid\"")

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
set_target_properties(Radio PROPERTIES COMPILE_DEFINITIONS "PID_FILE=\"/var/run/${PROJECT_NAME}_Radio.pid\"")


add_executable(
	LCDManager
		src/LiquidCrystal_I2C.cpp
		src/LCDManager.cpp
		src/mainLCD.cpp
)
target_link_libraries(
	LCDManager
    	wiringPi
		mosquittopp
		jsoncpp
		utils
)
set_target_properties(LCDManager PROPERTIES COMPILE_DEFINITIONS "PID_FILE=\"/var/run/${PROJECT_NAME}_LCDManager.pid\"")

set(MODULES_LIST "LCDManager IRRemote Radio")

##################
## INSTALL PART ##
##################
#Different install path :
set(INSTALL_RUNTIME "${CMAKE_INSTALL_PREFIX}/bin")
set(INSTALL_LIB "lib")
set(INSTALL_MISC "share/${PROJECT_NAME}")
set(INSTALL_SOUND "${INSTALL_MISC}/sound")

configure_file(scripts/initd.sh ${CMAKE_BINARY_DIR}/initd.sh @ONLY NEWLINE_STYLE UNIX)
install(PROGRAMS ${CMAKE_BINARY_DIR}/initd.sh
	DESTINATION /etc/init.d
	RENAME ${PROJECT_NAME})

configure_file(scripts/startModules.sh ${CMAKE_BINARY_DIR}/startModules.sh NEWLINE_STYLE UNIX)
install(PROGRAMS ${CMAKE_BINARY_DIR}/startModules.sh
	DESTINATION ${INSTALL_MISC})

install(FILES misc/webradio.m3u
	DESTINATION ${INSTALL_MISC})

add_definitions( -DLOG_FILE="/var/log/${PROJECT_NAME}.log" )
add_definitions( -DLIRCRC_FILE="${CMAKE_INSTALL_PREFIX}/${INSTALL_MISC}/.lircrcd" )
install(FILES misc/lircrcd
	DESTINATION ${INSTALL_MISC}
	RENAME ".lircrcd")

add_definitions( -DSOUND_DIR="${CMAKE_INSTALL_PREFIX}/${INSTALL_SOUND}" )
set( soundFiles misc/byebye.wav misc/COUGH1.WAV misc/comeonthen.wav misc/hello_uk.wav misc/uh-oh.wav misc/youllregretthat.wav)
install(FILES ${soundFiles}
	DESTINATION ${INSTALL_SOUND})

install(TARGETS IRRemote Radio LCDManager utils
	RUNTIME DESTINATION ${INSTALL_RUNTIME}
	LIBRARY DESTINATION ${INSTALL_LIB})


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

#Post install script for .deb installation
	configure_file(scripts/postInstall.sh ${CMAKE_BINARY_DIR}/postinst @ONLY NEWLINE_STYLE UNIX)
	configure_file(scripts/preRemove.sh ${CMAKE_BINARY_DIR}/prerm @ONLY NEWLINE_STYLE UNIX)
	SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
            "${CMAKE_BINARY_DIR}/postinst;${CMAKE_BINARY_DIR}/prerm;")
	INCLUDE(CPack)
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")



#install(CODE "execute_process(COMMAND echo mes couilles)")

