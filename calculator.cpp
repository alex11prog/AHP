#include "calculator.h"
Calculator::Calculator(QVector <double> criterionsWeights,
                       QVector <QString> variants,
                       QVector <QVector <double>> grades)
{
    this->criterionsWeights = criterionsWeights;
    this->variants = variants;
    this->grades = grades;

    //нормализуем веса критериев
    {
        double sum = 0.0;
        for(double cw : criterionsWeights)
            sum+= cw;
        for(int i = 0; i < criterionsWeights.length(); i++)
            criterionsWeights[i] /= sum;
    }
}

QVector<rowRec> Calculator::getCalculateRes()
{
    const int CNT_CRITERIONS = criterionsWeights.size();
    criterions.resize(CNT_CRITERIONS);
    //заполняем данные по критериям
    for(int indexCr = 0; indexCr < CNT_CRITERIONS; indexCr++)
        for(auto variantGrades : grades)
            criterions[indexCr].grade_variants.push_back(variantGrades[indexCr]);

    const int CNT_VARIANTS = variants.size();
    //calculate_mps
    for(criterion &crit : criterions)
    {
        crit.mps.resize(CNT_VARIANTS);
        crit.vkp.resize(CNT_VARIANTS);
        crit.vkp_normal.resize(CNT_VARIANTS);
        double vkp_summ = 0.0;
        for (int num_var = 0; num_var < CNT_VARIANTS; num_var++) {
            crit.mps[num_var].resize(CNT_VARIANTS);
            double product = 1.0;
            for (int num_column = 0; num_column < CNT_VARIANTS; num_column++) {
                crit.mps[num_var][num_column] =
                     crit.grade_variants[num_var] / crit.grade_variants[num_column];
                product *= crit.mps[num_var][num_column];
            }
            crit.vkp[num_var] = pow(product, 1.0 / (double)CNT_VARIANTS);
            vkp_summ += crit.vkp[num_var];
        }
        for (int num_var = 0; num_var < CNT_VARIANTS; num_var++)
            crit.vkp_normal[num_var] = crit.vkp[num_var] / vkp_summ;
    }

    //calculate_AHP+
    QVector <QVector <QVector <QVector <double>>>> table_B(CNT_CRITERIONS, QVector <QVector <QVector <double>>>(CNT_VARIANTS, QVector <QVector <double>>(CNT_VARIANTS, QVector <double>(2))));
    QVector <QVector <QVector <double>>> table_W (CNT_VARIANTS, QVector <QVector <double>>(CNT_VARIANTS, QVector <double>(2)));

    double temp_summ;
    // заполняем таблицу B
    for (int num_cr = 0; num_cr < CNT_CRITERIONS; num_cr++) {
        for (int num_var = 0; num_var < CNT_VARIANTS; num_var++) {
            for (int num_column = 0; num_column < CNT_VARIANTS; num_column++) {
                if (num_var == num_column) {
                    table_B[num_cr][num_var][num_column][0] = 1.0;
                    table_B[num_cr][num_var][num_column][1] = 1.0;
                }
                else {
                    table_B[num_cr][num_var][num_column][0] = criterions[num_cr].vkp_normal[num_var];
                    table_B[num_cr][num_var][num_column][1] = criterions[num_cr].vkp_normal[num_column];
                }
                temp_summ = table_B[num_cr][num_var][num_column][0] + table_B[num_cr][num_var][num_column][1];
                table_B[num_cr][num_var][num_column][0] /= temp_summ;
                table_B[num_cr][num_var][num_column][1] /= temp_summ;
            }
        }
    }

    //заполняем таблицу W
    for (int num_var = 0; num_var < CNT_VARIANTS; num_var++) {
        for (int num_column = 0; num_column < CNT_VARIANTS; num_column++) {
            for (int i = 0; i < 2; i++) {
                for (int num_cr = 0; num_cr < CNT_CRITERIONS; num_cr++) {
                    table_W[num_var][num_column][i] += table_B[num_cr][num_var][num_column][i] * criterionsWeights[num_cr];
                }
            }
        }
    }

    QVector <double> alter_weights(CNT_VARIANTS);

    for (int num_var = 0; num_var < CNT_VARIANTS; num_var++) {
        for (int num_column = 0; num_column < CNT_VARIANTS; num_column++) {
            alter_weights[num_var] += table_W[num_var][num_column][0];
        }
    }
    double tempSum = 0.0;
    for (int num_var = 0; num_var < CNT_VARIANTS; num_var++) {
        tempSum+= alter_weights[num_var];
    }

    for (int num_var = 0; num_var < CNT_VARIANTS; num_var++) {
        alter_weights[num_var] /= tempSum;
    }

    QVector<rowRec> res;
    for (int num_var = 0; num_var < CNT_VARIANTS; num_var++) {
        res.push_back(rowRec(variants[num_var], alter_weights[num_var]));
    }
    std::sort(res.begin(), res.end(), [](const rowRec& a, const rowRec& b) { return a.weight > b.weight; });
    return res;
}

rowRec::rowRec(QString name, double weight)
{
    this->name = name;
    this->weight = weight;
}
