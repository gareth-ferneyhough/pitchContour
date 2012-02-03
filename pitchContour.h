#ifndef PITCH_CONTOUR_H
#define PITCH_COUNTOUR_H

#include "jackcpp/jackaudioio.hpp"
#include "pitch.h"

class JackClient: public JackCpp::AudioIO
{
 public:
  JackClient();
  ~JackClient();
  virtual int audioCallback(jack_nframes_t nframes,
                            audioBufVector inBufs,
                            audioBufVector outBufs);

 private:
  // Private Methods
  void processAudio();
  void downSample();
 
  // Private Data Members
  float *input_frames;
  static const int input_len = 1024;

  float *downsampled_frames;
  int downsampled_len;

  int sample_rate;
  static const int desired_sample_rate = 6000;

  // Private classes
  Pitch *pitch;
};

#endif //PITCH_COUNTOUR_H
