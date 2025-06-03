// Class to perform Matsui's attack on Feistel Network

#ifndef ATTACK_H
#define ATTACK_H

// Library Imports
#include <iostream>
#include <vector>

// Custom Library Imports
#include "s_box.h"
#include "placement.h"
#include "bitstring.h"
#include "feistel.h"

// Class to perform Matsui's attack on Feistel Network
class matsui
{
  private:
    feistel cipher;                   // Feistel cipher
    int num_rounds;                 // Number of rounds

  public:
    // Constructors & Destructors
    matsui(feistel cipher, int num_rounds);
    ~matsui();

    // Accessors
    int attack_1(int pair_count, bitstring input_mask, bitstring output_mask, float bias, placement ip, placement fp);
    bitstring attack_2(int pair_count, bitstring input_mask, bitstring output_mask, bitstring round_mask, float bias, placement ip, placement fp, placement post_sbox, placement prev_sbox);

};

#endif
