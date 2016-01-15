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

#include "network_stream_out.h"

namespace Vikki
{
	NetworkStreamOut::~NetworkStreamOut()
	{
		sendData();
	}

	qint64 NetworkStreamOut::eventId() const
	{
		return mEventId;
	}

	void NetworkStreamOut::writeInt8(qint8 data)
	{
		writeBasic<qint8>(data);
	}

	void NetworkStreamOut::writeUInt8(quint8 data)
	{
		writeBasic<quint8>(data);
	}

	void NetworkStreamOut::writeInt16(qint16 data)
	{
		writeBasic<qint16>(data);
	}

	void NetworkStreamOut::writeUInt16(quint16 data)
	{
		writeBasic<quint16>(data);
	}

	void NetworkStreamOut::writeInt32(qint32 data)
	{
		writeBasic<qint32>(data);
	}

	void NetworkStreamOut::writeUInt32(quint32 data)
	{
		writeBasic<quint32>(data);
	}

	void NetworkStreamOut::writeInt64(qint64 data)
	{
		writeBasic<qint64>(data);
	}

	void NetworkStreamOut::writeUInt64(quint64 data)
	{
		writeBasic<quint64>(data);
	}

	void NetworkStreamOut::writeFloat32(float data)
	{
		writeBasic<float>(data);
	}

	void NetworkStreamOut::writeFloat64(double data)
	{
		writeBasic<double>(data);
	}

	void NetworkStreamOut::sendData()
	{
		if (mDataSended)
		{
			return;
		}

		mStream->device()->seek(0);

		writeUInt64(mBuffer.size() - sizeof(quint64));

		emit raiseSendData(mBuffer);

		mDataSended = true;
	}
}
