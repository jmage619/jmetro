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
  sample_t* out_l = static_cast<sample_t*>(jack_port_get_buffer(window->out_l, nframes));
  sample_t* out_r = static_cast<sample_t*>(jack_port_get_buffer(window->out_r, nframes));
  // clear buffer to all zeros
  memset(out_l, 0, sizeof(sample_t) * nframes);
  memset(out_r, 0, sizeof(sample_t) * nframes);

  if (window->enabled) {
    for (unsigned int i = 0; i < nframes; ++i) {
      if (window->cur_frame < window->wav_len) {
        out_l[i] = window->amp * window->wav[window->cur_frame];
        out_r[i] = window->amp * window->wav[window->cur_frame];
        ++window->cur_frame;
      }

      ++window->cur_time;

      if (window->cur_time >= lround(window->next_click)) {
        window->cur_frame = 0;
        window->next_click += window->dt;
      }
    }
  }

  return 0;
}

void Window::enable(bool checked) {
  if (checked) {
    cur_frame = 0;
    cur_time = 0;
    next_click = dt;
    enabled = true;
    on_button->setText("stop");
  }
  else {
    enabled = false;
    on_button->setText("start");
  }
}

Window::Window() {
  QVBoxLayout* v_layout = new QVBoxLayout;

  QHBoxLayout* h_layout = new QHBoxLayout;
  QLabel* label = new QLabel;
  label->setText("vol:");
  h_layout->addWidget(label);

  vol_slider = new QSlider(Qt::Horizontal);
  vol_slider->setMaximum(100);

  h_layout->addWidget(vol_slider);

  v_layout->addLayout(h_layout);

  h_layout = new QHBoxLayout;
  label = new QLabel;
  label->setText("bpm:");
  h_layout->addWidget(label);

  bpm_box = new QSpinBox;
  bpm_box->setMaximum(400);

  h_layout->addWidget(bpm_box);
  v_layout->addLayout(h_layout);

  on_button = new QPushButton("start");
  on_button->setCheckable(true);
  v_layout->addWidget(on_button);
  setLayout(v_layout);

  // init jack garbage
  jack_client = jack_client_open("jmetro", JackNullOption, nullptr);

  if (jack_client == nullptr)
    throw std::runtime_error("failed to initialize jack!");

  jack_set_process_callback(jack_client, &process, this);

  out_l = jack_port_register(jack_client, "outL", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  if (out_l == nullptr) {
    jack_client_close(jack_client);
    throw std::runtime_error("failed to open jack output port!");
  }

  out_r = jack_port_register(jack_client, "outR", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  if (out_r == nullptr) {
    jack_port_unregister(jack_client, out_l);
    jack_client_close(jack_client);
    throw std::runtime_error("failed to open jack output port!");
  }

  // sound file garbage
  SF_INFO sf_info;
  sf_info.format = 0;
  SNDFILE* sf_wav = sf_open("rimshot.wav", SFM_READ, &sf_info);
  if (sf_wav == NULL) {
    jack_port_unregister(jack_client, out_l);
    jack_port_unregister(jack_client, out_r);
    jack_client_close(jack_client);
    throw std::runtime_error("could not load sound file!");
  }

  wav_len = sf_info.frames;
  wav = new float[sf_info.channels * sf_info.samplerate];
  sf_read_float(sf_wav, wav, sf_info.channels * wav_len);
  sf_close(sf_wav);

  sample_rate = jack_get_sample_rate(jack_client);
  connect(on_button, &QAbstractButton::toggled, this, &Window::enable);
  connect(vol_slider, &QSlider::valueChanged, this, &Window::updateVol);
  connect(bpm_box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Window::updateBpm);

  vol_slider->setValue(100);
  bpm_box->setValue(80);
  next_click = dt;

  // if all went well, activate it!
  if (jack_activate(jack_client)) {
    delete [] wav;
    jack_port_unregister(jack_client, out_l);
    jack_port_unregister(jack_client, out_r);
    jack_client_close(jack_client);
    throw std::runtime_error("could not activate jack client!");
  }
}

Window::~Window() {
  jack_deactivate(jack_client);
  delete [] wav;
  jack_port_unregister(jack_client, out_l);
  jack_port_unregister(jack_client, out_r);
  jack_client_close(jack_client);
}
