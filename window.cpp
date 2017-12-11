#include <stdexcept>
#include <QtWidgets>
#include "window.h"

#include <jack/jack.h>

int process(jack_nframes_t nframes, void* data) {
  return 0;
}

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

  // init jack garbage
  jack_client = jack_client_open("jmetro", JackNullOption, nullptr);

  if (jack_client == nullptr)
    throw std::runtime_error("failed to initialize jack!");

  jack_set_process_callback(jack_client, &process, this);
  out_port = jack_port_register(jack_client, "out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

  if (out_port == nullptr) {
    jack_client_close(jack_client);
    throw std::runtime_error("failed to open jack output port!");
  }

  if (jack_activate(jack_client)) {
    jack_port_unregister(jack_client, out_port);
    jack_client_close(jack_client);
    throw std::runtime_error("could not activate jack client!");
  }
}

Window::~Window() {
  jack_deactivate(jack_client);
  jack_port_unregister(jack_client, out_port);
  jack_client_close(jack_client);
}
