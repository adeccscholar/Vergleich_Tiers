#pragma once

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include "ui_MainForm.h"

#include "Process_Impl_Qt.h"

class MainForm : public QMainWindow
{
    Q_OBJECT

public:
    MainForm(QWidget *parent = nullptr);
    ~MainForm();

private:
    Ui::MainFormClass ui;
    TProcess_Impl_Qt proc;
    QLabel* statusLabel;
};
