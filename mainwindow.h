#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addVariant_clicked();

    void on_addCriterion_clicked();

    void on_tableWidgetCriterions_cellChanged(int row, int column);

    void on_tableWidgetGrades_cellChanged(int row, int column);

    void on_pushButtonCalculate_clicked();

    void on_delVariant_clicked();

    void on_delCriterion_clicked();

    void on_tableWidgetVariants_cellChanged(int row, int column);

    void on_saveReport_clicked();

private:
    Ui::MainWindow *ui;

    bool checkFillAllFields();
    bool reportReady= false;
};
#endif // MAINWINDOW_H
