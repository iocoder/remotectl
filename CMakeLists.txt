cmake_minimum_required(VERSION 2.8.12)

# project header
project (remotctl)

# declaring source files
file(GLOB SOURCES      "src/*.cpp")
file(GLOB MOC_HEADERS  "src/*.h"  )
file(GLOB UIS          "src/*.ui" )
file(GLOB RESOURCES    "src/*.qrc")

# supress CMAKE policy warning
CMAKE_POLICY(SET CMP0011 OLD)

# find qt packages
find_package(PkgConfig                REQUIRED)
find_package(Qt5Widgets     "5.11.1"  REQUIRED)
find_package(qtermwidget5   "0.9.0"   REQUIRED)

# find pkgconfig packages
pkg_check_modules(LIBSSH2 libssh2>=1.5.0      REQUIRED)

# command checking
function(check_cmd cmd)
    # run which
    execute_process(COMMAND which ${cmd}
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    RESULT_VARIABLE result
                    OUTPUT_VARIABLE version)
    # check result
    if(result EQUAL 0)
        message(STATUS "${cmd} is found.")
    else(result EQUAL 0)
        message(FATAL_ERROR "\nERROR: ${cmd} needs to be installed to continue.\n")
    endif(result EQUAL 0)
endfunction(check_cmd)

# check for installed programs
check_cmd("ssh")
check_cmd("sshpass")
check_cmd("scp")
check_cmd("vim")

# include paths
include_directories( ${CMAKE_BINARY_DIR} )

# setup Qt meta-system (precompiler)
QT5_ADD_RESOURCES(RES_SOURCES ${RESOURCES}   )
QT5_WRAP_UI      (UI_HEADERS  ${UIS}         )
QT5_WRAP_CPP     (MOC_SRCS    ${MOC_HEADERS} )

# compile
add_executable( remotectl ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} )
#add_executable( remotectl WIN32 ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} )

# build it (link libraries)
target_link_libraries( remotectl Qt5::Widgets Qt5::Gui Qt5::Core qtermwidget5 ssh2)

# installation
install(TARGETS remotectl DESTINATION bin)
