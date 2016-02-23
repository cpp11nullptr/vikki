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

#include "memory_usage_sensor_dashboard.h"

namespace Vikki
{
	MemoryUsageSensorDashboard::MemoryUsageSensorDashboard(const QString& sensorName, const QString& sensorTitle)
		: SensorDashboard(sensorName, sensorTitle), mMaxY(0), mPlot(nullptr), mGraphMemoryTotal(nullptr),
		  mGraphMemoryUsed(nullptr), mGraphSwapTotal(nullptr), mGraphSwapUsed(nullptr)
	{
	}

	MemoryUsageSensorDashboard::~MemoryUsageSensorDashboard()
	{
	}

	void MemoryUsageSensorDashboard::sensorDataReceived(NetworkStreamInPointer stream)
	{
		QVector<double> time;
		QVector<double> valueMemoryTotal;
		QVector<double> valueMemoryUsed;
		QVector<double> valueSwapTotal;
		QVector<double> valueSwapUsed;

		mMaxY = 0.0;
		uint64_t size = stream->readUInt64();

		for (uint64_t i = 0; i < size; ++i)
		{
			int64_t timePoint = stream->readInt64();
			QVector<char> dataChunk = stream->readDataChunk();

			uint64_t memoryTotal = *reinterpret_cast<uint64_t*>(dataChunk.data());
			uint64_t memoryFree = *reinterpret_cast<uint64_t*>(dataChunk.data() + sizeof(uint64_t));

			uint64_t swapTotal = *reinterpret_cast<uint64_t*>(dataChunk.data() + sizeof(uint64_t) * 4);
			uint64_t swapFree = *reinterpret_cast<uint64_t*>(dataChunk.data() + sizeof(uint64_t) * 5);

			time.push_back(static_cast<double>(timePoint));

			valueMemoryTotal.push_back(static_cast<double>(memoryTotal) / (1024 * 1024));
			valueMemoryUsed.push_back(static_cast<double>(memoryTotal - memoryFree) / (1024 * 1024));

			valueSwapTotal.push_back(static_cast<double>(swapTotal) / (1024 * 1024));
			valueSwapUsed.push_back(static_cast<double>(swapTotal - swapFree) / (1024 * 1024));

			mMaxY = memoryTotal > mMaxY ? memoryTotal : mMaxY;
			mMaxY = swapTotal > mMaxY ? swapTotal : mMaxY;
		}

		mGraphMemoryTotal->setData(time, valueMemoryTotal);
		mGraphMemoryUsed->setData(time, valueMemoryUsed);

		mGraphSwapTotal->setData(time, valueSwapTotal);
		mGraphSwapUsed->setData(time, valueSwapUsed);

		mPlot->setRanges(periodFrom()->dateTime().toTime_t(), periodTo()->dateTime().toTime_t(),
			0.0, mMaxY / (1024 * 1024) * 1.1);
	}

	void MemoryUsageSensorDashboard::sensorDataUpdated(NetworkStreamInPointer stream)
	{
		int64_t timePoint = stream->readInt64();
		QVector<char> dataChunk = stream->readDataChunk();

		uint64_t memoryTotal = *reinterpret_cast<uint64_t*>(dataChunk.data());
		uint64_t memoryFree = *reinterpret_cast<uint64_t*>(dataChunk.data() + sizeof(uint64_t));

		uint64_t swapTotal = *reinterpret_cast<uint64_t*>(dataChunk.data() + sizeof(uint64_t) * 4);
		uint64_t swapFree = *reinterpret_cast<uint64_t*>(dataChunk.data() + sizeof(uint64_t) * 5);

		double time = static_cast<double>(timePoint);

		uint timeDiff = periodTo()->dateTime().toTime_t() - periodFrom()->dateTime().toTime_t();
		timeDiff = qAbs(timeDiff);

		periodFrom()->setDateTime(QDateTime::fromTime_t(time - timeDiff));
		periodTo()->setDateTime(QDateTime::fromTime_t(time));

		mMaxY = memoryTotal > mMaxY ? memoryTotal : mMaxY;
		mMaxY = swapTotal > mMaxY ? swapTotal : mMaxY;

		mGraphSwapTotal->addData(time, static_cast<double>(swapTotal) / (1024 * 1024));
		mGraphSwapUsed->addData(time, static_cast<double>(swapTotal - swapFree) / (1024 * 1024));

		mGraphMemoryTotal->addData(time, static_cast<double>(memoryTotal) / (1024 * 1024));
		mGraphMemoryUsed->addData(time, static_cast<double>(memoryTotal - memoryFree) / (1024 * 1024));

		mPlot->setRanges(periodFrom()->dateTime().toTime_t(), periodTo()->dateTime().toTime_t(),
			0.0, mMaxY / (1024 * 1024) * 1.1);
	}

	QWidget* MemoryUsageSensorDashboard::createDashboardWidget()
	{
		mPlot = new SensorDashboardPlot();

		mGraphMemoryTotal = mPlot->createGraph(tr("Memory total"), Qt::green);
		mGraphMemoryUsed = mPlot->createGraph(tr("Memory used"), Qt::blue);
		mGraphSwapTotal = mPlot->createGraph(tr("Swap total"), Qt::magenta);
		mGraphSwapUsed = mPlot->createGraph(tr("Swap used"), Qt::cyan);

		return mPlot;
	}
}

