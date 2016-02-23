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

#include "file_system_usage_sensor_dashboard.h"

#include <cstring>

namespace Vikki
{
	FileSystemUsageSensorDashboard::FileSystemUsageSensorDashboard(const QString& sensorName, const QString& sensorTitle)
		: SensorDashboard(sensorName, sensorTitle)
	{
	}

	FileSystemUsageSensorDashboard::~FileSystemUsageSensorDashboard()
	{
	}

	void FileSystemUsageSensorDashboard::sensorDataReceived(NetworkStreamInPointer stream)
	{
		uint64_t size = stream->readUInt64();

		mTimeStampData.clear();
		mTimeStampData.resize(size);

		for (uint64_t i = 0; i < size; ++i)
		{
			TimeStampInfo info;

			info.timePoint = stream->readInt64();
			info.data = stream->readDataChunk();

			mTimeStampData[i] = info;
		}

		mTimeScale->setMaximum(mTimeStampData.count() - 1);
		mTimeScale->setValue(mTimeStampData.count() - 1);

		mTimeScale->setEnabled(size != 0);
		mTimeStamp->setVisible(size != 0);

		timeScaleMoved();
		timeScaleReleased();
	}

	void FileSystemUsageSensorDashboard::sensorDataUpdated(NetworkStreamInPointer stream)
	{
		TimeStampInfo info;

		info.timePoint = stream->readInt64();
		info.data = stream->readDataChunk();

		mTimeStampData.push_back(info);

		mTimeScale->setMaximum(mTimeStampData.count() - 1);
		mTimeScale->setValue(mTimeStampData.count() - 1);

		periodTo()->setDateTime(QDateTime::fromTime_t(info.timePoint));

		timeScaleMoved();
		timeScaleReleased();
	}

	QWidget* FileSystemUsageSensorDashboard::createDashboardWidget()
	{
		mSpacePlot = createPlot(mSpaceUsageBar);
		mFilePlot = createPlot(mFileUsageBar);

		QTabWidget *tabWidget = new QTabWidget();
		tabWidget->addTab(mSpacePlot, tr("Space usage"));
		tabWidget->addTab(mFilePlot, tr("File usage"));

		mTimeScale = new QSlider(Qt::Horizontal);
		mTimeScale->setMinimum(0);
		mTimeScale->setMaximum(0);
		mTimeScale->setTickInterval(1);
		mTimeScale->setValue(0);

		connect(mTimeScale, &QSlider::sliderMoved, this, &FileSystemUsageSensorDashboard::timeScaleMoved);
		connect(mTimeScale, &QSlider::sliderReleased, this, &FileSystemUsageSensorDashboard::timeScaleReleased);

		mTimeStamp = new QLabel();

		QHBoxLayout *timeScaleLayout = new QHBoxLayout();
		timeScaleLayout->addWidget(new QLabel(tr("Time scale")));
		timeScaleLayout->addWidget(mTimeScale);
		timeScaleLayout->addWidget(mTimeStamp);

		QVBoxLayout *layout = new QVBoxLayout();
		layout->addLayout(timeScaleLayout);
		layout->addWidget(tabWidget);

		QWidget *widget = new QWidget();
		widget->setLayout(layout);

		return widget;
	}

	SensorDashboardPlot* FileSystemUsageSensorDashboard::createPlot(QCPBars *&usageBar) const
	{
		QVector<double> usageTicks;

		for (int i = 0; i <= 10; ++i)
		{
			usageTicks.push_back(i * 10.0);
		}

		SensorDashboardPlot *plot = new SensorDashboardPlot();

		QCPAxisRect *usageRect = new QCPAxisRect(plot, false);
		usageRect->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);

		usageRect->axis(QCPAxis::atBottom)->setAutoTicks(false);
		usageRect->axis(QCPAxis::atBottom)->setAutoTickLabels(false);
		usageRect->axis(QCPAxis::atBottom)->setSubTickCount(0);

		usageRect->axis(QCPAxis::atLeft)->setAutoTicks(false);
		usageRect->axis(QCPAxis::atLeft)->setTickVector(usageTicks);
		usageRect->axis(QCPAxis::atLeft)->setTickLength(0, usageTicks.count());

		usageRect->axis(QCPAxis::atLeft)->setRange(0.0, 100.0);
		usageRect->axis(QCPAxis::atLeft)->setPadding(10);
		usageRect->axis(QCPAxis::atLeft)->setLabel("Usage, %");
		usageRect->axis(QCPAxis::atLeft)->grid()->setVisible(true);
		usageRect->axis(QCPAxis::atLeft)->grid()->setSubGridVisible(true);

		usageBar = new QCPBars(usageRect->axis(QCPAxis::atBottom), usageRect->axis(QCPAxis::atLeft));
		usageBar->setBrush(QColor("#705BE8"));

		plot->addPlottable(usageBar);

		QPen gridPen;
		gridPen.setStyle(Qt::SolidLine);

		gridPen.setColor(QColor(0, 0, 0, 25));
		usageRect->axis(QCPAxis::atLeft)->grid()->setPen(gridPen);

		gridPen.setStyle(Qt::DotLine);
		usageRect->axis(QCPAxis::atLeft)->grid()->setSubGridPen(gridPen);

		plot->plotLayout()->clear();

		plot->plotLayout()->addElement(0, 0, usageRect);

		return plot;
	}

	QVector<FileSystemUsageSensorDashboard::FileSystemUsage> FileSystemUsageSensorDashboard::fileSystemUsage(const QVector<char>& data) const
	{
		const char *p = data.constData();

		uint64_t systemCount = *reinterpret_cast<const uint64_t*>(p);
		p += sizeof(uint64_t);

		QVector<FileSystemUsage> fsUsage(systemCount);

		for (uint64_t i = 0; i < systemCount; ++i)
		{
			uint64_t dirStrSize = *reinterpret_cast<const uint64_t*>(p);
			p += sizeof(uint64_t);

			char* dirStr = new char[dirStrSize];
			std::memcpy(dirStr, p, dirStrSize);
			p += dirStrSize;

			QString dir = QString::fromLocal8Bit(dirStr, dirStrSize);

			//

			uint64_t deviceStrSize = *reinterpret_cast<const uint64_t*>(p);
			p += sizeof(uint64_t);

			char* deviceStr = new char[deviceStrSize];
			std::memcpy(deviceStr, p, deviceStrSize);
			p += deviceStrSize;

			QString device = QString::fromLocal8Bit(deviceStr, deviceStrSize);

			//

			uint64_t typeStrSize = *reinterpret_cast<const uint64_t*>(p);
			p += sizeof(uint64_t);

			char* typeStr = new char[typeStrSize];
			std::memcpy(typeStr, p, typeStrSize);
			p += typeStrSize;

			QString type = QString::fromLocal8Bit(typeStr, typeStrSize);

			//

			uint64_t optionsStrSize = *reinterpret_cast<const uint64_t*>(p);
			p += sizeof(uint64_t);

			char* optionsStr = new char[optionsStrSize];
			std::memcpy(optionsStr, p, optionsStrSize);
			p += optionsStrSize;

			QString options = QString::fromLocal8Bit(optionsStr, optionsStrSize);

			//

			uint64_t total = *reinterpret_cast<const uint64_t*>(p);
			p += sizeof(uint64_t);

			uint64_t free = *reinterpret_cast<const uint64_t*>(p);
			p += sizeof(uint64_t);

			uint64_t available = *reinterpret_cast<const uint64_t*>(p);
			p += sizeof(uint64_t);

			uint64_t files = *reinterpret_cast<const uint64_t*>(p);
			p += sizeof(uint64_t);

			uint64_t freeFiles = *reinterpret_cast<const uint64_t*>(p);
			p += sizeof(uint64_t);

			//

			FileSystemUsage fsUsageItem;

			fsUsageItem.dir = dir;
			fsUsageItem.device = device;
			fsUsageItem.options = options;
			fsUsageItem.total = total;
			fsUsageItem.free = free;
			fsUsageItem.available = available;
			fsUsageItem.files = files;
			fsUsageItem.freeFiles = freeFiles;

			fsUsage[i] = fsUsageItem;
		}

		return fsUsage;
	}

	void FileSystemUsageSensorDashboard::timeScaleMoved()
	{
		if (mTimeStampData.empty())
		{
			return;
		}

		const int value = mTimeScale->value();

		if (value < 0 || value > mTimeStampData.count() - 1)
		{
			return;
		}

		const TimeStampInfo& info = mTimeStampData.at(value);

		mTimeStamp->setText(QDateTime::fromTime_t(info.timePoint).toString("dd/MM/yyyy HH:mm:ss"));
	}

	void FileSystemUsageSensorDashboard::timeScaleReleased()
	{
		if (mTimeStampData.empty())
		{
			return;
		}

		const int value = mTimeScale->value();

		if (value < 0 || value > mTimeStampData.count() - 1)
		{
			return;
		}

		const TimeStampInfo& info = mTimeStampData.at(value);

		QVector<QString> spaceLabels;
		QVector<double> spaceTicks;

		QVector<double> spaceUsageData;
		QVector<double> fileUsageData;

		QVector<FileSystemUsage> fsUsage = fileSystemUsage(info.data);

		for (int i = 0; i < fsUsage.count(); ++i)
		{
			const FileSystemUsage& fsUsageItem = fsUsage.at(i);

			double spaceUsed = 100.0 * (fsUsageItem.total - fsUsageItem.free) / fsUsageItem.total;
			double fileUsed = 100.0 * (fsUsageItem.files - fsUsageItem.freeFiles) / fsUsageItem.files;

			spaceLabels.push_back(fsUsageItem.dir);
			spaceTicks.push_back(i + 1);

			spaceUsageData.push_back(spaceUsed);
			fileUsageData.push_back(fileUsed);
		}

		auto setupUsageRect = [&spaceTicks, &spaceLabels](SensorDashboardPlot *plot)
		{
			QCPAxisRect *usageRect = qobject_cast<QCPAxisRect*>(plot->plotLayout()->element(0, 0));

			usageRect->axis(QCPAxis::atBottom)->setTickVector(spaceTicks);
			usageRect->axis(QCPAxis::atBottom)->setTickVectorLabels(spaceLabels);
			usageRect->axis(QCPAxis::atBottom)->setRange(0, spaceLabels.count() + 1);
		};

		setupUsageRect(mSpacePlot);
		setupUsageRect(mFilePlot);

		mSpaceUsageBar->setData(spaceTicks, spaceUsageData);
		mFileUsageBar->setData(spaceTicks, fileUsageData);

		mSpacePlot->replot();
		mFilePlot->replot();
	}
}
