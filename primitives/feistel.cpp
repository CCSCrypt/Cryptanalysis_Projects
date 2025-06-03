// Implementation of the Feistel cipher
#include "feistel.h"

// Include Libraries
#include <iostream>
#include <vector>
#include <tuple>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <random>
#include <iterator>

#define DECAY 0.2

// Helper (to eval bias aover a vector)
float eval_bias(std::vector<float> bias, int lim)
{
    // Check if the vector is valid
    assert(bias.size() > 0);

    // Initialize bias
    float res = 0.5;
    for (int i = 0; i < lim; i++)
    {
        // Update bias
        res *= 2*bias[i];
    }
    
    // Absolute value
    if (res < 0) res = -res;

    // Return bias
    return res;
}

// Print state
void print_state(trail_state state)
{
    // Print the state
    std::cout << "\033[31mCurrent Round: " << state.pres_round << std::endl;
    std::cout << "Current S-Box: " << state.pres_sbox << std::endl;
    std::cout << "Dummy: " << state.dummy << std::endl;

    // Print roundwise info
    for (int i = 0; i < state.total_rounds; i++)
    {
        std::cout << "Round " << i + 1 << ": ";
        state.input_masks[i].print();
        state.output_masks[i].print();
        state.key_masks[i].print();
        std::cout << "Bias: " << state.round_biases[i] << std::endl;
    }

    // Print current round info
    for (int i = 0; i < state.total_sboxes; i++)
    {
        std::cout << "S-Box " << i + 1 << ": ";
        std::cout << "Alpha: " << state.alphas[i] << ", Beta: " 
                  << state.betas[i] << ", Bias: "
                  << state.s_box_biases[i] << std::endl;
    }

    std::cout << "\033[0m" << std::endl;
}

// Constructors and Destructors
feistel::feistel(int block_size, int max_rounds, std::vector<int> ip,
                 std::vector<int> fp, int num_sboxes, int sbox_in,
                 int sbox_out, std::vector<s_box> sboxes,
                 std::vector<int> prev_sbox, std::vector<int> post_sbox,
                 int key_size, std::vector<std::vector<int>> key_schedule, bitstring master_key) : key(master_key)
{
    // Check if the parameters are valid
    assert(block_size > 0 && max_rounds > 0);
    assert(block_size % 2 == 0);

    // Permutation size checks
    assert(ip.size() == block_size);
    assert(fp.size() == block_size);
    assert(prev_sbox.size() == sbox_in*num_sboxes);
    assert(post_sbox.size() == sbox_out*num_sboxes);
    assert(post_sbox.size() == block_size/2);

    // Key Schedule & S-Box
    assert(key_schedule.size() == max_rounds);
    for (int i = 0; i < max_rounds; i++)
    {
        assert(key_schedule[i].size() == sbox_in*num_sboxes);
        for (int j = 0; j < sbox_in*num_sboxes; j++)
        {
            assert(key_schedule[i][j] >= 0 && key_schedule[i][j] < key_size);
        }
    }
    assert(sboxes.size() == num_sboxes);
    assert(master_key.get_size() == key_size);

    // Populate
    this->block_size = block_size;
    this->max_rounds = max_rounds;
    this->ip = placement(block_size, block_size, ip);
    this->fp = placement(block_size, block_size, fp);
    this->num_sboxes = num_sboxes;
    this->sbox_in = sbox_in;
    this->sbox_out = sbox_out;
    this->sboxes = sboxes;
    this->prev_sbox = placement(block_size/2, sbox_in*num_sboxes, prev_sbox);
    this->post_sbox = placement(block_size/2, block_size/2, post_sbox);
    this->key_size = key_size;
    this->key_schedule = key_schedule;
}

feistel::~feistel()
{
    // Destructor Logic
}

// Apply Round Function (Y = F(X, K))
bitstring feistel::round_function(bitstring input, bitstring round_key)
{
    // Check if the input and round key sizes are valid
    assert(input.get_size() == this->block_size/2);
    assert(round_key.get_size() == this->sbox_in*this->num_sboxes);

    // Apply expansion layer
    bitstring expanded_input = input.place(this->prev_sbox);

    // Key mixing layer
    bitstring mixed_input = expanded_input ^ round_key;

    // Apply S-Box layer
    bitstring sbox_output = bitstring(round_key.get_size());
    for (int i = 0; i < this->num_sboxes; i++)
    {
        // Get slice of the input
        int s_in = mixed_input.get_slice_int(i*this->sbox_in, (i+1)*this->sbox_in); 
        // Apply S-Box
        int s_out = this->sboxes[i].eval(s_in);
        // Set the output
        sbox_output.set_slice(i*this->sbox_out, (i+1)*this->sbox_out, s_out);
    }

    // Apply post-S-Box layer
    bitstring post_sbox_output = sbox_output.place(this->post_sbox);

    return post_sbox_output;
} 

// Encrypt
bitstring feistel::encrypt(bitstring plaintext, int rounds)
{
    // Check if the plaintext and key sizes are valid
    assert(plaintext.get_size() == this->block_size);
    assert(key.get_size() == this->key_size);
    assert(rounds <= this->max_rounds);

    // Apply initial permutation
    bitstring permuted_input = plaintext.place(this->ip);

    // Split into two halves
    bitstring left_half = permuted_input.get_slice(0, this->block_size/2);
    bitstring right_half = permuted_input.get_slice(this->block_size/2, this->block_size);

    // Apply rounds
    for (int i = 0; i < rounds; i++)
    {
        // Get round key placement
        placement temp_place = placement(this->key_size, this->key_schedule[i].size(), this->key_schedule[i]);
        // Get round key
        bitstring round_key = key.place(temp_place);
        
        // Apply round function
        bitstring round_output = round_function(right_half, round_key);
        // Apply XOR
        left_half = left_half ^ round_output;
        // Swap halves
        if (i < rounds - 1)
        {
          bitstring temp = left_half;
          left_half = right_half;
          right_half = temp; 
        }
    }

    // Combine halves
    bitstring combined_output = left_half + right_half;

    // Apply final permutation
    bitstring ciphertext = combined_output.place(this->fp);

    return ciphertext;
}

// Decrypt
bitstring feistel::decrypt(bitstring ciphertext, int rounds)
{
    // Check if the ciphertext and key sizes are valid
    assert(ciphertext.get_size() == this->block_size);
    assert(key.get_size() == this->key_size);
    assert(rounds <= this->max_rounds);

    // Apply initial permutation
    bitstring permuted_input = ciphertext.inv_place(this->fp);

    // Split into two halves
    bitstring left_half = permuted_input.get_slice(0, this->block_size/2);
    bitstring right_half = permuted_input.get_slice(this->block_size/2, this->block_size);

    // Apply rounds in reverse order
    for (int i = rounds - 1; i >= 0; i--)
    {
        // Get round key placement
        placement temp_place = placement(this->key_size, this->key_schedule[i].size(), this->key_schedule[i]);
        // Get round key
        bitstring round_key = key.place(temp_place);
        
        // Apply round function
        bitstring round_output = round_function(right_half, round_key);
        // Apply XOR
        left_half = left_half ^ round_output;
        // Swap halves
        if (i > 0)
        {
          bitstring temp = left_half;
          left_half = right_half;
          right_half = temp;
        }
    }

    // Combine halves
    bitstring combined_output = left_half + right_half;

    // Apply final permutation
    bitstring plaintext = combined_output.inv_place(this->ip);

    return plaintext;
}

// Round Approximations
std::tuple<bitstring, bitstring, bitstring> feistel::round_approx(int s_box_num, 
                                                     int input_mask,
                                                     int output_mask)
{
    // Check if the S-Box number is valid
    assert(s_box_num >= 0 && s_box_num < this->num_sboxes);

    // Get the S-Box
    s_box sbox = this->sboxes[s_box_num];

    // Apply the input mask (apply, pseudo-inverse, and apply)
    bitstring slayer_input = bitstring(this->sbox_in * this->num_sboxes);
    slayer_input.set_slice(s_box_num * this->sbox_in, (s_box_num + 1) * this->sbox_in, input_mask);
    bitstring main_input = slayer_input.pseudo_inv_place(this->prev_sbox);
  
    // Iterate over S-Boxes and decide output masks
    bitstring slayer_output = bitstring(this->sbox_out * this->num_sboxes);
    slayer_output.set_slice(s_box_num * this->sbox_out, (s_box_num + 1) * this->sbox_out, output_mask);

    // Get the output mask
    bitstring main_output = slayer_output.place(this->post_sbox);

    // Return
    return std::make_tuple(main_input, slayer_input, main_output);
}

// Find lin trail
trail_state feistel::find_linear_trail(trail_state state, int rounds, bool start)
{
    // If start, initialize the state
    if (start)
    {
        // Net Info
        state.total_rounds = rounds;
        state.total_sboxes = this->num_sboxes;
        state.pres_round = 0;
        state.pres_sbox = 0;
        state.dummy = true;

        // Roundwise Info
        for (int i = 0; i < rounds; i++)
        {
            // Input Masks
            state.input_masks.push_back(bitstring(this->block_size/2));
            // Output Masks
            state.output_masks.push_back(bitstring(this->block_size/2));
            // Key Masks
            state.key_masks.push_back(bitstring(this->sbox_in*this->num_sboxes));
            // Round Biases
            state.round_biases.push_back(0.0);
        }

        // Current Round Info
        for (int i = 0; i < this->num_sboxes; i++)
        {
            // Alphas
            state.alphas.push_back(0);
            // Betas
            state.betas.push_back(0);
            // S-Box Biases
            state.s_box_biases.push_back(0.0);
        }
    }

    // When pres_round = 0
    if (state.pres_round == 0)
    {
        // For every candidate of round-output-mask, find the best candidate for round-input-mask
        // For the first round, we can safely assume we only work with one active S-Box at a time
        // Ignore 0-output-masks
        
        // Keep track of possible new bias
        std::vector<float> new_biases = state.round_biases;

        for (int i = 0; i < this->num_sboxes; i++)
        {
            // Get the S-Box
            s_box sbox = this->sboxes[i];

            // Iterate over output masks
            for (int j = 1; j < (1 << this->sbox_out); j++)
            {
                // Get the output mask
                int output_mask = j;

                // Best possible lat-entry for output mask
                lat_entry best_entry = sbox.get_lat_top_out(output_mask);

                // Update the new biases
                new_biases[0] = (best_entry.bias)/(1 << this->sbox_in);

                // If evaluated bias is better than current, update
                if (state.dummy || eval_bias(new_biases, rounds) >= eval_bias(state.round_biases, rounds))
                {
                    // Expand s-box masks into round approximations 
                    int input_mask = best_entry.a;
                    std::tuple<bitstring, bitstring, bitstring> approx = round_approx(i, input_mask, output_mask);
                    // Update the state
                    state.input_masks[0] = std::get<0>(approx);
                    state.key_masks[0] = std::get<1>(approx);
                    state.output_masks[0] = std::get<2>(approx);
                    state.round_biases[0] = new_biases[0];
                    std::cout << "SBOX " << i << std::endl;
                    print_state(state);
                    std::cout << std::endl;
                    // Call the function again
                    if (state.total_rounds > 1)
                    {
                        state.pres_round = 1;
                        state = find_linear_trail(state, rounds, false);
                        new_biases = state.round_biases;
                    }
                    else
                    {
                        // End of recursion
                        if (state.dummy) state.dummy = false;
                    }
                }
            }
        }
    }

    // When pres_round = 1
    else if (state.pres_round == 1)
    {
        // We iterate over all input-mask and output-mask pairs for each S-Box
        // Again, for this round, we only work with one active S-Box at a time

        // Keep track of possible new bias
        std::vector<float> new_biases = state.round_biases;

        // Iterate over S-Boxes
        for (int i = 0; i < this->num_sboxes; i++)
        {
            // Get the S-Box
            s_box sbox = this->sboxes[i];

            // Get lat_entries (automatically sorted)
            std::vector<lat_entry> lat_entries = sbox.get_lat();

            // Iterate over vector
            for (int j = 0; j < lat_entries.size(); j++)
            {
                // Get the entry
                lat_entry entry = lat_entries[j];

                // Get the input and output masks
                int input_mask = entry.a;
                int output_mask = entry.b;

                // Update the new biases
                new_biases[1] = (entry.bias)/(1 << this->sbox_in);

                // If evaluated bias is better than current, update
                if (state.dummy || eval_bias(new_biases, rounds) >= eval_bias(state.round_biases, rounds))
                {
                    // Expand s-box masks into round approximations 
                    std::tuple<bitstring, bitstring, bitstring> approx = round_approx(i, input_mask, output_mask);
                    // Update the state
                    state.input_masks[1] = std::get<0>(approx);
                    state.key_masks[1] = std::get<1>(approx);
                    state.output_masks[1] = std::get<2>(approx);
                    state.round_biases[1] = new_biases[1];
                    std::cout << "SBOX " << i << std::endl;
                    print_state(state);
                    std::cout << std::endl;
                    // Call the function again
                    if (state.total_rounds > 2)
                    {
                        state.pres_round = 2;
                        state = find_linear_trail(state, rounds, false);
                        new_biases = state.round_biases;
                    }
                    else
                    {
                        // End of recursion
                        if (state.dummy) state.dummy = false;
                    }
                }
            }
        }
    }

    // When pres_round lies in 2, 3... total_rounds-2
    else if (state.pres_round < state.total_rounds - 1)
    {
        // Get output mask (O[i] = O[i-2] + I[i-1])
        bitstring output_mask = state.output_masks[state.pres_round - 2] ^ state.input_masks[state.pres_round - 1];
        bitstring slayer_output = output_mask.inv_place(this->post_sbox);
        
        // We perform this but over all S-Boxes via recursion on pres_sbox. 
        if (state.pres_sbox < this->num_sboxes - 1)
        {
            // If state.pres_sbox == 0
            if (state.pres_sbox == 0)
            {
                // Set the all pres_rounds masks to all 0s
                state.input_masks[state.pres_round] = bitstring(this->block_size/2);
                state.key_masks[state.pres_round] = bitstring(this->sbox_in*this->num_sboxes);
                state.output_masks[state.pres_round] = bitstring(this->block_size/2);
            }

            // Get the S-Box
            s_box sbox = this->sboxes[state.pres_sbox];

            // Get output mask
            int op_mask = slayer_output.get_slice_int(state.pres_sbox*this->sbox_out, (state.pres_sbox + 1)*this->sbox_out);
            // Get lat_entries just for this output mask
            std::vector<lat_entry> lat_entries = sbox.get_lat_outs(op_mask);

            // Copy both biases
            std::vector<float> new_round_biases = state.round_biases;
            std::vector<float> new_sbox_biases = state.s_box_biases;

            // Iterate over vector
            for (int j = 0; j < lat_entries.size(); j++)
            {
                // Get the entry
                lat_entry entry = lat_entries[j];

                // Get the input and output masks
                int input_mask = entry.a;
                int output_mask = entry.b;

                // Update new s_box biases
                new_sbox_biases[state.pres_sbox] = (entry.bias)/(1 << this->sbox_in);
                float total_sbox_bias = eval_bias(new_sbox_biases, state.pres_sbox + 1);
                new_round_biases[state.pres_round] = total_sbox_bias;

                // If evaluated bias is better than current, update
                if (state.dummy || eval_bias(new_round_biases, rounds) >= eval_bias(state.round_biases, rounds))
                {
                    // Expand s-box masks into round approximations 
                    std::tuple<bitstring, bitstring, bitstring> approx = round_approx(state.pres_sbox, input_mask, output_mask);
                    // Update the state
                    state.input_masks[state.pres_round] = state.input_masks[state.pres_round] | std::get<0>(approx);
                    state.key_masks[state.pres_round] = state.key_masks[state.pres_round] | std::get<1>(approx);
                    state.output_masks[state.pres_round] = state.output_masks[state.pres_round] | std::get<2>(approx);
                    state.s_box_biases[state.pres_sbox] = new_sbox_biases[state.pres_sbox];
                    print_state(state);
                    // Call the function again
                    state.pres_sbox++;
                    state = find_linear_trail(state, rounds, false);
                    new_sbox_biases = state.s_box_biases;
                }
            }
        }
        // Final S-Box
        else
        {
            // Get the S-Box
            s_box sbox = this->sboxes[state.pres_sbox];

            // Get output mask
            int op_mask = slayer_output.get_slice_int(state.pres_sbox*this->sbox_out, (state.pres_sbox + 1)*this->sbox_out);
            // Get lat_entries just for this output mask
            std::vector<lat_entry> lat_entries = sbox.get_lat_outs(op_mask);

            // Copy both biases
            std::vector<float> new_round_biases = state.round_biases;
            std::vector<float> new_sbox_biases = state.s_box_biases;

            // Iterate over vector
            for (int j = 0; j < lat_entries.size(); j++)
            {
                // Get the entry
                lat_entry entry = lat_entries[j];

                // Get the input and output masks
                int input_mask = entry.a;
                int output_mask = entry.b;

                // Update new s_box biases
                new_sbox_biases[state.pres_sbox] = (entry.bias)/(1 << this->sbox_in);
                float total_sbox_bias = eval_bias(new_sbox_biases, state.pres_sbox + 1);
                new_round_biases[state.pres_round] = total_sbox_bias;

                // If evaluated bias is better than current, update
                if (state.dummy || eval_bias(new_round_biases, rounds) >= eval_bias(state.round_biases, rounds))
                {
                    // Expand s-box masks into round approximations 
                    std::tuple<bitstring, bitstring, bitstring> approx = round_approx(state.pres_sbox, input_mask, output_mask);
                    // Update the state
                    state.input_masks[state.pres_round] = state.input_masks[state.pres_round] | std::get<0>(approx);
                    state.key_masks[state.pres_round] = state.key_masks[state.pres_round] | std::get<1>(approx);
                    state.output_masks[state.pres_round] = state.output_masks[state.pres_round] | std::get<2>(approx);
                    state.s_box_biases[state.pres_sbox] = new_sbox_biases[state.pres_sbox];
                    state.round_biases[state.pres_round] = new_round_biases[state.pres_round];
                    print_state(state);
                    // Call the function again
                    state.pres_sbox = 0;
                    state.pres_round += 1 + state.pres_sbox;
                    state = find_linear_trail(state, rounds, false);
                    print_state(state);
                    new_sbox_biases = state.s_box_biases;
                    new_round_biases = state.round_biases;
                }
            }
        }
    }
    
    // Last Round
    else
    {
        // Get output mask (O[i] = O[i-2] + I[i-1]) 
        bitstring output_mask = state.output_masks[state.pres_round - 2] ^ state.input_masks[state.pres_round - 1];
        bitstring slayer_output = output_mask.inv_place(this->post_sbox);
        std::cout << "Output Mask: ";
        output_mask.print();
        std::cout << "Slayer Output: ";
        slayer_output.print();

        // We do the same as before, except just fetch the best possible input mask
        if (state.pres_sbox < this->num_sboxes - 1)
        {
            // If state.pres_sbox == 0
            if (state.pres_sbox == 0)
            {
                // Set the all pres_rounds masks to all 0s
                state.input_masks[state.pres_round] = bitstring(this->block_size/2);
                state.key_masks[state.pres_round] = bitstring(this->sbox_in*this->num_sboxes);
                state.output_masks[state.pres_round] = bitstring(this->block_size/2);
            }

            // Get the S-Box
            s_box sbox = this->sboxes[state.pres_sbox];

            // Get output mask
            int op_mask = slayer_output.get_slice_int(state.pres_sbox*this->sbox_out, (state.pres_sbox + 1)*this->sbox_out);
            // Get best lat_entry for this output mask
            lat_entry best_entry = sbox.get_lat_top_out(op_mask);

            // Copy both biases
            std::vector<float> new_round_biases = state.round_biases;
            std::vector<float> new_sbox_biases = state.s_box_biases;

            // Evaluate biases
            new_sbox_biases[state.pres_sbox] = (best_entry.bias)/(1 << this->sbox_in);
            float total_sbox_bias = eval_bias(new_sbox_biases, state.pres_sbox + 1);
            new_round_biases[state.pres_round] = total_sbox_bias;

            // If evaluated bias is better than current, update
            if (state.dummy || eval_bias(new_round_biases, rounds) >= eval_bias(state.round_biases, rounds))
            {
                // Expand s-box masks into round approximations 
                int input_mask = best_entry.a;
                std::tuple<bitstring, bitstring, bitstring> approx = round_approx(state.pres_sbox, input_mask, op_mask);
                // Update the state
                state.input_masks[state.pres_round] = state.input_masks[state.pres_round] | std::get<0>(approx);
                state.key_masks[state.pres_round] = state.key_masks[state.pres_round] | std::get<1>(approx);
                state.output_masks[state.pres_round] = state.output_masks[state.pres_round] | std::get<2>(approx);
                state.s_box_biases[state.pres_sbox] = new_sbox_biases[state.pres_sbox];
                print_state(state);
                // Call the function again
                state.pres_sbox++;
                state = find_linear_trail(state, rounds, false);
                new_sbox_biases = state.s_box_biases;
            }
        }
        // If last s-box
        else
        {
            // Get the S-Box
            s_box sbox = this->sboxes[state.pres_sbox];

            // Get output mask
            int op_mask = slayer_output.get_slice_int(state.pres_sbox*this->sbox_out, (state.pres_sbox + 1)*this->sbox_out);
            // Get best lat_entry for this output mask
            lat_entry best_entry = sbox.get_lat_top_out(op_mask);

            // Copy both biases
            std::vector<float> new_round_biases = state.round_biases;
            std::vector<float> new_sbox_biases = state.s_box_biases;

            // Evaluate biases
            new_sbox_biases[state.pres_sbox] = (best_entry.bias)/(1 << this->sbox_in);
            float total_sbox_bias = eval_bias(new_sbox_biases, state.pres_sbox + 1);
            new_round_biases[state.pres_round] = total_sbox_bias;

            // If evaluated bias is better than current, update
            if (state.dummy || eval_bias(new_round_biases, rounds) >= eval_bias(state.round_biases, rounds))
            {
                // Expand s-box masks into round approximations 
                int input_mask = best_entry.a;
                std::tuple<bitstring, bitstring, bitstring> approx = round_approx(state.pres_sbox, input_mask, op_mask);
                // Update the state
                state.input_masks[state.pres_round] = state.input_masks[state.pres_round] | std::get<0>(approx);
                state.key_masks[state.pres_round] = state.key_masks[state.pres_round] | std::get<1>(approx);
                state.output_masks[state.pres_round] = state.output_masks[state.pres_round] | std::get<2>(approx);
                state.s_box_biases[state.pres_sbox] = new_sbox_biases[state.pres_sbox];
                state.round_biases[state.pres_round] = new_round_biases[state.pres_round];
                print_state(state);
                // End of recursion
                state.pres_sbox = 0;
                state.pres_round += state.total_sboxes;
                if (state.dummy) state.dummy = false;
                new_sbox_biases = state.s_box_biases;
                new_round_biases = state.round_biases;
            }
        }
    }


    // Return
    state.pres_round--;
    std::cout << "Going from " << state.pres_round + 1 << " to " << state.pres_round << std::endl;
    return state;
}
