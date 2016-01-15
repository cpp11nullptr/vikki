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

#include "../plot/qcustomplot.h"
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

	protected:
		virtual void createEvent();
		virtual void destroyEvent();

		QCustomPlot* plot() const;

		QDateTimeEdit* periodFrom() const;
		QDateTimeEdit* periodTo() const;

		QCPGraph* createGraph(const QString& name, const QColor& color);
		void updatePlot(double yLower, double yUpper);

	private:
		QString mSensorName;
		QString mSensorTitle;

		QDateTimeEdit *mPeriodFrom;
		QDateTimeEdit *mPeriodTo;

		QCheckBox *mAutoRefresh;
		QPushButton *mRefresh;
		QCheckBox *mShowLegend;

		QCustomPlot *mPlot;

		void refreshData();

	signals:
		void getSensorData(uint from, uint to);
		void subscribeSensorData(bool subscribe);

	private slots:
		void plotMouseWheel(QWheelEvent *event);

		void periodFromChanged(const QDateTime& dateTime);
		void periodToChanged(const QDateTime& dateTime);

		void autoRefreshToggled(bool checked);
		void refreshClicked();

		void showLegendToggled(bool checked);

	};
}

#endif // VIKKI_SENSOR_DASHBOARD_H

