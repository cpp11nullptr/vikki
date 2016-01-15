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

#include "network_stream_in.h"

namespace Vikki
{
	NetworkStreamIn::NetworkStreamIn(const QByteArray& buffer)
		: mBuffer(buffer)
	{
		mStream.reset(new QDataStream(&mBuffer, QIODevice::ReadOnly));

		mStream->setVersion(QDataStream::Qt_5_0);

		mEventId = readUInt64();
		mCommand = readUInt64();
	}

	NetworkStreamIn::~NetworkStreamIn()
	{
	}

	qint64 NetworkStreamIn::eventId() const
	{
		return mEventId;
	}

	qint8 NetworkStreamIn::readInt8()
	{
		return readBasic<qint8>();
	}

	quint8 NetworkStreamIn::readUInt8()
	{
		return readBasic<quint8>();
	}

	qint16 NetworkStreamIn::readInt16()
	{
		return readBasic<qint16>();
	}

	quint16 NetworkStreamIn::readUInt16()
	{
		return readBasic<quint16>();
	}

	qint32 NetworkStreamIn::readInt32()
	{
		return readBasic<qint32>();
	}

	quint32 NetworkStreamIn::readUInt32()
	{
		return readBasic<quint32>();
	}

	qint64 NetworkStreamIn::readInt64()
	{
		return readBasic<qint64>();
	}

	quint64 NetworkStreamIn::readUInt64()
	{
		return readBasic<quint64>();
	}

	float NetworkStreamIn::readFloat32()
	{
		return readBasic<float>();
	}

	double NetworkStreamIn::readFloat64()
	{
		return readBasic<double>();
	}

	QSharedPointer<NetworkStreamOut> NetworkStreamIn::createResponse() const
	{
		QSharedPointer<NetworkStreamOut> stream = QSharedPointer<NetworkStreamOut>(new NetworkStreamOut(mEventId, mCommand));

		connect(stream.data(), &NetworkStreamOut::raiseSendData, this, &NetworkStreamIn::raiseSendResponseData);

		return stream;
	}
}
