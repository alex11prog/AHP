#ifndef CALCULATOR_H
#define CALCULATOR_H
#include <QVector>
#include <QString>

struct criterion {
    QVector <double> grade_variants;
    QVector <QVector <double>> mps;
    QVector <double> vkp;
    QVector <double> vkp_normal;
};

struct rowRec{
    QString name;
    double weight;
    rowRec(QString name, double weight);
};

class Calculator
{
private:
    QVector <double> criterionsWeights;
    QVector <QString> variants;
    QVector <QVector <double>> grades;
    QVector <criterion> criterions;

public:
    Calculator(QVector <double> criterionsWeights,
               QVector <QString> variants,
               QVector <QVector <double>> grades);
    QVector <rowRec> getCalculateRes();
};

#endif // CALCULATOR_H
