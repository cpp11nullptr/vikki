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

#include "network_worker.h"

namespace Vikki
{
	NetworkWorker::NetworkWorker()
		: mBytesExpected(0), mReceiverReady(false), mEncryptionEnabled(false)
	{
	}

	NetworkWorker::~NetworkWorker()
	{
	}

	void NetworkWorker::createSocket()
	{
		mSocket.reset(new QSslSocket());

		mSocket->setProtocol(QSsl::TlsV1_1);

		connect(mSocket.data(), &QSslSocket::connected, this, &NetworkWorker::connected);
		connect(mSocket.data(), &QSslSocket::disconnected, this, &NetworkWorker::raiseConnectionClosed);

		connect(mSocket.data(), &QSslSocket::encrypted, this, &NetworkWorker::encrypted);
		connect(mSocket.data(), &QSslSocket::readyRead, this, &NetworkWorker::readyRead);

		connect(mSocket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(error(QAbstractSocket::SocketError)));

		connect(mSocket.data(), SIGNAL(sslErrors(QList<QSslError>)),
			this, SLOT(sslErrors(QList<QSslError>)));
	}

	bool NetworkWorker::isEncryptionEnabled() const
	{
		return mEncryptionEnabled;
	}

	void NetworkWorker::prepareEncryption()
	{
		if (mSocket == nullptr || !mEncryptionEnabled)
		{
			return;
		}

		if (mCaCertificates.count() > 0)
		{
			mSocket->setCaCertificates(mCaCertificates);
		}

		if (!mPrivateKey.isNull())
		{
			mSocket->setPrivateKey(mPrivateKey);
		}

		if (!mCertificate.isNull())
		{
			mSocket->setLocalCertificate(mCertificate);
		}
	}

	void NetworkWorker::sendData(const QByteArray& data)
	{
		if (mSocket->state() != QAbstractSocket::ConnectedState)
		{
			emit raiseError(tr("Socket error: Socket hasn't connected"), false);

			return;
		}

		mSocket->write(data);

		mSocket->flush();
	}

	void NetworkWorker::closeConnection()
	{
		mSocket->close();
	}

	void NetworkWorker::setEncryptionEnabled(const bool enabled)
	{
		mEncryptionEnabled = enabled;
	}

	void NetworkWorker::setCaCertificates(const QList<QSslCertificate>& certificates)
	{
		mCaCertificates = certificates;
	}

	void NetworkWorker::setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate)
	{
		mPrivateKey = privateKey;
		mCertificate = certificate;
	}

	void NetworkWorker::setIgnoredSslErrors(const QList<QSslError::SslError>& errors)
	{
		mIgnoredSslErrors = errors;
	}

	void NetworkWorker::receiverReady()
	{
		mReceiverReady = true;

		while (!mBufferQueue.isEmpty())
		{
			emit raiseDataReceived(mBufferQueue.dequeue());
		}
	}

	void NetworkWorker::connected()
	{
		if (!mEncryptionEnabled)
		{
			emit raiseConnectionEstablished();
		}
	}

	void NetworkWorker::encrypted()
	{
		emit raiseConnectionEstablished();
	}

	void NetworkWorker::readyRead()
	{
		while (mSocket->bytesAvailable() > 0)
		{
			if (mBytesExpected == 0)
			{
				QByteArray buffer = mSocket->read(sizeof(mBytesExpected));

				QDataStream stream(&buffer, QIODevice::ReadOnly);

				stream.setVersion(QDataStream::Qt_5_0);

				stream.readRawData(reinterpret_cast<char*>(&mBytesExpected), sizeof(mBytesExpected));
			}

			qint64 chunkSize = mSocket->bytesAvailable();

			chunkSize = qMin<qint64>(chunkSize, mBytesExpected - mBuffer.size());

			mBuffer.append(mSocket->read(chunkSize));

			if (mBytesExpected == (quint32)mBuffer.size())
			{
				if (mReceiverReady)
				{
					emit raiseDataReceived(mBuffer);
				}
				else
				{
					mBufferQueue.enqueue(mBuffer);
				}

				mBytesExpected = 0;
				mBuffer.clear();
			}
		}
	}

	void NetworkWorker::error(QAbstractSocket::SocketError socketError)
	{
		if (socketError == QAbstractSocket::RemoteHostClosedError)
		{
			return;
		}

		emit raiseError(tr("Socket error: %1").arg(static_cast<int>(socketError)), false);
	}

	void NetworkWorker::sslErrors(QList<QSslError> errors)
	{
		bool ignored = true;

		for (const QSslError& error : errors)
		{
			bool errorIgnored = mIgnoredSslErrors.contains(error.error());

			if (mIgnoredSslErrors.count() == 0)
			{
				return;
			}

			ignored = ignored && errorIgnored;

			emit raiseError(tr("SSL error: %1").arg(error.errorString()), errorIgnored);
		}

		if (mIgnoredSslErrors.count() != 0 && ignored)
		{
			mSocket->ignoreSslErrors(errors);
		}
	}
}
