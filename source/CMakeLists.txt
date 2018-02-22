#######################################################
# cmake file for building DQMOnline package
# @author Eté Rémi
# Copyright (c)
#######################################################

# include directories
include_directories( BEFORE include )

# require proper c++
add_definitions( "-pedantic -Wunused-value -O2" )
add_definitions("-Wno-long-long -Wreturn-type -Werror")

#AUX_SOURCE_DIRECTORY( src SRC_FILES )
<<<<<<< HEAD
SET( SRC_FILES ${SRC_FILES} src/RemoteLogger.cc )
SET( SRC_FILES ${SRC_FILES} src/OnlineManagerServer.cc )
SET( SRC_FILES ${SRC_FILES} src/OnlineRoutes.cc )
SET( SRC_FILES ${SRC_FILES} src/EventSource.cc )
SET( SRC_FILES ${SRC_FILES} src/RunControl.cc )
SET( SRC_FILES ${SRC_FILES} src/RunControlServer.cc )
SET( SRC_FILES ${SRC_FILES} src/RunControlInterface.cc )
SET( SRC_FILES ${SRC_FILES} src/AppEvent.cc )
SET( SRC_FILES ${SRC_FILES} src/AppEventLoop.cc )
SET( SRC_FILES ${SRC_FILES} src/Application.cc )
SET( SRC_FILES ${SRC_FILES} src/EventCollector.cc )
=======
set( SRC_FILES ${SRC_FILES} src/OnlineRoutes.cc )
set( SRC_FILES ${SRC_FILES} src/RunControl.cc )
set( SRC_FILES ${SRC_FILES} src/RunControlServer.cc )
set( SRC_FILES ${SRC_FILES} src/RunControlInterface.cc )
set( SRC_FILES ${SRC_FILES} src/AppEvent.cc )
set( SRC_FILES ${SRC_FILES} src/AppEventLoop.cc )
set( SRC_FILES ${SRC_FILES} src/Application.cc )
set( SRC_FILES ${SRC_FILES} src/EventCollector.cc )
>>>>>>> Tidy up CMakeLists.txt and added clang format and tidy to make rules

if( BUILD_HTTP_RC )
  find_package( Mongoose REQUIRED )
  
  if( MONGOOSE_FOUND )
  	message( STATUS "Check for Mongoose_INCLUDE_DIRS : ${Mongoose_INCLUDE_DIRS} -- ok" )
  	message( STATUS "Check for Mongoose_LIBRARIES : ${Mongoose_LIBRARIES} -- ok" )

    set(SRC_FILES ${SRC_FILES} src/plugins/HttpRunControlPlugin.cc)

  	include_directories( ${Mongoose_INCLUDE_DIRS} )
  	link_libraries( ${Mongoose_LIBRARIES} )

  else()
  	message( SEND_ERROR "Mongoose not found. Http won't be enabled !" )
  endif()
  
endif()

add_shared_library( ${PROJECT_NAME} ${SRC_FILES} )

install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
)

# add executable macro
macro( BUILD_EXECUTABLE _dir _name _install_loc )
	add_executable( ${_name} ${_dir}/${_name}.cc )
	target_link_libraries( ${_name} ${PROJECT_NAME} )
	install (
	    TARGETS ${_name}
	    RUNTIME DESTINATION ${_install_loc}
	)
endmacro()

BUILD_EXECUTABLE( main dqm4hep-start-rc-server bin )
BUILD_EXECUTABLE( main dqm4hep-start-random-event-source bin )
BUILD_EXECUTABLE( main dqm4hep-start-online-mgr bin )
BUILD_EXECUTABLE( main dqm4hep-online-logger bin )
BUILD_EXECUTABLE( main dqm4hep-start-event-collector bin )

dqm4hep_run_clang_tidy()
dqm4hep_run_clang_format()

install_directory( include DESTINATION . FILES_MATCHING PATTERN "*.h" PATTERN "*.cc" )
