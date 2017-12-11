#include <QtWidgets>
#include "window.h"

Window::Window() {
  QHBoxLayout* h_layout = new QHBoxLayout;
  bpm_label = new QLabel;
  bpm_label->setText("bpm:");
  h_layout->addWidget(bpm_label);

  bpm_box = new QSpinBox;
  bpm_box->setMaximum(400);
  bpm_box->setValue(80);

  h_layout->addWidget(bpm_box);
  setLayout(h_layout);
}
