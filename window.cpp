#include <iostream>
using std::cout;
using std::endl;
#include <stdexcept>
#include <cstring>
#include <cmath>
#include <QtWidgets>
#include "window.h"

#include <jack/jack.h>
#include <sndfile.h>

typedef jack_default_audio_sample_t sample_t;

int process(jack_nframes_t nframes, void* data) {
  Window* window = static_cast<Window*>(data);
  sample_t* buffer = static_cast<sample_t*>(jack_port_get_buffer(window->out_port, nframes));
  // clear buffer to all zeros
  memset(buffer, 0, sizeof(sample_t) * nframes);

  for (unsigned int i = 0; i < nframes; ++i) {
    if (window->cur_frame < window->wav_len) {
      buffer[i] = window->wav[window->cur_frame];
      ++window->cur_frame;
    }

    ++window->cur_time;

    if (window->cur_time >= lround(window->next_click)) {
      window->cur_frame = 0;
      window->next_click += window->dt;
    }
  }

  return 0;
}

Window::Window() {
  QHBoxLayout* h_layout = new QHBoxLayout;
  bpm_label = new QLabel;
  bpm_label->setText("bpm:");
  h_layout->addWidget(bpm_label);

  bpm_box = new QSpinBox;
  bpm_box->setMaximum(400);

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

  // sound file garbage
  SF_INFO sf_info;
  sf_info.format = 0;
  SNDFILE* sf_wav = sf_open("rimshot.wav", SFM_READ, &sf_info);
  if (sf_wav == NULL) {
    jack_port_unregister(jack_client, out_port);
    jack_client_close(jack_client);
    throw std::runtime_error("could not load sound file!");
  }

  wav_len = sf_info.frames;
  wav = new float[sf_info.channels * sf_info.samplerate];
  sf_read_float(sf_wav, wav, sf_info.channels * wav_len);
  sf_close(sf_wav);

  sample_rate = jack_get_sample_rate(jack_client);
  connect(bpm_box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Window::updateBpm);
  bpm_box->setValue(80);
  next_click = dt;

  // if all went well, activate it!
  if (jack_activate(jack_client)) {
    delete [] wav;
    jack_port_unregister(jack_client, out_port);
    jack_client_close(jack_client);
    throw std::runtime_error("could not activate jack client!");
  }
}

Window::~Window() {
  jack_deactivate(jack_client);
  delete [] wav;
  jack_port_unregister(jack_client, out_port);
  jack_client_close(jack_client);
}
