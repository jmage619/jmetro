#ifndef WINDOW
#define WINDOW

#include <QWidget>

class QSpinBox;
class QLabel;

class Window: public QWidget {
  Q_OBJECT

  private:
    QLabel* bpm_label;
    QSpinBox* bpm_box;

  public:
    Window();
};

#endif
