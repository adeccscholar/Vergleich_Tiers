#pragma once

#include <iostream>
#include <QMainWindow>
#include "ui_OutputDlgClass.h"

class OutputDlgClass : public QMainWindow
{
    Q_OBJECT

public:
    OutputDlgClass(QWidget *parent = nullptr);
    ~OutputDlgClass();

private:
    Ui::OutputDlgClassClass ui;
    //std::ostream
};
