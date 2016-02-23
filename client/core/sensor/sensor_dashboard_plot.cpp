#include "sensor_dashboard_plot.h"

namespace Vikki
{
	SensorDashboardPlot::SensorDashboardPlot()
	{
		setup();
	}

	SensorDashboardPlot::~SensorDashboardPlot()
	{
	}

	void SensorDashboardPlot::setRanges(double timeFrom, double timeTo, double dataFrom, double dataTo)
	{
		xAxis->setRange(timeFrom, timeTo);
		yAxis->setRange(dataFrom, dataTo);

		replot();
	}

	QCPGraph* SensorDashboardPlot::createGraph(const QString& name, const QColor& color)
	{
		QCPGraph *graph = addGraph();

		graph->setName(name);
		graph->setLineStyle(QCPGraph::lsLine);
		graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
		graph->setPen(QPen(QBrush(color), 1));

		return graph;
	}

	void SensorDashboardPlot::setup()
	{
		setNotAntialiasedElement(QCP::aeAll);
		setNoAntialiasingOnDrag(true);

		xAxis->grid()->setAntialiased(false);
		yAxis->grid()->setAntialiased(false);

		xAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));
		yAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));

		xAxis->grid()->setSubGridPen(QPen(QColor(160, 160, 160), 1, Qt::DotLine));
		yAxis->grid()->setSubGridPen(QPen(QColor(160, 160, 160), 1, Qt::DotLine));

		xAxis->grid()->setSubGridVisible(true);
		yAxis->grid()->setSubGridVisible(true);

		xAxis->setLabel("Timeline");
		yAxis->setLabel("Data");

		xAxis->setDateTimeFormat("HH:mm:ss\ndd/MM/yyyy");

		xAxis->setTickLabelType(QCPAxis::ltDateTime);
		xAxis->setTickStep(60);

		axisRect()->setRangeDragAxes(xAxis, nullptr);
		axisRect()->setRangeZoomAxes(xAxis, nullptr);

		setInteraction(QCP::iRangeDrag, true);
		setInteraction(QCP::iRangeZoom, true);

		legend->setVisible(true);
	}
}
