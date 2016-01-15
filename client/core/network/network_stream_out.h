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

#ifndef VIKKI_NETWORK_STREAM_OUT_H
#define VIKKI_NETWORK_STREAM_OUT_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QDataStream>
#include <QSharedPointer>

namespace Vikki
{
	class NetworkStreamOut
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(NetworkStreamOut)

	public:
		template<typename T>
		NetworkStreamOut(quint64 eventId, const T& command);

		~NetworkStreamOut() Q_DECL_OVERRIDE;

		qint64 eventId() const;

		template<typename T>
		T command() const;

		void writeInt8(qint8 data);
		void writeUInt8(quint8 data);

		void writeInt16(qint16 data);
		void writeUInt16(quint16 data);

		void writeInt32(qint32 data);
		void writeUInt32(quint32 data);

		void writeInt64(qint64 data);
		void writeUInt64(quint64 data);

		void writeFloat32(float data);
		void writeFloat64(double data);

		template<typename T = quint64>
		void writeString(const QString& data);

		template<typename T = char, typename U = quint64>
		void writeDataChunk(const T *data, U size);

		template<typename T = char, typename U = quint64>
		void writeDataChunk(const QVector<T>& data);

		void sendData();

	signals:
		void raiseSendData(const QByteArray& data);

	private:
		quint64 mEventId;
		quint64 mCommand;

		QByteArray mBuffer;
		QScopedPointer<QDataStream> mStream;
		bool mDataSended;

		template<typename T>
		void writeBasic(T data);

	};

	template<typename T>
	NetworkStreamOut::NetworkStreamOut(quint64 eventId, const T& command)
		: mEventId(eventId), mCommand(static_cast<quint64>(command)), mDataSended(false)
	{
		mStream.reset(new QDataStream(&mBuffer, QIODevice::WriteOnly));

		mStream->setVersion(QDataStream::Qt_5_0);

		writeUInt64(0);

		writeUInt64(mEventId);
		writeUInt64(mCommand);
	}

	template<typename T>
	T NetworkStreamOut::command() const
	{
		return static_cast<T>(mCommand);
	}

	template<typename T>
	void NetworkStreamOut::writeString(const QString& data)
	{
		QByteArray bytes = data.toLocal8Bit();

		writeBasic<T>(static_cast<T>(bytes.size()));

		mStream->writeRawData(bytes.data(), bytes.size());
	}

	template<typename T, typename U>
	void NetworkStreamOut::writeDataChunk(const T *data, U size)
	{
		writeBasic<U>(size);

		mStream->writeRawData(reinterpret_cast<const char*>(data), size * sizeof(T));
	}

	template<typename T, typename U>
	void NetworkStreamOut::writeDataChunk(const QVector<T>& data)
	{
		writeDataChunk<T, U>(data.data(), data.size());
	}

	template<typename T>
	void NetworkStreamOut::writeBasic(T data)
	{
		mStream->writeRawData(reinterpret_cast<const char*>(&data), sizeof(T));
	}

	using NetworkStreamOutPointer = QSharedPointer<NetworkStreamOut>;
}

#endif // VIKKI_NETWORK_STREAM_OUT_H

