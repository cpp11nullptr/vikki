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

#include "sensor_client_proxy.h"

namespace Vikki
{
	SensorClientProxy::SensorClientProxy(SensorDashboard *sensorDashboard, QSharedPointer<Client> client)
		: mSensorDashboard(sensorDashboard), mClient(client)
	{
		connect(mClient.data(), &Client::raiseDataReceived, this, &SensorClientProxy::clientDataReceived);

		connect(mSensorDashboard, &SensorDashboard::getSensorData, this, &SensorClientProxy::getSensorData);
		connect(mSensorDashboard, &SensorDashboard::subscribeSensorData, this, &SensorClientProxy::subscribeSensorData);

		mSensorDashboard->create();
	}

	SensorClientProxy::~SensorClientProxy()
	{
		subscribeSensorData(false);

		mClient->connection()->removeResponseCallbacks();

		mSensorDashboard->destroy();
	}

	void SensorClientProxy::updateSensorData(NetworkStreamInPointer stream)
	{
		const QString& sensName = stream->readString();

		if (sensName == mSensorDashboard->sensorName())
		{
			mSensorDashboard->sensorDataUpdated(stream);
		}
	}

	void SensorClientProxy::clientDataReceived(NetworkConnectionPointer connection, NetworkStreamInPointer stream)
	{
		Q_UNUSED(connection);

		switch (stream->command<Command>())
		{
		case Command::SENSOR_DATA_UPDATED:
			updateSensorData(stream);
			break;

		default:
			break;
		}
	}

	void SensorClientProxy::getSensorData(uint from, uint to)
	{
		auto callback = [this](NetworkStreamInPointer stream)
		{
			mSensorDashboard->sensorDataReceived(stream);
		};

		NetworkStreamOutPointer stream = mClient->connection()->createStream(Command::GET_SENSOR_DATA, callback);

		stream->writeString(mSensorDashboard->sensorName());
		stream->writeInt64(static_cast<int64_t>(from));
		stream->writeInt64(static_cast<int64_t>(to));
	}

	void SensorClientProxy::subscribeSensorData(bool subscribe)
	{
		NetworkStreamOutPointer stream = mClient->connection()->createStream(Command::SENSOR_DATA_SUBSCRIBE);

		stream->writeString(mSensorDashboard->sensorName());
		stream->writeInt8(subscribe ? 1 : 0);
	}
}
