#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

#include "PPM.h"
#include "Rnumgen.h"

const char ETX = 0x3; // end of text like ctrl+C

struct pixel {
  pixel()
  {
    r = 0;
    c = 0;
  }
  int r;
  int c;
};

void set_pixel_list(PPM &img, vector <pixel> &v, int seed  ) {
    vector <int> h; // histogram
    pixel p;
    int index;
    unsigned int r1, r2, rand_of_24;

    for (int i = 0; i < img.get_Nrows(); i++)
    {
        for (int j = 0; j < img.get_Ncols(); j++)
        {
            //extract four bits (b6..b3) from each color byte
            index |= img[i][j].R & 0x78;
            index = index << 4;
            index |= img[i][j].G & 0x78;
            index = index << 4;
            index |= img[i][j].B & 0x78;
            h.push_back(index);

            p.r = i;
            p.c = j;
            v.push_back(p);
        }
    }
    rnumgen RNG (seed, h);

    for (int i = v.size() - 1; i > 0; --i)
    {
        //random number across the image file
        r1 = h[(rand() % (h.size() - 1))];
        r2 = h[(rand() % (h.size() - 1))];
        rand_of_24 = ((r1 << 12 | r2));
        swap(v[i],v[rand_of_24 % (i +1)]);
    }
}

void encode(PPM &img, vector <pixel> &v) {
  int i = 0;
  int color_change = 3;
  char c;
  char bit;
  char bit_ETX;
  while(cin.get(c))
  {
    for (int k_th = 0; k_th < 7; k_th++)
    {
      bit = c >> k_th;
      if ((color_change % 3) == 0)
      {
        img[v[i].r][v[i].c].R &= 0xFE;
        img[v[i].r][v[i].c].R |= bit & 0x1;
      }
      if ((color_change % 3) == 1)
      {
        img[v[i].r][v[i].c].G &= 0xFE;
        img[v[i].r][v[i].c].G |= bit & 0x1;
      }
      if ((color_change % 3) == 2)
      {
        img[v[i].r][v[i].c].B &= 0xFE;
        img[v[i].r][v[i].c].B |= bit & 0x1;
      }
      color_change++;
      i++;
    }
  }
  for (int k_th = 0; k_th < 7; k_th++)
  {
    bit_ETX = ETX >> k_th;
    if ((color_change % 3) == 0)
      {
        img[v[i].r][v[i].c].R &= 0xFE;
        img[v[i].r][v[i].c].R |= bit_ETX & 0x1;
      }
      if ((color_change % 3) == 1)
      {
        img[v[i].r][v[i].c].G &= 0xFE;
        img[v[i].r][v[i].c].G |= bit_ETX & 0x1;
      }
      if ((color_change % 3) == 2)
      {
        img[v[i].r][v[i].c].B &= 0xFE;
        img[v[i].r][v[i].c].B |= bit_ETX & 0x1;
      }
      color_change++;
      i++;
  }
}

void decode(PPM &img, vector <pixel> &v) {  
  int color_change = 3;
  int i = 0;
  char c;
  char text;
  while(1)
  {
    for (int k_th = 0; k_th < 7; k_th++)
    {
      if ((color_change % 3) == 0)
      {
        c |= img[v[i].r][v[i].c].R & 0x1;
        c = c << 1;
      }
      if ((color_change % 3) == 1)
      {
        c |= img[v[i].r][v[i].c].G & 0x1;
        c = c << 1;
      }
      if ((color_change % 3) == 2)
      {
        c |= img[v[i].r][v[i].c].B & 0x1;
        c = c << 1;
      }
      color_change++;
      i++;
    }
    for (int j = 0; j < 8; j++)
    {
      if ((c & (1 << j)))
       text |= 1 << (7 - j);
      
    }
    if (text == ETX) { break;} // stop when hit ETX
    cout.put (text); // print the decrypted char
    text = 0; // start over
  }
    
}

int main(int argc, char *argv[]) {
    PPM image;
    string mode;
    mode = argv[1];
    vector <pixel> pi; //declare pixel_list
    int key_number;    // key number for encode and decode
    string seed = argv[argc - 2]; // get the location in command line
    seed = seed.substr(6, seed.length()); // parse the command line to eliminate the (-seed=)'s part
    key_number = atoi(seed.c_str());  // convert to number
    image.read(argv[argc - 1]);     // read the ppm file
    
    if (argc != 4 || (mode != "-encode" && mode != "-decode"))
    {
        cout << "Usage: ./Crypto2 -encode|-decode -seed=key_number image_name.ppm" << endl;
        return 0;
    }
    set_pixel_list(image, pi, key_number);

    if (mode == "-encode") {
        encode(image, pi); 
        image.write(argv[argc - 1]);
    }
    if (mode == "-decode") 
        decode(image, pi);

}
