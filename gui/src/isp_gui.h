#ifndef ISP_GUI_H
#define ISP_GUI_H

#include <QMainWindow>

#include <QTimer>

namespace Ui {
class isp_gui;
}

class isp_gui : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit isp_gui(QWidget *parent = 0);
    ~isp_gui();
    
private slots:
    void on_pushButtonResetOn_clicked();

    void on_pushButtonResetOff_clicked();

    void on_pushButtonRestart_clicked();

    void on_pushButtonReadSig_clicked();

    void on_pushButtonErase_clicked();

    void on_pushButtonSetup_clicked();

    void on_pushButtonReadFlash_clicked();

    void on_pushButtonReadEeprom_clicked();

    void on_pushButtonReadUserSig_clicked();

    void on_pushButtonPgmFlash_clicked();

    void on_pushButtonPgmEeprom_clicked();

    void on_pushButtonPgmUserSig_clicked();

    void on_pushButtonVerifyFlash_clicked();

    void on_pushButtonVerifyEeprom_clicked();

    void on_pushButtonVerifyUserSig_clicked();

    void on_pushButtonErasePgmAll_clicked();

    void on_pushButtonFuseLockBits_clicked();

    void on_pushButtonExit_clicked();

    void on_pushButtonOpenFlash_clicked();

    void on_pushButtonClearOpenFlash_clicked();

    void on_pushButtonOpenEeprom_clicked();

    void on_pushButtonClearOpenEeprom_clicked();

    void on_pushButtonOpenUserSig_clicked();

    void on_pushButtonClearOpenUserSig_clicked();

    void on_pushButtonSaveFlash_clicked();

    void on_pushButtonClearSaveFlash_clicked();

    void on_pushButtonSaveEeprom_clicked();

    void on_pushButtonClearSaveEeprom_clicked();

    void on_pushButtonSaveUserSig_clicked();

    void on_pushButtonClearSaveUserSig_clicked();

    void on_radioButtonAutodetect_clicked();

    void on_radioButtonForce_clicked();

private:
    Ui::isp_gui *ui;
};

#endif // ISP_GUI_H
