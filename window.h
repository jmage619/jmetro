/****************************************************************************
    Copyright (C) 2017  jmage619

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#ifndef WINDOW
#define WINDOW

#include <atomic>
#include <mutex>
//#include <iostream>
#include <QWidget>

#include <jack/jack.h>

int process(jack_nframes_t nframes, void* data);

class QPushButton;
class QSlider;
class QSpinBox;

class Window: public QWidget {
  Q_OBJECT

  private:
    QPushButton* on_button;
    QSlider* vol_slider;
    QSpinBox* bpm_box;
    jack_client_t* jack_client;
    int sample_rate;

  private slots:
    void enable(bool checked);
    // should probably put mutexes around dt?
    void updateBpm(int val) {dt = sample_rate * 60. / val;}
    void updateVol(int val) {amp = val / 100.f;}

  public:
    jack_port_t * out_l;
    jack_port_t * out_r;
    float* wav;
    int wav_len;
    std::atomic<float> amp;
    bool enabled = false;
    std::mutex enabled_mutex;
    int cur_frame = 0;
    double next_click;
    unsigned int cur_time = 0;
    std::atomic<double> dt;
    Window();
    ~Window();
};

#endif
