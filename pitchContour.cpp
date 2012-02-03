#include <stdlib.h>
#include <samplerate.h>
#include <iostream>
#include "pitchContour.h"

using std::cout;
using std::cin;
using std::endl;

JackClient::JackClient() : 
  JackCpp::AudioIO("pitchContour", 1, 1)
{
  start();

  sample_rate = getSampleRate();

  if (sample_rate != 48000){
    cout << "sample rate must be 48000 for even downsampling\n";
    close();
    exit(0);
  }

  downsampled_len = (float)desired_sample_rate / sample_rate * input_len;
  downsampled_frames = new float[downsampled_len];

  // Initialize pitch class
  pitch = new Pitch(downsampled_len, desired_sample_rate);
}

JackClient::~JackClient()
{
  delete[] input_frames;
  delete[] downsampled_frames;

  close();
}

int JackClient::audioCallback(jack_nframes_t nframes, audioBufVector inBufs, audioBufVector outBufs)
{
  if (nframes == input_len){
    input_frames = (float*)&inBufs[0];
    processAudio();
  }
  
  return 0;
}

void JackClient::processAudio()
{
  downSample();
  float the_pitch = pitch->getPitch(downsampled_frames, downsampled_len);
  cout << the_pitch << endl;
}

void JackClient::downSample()
{
  SRC_DATA src_data;
  src_data.data_in = input_frames;
  src_data.data_out = downsampled_frames;
  src_data.input_frames = input_len;
  src_data.output_frames = downsampled_len;
  src_data.src_ratio = (float)desired_sample_rate / sample_rate;

  int return_val = src_simple( &src_data, SRC_SINC_MEDIUM_QUALITY, 1);

  if (return_val != 0){
    const char* error_string = src_strerror(return_val);
    cout << error_string << endl;
  }
}

