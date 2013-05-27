#include "isp_gui.h"
#include "ui_isp_main.h"

isp_gui::isp_gui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::isp_gui)
{
    ui->setupUi(this);
}

isp_gui::~isp_gui()
{
    delete ui;
}

//Actions for buttons
void isp_gui::on_pushButtonResetOn_clicked()
{
    //will be add variable for reset status
    ui->labelResetStatus->setText("ON");
}

void isp_gui::on_pushButtonResetOff_clicked()
{
    //will be add variable for reset status
    ui->labelResetStatus->setText("OFF");
}

void isp_gui::on_pushButtonRestart_clicked()
{
    //will be add variable for reset status
    ui->labelResetStatus->setText("ON");
    //will be add 'wait'
    ui->labelResetStatus->setText("OFF");
}

void isp_gui::on_pushButtonReadSig_clicked()
{

}

void isp_gui::on_pushButtonErase_clicked()
{

}

void isp_gui::on_pushButtonSetup_clicked()
{

}

void isp_gui::on_pushButtonReadFlash_clicked()
{

}

void isp_gui::on_pushButtonReadEeprom_clicked()
{

}

void isp_gui::on_pushButtonReadUserSig_clicked()
{

}

void isp_gui::on_pushButtonPgmFlash_clicked()
{

}

void isp_gui::on_pushButtonPgmEeprom_clicked()
{

}

void isp_gui::on_pushButtonPgmUserSig_clicked()
{

}

void isp_gui::on_pushButtonVerifyFlash_clicked()
{

}

void isp_gui::on_pushButtonVerifyEeprom_clicked()
{

}

void isp_gui::on_pushButtonVerifyUserSig_clicked()
{

}

void isp_gui::on_pushButtonErasePgmAll_clicked()
{

}

void isp_gui::on_pushButtonFuseLockBits_clicked()
{

}

void isp_gui::on_pushButtonExit_clicked()
{
    close();    //close ISP Programmer
}

//Open file and clear
void isp_gui::on_pushButtonOpenFlash_clicked()
{

}

void isp_gui::on_pushButtonClearOpenFlash_clicked()
{

}

void isp_gui::on_pushButtonOpenEeprom_clicked()
{

}

void isp_gui::on_pushButtonClearOpenEeprom_clicked()
{

}

void isp_gui::on_pushButtonOpenUserSig_clicked()
{

}

void isp_gui::on_pushButtonClearOpenUserSig_clicked()
{

}

//Save file and clear
void isp_gui::on_pushButtonSaveFlash_clicked()
{

}

void isp_gui::on_pushButtonClearSaveFlash_clicked()
{

}

void isp_gui::on_pushButtonSaveEeprom_clicked()
{

}

void isp_gui::on_pushButtonClearSaveEeprom_clicked()
{

}

void isp_gui::on_pushButtonSaveUserSig_clicked()
{

}

void isp_gui::on_pushButtonClearSaveUserSig_clicked()
{

}

//Select Autodetect/Force
void isp_gui::on_radioButtonAutodetect_clicked()
{

}

void isp_gui::on_radioButtonForce_clicked()
{

}
