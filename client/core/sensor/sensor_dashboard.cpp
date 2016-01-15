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
		: mSensorName(sensorName), mSensorTitle(sensorTitle)
	{
	}

	SensorDashboard::~SensorDashboard()
	{
	}

	void SensorDashboard::create()
	{
		mPeriodFrom = new QDateTimeEdit();
		mPeriodFrom->setDateTime(QDateTime::currentDateTime().addSecs(-60 * 60));
		mPeriodFrom->setDisplayFormat("MM/dd/yyyy HH:mm:ss");
		mPeriodFrom->setCalendarPopup(true);

		connect(mPeriodFrom, &QDateTimeEdit::dateTimeChanged, this, &SensorDashboard::periodFromChanged);

		mPeriodTo = new QDateTimeEdit();
		mPeriodTo->setDateTime(QDateTime::currentDateTime());
		mPeriodTo->setDisplayFormat("MM/dd/yyyy HH:mm:ss");
		mPeriodTo->setCalendarPopup(true);

		connect(mPeriodTo, &QDateTimeEdit::dateTimeChanged, this, &SensorDashboard::periodToChanged);

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

		mShowLegend = new QCheckBox(tr("Show legend"));
		mShowLegend->setChecked(true);

		connect(mShowLegend, &QCheckBox::toggled, this, &SensorDashboard::showLegendToggled);

		QHBoxLayout *optionsLayout = new QHBoxLayout();
		optionsLayout->addWidget(mShowLegend);
		optionsLayout->addStretch();

		mPlot = new QCustomPlot();
		mPlot->setNotAntialiasedElement(QCP::aeAll);
		mPlot->setNoAntialiasingOnDrag(true);

		connect(mPlot, &QCustomPlot::mouseWheel, this, &SensorDashboard::plotMouseWheel);

		mPlot->xAxis->grid()->setAntialiased(false);
		mPlot->yAxis->grid()->setAntialiased(false);

		mPlot->xAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));
		mPlot->yAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));
		mPlot->xAxis->grid()->setSubGridPen(QPen(QColor(160, 160, 160), 1, Qt::DotLine));
		mPlot->yAxis->grid()->setSubGridPen(QPen(QColor(160, 160, 160), 1, Qt::DotLine));
		mPlot->xAxis->grid()->setSubGridVisible(true);
		mPlot->yAxis->grid()->setSubGridVisible(true);

		mPlot->xAxis->setLabel("Timeline");
		mPlot->yAxis->setLabel("Data");

		mPlot->xAxis->setDateTimeFormat("HH:mm:ss\nMM/dd/yyyy");
		mPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
		mPlot->xAxis->setTickStep(60);

		mPlot->axisRect()->setRangeDragAxes(mPlot->xAxis, nullptr);
		mPlot->axisRect()->setRangeZoomAxes(mPlot->xAxis, nullptr);

		mPlot->setInteraction(QCP::iRangeDrag, true);
		mPlot->setInteraction(QCP::iRangeZoom, true);

		mPlot->legend->setVisible(true);

		QVBoxLayout *dashboardLayout = new QVBoxLayout();
		dashboardLayout->addLayout(periodLayout);
		dashboardLayout->addLayout(optionsLayout);
		dashboardLayout->addWidget(mPlot);

		QGroupBox *dashboardGroupBox = new QGroupBox(tr("Dashboard"));
		dashboardGroupBox->setLayout(dashboardLayout);

		QVBoxLayout *layout = new QVBoxLayout();
		layout->addWidget(dashboardGroupBox);

		setLayout(layout);

		createEvent();

		refreshClicked();
	}

	void SensorDashboard::destroy()
	{
		destroyEvent();
	}

	QString SensorDashboard::sensorName() const
	{
		return mSensorName;
	}

	QString SensorDashboard::sensorTitle() const
	{
		return mSensorTitle;
	}

	void SensorDashboard::createEvent()
	{
	}

	void SensorDashboard::destroyEvent()
	{
	}

	QCustomPlot* SensorDashboard::plot() const
	{
		return mPlot;
	}

	QDateTimeEdit* SensorDashboard::periodFrom() const
	{
		return mPeriodFrom;
	}

	QDateTimeEdit* SensorDashboard::periodTo() const
	{
		return mPeriodTo;
	}

	QCPGraph* SensorDashboard::createGraph(const QString& name, const QColor& color)
	{
		QPen pen;
		pen.setBrush(color);

		QCPGraph *graph = mPlot->addGraph();
		graph->setName(name);
		graph->setLineStyle(QCPGraph::lsLine);
		graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
		graph->setPen(pen);

		return graph;
	}

	void SensorDashboard::updatePlot(double yLower, double yUpper)
	{
		double timeFrom = mPeriodFrom->dateTime().toTime_t();
		double timeTo = mPeriodTo->dateTime().toTime_t();

		mPlot->xAxis->setRange(timeFrom, timeTo);
		mPlot->yAxis->setRange(yLower, yUpper);

		mPlot->replot();
	}

	void SensorDashboard::refreshData()
	{
		uint from = mPeriodFrom->dateTime().toTime_t();
		uint to = mPeriodTo->dateTime().toTime_t();

		emit getSensorData(from, to);
	}

	void SensorDashboard::plotMouseWheel(QWheelEvent *event)
	{
		Q_UNUSED(event);
	}

	void SensorDashboard::periodFromChanged(const QDateTime& dateTime)
	{
		Q_UNUSED(dateTime);
	}

	void SensorDashboard::periodToChanged(const QDateTime& dateTime)
	{
		Q_UNUSED(dateTime);
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

	void SensorDashboard::showLegendToggled(bool checked)
	{
		mPlot->legend->setVisible(checked);

		mPlot->replot();
	}
}
