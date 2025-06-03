// Implementation
#include "attack.h"

// Include Standard Libraries
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <cmath>

// Constructors and Destructors
matsui::matsui(feistel cipher, int num_rounds): cipher(cipher)
{
    // Check if the number of rounds is valid
    assert(num_rounds > 0);
    assert(num_rounds <= cipher.get_max_rounds());

    // Populate
    this->cipher = cipher;
    this->num_rounds = num_rounds;
}

matsui::~matsui()
{
    // Destructor
    // this->cipher = NULL;
}

// Helper to create a random bitstring
bitstring create_random_bitstring(int size)
{
    // Check if the size is valid
    assert(size > 0);

    // Create a random bitstring
    bitstring random_bitstring(size);
    for (int i = 0; i < size; i++)
    {
        int random_value = rand() % 2;
        random_bitstring.set_bit(i, random_value);
    }
    return random_bitstring;
}

// Attack 1
int matsui::attack_1(int pair_count, bitstring input_mask, bitstring output_mask, float bias, placement ip, placement fp)
{
    // Peel off IP and FP
    bitstring ip_mask = input_mask.place(ip);
    bitstring fp_mask = output_mask.inv_place(fp);

    // Try to find best RHS value
    int count = 0;
    for (int i = 0; i < pair_count; i++)
    {
        // Create random input and output
        bitstring input = create_random_bitstring(this->cipher.get_block_size());
        bitstring output = this->cipher.encrypt(input, this->num_rounds);

        // Apply masks
        int ip_dot = input*ip_mask;
        int fp_dot = output*fp_mask;

        // Increment count conditionally
        if (ip_dot == fp_dot) count ++;
    }

    // Compute expected bias
    float expected_bias = ((float)count / (float)pair_count) - 0.5;

    // If both have same signs, return 0
    if (expected_bias * bias > 0) return 0;
    else return 1;
}

// Attack 2
bitstring matsui::attack_2(int pair_count, bitstring input_mask, bitstring output_mask, bitstring round_mask, float bias, placement ip, placement fp, placement post_sbox, placement prev_sbox)
{
    // Assert 
    assert(pair_count > 0);
    assert(input_mask.get_size() == this->cipher.get_block_size());
    assert(output_mask.get_size() == this->cipher.get_block_size());

    // Identify active s-boxes
    bitstring active_sboxes(this->cipher.get_num_sboxes());
    bitstring slayer_mask = round_mask.inv_place(post_sbox);
    int sbox_out = this->cipher.get_sbox_out();
    int sbox_in = this->cipher.get_sbox_in();
    int num_sboxes = this->cipher.get_num_sboxes();
    for (int i = 0; i < num_sboxes; i++)
    {
        if (slayer_mask.get_slice_int(i*sbox_out, (i+1)*sbox_out) != 0) active_sboxes.set_bit(i, 1);
    }

    // Bits to guess over
    int guess_bits = active_sboxes.hamming_weight() * sbox_in;

    // Create a vector of ints (buckets for each guess)
    std::vector<int> buckets((1 << guess_bits), 0);
    std::vector<bitstring> round_keys((1 << guess_bits), bitstring(sbox_out * num_sboxes));
    // Iterate over guesses
    for (int j = 0; j < (1 << guess_bits); j++)
    {
        // Create round key
        bitstring round_key = bitstring(sbox_in * num_sboxes);
        int a = 0;
        for (int k = 0; k < num_sboxes; k++)
        {
            // Check if the s-box is active
            if (active_sboxes.get_bit(k))
            {
               // Extract the ath bundle of sbox_in bits
               int bundle = (j >> (a*sbox_in)) & ((1 << sbox_in) - 1);
               // Set the bits in the round key
               round_key.set_slice(k*sbox_in, (k+1)*sbox_in, bundle);
            }
        }
        round_keys[j] = round_key;
    }

    // For each guess
    for (int i = 0; i < pair_count; i++)
    {
        // Create random input and output
        bitstring input = create_random_bitstring(this->cipher.get_block_size());
        bitstring output = this->cipher.encrypt(input, this->num_rounds);

        // Peel off IP and FP
        bitstring mod_input = input.place(ip);
        bitstring mod_output = output.inv_place(fp);

        // Right half of ciphertext
        bitstring right_half = mod_output.get_slice_int(this->cipher.get_block_size()/2, this->cipher.get_block_size());
        
        // Iterate over guesses
        for (int j = 0; j < (1 << guess_bits); j++)
        {
            // Create round key
            bitstring round_key = round_keys[j];

            // Apply masks
            int ip_dot = mod_input*input_mask;
            int fp_dot = mod_output*output_mask;
            
            // Perform round_key function on right half
            bitstring round_key_out = this->cipher.round_function(right_half, round_key);
            int rk_dot = round_key_out*round_mask;

            // Conditionally increment buckets
            if ((ip_dot ^ fp_dot ^ rk_dot) == 0)
            {
                // Increment bucket
                buckets[j] ++;
            }
        } 
    }

    // Modify buckets (subtract num_pairs/2)
    for (int i = 0; i < (1 << guess_bits); i++)
    {
        // Subtract num_pairs/2
        buckets[i] -= pair_count / 2;
    }

    // Find the bucket index with maximum absolute value
    int key_index = 0;
    int max_value = 0;
    for (int i = 0; i < (1 << guess_bits); i++)
    {
        // Check if the absolute value is greater than the current max
        if (abs(buckets[i]) > max_value)
        {
            max_value = abs(buckets[i]);
            key_index = i;
        }
    }
    bitstring reqd_key = round_keys[key_index];

    // Check what is RHS, based on whether buckets[key_index] matches the bias (sign-wise)
    int rhs;
    if (buckets[key_index] * bias > 0) rhs = 0;
    else rhs = 1;

    // Get key schedule (last round)
    std::vector<int> final_schedule = this->cipher.get_key_schedule()[this->num_rounds - 1];

    // Create a partial key
    bitstring partial_key = bitstring(this->cipher.get_key_size());
    for (int i = 0; i < sbox_in*num_sboxes ; i++)
    {
        // Set the bits in the partial key
        partial_key.set_bit(final_schedule[i], reqd_key.get_bit(i));
    }

    // Return the partial key
    return partial_key;
}
