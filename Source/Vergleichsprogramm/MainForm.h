#pragma once

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include "ui_MainForm.h"

#include "Processes/BusinessOperations.h"



class MainForm : public QMainWindow
{
    Q_OBJECT

public:
    MainForm(QWidget *parent = nullptr);
    ~MainForm();

    TBusinessOperations& processes();
    TBusinessOperations const& processes() const;
private:
    Ui::MainFormClass ui;
    QLabel* statusLabel;

    void Test(void);
};
