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

#include "load_average_sensor_dashboard.h"

namespace Vikki
{
	LoadAverageSensorDashboard::LoadAverageSensorDashboard(const QString& sensorName, const QString& sensorTitle)
		: SensorDashboard(sensorName, sensorTitle), mMaxY(0.0), mPlot(nullptr),
		  mGraph1m(nullptr), mGraph5m(nullptr), mGraph15m(nullptr)
	{
	}

	LoadAverageSensorDashboard::~LoadAverageSensorDashboard()
	{
	}

	void LoadAverageSensorDashboard::sensorDataReceived(NetworkStreamInPointer stream)
	{
		QVector<double> time;
		QVector<double> value1;
		QVector<double> value5;
		QVector<double> value15;

		mMaxY = 0.0;
		uint64_t size = stream->readUInt64();

		for (uint64_t i = 0; i < size; ++i)
		{
			int64_t timePoint = stream->readInt64();
			QVector<char> dataChunk = stream->readDataChunk();

			double la1 = *reinterpret_cast<double*>(dataChunk.data());
			double la5 = *reinterpret_cast<double*>(dataChunk.data() + sizeof(double));
			double la15 = *reinterpret_cast<double*>(dataChunk.data() + sizeof(double) * 2);

			time.push_back(static_cast<double>(timePoint));

			value1.push_back(la1);
			value5.push_back(la5);
			value15.push_back(la15);

			mMaxY = la1 > mMaxY ? la1 : mMaxY;
			mMaxY = la5 > mMaxY ? la5 : mMaxY;
			mMaxY = la15 > mMaxY ? la15 : mMaxY;
		}

		mGraph1m->setData(time, value1);
		mGraph5m->setData(time, value5);
		mGraph15m->setData(time, value15);

		mPlot->setRanges(periodFrom()->dateTime().toTime_t(), periodTo()->dateTime().toTime_t(),
			0.0, mMaxY * 1.1);
	}

	void LoadAverageSensorDashboard::sensorDataUpdated(NetworkStreamInPointer stream)
	{
		int64_t timePoint = stream->readInt64();
		QVector<char> dataChunk = stream->readDataChunk();

		double la1 = *reinterpret_cast<double*>(dataChunk.data());
		double la5 = *reinterpret_cast<double*>(dataChunk.data() + sizeof(double));
		double la15 = *reinterpret_cast<double*>(dataChunk.data() + sizeof(double) * 2);

		double time = static_cast<double>(timePoint);

		uint timeDiff = periodTo()->dateTime().toTime_t() - periodFrom()->dateTime().toTime_t();
		timeDiff = qAbs(timeDiff);

		periodFrom()->setDateTime(QDateTime::fromTime_t(time - timeDiff));
		periodTo()->setDateTime(QDateTime::fromTime_t(time));

		mMaxY = la1 > mMaxY ? la1 : mMaxY;
		mMaxY = la5 > mMaxY ? la5 : mMaxY;
		mMaxY = la15 > mMaxY ? la15 : mMaxY;

		mGraph1m->addData(time, la1);
		mGraph5m->addData(time, la5);
		mGraph15m->addData(time, la15);

		mPlot->setRanges(periodFrom()->dateTime().toTime_t(), periodTo()->dateTime().toTime_t(),
			0.0, mMaxY * 1.1);
	}

	QWidget* LoadAverageSensorDashboard::createDashboardWidget()
	{
		mPlot = new SensorDashboardPlot();

		mGraph1m = mPlot->createGraph(tr("1 minute"), Qt::green);
		mGraph5m = mPlot->createGraph(tr("5 minutes"), Qt::blue);
		mGraph15m = mPlot->createGraph(tr("15 minutes"), Qt::magenta);

		return mPlot;
	}
}
