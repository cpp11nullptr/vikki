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

#include "client.h"

namespace Vikki
{
	Client::Client()
		: mStarted(false)
	{
		qRegisterMetaType<QSslKey>("QSslKey");
		qRegisterMetaType<QSslCertificate>("QSslCertificate");
		qRegisterMetaType<QList<QSslCertificate>>("QList<QSslCertificate>");
		qRegisterMetaType<QList<QSslError::SslError>>("QList<QSslError::SslError>");
		qRegisterMetaType<NetworkConnectionPointer>("NetworkConnectionPointer");

		mController = ClientControllerPointer(new ClientController());

		connect(mController.data(), &ClientController::raiseConnectionEstablished, this, &Client::connected);
		connect(mController.data(), &ClientController::raiseConnectionClosed, this, &Client::disconnected);

		connect(mController.data(), &ClientController::raiseError, this, &Client::error);

		connect(mController->connection().data(), &NetworkConnection::raiseDataReceived, this, &Client::dataReceived);
	}

	Client::~Client()
	{
	}

	bool Client::setEncryptionEnabled(const bool enabled)
	{
		if (mStarted)
		{
			return false;
		}

		mController->setEncryptionEnabled(enabled);

		return true;
	}

	bool Client::setCaCertificates(const QList<QSslCertificate>& certificates)
	{
		if (mStarted)
		{
			return false;
		}

		mController->setCaCertificates(certificates);

		return true;
	}

	bool Client::setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate)
	{
		if (mStarted)
		{
			return false;
		}

		mController->setEncryption(privateKey, certificate);

		return true;
	}

	bool Client::setIgnoredSslErrors(const QList<QSslError::SslError>& errors)
	{
		if (mStarted)
		{
			return false;
		}

		mController->setIgnoredSslErrors(errors);

		return true;
	}

	NetworkConnectionPointer Client::connection() const
	{
		return mController->connection();
	}

	void Client::start(const QString& hostName, const quint16 port)
	{
		mController->setConnectionOptions(hostName, port);

		mController->connectToHost();
	}

	void Client::stop()
	{
		mController->disconnectFromHost();
	}

	bool Client::isStarted() const
	{
		return mStarted;
	}

	void Client::connected()
	{
		mStarted = true;

		emit raiseConnected(mController->connection());

		mController->connection()->receiverReady();
	}

	void Client::disconnected()
	{
		mStarted = false;

		emit raiseDisconnected(mController->connection());
	}

	void Client::dataReceived(NetworkStreamInPointer stream)
	{
		emit raiseDataReceived(mController->connection(), stream);
	}

	void Client::error(const QString& message, const bool ignored)
	{
		emit raiseError(mController->connection(), message, ignored);
	}
}
