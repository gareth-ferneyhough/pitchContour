#ifndef EMO_PITCH_H
#define EMO_PITCH_H

/* Note: Requires aubio >= 0.3.3 */
#include <aubio/aubio.h>

class Pitch
{
 public:
  Pitch(int window_size, int sample_rate);
  ~Pitch();
  float getPitch(const float* const, int);

 private:
  fvec_t * yin_in;
  fvec_t * yin_out;
  aubio_pitch_t * o;

  int window_size_;
  int sample_rate_;
  bool is_initialized_;

  int init();
};

#endif // EMO_PITCH_H
