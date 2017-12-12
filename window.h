#ifndef WINDOW
#define WINDOW

#include <QWidget>

#include <jack/jack.h>

int process(jack_nframes_t nframes, void* data);

class QSpinBox;
class QLabel;

class Window: public QWidget {
  Q_OBJECT

  private:
    QLabel* bpm_label;
    QSpinBox* bpm_box;
    jack_client_t* jack_client;
    int sample_rate;

  private slots:
    // should probably put mutexes around dt?
    void updateBpm(int val) {dt = sample_rate * 60. / val;}

  public:
    jack_port_t * out_port;
    float* wav;
    int wav_len;
    int cur_frame = 0;
    double next_click;
    unsigned int cur_time = 0;
    double dt;
    Window();
    ~Window();
};

#endif
