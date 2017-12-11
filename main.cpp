#include <iostream>
using std::cout;
using std::endl;
#include <memory>
#include <QApplication>
#include <QtWidgets>

#include <jack/jack.h>

#include "window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  Window window;
  window.show();

  return app.exec();
}
