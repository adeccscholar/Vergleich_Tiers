#include "MainForm.h"

#include "Processes/Process.h"
#include "Processes/Process_Impl.h"


MainForm::MainForm(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    TProcess_Impl<TMyMSSQL> test;
    
}

MainForm::~MainForm()
{}
