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

#include "client_controller.h"

namespace Vikki
{
	ClientController::ClientController()
		: mThread(new QThread())
	{
		mWorker = createWorker();

		mConnection = createConnection(mWorker);

		mThread->start();
	}

	ClientController::~ClientController()
	{
		mWorker->deleteLater();

		mThread->quit();
		mThread->wait();
	}

	NetworkConnectionPointer ClientController::connection() const
	{
		return mConnection;
	}

	void ClientController::setEncryptionEnabled(const bool enabled)
	{
		QMetaObject::invokeMethod(mWorker, "setEncryptionEnabled", Qt::QueuedConnection,
			Q_ARG(bool, enabled));
	}

	void ClientController::setCaCertificates(const QList<QSslCertificate>& certificates)
	{
		QMetaObject::invokeMethod(mWorker, "setCaCertificates", Qt::QueuedConnection,
			Q_ARG(QList<QSslCertificate>, certificates));
	}

	void ClientController::setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate)
	{
		QMetaObject::invokeMethod(mWorker, "setEncryption", Qt::QueuedConnection,
			Q_ARG(QSslKey, privateKey), Q_ARG(QSslCertificate, certificate));
	}

	void ClientController::setIgnoredSslErrors(const QList<QSslError::SslError>& errors)
	{
		QMetaObject::invokeMethod(mWorker, "setIgnoredSslErrors", Qt::QueuedConnection,
			Q_ARG(QList<QSslError::SslError>, errors));
	}

	void ClientController::setConnectionOptions(const QString& hostName, const quint16 port)
	{		
		QMetaObject::invokeMethod(mWorker, "setConnectionOptions", Qt::QueuedConnection,
			Q_ARG(QString, hostName), Q_ARG(quint16, port));
	}

	void ClientController::connectToHost()
	{
		QMetaObject::invokeMethod(mWorker, "connect", Qt::QueuedConnection);
	}

	void ClientController::disconnectFromHost()
	{		
		QMetaObject::invokeMethod(mWorker, "disconnect", Qt::QueuedConnection);
	}

	ClientWorker* ClientController::createWorker() const
	{
		ClientWorker *worker = new ClientWorker();

		worker->moveToThread(mThread.data());

		connect(worker, &ClientWorker::raiseConnectionEstablished, this, &ClientController::raiseConnectionEstablished);
		connect(worker, &ClientWorker::raiseConnectionClosed, this, &ClientController::raiseConnectionClosed);

		connect(worker, &ClientWorker::raiseError, this, &ClientController::raiseError);

		return worker;
	}

	NetworkConnectionPointer ClientController::createConnection(ClientWorker *worker) const
	{
		NetworkConnectionPointer connection = NetworkConnectionPointer(new NetworkConnection());

		connect(connection.data(), &NetworkConnection::raiseSendData, worker, &ClientWorker::sendData);
		connect(connection.data(), &NetworkConnection::raiseReceiverReady, worker, &ClientWorker::receiverReady);
		connect(connection.data(), &NetworkConnection::raiseCloseConnection, worker, &ClientWorker::closeConnection);

		connect(worker, &ClientWorker::raiseDataReceived, connection.data(), &NetworkConnection::dataReceived);

		return connection;
	}
}
