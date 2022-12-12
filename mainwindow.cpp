#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include "calculator.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Метод принятия решений AHP");
    ui->tableWidgetVariants->setColumnCount(1); // Указываем число колонок
    ui->tableWidgetVariants->setShowGrid(true); // Включаем сетку
    ui->tableWidgetVariants->setHorizontalHeaderLabels(QStringList() << "Вариант");
    ui->tableWidgetVariants->horizontalHeader()->setStretchLastSection(true);   // Растягиваем на всё доступное пространство
    ui->tableWidgetCriterions->setColumnCount(2); // Указываем число колонок
    ui->tableWidgetCriterions->setShowGrid(true); // Включаем сетку
    ui->tableWidgetCriterions->setHorizontalHeaderLabels(QStringList() << "Критерий" << "Вес от 1 до 9");
    ui->tableWidgetCriterions->horizontalHeader()->setStretchLastSection(true); // Растягиваем на всё доступное пространство
    ui->tableWidgetRes->setColumnCount(2); // Указываем число колонок;
    ui->tableWidgetRes->setHorizontalHeaderLabels(QStringList() << "Вариант" << "Итоговый вес %");
    ui->tableWidgetRes->resizeColumnsToContents();
    ui->tableWidgetRes->setEditTriggers(QTableWidget::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_addVariant_clicked()
{
    ui->tableWidgetVariants->insertRow( ui->tableWidgetVariants->rowCount());

    QStringList headerLabels;
    for(int i= 1; i < ui->tableWidgetVariants->rowCount()+1; i++){
        headerLabels.push_back("A" + QString::number(i));
    }
    ui->tableWidgetVariants->setVerticalHeaderLabels(headerLabels);

    ui->tableWidgetGrades->insertRow( ui->tableWidgetGrades->rowCount());
    ui->tableWidgetGrades->setVerticalHeaderLabels(headerLabels);
}


void MainWindow::on_addCriterion_clicked()
{
    ui->tableWidgetCriterions->insertRow( ui->tableWidgetCriterions->rowCount());

    QStringList headerLabels;
    for(int i= 1; i < ui->tableWidgetCriterions->rowCount()+1; i++){
        headerLabels.push_back("C" + QString::number(i));
    }
    ui->tableWidgetCriterions->setVerticalHeaderLabels(headerLabels);

    ui->tableWidgetGrades->insertColumn(ui->tableWidgetGrades->columnCount());
    ui->tableWidgetGrades->setHorizontalHeaderLabels(headerLabels);
}


void MainWindow::on_tableWidgetCriterions_cellChanged(int row, int column)
{
    if(reportReady)
    {
        reportReady = false;
        ui->tableWidgetRes->setRowCount(0);
    }
    if(column == 0)//в первом столбце текст
        return;
    //проверка формата
    QString strValue = ui->tableWidgetCriterions->item(row,column)->text();
    if(strValue.length() == 0)
        return;
    QChar sym = strValue[strValue.length()-1];

    if(sym < '1' || sym > '9')
        ui->tableWidgetCriterions->item(row,column)->setText("");
    else
        ui->tableWidgetCriterions->item(row,column)->setText(sym);
}


void MainWindow::on_tableWidgetGrades_cellChanged(int row, int column)
{
    if(reportReady)
    {
        reportReady = false;
        ui->tableWidgetRes->setRowCount(0);
    }
    //проверка формата
    QString strValue = ui->tableWidgetGrades->item(row,column)->text();
    if(strValue.length() == 0)
        return;
    QChar sym = strValue[strValue.length()-1];

    if(sym < '1' || sym > '9')
        ui->tableWidgetGrades->item(row,column)->setText("");
    else
        ui->tableWidgetGrades->item(row,column)->setText(sym);
}


void MainWindow::on_pushButtonCalculate_clicked()
{
    if(ui->tableWidgetVariants->rowCount() == 0)
    {
        QMessageBox::warning(this, "Предупреждение", "Не создано ни одного варианта!");
        return;
    }
    if(ui->tableWidgetCriterions->rowCount() == 0)
    {
        QMessageBox::warning(this, "Предупреждение", "Не создано ни одного критерия!");
        return;
    }
    if(!checkFillAllFields())
    {
        QMessageBox::warning(this, "Предупреждение", "Не все ячейки заполнены!");
        return;
    }

    QVector <double> criterionsWeights;
    for(int i= 0; i < ui->tableWidgetCriterions->rowCount(); i++){
        criterionsWeights.push_back(ui->tableWidgetCriterions->item(i,1)->text().toDouble());
    }
    QVector <QString> variants;
    for(int i= 0; i < ui->tableWidgetVariants->rowCount(); i++){
        variants.push_back(ui->tableWidgetVariants->item(i,0)->text());
    }
    QVector <QVector <double>> grades(ui->tableWidgetGrades->rowCount());
    size_t cnt_column = ui->tableWidgetGrades->columnCount();
    for(int i= 0; i < ui->tableWidgetGrades->rowCount(); i++){
        for(int j= 0; j < cnt_column; j++)
        {
            grades[i].push_back(ui->tableWidgetGrades->item(i,j)->text().toDouble());
        }
    }
    Calculator calculator(criterionsWeights, variants, grades);
    QVector <rowRec> res = calculator.getCalculateRes();
    ui->tableWidgetRes->setRowCount(0);
    for(int numRow = 0; numRow < res.length(); numRow++)
    {
        ui->tableWidgetRes->insertRow(numRow);
        QTableWidgetItem *name = new QTableWidgetItem(res[numRow].name);
        ui->tableWidgetRes->setItem(numRow,0,name);
        QTableWidgetItem *weight = new QTableWidgetItem(QString::number(res[numRow].weight));
        ui->tableWidgetRes->setItem(numRow,1,weight);
    }
    ui->tableWidgetRes->resizeColumnsToContents();
    reportReady = true;
}

bool MainWindow::checkFillAllFields()
{

    for(int i= 0; i < ui->tableWidgetVariants->rowCount(); i++){
        for(int j= 0; j < ui->tableWidgetVariants->columnCount(); j++)
        {
            if (ui->tableWidgetVariants->item(i,j) == 0)
                return false;
            if(ui->tableWidgetVariants->item(i,j)->text() == "")
                return false;
        }
    }

    for(int i= 0; i < ui->tableWidgetCriterions->rowCount(); i++){
        for(int j= 0; j < ui->tableWidgetCriterions->columnCount(); j++)
        {
            if (ui->tableWidgetCriterions->item(i,j) == 0)
                return false;
            if(ui->tableWidgetCriterions->item(i,j)->text() == "")
                return false;
        }
    }

    for(int i= 0; i < ui->tableWidgetGrades->rowCount(); i++){
        for(int j= 0; j < ui->tableWidgetGrades->columnCount(); j++)
        {
            if (ui->tableWidgetGrades->item(i,j) == 0)
                return false;
            if(ui->tableWidgetGrades->item(i,j)->text() == "")
                return false;
        }
    }

    return true;
}


void MainWindow::on_delVariant_clicked()
{
    int num_row_del = ui->tableWidgetVariants->currentIndex().row();
    if(num_row_del < 0)
    {
        QMessageBox::warning(this, "Ошибка", "Вариант не выбран!");
        return;
    }
    QMessageBox::StandardButton confirm = QMessageBox::question(this, "Подтверждение операции", "Вы уверены?"
                                                                , QMessageBox::Yes | QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        ui->tableWidgetVariants->removeRow(num_row_del);
        ui->tableWidgetGrades->removeRow(num_row_del);

        QStringList headerLabels;
        for(int i= 1; i < ui->tableWidgetVariants->rowCount()+1; i++){
            headerLabels.push_back("A" + QString::number(i));
        }
        ui->tableWidgetVariants->setVerticalHeaderLabels(headerLabels);
        ui->tableWidgetGrades->setVerticalHeaderLabels(headerLabels);

    }
}


void MainWindow::on_delCriterion_clicked()
{
    int num_row_del = ui->tableWidgetCriterions->currentIndex().row();
    if(num_row_del < 0)
    {
        QMessageBox::warning(this, "Ошибка", "Критерий не выбран!");
        return;
    }
    QMessageBox::StandardButton confirm = QMessageBox::question(this, "Подтверждение операции", "Вы уверены?"
                                                                , QMessageBox::Yes | QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        ui->tableWidgetCriterions->removeRow(num_row_del);
        ui->tableWidgetGrades->removeColumn(num_row_del);

        QStringList headerLabels;
        for(int i= 1; i < ui->tableWidgetCriterions->rowCount()+1; i++){
            headerLabels.push_back("C" + QString::number(i));
        }
        ui->tableWidgetCriterions->setVerticalHeaderLabels(headerLabels);
        ui->tableWidgetGrades->setHorizontalHeaderLabels(headerLabels);
    }
}


void MainWindow::on_tableWidgetVariants_cellChanged(int row, int column)
{
    if(reportReady)
    {
        reportReady = false;
        ui->tableWidgetRes->setRowCount(0);
    }
}


void MainWindow::on_saveReport_clicked()
{
    if(!reportReady)
    {
        QMessageBox::warning(this, "Ошибка", "Сначала выполните расчет!");
        return;
    }
    QString name = ui->reportName->text();
    if(name == "")
    {
        QMessageBox::warning(this, "Ошибка", "Введите название!");
        return;
    }
    QString nameFile = name + ".csv";
    if(!QDir("reports").exists())
        QDir().mkdir("reports");
    QFile report("reports/" + nameFile);
    if(report.exists())
    {
        QMessageBox::warning(this, "Ошибка", "Отчет с данным именем уже существует!");
        return;
    }
    if(report.open(QIODevice::WriteOnly)){
        QTextStream stream(&report);
        stream << "Отчет;"
               << name + "\n\n"
               << "Варианты:\n";

        for(int i= 0; i < ui->tableWidgetVariants->rowCount(); i++){
            stream << "A" + QString::number(i+1) + ";";
            stream << ui->tableWidgetVariants->item(i,0)->text() + "\n";
        }
        stream << "\n;Критерий;Вес\n";
        for(int i= 0; i < ui->tableWidgetCriterions->rowCount(); i++){
            stream << "C" + QString::number(i+1) + ";";
            stream << ui->tableWidgetCriterions->item(i,0)->text() + ";";
            stream << ui->tableWidgetCriterions->item(i,1)->text() + "\n";
        }
        stream << "\nТаблица индивидуальных оценок вариантов по критериям\n;";
        for(int i= 0; i < ui->tableWidgetGrades->columnCount(); i++){
            stream << "C" + QString::number(i+1) + ";";
        }
        stream << "\n";
        for(int i= 0; i < ui->tableWidgetGrades->rowCount(); i++){
            stream << "A" + QString::number(i+1) + ";";
            for(int j= 0; j < ui->tableWidgetGrades->columnCount(); j++)
            {
                stream << ui->tableWidgetGrades->item(i,j)->text() + ";";
            }
            stream << "\n";
        }
        stream << "\nРезультат;Вариант;Итоговый вес %\n";
        for(int i= 0; i < ui->tableWidgetRes->rowCount(); i++){
            stream << QString::number(i+1) + ";";
            for(int j= 0; j < ui->tableWidgetRes->columnCount(); j++)
            {
                stream << ui->tableWidgetRes->item(i,j)->text() + ";";
            }
            stream << "\n";
        }

        report.close();
        QMessageBox::information(this, "Уведомление", "Отчет сохранен!");
    }
}

