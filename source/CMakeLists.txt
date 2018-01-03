#######################################################
# cmake file for building DQMOnline package
# @author Eté Rémi
# Copyright (c)
#######################################################

# include directories
INCLUDE_DIRECTORIES( BEFORE include )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type -Werror")

#AUX_SOURCE_DIRECTORY( src SRC_FILES )
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

IF( BUILD_HTTP_RC )
  FIND_PACKAGE( Mongoose REQUIRED )
  
  IF( MONGOOSE_FOUND )
  	MESSAGE( STATUS "Check for Mongoose_INCLUDE_DIRS : ${Mongoose_INCLUDE_DIRS} -- ok" )
  	MESSAGE( STATUS "Check for Mongoose_LIBRARIES : ${Mongoose_LIBRARIES} -- ok" )

    SET(SRC_FILES ${SRC_FILES} src/plugins/HttpRunControlPlugin.cc)

  	INCLUDE_DIRECTORIES( ${Mongoose_INCLUDE_DIRS} )
  	LINK_LIBRARIES( ${Mongoose_LIBRARIES} )

  ELSE()
  	MESSAGE( SEND_ERROR "Mongoose not found. Http won't be enabled !" )
  ENDIF()
  
ENDIF()

ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${SRC_FILES} )

INSTALL(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
)

# add executable macro
MACRO(BUILD_EXECUTABLE _dir _name _install_loc)
	ADD_EXECUTABLE( ${_name} ${_dir}/${_name}.cc )
	TARGET_LINK_LIBRARIES( ${_name} ${PROJECT_NAME} )
	INSTALL (
	    TARGETS ${_name}
	    RUNTIME DESTINATION ${_install_loc}
	)
ENDMACRO()

BUILD_EXECUTABLE( main dqm4hep-start-rc-server bin )
BUILD_EXECUTABLE( main dqm4hep-start-random-event-source bin )
BUILD_EXECUTABLE( main dqm4hep-start-online-mgr bin )
BUILD_EXECUTABLE( main dqm4hep-online-logger bin )

# # -------------------------------------------------
# # build start external application executable
#
# ADD_EXECUTABLE( dqm4hep_start_external_application main/dqm4hep_start_external_application.cc )
# TARGET_LINK_LIBRARIES( dqm4hep_start_external_application ${PROJECT_NAME} )
#
# INSTALL (
#     TARGETS dqm4hep_start_external_application
#     RUNTIME DESTINATION bin
# )
#
#
# # -------------------------------------------------
# # build start event collector executable
#
# ADD_EXECUTABLE( dqm4hep_start_event_collector main/dqm4hep_start_event_collector.cc )
# TARGET_LINK_LIBRARIES( dqm4hep_start_event_collector ${PROJECT_NAME} )
#
# INSTALL (
#     TARGETS dqm4hep_start_event_collector
#     RUNTIME DESTINATION bin
# )
#
#
# # -------------------------------------------------
# # build start monitor element collector executable
#
# ADD_EXECUTABLE( dqm4hep_start_monitor_element_collector main/dqm4hep_start_monitor_element_collector.cc )
# TARGET_LINK_LIBRARIES( dqm4hep_start_monitor_element_collector ${PROJECT_NAME} )
#
# INSTALL (
#     TARGETS dqm4hep_start_monitor_element_collector
#     RUNTIME DESTINATION bin
# )
#
#
# # -------------------------------------------------
# # build start analysis module executable
#
# ADD_EXECUTABLE( dqm4hep_start_analysis_module main/dqm4hep_start_analysis_module.cc )
# TARGET_LINK_LIBRARIES( dqm4hep_start_analysis_module ${PROJECT_NAME} )
#
# INSTALL (
#     TARGETS dqm4hep_start_analysis_module
#     RUNTIME DESTINATION bin
# )
#
#
# # -------------------------------------------------
# # build start standalone module executable
#
# ADD_EXECUTABLE( dqm4hep_start_standalone_module main/dqm4hep_start_standalone_module.cc )
# TARGET_LINK_LIBRARIES( dqm4hep_start_standalone_module ${PROJECT_NAME} )
#
# INSTALL (
#     TARGETS dqm4hep_start_standalone_module
#     RUNTIME DESTINATION bin
# )
#
#
# # -------------------------------------------------
# # build open root canvas executable
#
# ADD_EXECUTABLE( dqm4hep_open_root_canvas main/dqm4hep_open_root_canvas.cc )
# TARGET_LINK_LIBRARIES( dqm4hep_open_root_canvas ${PROJECT_NAME} ${ROOT_GUI_LIBRARY} ${ROOT_GRAPH_LIBRARY} ${ROOT_HIST_LIBRARY} ${ROOT_CINT_LIBRARY} )
#
# INSTALL (
#     TARGETS dqm4hep_open_root_canvas
#     RUNTIME DESTINATION bin
# )
#
#
# # -------------------------------------------------
# # build db insert config file executable
#
# ADD_EXECUTABLE( dqm4hep_db_insert_config_file main/dqm4hep_db_insert_config_file.cc )
# TARGET_LINK_LIBRARIES( dqm4hep_db_insert_config_file ${PROJECT_NAME} )
#
# INSTALL (
#     TARGETS dqm4hep_db_insert_config_file
#     RUNTIME DESTINATION bin
# )
#
# # -------------------------------------------------
# # build run control interface executable
#
# ADD_EXECUTABLE( dqm4hep_run_control main/dqm4hep_run_control.cc )
# TARGET_LINK_LIBRARIES( dqm4hep_run_control ${PROJECT_NAME} )
#
# INSTALL (
#     TARGETS dqm4hep_run_control
#     RUNTIME DESTINATION bin
# )
#
# # -------------------------------------------------
# # build run control server executable
#
# ADD_EXECUTABLE( dqm4hep_start_run_control_server main/dqm4hep_start_run_control_server.cc )
# TARGET_LINK_LIBRARIES( dqm4hep_start_run_control_server ${PROJECT_NAME} )
#
# INSTALL (
#     TARGETS dqm4hep_start_run_control_server
#     RUNTIME DESTINATION bin
# )
#
# # -------------------------------------------------
# # build run control server executable
#
# ADD_EXECUTABLE( dqm4hep-start-log-collector main/dqm4hep-start-log-collector.cc )
# TARGET_LINK_LIBRARIES( dqm4hep-start-log-collector ${PROJECT_NAME} )
#
# INSTALL (
#     TARGETS dqm4hep-start-log-collector
#     RUNTIME DESTINATION bin
# )

INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" PATTERN "*.cc" )
