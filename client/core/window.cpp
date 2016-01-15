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

#include "window.h"

namespace Vikki
{
	Window::Window()
	{				
		createServerTreeDock();
	}

	Window::~Window()
	{
	}

	void Window::installSensors(QVector<QSharedPointer<SensorPlugin>> sensors)
	{
		mSensors = sensors;
	}

	void Window::installServers(QVector<ServerData> servers)
	{
		for (const ServerData& serverData : servers)
		{
			QTreeWidgetItem *serverItem = new QTreeWidgetItem();

			QFont serverItemFont = serverItem->font(0);
			serverItemFont.setBold(true);

			serverItem->setFont(0, serverItemFont);

			serverItem->setText(0, serverData.title);
			serverItem->setToolTip(0, QString("%1:%2").arg(serverData.host, QString::number(serverData.port)));

			serverItem->setData(0, ITEM_TYPE_ROLE, "server");
			serverItem->setData(0, SERVER_HOST_ROLE, serverData.host);
			serverItem->setIcon(0, QIcon(":/icons/server"));

			mServerTree->addTopLevelItem(serverItem);

			prepareServer(serverItem, serverData);
		}

		mServerTree->expandAll();
	}

	void Window::createServerTreeDock()
	{
		mServerTree = new QTreeWidget();
		mServerTree->header()->hide();

		connect(mServerTree, &QTreeWidget::currentItemChanged, this, &Window::currentServerSensorChanged);

		QVBoxLayout *layout = new QVBoxLayout();
		layout->addWidget(mServerTree);

		QWidget *widget = new QWidget();
		widget->setLayout(layout);

		mServerTreeDock = new QDockWidget();
		mServerTreeDock->setObjectName("servers");
		mServerTreeDock->setWindowTitle(tr("Servers"));
		mServerTreeDock->setWidget(widget);

		addDockWidget(Qt::LeftDockWidgetArea, mServerTreeDock);
	}

	void Window::prepareServer(QTreeWidgetItem *serverItem, const ServerData& serverData)
	{
		QSharedPointer<Client> client = createServerClient(serverData);

		connect(client.data(), &Client::raiseConnected, [this, serverItem](NetworkConnectionPointer connection)
		{
			QString serverHost = serverItem->data(0, Qt::UserRole + 1).toString();
			mServers[serverHost].active = true;

			auto callback = [this, serverItem](NetworkStreamInPointer stream)
			{
				quint64 sensorCount = stream->readUInt64();

				for (quint64 i = 0; i < sensorCount; ++i)
				{
					QString sensorName = stream->readString();

					addServerSensor(serverItem, sensorName);
				}
			};

			connection->createStream(Command::GET_SENSOR_LIST, callback);
		});

		connect(client.data(), &Client::raiseDisconnected, [this, serverData, serverItem](NetworkConnectionPointer connection)
		{
			Q_UNUSED(connection);

			while (serverItem->childCount() > 0)
			{
				serverItem->removeChild(serverItem->child(0));
			}

			QString errorMessage = tr("Connection to %1 closed.").arg(serverData.host);

			QMessageBox messageBox(QMessageBox::Critical, appName, errorMessage);
			messageBox.exec();
		});

		if (serverData.security.enable)
		{
			if (serverData.security.type == "ssl")
			{
				QList<QSslError::SslError> ignoredSslErrors;
				ignoredSslErrors.push_back(QSslError::HostNameMismatch);
				ignoredSslErrors.push_back(QSslError::SelfSignedCertificateInChain);

				QMap<QString, QString> params = serverData.security.params;

				QSslKey privateKey;
				if (params.contains("private_key"))
				{
					privateKey = loadPrivateKey(serverData.host, params["private_key"]);
				}

				QSslCertificate certificate;
				if (params.contains("cert"))
				{
					certificate = loadCertificate(serverData.host, params["cert"]);
				}

				QList<QSslCertificate> caCertificates;
				if (params.contains("ca"))
				{
					caCertificates << loadCertificate(serverData.host, params["ca"]);
				}

				client->setCaCertificates(caCertificates);
				client->setEncryption(privateKey, certificate);
				client->setIgnoredSslErrors(ignoredSslErrors);
				client->setEncryptionEnabled(true);
			}
			else
			{
				QString errorMessage = tr("Unsupported network security type \"%1\" for server %2.").arg(serverData.security.type, serverData.host);

				QMessageBox messageBox(QMessageBox::Critical, appName, errorMessage);
				messageBox.exec();
			}
		}

		client->start(serverData.host, serverData.port);

		ServerInfo serverInfo;

		serverInfo.active = false;
		serverInfo.data = serverData;
		serverInfo.client = client;

		mServers[serverData.host] = serverInfo;
	}

	QSharedPointer<Client> Window::createServerClient(const ServerData& serverData)
	{
		QSharedPointer<Client> client = QSharedPointer<Client>(new Client());

		connect(client.data(), &Client::raiseError, [this, serverData](NetworkConnectionPointer connection, const QString& message, const bool ignored)
		{
			Q_UNUSED(connection);
			Q_UNUSED(message);

			if (!ignored)
			{
				mServers[serverData.host].active = false;

				QString errorMessage = tr("Can't connect to %1:%2.").arg(serverData.host, QString::number(serverData.port));

				QMessageBox messageBox(QMessageBox::Critical, appName, errorMessage);
				messageBox.exec();
			}
		});

		return client;
	}

	void Window::addServerSensor(QTreeWidgetItem *serverItem, const QString& sensorName)
	{
		QSharedPointer<SensorPlugin> sensor = sensorByName(sensorName);

		if (sensor == nullptr)
		{
			return;
		}

		QTreeWidgetItem *sensorItem = new QTreeWidgetItem(serverItem);

		QFont sensorItemFont = sensorItem->font(0);
		sensorItemFont.setItalic(true);

		sensorItem->setFont(0, sensorItemFont);
		sensorItem->setText(0, sensor->title());
		sensorItem->setIcon(0, sensor->icon());

		sensorItem->setData(0, ITEM_TYPE_ROLE, "sensor");
		sensorItem->setData(0, SENSOR_NAME_ROLE, sensor->name());
	}

	QSharedPointer<SensorPlugin> Window::sensorByName(const QString& name)
	{
		QSharedPointer<SensorPlugin> sensor;

		for (QSharedPointer<SensorPlugin> sensorPlugin : mSensors)
		{
			if (sensorPlugin->name() == name)
			{
				sensor = sensorPlugin;

				break;
			}
		}

		return sensor;
	}

	QSslKey Window::loadPrivateKey(const QString& serverHost, const QString& fileName) const
	{
		QFile file(fileName);

		if (!file.open(QIODevice::ReadOnly))
		{
			QString errorMessage = tr("Can't load private key %1 for server %2.").arg(fileName, serverHost);

			QMessageBox messageBox(QMessageBox::Critical, appName, errorMessage);
			messageBox.exec();

			return QSslKey();
		}

		return QSslKey(&file, QSsl::Rsa);
	}

	QSslCertificate Window::loadCertificate(const QString& serverHost, const QString& fileName) const
	{
		QFile file(fileName);

		if (!file.open(QIODevice::ReadOnly))
		{
			QString errorMessage = tr("Can't load certificate %1 for server %2.").arg(fileName, serverHost);

			QMessageBox messageBox(QMessageBox::Critical, appName, errorMessage);
			messageBox.exec();

			return QSslCertificate();
		}

		return QSslCertificate(&file);
	}

	void Window::currentServerSensorChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
	{
		Q_UNUSED(previous);

		mSensorClientProxy.clear();

		QWidget *dummyCentralWidget = new QFrame();
		dummyCentralWidget->showMaximized();

		setCentralWidget(dummyCentralWidget);

		if (current == nullptr)
		{
			return;
		}

		QTreeWidgetItem *item = current;

		while (item->data(0, ITEM_TYPE_ROLE).toString() != "server")
		{
			item = item->parent();

			if (item == nullptr)
			{
				return;
			}
		}

		QString serverHost = item->data(0, SERVER_HOST_ROLE).toString();

		QSharedPointer<Client> client = mServers[serverHost].client;

		if (current->data(0, ITEM_TYPE_ROLE).toString() != "sensor")
		{
			return;
		}

		QString sensorName = current->data(0, SENSOR_NAME_ROLE).toString();
		QSharedPointer<SensorPlugin> sensor = sensorByName(sensorName);

		SensorDashboard *sensorDashboard = sensor->createDashboard();
		sensorDashboard->showMaximized();

		mSensorClientProxy = QSharedPointer<SensorClientProxy>(new SensorClientProxy(sensorDashboard, client));

		setCentralWidget(sensorDashboard);
	}
}
