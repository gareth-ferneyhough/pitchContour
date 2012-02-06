/* Really simply, really shitty sound player thing.
   Love, Gareth */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>
#include <sndfile.h>

void readFile(const char*);
void jack_shutdown (void *arg);

jack_port_t *output_port;
jack_client_t *client;
int current_index;
int length;
int num_channels;

float* audio_frames;

int process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *out;
  void* temp_out;

  if(length - current_index >= nframes){

    temp_out = (jack_port_get_buffer (output_port, nframes));
    out = (float*)temp_out;

    int i = 0;
    while(i < nframes){
      out[i] = audio_frames[current_index+1];
      i++;
      current_index += num_channels;
    }
    return 0;
  }

  else jack_shutdown(NULL);
}

void jack_shutdown (void *arg)
{
  exit (1);
}

int main (int argc, char *argv[])
{
  if(argc == 2){
    char filename[20];
    strncpy(filename, argv[1], 20);
    readFile(filename);
  }

  else return 0;

  const char **ports;
  const char *client_name = "shitplayer";
  const char *server_name = NULL;
  jack_options_t options = JackNullOption;
  jack_status_t status;

  /* open a client connection to the JACK server */

  client = jack_client_open (client_name, options, &status, server_name);
  if (client == NULL) {
    fprintf (stderr, "jack_client_open() failed, "
             "status = 0x%2.0x\n", status);
    if (status & JackServerFailed) {
      fprintf (stderr, "Unable to connect to JACK server\n");
    }
    exit (1);
  }
  if (status & JackServerStarted) {
    fprintf (stderr, "JACK server started\n");
  }
  if (status & JackNameNotUnique) {
    client_name = jack_get_client_name(client);
    fprintf (stderr, "unique name `%s' assigned\n", client_name);
  }

  /* tell the JACK server to call `process()' whenever
     there is work to be done.
  */

  jack_set_process_callback (client, process, 0);

  /* tell the JACK server to call `jack_shutdown()' if
     it ever shuts down, either entirely, or if it
     just decides to stop calling us.
  */

  jack_on_shutdown (client, jack_shutdown, 0);

  /* display the current sample rate.
   */

  /* printf ("engine sample rate: %" PRIu32 "\n", */
  /*         jack_get_sample_rate (client)); */

  /* create out port */

  output_port = jack_port_register (client, "output",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput, 0);

  if (output_port == NULL){
    fprintf(stderr, "no more JACK ports available\n");
    exit (1);
  }

  /* Tell the JACK server that we are ready to roll.  Our
   * process() callback will start running now. */

  if (jack_activate (client)) {
    fprintf (stderr, "cannot activate client");
    exit (1);
  }

  /* Connect the ports.  You can't do this before the client is
   * activated, because we can't make connections to clients
   * that aren't running.  Note the confusing (but necessary)
   * orientation of the driver backend ports: playback ports are
   * "input" to the backend, and capture ports are "output" from
   * it.
   */

  ports = jack_get_ports (client, NULL, NULL,
                          JackPortIsPhysical|JackPortIsOutput);
  if (ports == NULL) {
    fprintf(stderr, "no physical capture ports\n");
    exit (1);
  }

  ports = jack_get_ports (client, NULL, NULL,
                          JackPortIsPhysical|JackPortIsInput);
  if (ports == NULL) {
    fprintf(stderr, "no physical playback ports\n");
    exit (1);
  }

  // Dont connect to output port
  /* if (jack_connect (client, jack_port_name (output_port), ports[0])) { */
  /*   fprintf (stderr, "cannot connect output ports\n"); */
  /* } */

  free (ports);

  /* keep running until stopped by the user */

  sleep (-1);

  /* this is never reached but if the program
     had some other way to exit besides being killed,
     they would be important to call.
  */

  jack_client_close (client);
  exit (0);
}

void readFile( const char* filename )
{
  // sndfile variables
  SNDFILE *infile;
  SF_INFO sfinfo;

  // other variables
  int readcount;

  // check file
  if(!(infile = sf_open(filename, SFM_READ, &sfinfo)))
    {
      printf("Unable to open %s\n", filename);
      puts(sf_strerror(NULL));
      return;
    }

  if(sfinfo.channels > 6)
    {
      printf("Not able to process more than 6 channels.\n");
      return;
    }

  printf("Channels: %i\n", sfinfo.channels);
  printf("Sample_rate: %i\n", sfinfo.samplerate);

  int num_frames = sfinfo.frames;
  num_channels = sfinfo.channels;

  audio_frames = (float*) malloc(num_frames * sfinfo.channels * sizeof(float));

  readcount = sf_read_float(infile, audio_frames, num_frames);
  length = readcount;
  current_index = 0;
}
