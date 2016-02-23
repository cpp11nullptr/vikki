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

#include "sensor_dashboard.h"

namespace Vikki
{
	SensorDashboard::SensorDashboard(const QString& sensorName, const QString& sensorTitle)
		: mSensorName(sensorName), mSensorTitle(sensorTitle), mDashboardWidget(nullptr)
	{
	}

	SensorDashboard::~SensorDashboard()
	{
	}

	void SensorDashboard::create()
	{
		mPeriodFrom = new QDateTimeEdit();
		mPeriodFrom->setDateTime(QDateTime::currentDateTime().addSecs(-60 * 60));
		mPeriodFrom->setDisplayFormat("dd/MM/yyyy HH:mm:ss");
		mPeriodFrom->setCalendarPopup(true);

		mPeriodTo = new QDateTimeEdit();
		mPeriodTo->setDateTime(QDateTime::currentDateTime());
		mPeriodTo->setDisplayFormat("dd/MM/yyyy HH:mm:ss");
		mPeriodTo->setCalendarPopup(true);

		mAutoRefresh = new QCheckBox(tr("Auto"));

		connect(mAutoRefresh, &QCheckBox::toggled, this, &SensorDashboard::autoRefreshToggled);

		mRefresh = new QPushButton(tr("Refresh"));

		connect(mRefresh, &QPushButton::clicked, this, &SensorDashboard::refreshClicked);

		QHBoxLayout *periodLayout = new QHBoxLayout();
		periodLayout->addWidget(new QLabel(tr("From")));
		periodLayout->addWidget(mPeriodFrom);
		periodLayout->addWidget(new QLabel(" to "));
		periodLayout->addWidget(mPeriodTo);
		periodLayout->addStretch();
		periodLayout->addWidget(mAutoRefresh);
		periodLayout->addWidget(mRefresh);

		QVBoxLayout *dashboardLayout = new QVBoxLayout();
		dashboardLayout->addLayout(periodLayout);

		mDashboardWidget = createDashboardWidget();

		mDummyWidget = new QWidget();
		mDummyWidget->setVisible(false);

		dashboardLayout->addWidget(mDashboardWidget);
		dashboardLayout->addWidget(mDummyWidget);

		QGroupBox *dashboardGroupBox = new QGroupBox(tr("Dashboard"));
		dashboardGroupBox->setLayout(dashboardLayout);

		QVBoxLayout *layout = new QVBoxLayout();
		layout->addWidget(dashboardGroupBox);

		setLayout(layout);

		refreshClicked();
	}

	void SensorDashboard::destroy()
	{
	}

	QString SensorDashboard::sensorName() const
	{
		return mSensorName;
	}

	QString SensorDashboard::sensorTitle() const
	{
		return mSensorTitle;
	}

	void SensorDashboard::showDashboardWidget()
	{
		mDashboardWidget->setVisible(true);
		mDummyWidget->setVisible(false);
	}

	void SensorDashboard::hideDashboardWidget()
	{
		mDashboardWidget->setVisible(false);
		mDummyWidget->setVisible(true);
	}

	QDateTimeEdit* SensorDashboard::periodFrom() const
	{
		return mPeriodFrom;
	}

	QDateTimeEdit* SensorDashboard::periodTo() const
	{
		return mPeriodTo;
	}

	void SensorDashboard::refreshData()
	{
		uint from = mPeriodFrom->dateTime().toTime_t();
		uint to = mPeriodTo->dateTime().toTime_t();

		emit getSensorData(from, to);
	}

	void SensorDashboard::autoRefreshToggled(bool checked)
	{
		uint time = QDateTime::currentDateTime().toTime_t();

		uint timeDiff = periodTo()->dateTime().toTime_t() - periodFrom()->dateTime().toTime_t();
		timeDiff = qAbs(timeDiff);

		mPeriodFrom->setDateTime(QDateTime::fromTime_t(time - timeDiff));
		mPeriodTo->setDateTime(QDateTime::fromTime_t(time));

		refreshData();

		emit subscribeSensorData(checked);
	}

	void SensorDashboard::refreshClicked()
	{
		mAutoRefresh->setChecked(false);

		emit subscribeSensorData(false);

		refreshData();
	}
}
