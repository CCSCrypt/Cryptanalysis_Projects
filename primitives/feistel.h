// Class to represent Feistel networks

/* 
 * We assume a simplified subset of Feistel networks that
 * follow the following pattern/primitives 
 */

/* 
 * Initial Permutation and Final Permutation:
 * Both input and output are permuted via a fixed and 
 * known permutations. They may be related in some cases 
 * like DES, but we will not assume that here.
 */

/* 
 * S-Box Layer:
 * The S-Box layer considers multiple S-Boxes in parallel.
 * Some Feistel networks need not have the same set of
 * S-Boxes in each round (eg, round-key dependent), but 
 * we will deal with simplified cases here. 
 */

/* 
 * Expansion Layer:
 * We consider this as a simple "placement" layer of the 
 * right half of the input. The expansion layer is a 
 * generally does not take xors of the plaintext bits
 * themselves and we make that assumption here. 
 */

/* 
 * Key Mixing Layer:
 * The key mixing layer is a simple xor of the output
 * of the expansion layer with the round key.
 */

/* 
 * Post-S-Box Layer:
 * The post-S-Box layer is generally a permutation of the
 * output of the S-Box layer. We will work with this
 * simple assumption here.
 */

/*
 * Key Schedule:
 * We assume that all the subkey bits are just original
 * master key bits. This is a simplification of the
 * key schedule. {TODO: Add more flexibility}
 */

#ifndef FEISTEL_H
#define FEISTEL_H

// Standard Library Imports
#include <iostream>
#include <vector>
#include <tuple>

// Custom Library Imports
#include "s_box.h"
#include "placement.h"
#include "bitstring.h"

// Define DECAY constant
#define DECAY 0.2

// Struct to hold state for trail
struct trail_state
{
  // Net Info
  int total_rounds;
  int total_sboxes;
  int pres_round;
  int pres_sbox;
  bool dummy;

  // Roundwise Info
  std::vector<bitstring> input_masks;
  std::vector<bitstring> output_masks;
  std::vector<bitstring> key_masks;
  std::vector<float> round_biases;

  // Current Round Info
  std::vector<int> alphas;
  std::vector<int> betas;
  std::vector<float> s_box_biases;
};

class feistel
{
  private:
    int block_size;                             // Block size of the Feistel network
    int max_rounds;                             // Maximum number of rounds
    placement ip;                        // Initial Permutation
    placement fp;                        // Final Permutation
    int num_sboxes;                             // Number of S-Boxes (assume same input-
                                                // output size for all S-Boxes)
    int sbox_in;                                // Input size of S-Boxes
    int sbox_out;                               // Output size of S-Boxes
    std::vector<s_box> sboxes;                  // S-Boxes
    placement prev_sbox;                 // Expansion layer
    placement post_sbox;                 // Post-S-Box layer
    int key_size;                               // Key size
    std::vector<std::vector<int>> key_schedule; // Key schedule
    bitstring key;                     // Master key

  public:
    // Constructors & Destructors
    feistel(int block_size, int max_rounds, std::vector<int> ip,
            std::vector<int> fp, int num_sboxes, int sbox_in,
            int sbox_out, std::vector<s_box> sboxes,
            std::vector<int> prev_sbox, std::vector<int> post_sbox,
            int key_size, std::vector<std::vector<int>> key_schedule, 
            bitstring master_key);
    ~feistel();

    // Accessors
    int get_block_size() { return this->block_size; }
    int get_key_size() { return this->key_size; }
    int get_max_rounds() { return this->max_rounds; }
    placement get_ip() { return this->ip; }
    placement get_fp() { return this->fp; }
    placement get_prev_sbox() { return this->prev_sbox; }
    placement get_post_sbox() { return this->post_sbox; }
    int get_num_sboxes() { return this->num_sboxes; }
    int get_sbox_in() { return this->sbox_in; }
    int get_sbox_out() { return this->sbox_out; }
    std::vector<std::vector<int>> get_key_schedule() { return this->key_schedule; }

    // Round primitives
    bitstring round_function(bitstring input, bitstring round_key); 

    // Encryption & Decryption
    bitstring encrypt(bitstring plaintext,
                              int rounds);
    bitstring decrypt(bitstring ciphertext,
                              int rounds);

    // Finding Linear Trails
    std::tuple<bitstring, bitstring, bitstring> round_approx(int s_box_num, 
                                                         int input_mask,
                                                         int output_mask
                                                         );
    trail_state find_linear_trail(trail_state state, int rounds, bool start);
};

#endif
