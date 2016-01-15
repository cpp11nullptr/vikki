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

#include "network_connection.h"

namespace Vikki
{
	NetworkConnection::NetworkConnection()
		: mEventIdCounter(1)
	{
	}

	NetworkConnection::~NetworkConnection()
	{
	}

	void NetworkConnection::receiverReady()
	{
		emit raiseReceiverReady();
	}

	void NetworkConnection::closeConnection()
	{
		emit raiseCloseConnection();
	}

	void NetworkConnection::removeResponseCallbacks()
	{
		mResponseCallbacks.clear();
	}

	void NetworkConnection::dataReceived(const QByteArray& data)
	{
		NetworkStreamInPointer stream = NetworkStreamInPointer(new NetworkStreamIn(data));

		connect(stream.data(), &NetworkStreamIn::raiseSendResponseData, this, &NetworkConnection::raiseSendData);

		if (mResponseCallbacks.contains(stream->eventId()))
		{
			NetworkResponseCallback responseCallback = mResponseCallbacks[stream->eventId()];

			mResponseCallbacks.remove(stream->eventId());

			responseCallback(stream);
		}
		else
		{
			emit raiseDataReceived(stream);
		}
	}
}
