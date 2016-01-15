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

#ifndef VIKKI_WINDOW_H
#define VIKKI_WINDOW_H

#include <functional>

#include "config.h"
#include "settings.h"
#include "command.h"

#include "plot/qcustomplot.h"
#include "network/client.h"

#include "sensor/sensor_plugin.h"
#include "sensor/sensor_client_proxy.h"

#include <QVector>
#include <QMap>
#include <QLabel>
#include <QComboBox>
#include <QTreeWidget>
#include <QDateTimeEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QListWidget>
#include <QMainWindow>

namespace Vikki
{
	class Window
		: public QMainWindow
	{
		Q_OBJECT
		Q_DISABLE_COPY(Window)

	public:
		enum ItemRoles
		{
			ITEM_TYPE_ROLE		= Qt::UserRole,
			SERVER_HOST_ROLE	= Qt::UserRole + 1,
			SENSOR_NAME_ROLE	= Qt::UserRole + 1
		};

		Window();
		~Window() override;

		void installSensors(QVector<QSharedPointer<SensorPlugin>> sensors);
		void installServers(QVector<ServerData> servers);

	private:		
		struct ServerInfo
		{
			bool active;
			ServerData data;
			QSharedPointer<Client> client;
		};

		QTreeWidget *mServerTree;
		QDockWidget *mServerTreeDock;

		void createServerTreeDock();

		QVector<QSharedPointer<SensorPlugin>> mSensors;
		QMap<QString, ServerInfo> mServers;

		void prepareServer(QTreeWidgetItem *serverItem, const ServerData& serverData);
		QSharedPointer<Client> createServerClient(const ServerData& serverData);
		void addServerSensor(QTreeWidgetItem *serverItem, const QString& sensorName);

		QSharedPointer<SensorPlugin> sensorByName(const QString& name);

		QSslKey loadPrivateKey(const QString& serverHost, const QString& fileName) const;
		QSslCertificate loadCertificate(const QString& serverHost, const QString& fileName) const;

		QSharedPointer<SensorClientProxy> mSensorClientProxy;

	private slots:
		void currentServerSensorChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	};
}

#endif // VIKKI_WINDOW_H

