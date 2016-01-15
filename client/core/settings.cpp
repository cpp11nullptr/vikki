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

#include "settings.h"

namespace Vikki
{
	Settings::Settings(const QString& fileName)
	{
		QFile file(fileName);

		if (!file.open(QIODevice::ReadOnly))
		{
			mLastError = QObject::tr("unable opening file.");

			return;
		}

		QByteArray fileData = file.readAll();

		QJsonParseError parseError;
		QJsonObject root = QJsonDocument::fromJson(fileData, &parseError).object();

		if (parseError.error != QJsonParseError::NoError)
		{
			mLastError = parseError.errorString() + "!";

			return;
		}

		loadServers(root["servers"]);
	}

	Settings::~Settings()
	{
	}

	QString	Settings::lastError() const
	{
		return mLastError;
	}

	QVector<ServerData> Settings::servers() const
	{
		return mServers;
	}

	void Settings::loadServers(const QJsonValue& object)
	{
		QSet<QString> serverHosts;

		for (const QJsonValue& server : object.toArray())
		{
			ServerData item;

			item.title = server.toObject()["title"].toString();
			item.host = server.toObject()["host"].toString();
			item.port = server.toObject()["port"].toInt();

			item.security.enable = false;

			const QJsonValue& security = server.toObject()["security"];

			if (!security.isNull())
			{
				item.security.enable = security.toObject()["enable"].toBool();
				item.security.type = security.toObject()["type"].toString();

				const QJsonObject& securityParams = security.toObject()["params"].toObject();

				for (const QString& paramName : securityParams.keys())
				{
					item.security.params[paramName] = securityParams[paramName].toString();
				}
			}

			if (serverHosts.find(item.host) != serverHosts.end())
			{
				mLastError = QObject::tr("dublicate server host %1").arg(item.host);

				break;
			}

			serverHosts.insert(item.host);

			mServers.push_back(item);
		}
	}
}
