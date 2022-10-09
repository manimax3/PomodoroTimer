set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Qml Quick QuickControls2 REQUIRED)

qt6_add_executable(pomodoro 
	# Source files
	main.cpp

	resources.qrc
)

target_link_libraries(pomodoro PRIVATE
	Qt6::Core Qt6::Qml Qt6::Quick Qt6::QuickControls2
	project_options project_warnings)

qt_add_qml_module(pomodoro
	URI pomo
	VERSION 1.0
	NO_RESOURCE_TARGET_PATH
	QML_FILES 
		qml/main.qml
	)

add_custom_target(run COMMAND pomodoro WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "run pomodoro" USES_TERMINAL)
add_dependencies(run pomodoro)

install(TARGETS pomodoro
		DESTINATION bin)
