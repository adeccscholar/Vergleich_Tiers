#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainForm.h"

#include "Processes/Process_Impl.h"

class MainForm : public QMainWindow
{
    Q_OBJECT

public:
    MainForm(QWidget *parent = nullptr);
    ~MainForm();

private:
    Ui::MainFormClass ui;
    TProcess_Impl<TMyMSSQL> proc;
};
