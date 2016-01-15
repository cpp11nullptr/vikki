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

#ifndef VIKKI_NETWORK_CONNECTION_H
#define VIKKI_NETWORK_CONNECTION_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QByteArray>
#include <QSharedPointer>
#include <functional>

#include "network_stream_in.h"
#include "network_stream_out.h"

namespace Vikki
{
	using NetworkResponseCallback = std::function<void(NetworkStreamInPointer)>;

	class NetworkConnection
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(NetworkConnection)

	public:
		NetworkConnection();
		~NetworkConnection() Q_DECL_OVERRIDE;

		template<typename T>
		NetworkStreamOutPointer createStream(const T& command);

		template<typename T>
		NetworkStreamOutPointer createStream(const T& command, NetworkResponseCallback responseCallback);

		void receiverReady();
		void closeConnection();

		void removeResponseCallbacks();

	private:
		quint64 mEventIdCounter;
		QMap<quint64, NetworkResponseCallback> mResponseCallbacks;

	signals:
		void raiseSendData(const QByteArray& data);
		void raiseDataReceived(NetworkStreamInPointer stream);

		void raiseReceiverReady();
		void raiseCloseConnection();

	public slots:
		void dataReceived(const QByteArray& data);

	};

	template<typename T>
	NetworkStreamOutPointer NetworkConnection::createStream(const T& command)
	{
		NetworkStreamOutPointer stream = NetworkStreamOutPointer(new NetworkStreamOut(mEventIdCounter++, command));

		connect(stream.data(), SIGNAL(raiseSendData(QByteArray)),
			this, SIGNAL(raiseSendData(QByteArray)));

		return stream;
	}

	template<typename T>
	NetworkStreamOutPointer NetworkConnection::createStream(const T& command, NetworkResponseCallback responseCallback)
	{
		NetworkStreamOutPointer stream = NetworkStreamOutPointer(new NetworkStreamOut(mEventIdCounter++, command));

		connect(stream.data(), SIGNAL(raiseSendData(QByteArray)),
			this, SIGNAL(raiseSendData(QByteArray)));

		mResponseCallbacks[stream->eventId()] = responseCallback;

		return stream;
	}

	using NetworkConnectionPointer = QSharedPointer<NetworkConnection>;
}

Q_DECLARE_METATYPE(Vikki::NetworkConnectionPointer)

#endif // VIKKI_NETWORK_CONNECTION_H

