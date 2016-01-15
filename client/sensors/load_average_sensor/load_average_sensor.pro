TEMPLATE = \
	lib

CONFIG += \
	plugin

QT += \
	core \
	widgets \
	printsupport

QMAKE_CXXFLAGS += \
	-std=c++11

TARGET =\
	load_average_sensor

SOURCES += \
	../../core/sensor/sensor_plugin.cpp \
	../../core/sensor/sensor_dashboard.cpp \
	../../core/plot/qcustomplot.cpp \
	../../core/network/network_stream_in.cpp \
	../../core/network/network_stream_out.cpp \
	load_average_sensor.cpp \
	load_average_sensor_dashboard.cpp

HEADERS += \
	../../core/sensor/sensor_plugin.h \
	../../core/sensor/sensor_dashboard.h \
	../../core/plot/qcustomplot.h \
	../../core/network/network_stream_in.h \
	../../core/network/network_stream_out.h \
	load_average_sensor.h \
	load_average_sensor_dashboard.h

RESOURCES += \
    icons.qrc
