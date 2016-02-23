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
	memory_usage_sensor

SOURCES += \
	../../core/sensor/sensor_plugin.cpp \
	../../core/sensor/sensor_dashboard.cpp \
	../../core/sensor/sensor_dashboard_plot.cpp \
	../../core/plot/qcustomplot.cpp \
	../../core/network/network_stream_in.cpp \
	../../core/network/network_stream_out.cpp \
	memory_usage_sensor.cpp \
    memory_usage_sensor_dashboard.cpp

HEADERS += \
	../../core/sensor/sensor_plugin.h \
	../../core/sensor/sensor_dashboard.h \
	../../core/sensor/sensor_dashboard_plot.h \
	../../core/plot/qcustomplot.h \
	../../core/network/network_stream_in.h \
	../../core/network/network_stream_out.h \
	memory_usage_sensor.h \
    memory_usage_sensor_dashboard.h

RESOURCES += \
    icons.qrc
