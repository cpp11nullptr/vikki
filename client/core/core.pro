TEMPLATE = \
	app

QT += \
	core \
	widgets \
	network \
	printsupport

QMAKE_CXXFLAGS += \
	-std=c++11

win32 {
	DEFINES += \
		VIKKI_PLATFORM_WIN32
}

unix {
	DEFINES += \
		VIKKI_PLATFORM_UNIX
}

TARGET =\
	vikki-client

SOURCES += \
	main.cpp \
    application.cpp \
    window.cpp \
    plot/qcustomplot.cpp \
    settings.cpp \
    network/client_controller.cpp \
    network/client_worker.cpp \
    network/client.cpp \
    network/network_connection.cpp \
	network/network_worker.cpp \
    network/network_stream_in.cpp \
    network/network_stream_out.cpp \
    sensor/sensor_plugin.cpp \
    sensor/sensor_dashboard.cpp \
    sensor/sensor_client_proxy.cpp

HEADERS += \
    application.h \
    window.h \
    config.h \
    plot/qcustomplot.h \
    settings.h \
    network/client_controller.h \
    network/client_worker.h \
    network/client.h \
    network/network_connection.h \
    network/network_worker.h \
    command.h \
    network/network_stream_in.h \
    network/network_stream_out.h \
    sensor/sensor_plugin.h \
    sensor/sensor_dashboard.h \
    sensor/sensor_client_proxy.h

RESOURCES += \
    resources.qrc
