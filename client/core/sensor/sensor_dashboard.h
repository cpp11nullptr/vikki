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

#ifndef VIKKI_SENSOR_DASHBOARD_H
#define VIKKI_SENSOR_DASHBOARD_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>

#include "../network/network_stream_in.h"

namespace Vikki
{
	class SensorDashboard
		: public QWidget
	{
		Q_OBJECT

	public:
		SensorDashboard(const QString& sensorName, const QString& sensorTitle);
		virtual ~SensorDashboard();

		void create();
		void destroy();

		QString sensorName() const;
		QString sensorTitle() const;

		virtual void sensorDataReceived(NetworkStreamInPointer stream) = 0;
		virtual void sensorDataUpdated(NetworkStreamInPointer stream) = 0;

		void showDashboardWidget();
		void hideDashboardWidget();

	protected:
		virtual QWidget* createDashboardWidget() = 0;

		QDateTimeEdit* periodFrom() const;
		QDateTimeEdit* periodTo() const;

	private:
		QString mSensorName;
		QString mSensorTitle;

		QDateTimeEdit *mPeriodFrom;
		QDateTimeEdit *mPeriodTo;

		QCheckBox *mAutoRefresh;
		QPushButton *mRefresh;

		QWidget *mDashboardWidget;
		QWidget *mDummyWidget;

		void refreshData();

	signals:
		void getSensorData(uint from, uint to);
		void subscribeSensorData(bool subscribe);

	private slots:
		void autoRefreshToggled(bool checked);
		void refreshClicked();

	};
}

#endif // VIKKI_SENSOR_DASHBOARD_H

