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

  public:
    jack_port_t * out_port;
    float* wav;
    int wav_len;
    int cur_frame = 0;
    Window();
    ~Window();
};

#endif
