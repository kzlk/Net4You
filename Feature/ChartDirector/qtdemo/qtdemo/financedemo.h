#ifndef FINANCEDEMO_H
#define FINANCEDEMO_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QButtonGroup>
#include "qchartviewer.h"
#include <vector>

class FinanceDemo : public QDialog
{
    Q_OBJECT
public:
    FinanceDemo(QWidget *parent = 0);
    ~FinanceDemo();

private:
    // The main ticker key and the compare to ticker key
    QString m_tickerKey;
    QString m_compareKey;

    struct PriceData
    {
        std::vector<double> timeStamps;
        std::vector<double> highData;
        std::vector<double> lowData;
        std::vector<double> openData;
        std::vector<double> closeData;
        std::vector<double> volData;

        // An extra field for price comparison
        std::vector<double> compareData;
    };

    // In this example, we can use daily, weekly or monthly data depending on the time range
    PriceData m_dailyPrice;
    PriceData m_weeklyPrice;
    PriceData m_monthlyPrice;

    void aggregateData(const ArrayMath& aggregator, PriceData& p);

    // The moving average periods
    int m_avgPeriod1;
    int m_avgPeriod2;

    // Routines to get data into the data arrays
    void loadData(const QString& ticker, const QString& compare);

    // The user interface
    QButtonGroup *mouseUsage;
    QLineEdit *m_TickerSymbol;
    QLineEdit *m_CompareWith;
    QComboBox *m_TimeRange;
    QCheckBox *m_VolumeBars;
    QCheckBox *m_ParabolicSAR;
    QCheckBox *m_LogScale;
    QCheckBox *m_PercentageScale;
    QComboBox *m_ChartType;
    QComboBox *m_PriceBand;
    QComboBox *m_AvgType1;
    QComboBox *m_AvgType2;
    QLineEdit *m_MovAvg1;
    QLineEdit *m_MovAvg2;
    QComboBox *m_Indicator1;
    QComboBox *m_Indicator2;
    QChartViewer *m_ChartViewer;

    void drawChart(QChartViewer *viewer);
    void trackFinance(MultiChart* m, int mouseX);

    // Utility to convert a std::vector to a DoubleArray
    DoubleArray vectorToArray(const std::vector<double>& v, int startIndex = 0, int length = -1);
    // utility to convert std::vector to a DoubleArray
    std::vector<double> arrayToVector(DoubleArray a);

private slots:
    void onMouseUsageChanged(QAbstractButton *b);
    void onComboBoxChanged(int);
    void onCheckBoxChanged();
    void onLineEditChanged();
    void onMouseMovePlotArea(QMouseEvent*);
    void onTimeRangeChanged(int);
    void onViewPortChanged();
};

#endif // FINANCEDEMO_H
