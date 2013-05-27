#include <QApplication>
#include "isp_gui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    isp_gui w;
    w.show();
    
    return a.exec();
}
