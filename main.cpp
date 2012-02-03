#include <iostream>
#include "pitchContour.h"

using std::endl;
using std::cout;
using std::cin;

int main()
{
  JackClient * client = new JackClient;
  
  if (client->isRealTime())
    cout << "is realtime " << endl;
  else
    cout << "is not realtime " << endl;

  // Wait till user exits with 'q'
  char x = NULL;
  while( x != 'q'){
    cin >> x;
  }

  delete client;

  return 0;
}

