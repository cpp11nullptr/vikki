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

#ifndef VIKKI_NETWORK_STREAM_IN_H
#define VIKKI_NETWORK_STREAM_IN_H

#include "network_stream_out.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QDataStream>
#include <QSharedPointer>

namespace Vikki
{
	class NetworkStreamIn
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(NetworkStreamIn)

	public:
		explicit NetworkStreamIn(const QByteArray& buffer);
		~NetworkStreamIn() Q_DECL_OVERRIDE;

		qint64 eventId() const;

		template<typename T>
		T command() const;

		qint8 readInt8();
		quint8 readUInt8();

		qint16 readInt16();
		quint16 readUInt16();

		qint32 readInt32();
		quint32 readUInt32();

		qint64 readInt64();
		quint64 readUInt64();

		float readFloat32();
		double readFloat64();

		template<typename T = quint64>
		QString readString();

		template<typename T = char, typename U = quint64>
		const T* readDataChunk(U& size);

		template<typename T = char, typename U = quint64>
		QVector<T> readDataChunk();

		QSharedPointer<NetworkStreamOut>	createResponse() const;

	signals:
		void raiseSendResponseData(const QByteArray& data);

	private:
		QByteArray mBuffer;
		QScopedPointer<QDataStream> mStream;

		quint64 mCommand;
		quint64 mEventId;

		template<typename T>
		T readBasic();

	};

	template<typename T>
	T NetworkStreamIn::command() const
	{
		return static_cast<T>(mCommand);
	}

	template<typename T>
	QString NetworkStreamIn::readString()
	{
		T size = readBasic<T>();

		char data[size];
		mStream->readRawData(data, size);

		return QString::fromLocal8Bit(data, size);
	}

	template<typename T, typename U>
	const T* NetworkStreamIn::readDataChunk(U& size)
	{
		size = readBasic<U>();

		T *data = new char[size];
		mStream->readRawData(reinterpret_cast<char*>(data), size * sizeof(T));

		return data;
	}

	template<typename T, typename U>
	QVector<T> NetworkStreamIn::readDataChunk()
	{
		U size = readBasic<U>();

		QVector<T> chunk(size);
		mStream->readRawData(reinterpret_cast<char*>(chunk.data()), chunk.size() * sizeof(T));

		return chunk;
	}

	template<typename T>
	T NetworkStreamIn::readBasic()
	{
		T data;

		mStream->readRawData(reinterpret_cast<char*>(&data), sizeof(T));

		return data;
	}

	using NetworkStreamInPointer = QSharedPointer<NetworkStreamIn>;
}

#endif // VIKKI_NETWORK_STREAM_IN_H

