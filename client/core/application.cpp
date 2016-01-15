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

#include "application.h"

namespace Vikki
{
	Application::Application(int& argc, char *argv[])
		: mApp(argc, argv)
	{
		QApplication::setOrganizationName(orgName);
		QApplication::setOrganizationDomain(orgDomain);
		QApplication::setApplicationName(appName);
		QApplication::setApplicationVersion(appVersion);
	}

	Application::~Application()
	{
	}

	int Application::run()
	{
		if (!loadSettings())
		{
			return 1;
		}

		loadSensors();
		createWindow();

		return mApp.exec();
	}

	bool Application::loadSettings()
	{
		mSettings = QSharedPointer<Settings>(new Settings("vikki-client.cfg"));

		if (!mSettings->lastError().isEmpty())
		{
			QMessageBox messageBox(QMessageBox::Critical, appName, tr("Error load config: %1.").arg(mSettings->lastError()));
			messageBox.exec();

			return false;
		}

		return true;
	}

	void Application::loadSensors()
	{
		QDir dir(QDir::currentPath());
		dir.cd("sensors");

		QStringList fileNameFilters;

#if defined(VIKKI_PLATFORM_UNIX)
		fileNameFilters.push_back("lib*.so");
#elif defined(VIKKI_PLATFORM_WIN32)
		fileNameFilters.push_back("*.dll");
#endif

		QStringList files = dir.entryList(fileNameFilters, QDir::Files);

		for (const QString& fileName : files)
		{
			QString fileFullName = dir.absoluteFilePath(fileName);

			QPluginLoader pluginLoader(fileFullName);

			QObject *pluginInstance = pluginLoader.instance();

			if (pluginInstance == nullptr)
			{
				QString message = tr("can't load sensor plugin ");
				message += fileFullName;
				message += ": " + pluginLoader.errorString();

				QMessageBox messageBox(QMessageBox::Critical, appName, tr("Load sensor: %1").arg(message));
				messageBox.exec();

				continue;
			}

			QSharedPointer<SensorPlugin> sensorPlugin = QSharedPointer<SensorPlugin>(qobject_cast<SensorPlugin*>(pluginInstance));

			if (sensorPlugin == nullptr)
			{
				QString message = tr("can't instantiate sensor plugin ");
				message += fileFullName;

				QMessageBox messageBox(QMessageBox::Critical, appName, tr("Load sensor: %1").arg(message));
				messageBox.exec();

				continue;
			}

			mSensors.push_back(sensorPlugin);
		}
	}

	void Application::createWindow()
	{
		mWindow = QSharedPointer<Window>(new Window());

		mWindow->setWindowTitle(appName + " " + appVersion);

		mWindow->installSensors(mSensors);
		mWindow->installServers(mSettings->servers());

		mWindow->showMaximized();
	}
}
