/*

The MIT License (MIT)

Copyright (c) 2016 Ievgen Polyvanyi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef VIKKI_FILE_SYSTEM_USAGE_SENSOR_DASHBOARD_H
#define VIKKI_FILE_SYSTEM_USAGE_SENSOR_DASHBOARD_H

#include "../../core/sensor/sensor_dashboard.h"
#include "../../core/sensor/sensor_dashboard_plot.h"

#include "../../core/plot/qcustomplot.h"
#include "../../core/network/network_stream_in.h"

namespace Vikki
{
	class FileSystemUsageSensorDashboard
		: public SensorDashboard
	{
		Q_OBJECT

	public:
		FileSystemUsageSensorDashboard(const QString& sensorName, const QString& sensorTitle);
		~FileSystemUsageSensorDashboard() override;

		void sensorDataReceived(NetworkStreamInPointer stream) override;
		void sensorDataUpdated(NetworkStreamInPointer stream) override;

	protected:
		QWidget* createDashboardWidget() override;

	private:
		struct FileSystemUsage
		{
			QString dir;
			QString device;
			QString options;
			uint64_t total;
			uint64_t free;
			uint64_t available;
			uint64_t files;
			uint64_t freeFiles;
		};

		struct TimeStampInfo
		{
			int64_t timePoint;
			QVector<char> data;
		};

		SensorDashboardPlot *mSpacePlot;
		SensorDashboardPlot *mFilePlot;

		QCPBars *mSpaceUsageBar;
		QCPBars *mFileUsageBar;

		QSlider *mTimeScale;
		QLabel *mTimeStamp;
		QVector<TimeStampInfo> mTimeStampData;

		SensorDashboardPlot* createPlot(QCPBars *&usageBar) const;

		QVector<FileSystemUsage> fileSystemUsage(const QVector<char>& data) const;

	private slots:
		void timeScaleMoved();
		void timeScaleReleased();

	};
}

#endif // VIKKI_FILE_SYSTEM_USAGE_SENSOR_DASHBOARD_H

