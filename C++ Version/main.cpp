#include <QApplication>
#include "mainWindow.h"

int main(int argc, char **argv)
{
    
  QApplication app(argc, argv);  
    
  MainWindow window;

  window.setWindowTitle("AwesomePaint");
  window.resize(1300, 540);
  window.show();
  
  return app.exec();
}