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
