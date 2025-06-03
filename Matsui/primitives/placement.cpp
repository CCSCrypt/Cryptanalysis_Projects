// Placement Definitions
#include "placement.h"

// Include Libraries 
#include <iostream>
#include <vector>
#include <cassert>

// Constructors and Destructors
placement::placement()
{
    // Default constructor
    this->input_size = 0;
    this->output_size = 0;
    this->placement_table = {};
    this->inverse_table = {};
}

placement::placement(int input_size, int output_size, std::vector<int> placement_table)
{
    // Check if the input and output sizes are valid
    assert(input_size > 0 && output_size > 0);
    assert(placement_table.size() == output_size);

    // Populate
    this->input_size = input_size;
    this->output_size = output_size;
    this->placement_table = placement_table;

    // Compute inverse table
    if(input_size == output_size) this->compute_inv();
}

placement::~placement()
{
    // Destructor
    // this->placement_table.clear();
    // this->inverse_table.clear();
}

// Compute inverse placement table
void placement::compute_inv()
{
    // Input-size == Output-size
    assert(this->input_size == this->output_size);

    // Initialize inverse table
    this->inverse_table.resize(this->input_size, -1);
    bool invertible = true;
    for (int i = 0; i < this->input_size; i++)
    {
        // Check if the placement is invertible
        if (this->inverse_table[this->placement_table[i]] != -1)
        {
            invertible = false;
            break;
        }
        this->inverse_table[this->placement_table[i]] = i;
    }
    if (!invertible)
    {
        std::cout << "Placement is not invertible!" << std::endl;
        this->inverse_table.clear();
    }

    // Set invertible flag
    this->invertible = invertible;
    return;
}

// Accessors
std::vector<int> placement::get_placement_table() const
{
    return this->placement_table;
}

std::vector<int> placement::get_inverse_table() const
{
    return this->inverse_table;
}
