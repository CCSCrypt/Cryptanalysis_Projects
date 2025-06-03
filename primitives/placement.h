// Class to represent placement layers, which 
// include possible expansions, permutations, 
// contractions or other transformations

#ifndef PLACEMENT_H
#define PLACEMENT_H

// Standard Library Imports
#include <iostream>
#include <vector>
#include <cassert>

// Custom Library Imports
// None for now

class placement
{
  private:
    int input_size;         // Input size
    int output_size;        // Output size
    std::vector<int> placement_table; // Placement table
    std::vector<int> inverse_table;   // Inverse placement table (if invertible, otherwise empty)
    bool invertible; // Flag to check if the placement is invertible

  public:
    // Constructors & Destructors
    placement(); // Default constructor
    placement(int input_size, int output_size, std::vector<int> placement_table);
    ~placement();
    void compute_inv();

    // Accessors
    int get_input_size() const { return this->input_size; }
    int get_output_size() const { return this->output_size; }
    std::vector<int> get_placement_table() const;
    std::vector<int> get_inverse_table() const;

    // Apply placement on any vector-template
    template <typename T>
    std::vector<T> apply_placement(std::vector<T> input_vector)
    {
        // Check if the input vector size matches the input size
        assert(input_vector.size() == this->input_size);

        // Initialize output vector
        std::vector<T> output_vector(this->output_size);

        // Apply placement
        for (int i = 0; i < this->input_size; i++)
        {
            output_vector[this->placement_table[i]] = input_vector[i];
        }

        return output_vector;
    }
    // Apply inverse placement on any vector-template
    template <typename T>
    std::vector<T> apply_inverse_placement(std::vector<T> input_vector)
    {
        // Check if the input vector size matches the output size
        assert(input_vector.size() == this->output_size);

        // Initialize output vector
        std::vector<T> output_vector(this->input_size);

        // Apply inverse placement
        for (int i = 0; i < this->output_size; i++)
        {
            output_vector[this->inverse_table[i]] = input_vector[i];
        }

        return output_vector;
    }
};

#endif 
