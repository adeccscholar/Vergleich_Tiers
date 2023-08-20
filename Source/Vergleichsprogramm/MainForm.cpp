#include "MainForm.h"




MainForm::MainForm(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    proc.InitMainForm(TMyForm(this, false));
    proc.Login();
}

MainForm::~MainForm()
{}
