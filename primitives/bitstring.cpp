// Implementation of the BitString class
#include "bitstring.h"

// Include Libraries
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

// Constructors
bitstring::bitstring(int size)
{
    // Check if the size is valid
    assert(size > 0);

    // Initialize member variables
    this->size = size;
    this->num_chunks = (size/CHUNK_SIZE) + ((size % CHUNK_SIZE) ? 1 : 0); // Calculate number of chunks
    this->chunks.resize(this->num_chunks, 0); // Initialize chunks to zero
}

// Destructor
bitstring::~bitstring()
{
    // Destructor does not need to do anything
    // since chunks are automatically deallocated
}

// Set bit in big-endian
void bitstring::set_bit(int index, int value)
{
    // Check if the index is valid
    assert(index >= 0);
    assert(value == 0 || value == 1);

    // Calculate chunk and bit position
    int chunk_index = index / CHUNK_SIZE;
    int bit_position = index % CHUNK_SIZE;

    // Set the bit
    if (value)
    {
        this->chunks[chunk_index] |= (1 << (CHUNK_SIZE - 1 - bit_position));
    }
    else
        this->chunks[chunk_index] &= ~(1 << (CHUNK_SIZE - 1 - bit_position));

    return;
}

// Set slice in big-endian
void bitstring::set_slice(int start, int end, int value)
{
    // Check if the start and end indices are valid
    assert(start >= 0 && end <= this->size);
    assert(start < end);

    // Iterate
    for (int i = start; i < end; i++)
    {
        // Chunk and bit position
        int chunk_index = i / CHUNK_SIZE;
        int bit_position = i % CHUNK_SIZE;
        // Value
        int bit = (value & (1 << (end - 1 - i))) ? 1 : 0;
        // Set the bit
        if (bit)
            this->chunks[chunk_index] |= (1 << (CHUNK_SIZE - 1 - bit_position));
        else
            this->chunks[chunk_index] &= ~(1 << (CHUNK_SIZE - 1 - bit_position));
    }

    return;
}

// Get bit in big-endian
int bitstring::get_bit(int index) const
{
    // Check if the index is valid
    assert(index >= 0);

    // Calculate chunk and bit position
    int chunk_index = index / CHUNK_SIZE;
    int bit_position = index % CHUNK_SIZE;

    // Get the bit
    return (this->chunks[chunk_index] >> (CHUNK_SIZE - 1 - bit_position)) & 1;
} 

// Get slice in big-endian
bitstring bitstring::get_slice(int start, int end) const
{
    // Check if the start and end indices are valid
    assert(start >= 0 && end <= this->size);
    assert(start < end);

    // Create a new bitstring for the slice
    bitstring slice(end - start);

    // Iterate
    for (int i = start; i < end; i++)
    {
        // Set the bit in the slice
        slice.set_bit(i - start, this->get_bit(i));
    }

    return slice;
} 

// Get small slice in big-endian (int)
int bitstring::get_slice_int(int start, int end) const
{
    // Check if the start and end indices are valid
    assert(start >= 0 && end <= this->size);
    assert(start < end);
    assert(end - start <= CHUNK_SIZE); // Ensure the slice fits in an int

    // Initialize value
    int value = 0;

    // Iterate
    for (int i = start; i < end; i++)
    {
        // Set the bit in the value
        value |= (this->get_bit(i) << (end - 1 - i));
    }

    return value;
} 

// Get string in big-endian
std::string bitstring::get_string()
{
    // Initialize string
    std::string str = "";

    // Iterate
    for (int i = 0; i < this->size; i++)
    {
        // Append the bit to the string
        str += std::to_string(this->get_bit(i));
    }

    return str;
}

// Print in big-endian
void bitstring::print()
{
    // Print the bitstring
    std::cout << this->get_string() << std::endl;
}

// Bitwise AND
bitstring bitstring::operator&(const bitstring &other)
{
    // Check if the sizes match
    assert(this->size == other.size);

    // Create a new bitstring for the result
    bitstring result(this->size);

    // Iterate
    for (int i = 0; i < this->num_chunks; i++)
    {
        // Perform bitwise AND
        result.chunks[i] = this->chunks[i] & other.chunks[i];
    }

    return result;
}

// Bitwise OR
bitstring bitstring::operator|(const bitstring &other)
{
    // Check if the sizes match
    assert(this->size == other.size);

    // Create a new bitstring for the result
    bitstring result(this->size);

    // Iterate
    for (int i = 0; i < this->num_chunks; i++)
    {
        // Perform bitwise OR
        result.chunks[i] = this->chunks[i] | other.chunks[i];
    }

    return result;
}

// Bitwise XOR
bitstring bitstring::operator^(const bitstring &other)
{
    // Check if the sizes match
    assert(this->size == other.size);

    // Create a new bitstring for the result
    bitstring result(this->size);

    // Iterate
    for (int i = 0; i < this->num_chunks; i++)
    {
        // Perform bitwise XOR
        result.chunks[i] = this->chunks[i] ^ other.chunks[i];
    }

    return result;
}

// Assignment operator
bitstring &bitstring::operator=(const bitstring &other)
{
    // Copy the size
    this->size = other.size;

    // Copy the chunks
    this->chunks = other.chunks;

    return *this;
}

// Concatenation
bitstring bitstring::operator+(const bitstring &other)
{
    // Create a new bitstring for the result
    bitstring result(this->size + other.size);

    // Copy the first bitstring
    for (int i = 0; i < this->size; i++)
    {
        result.set_bit(i, this->get_bit(i));
    }

    // Copy the second bitstring
    for (int i = 0; i < other.size; i++)
    {
        result.set_bit(i + this->size, other.get_bit(i));
    }

    return result;
}

// Inner product
int bitstring::operator*(const bitstring &other)
{
    // Check if the sizes match
    assert(this->size == other.size);

    // Initialize result
    int result = 0;

    // Iterate
    for (int i = 0; i < this->size; i++)
    {
        // Perform inner product
        result += this->get_bit(i) * other.get_bit(i);
    }

    return result%2;
}

// Hamming weight
int bitstring::hamming_weight() const
{
    // Initialize weight
    int weight = 0;

    // Iterate
    for (int i = 0; i < this->size; i++)
    {
        // Increment weight if the bit is set
        if (this->get_bit(i)) weight++;
    }

    return weight;
}

// Apply placement on the bitstring
bitstring bitstring::place(placement &p)
{
    // Output size
    int output_size = p.get_output_size();

    // Check if the placement is valid
    // assert(p.get_input_size() == this->size);

    // Create a new bitstring for the result
    bitstring result(output_size);

    // Apply placement
    for (int i = 0; i < output_size; i++) result.set_bit(i, this->get_bit(p.get_placement_table()[i]));

    return result;
}

// Apply inverse placement on the bitstring
bitstring bitstring::inv_place(placement &p)
{
    // Output size
    int output_size = p.get_input_size();

    // Check if the placement is valid
    assert(p.get_output_size() == this->size);

    // Create a new bitstring for the result
    bitstring result(output_size);

    // Apply inverse placement
    for (int i = 0; i < output_size; i++) result.set_bit(i, this->get_bit(p.get_inverse_table()[i]));

    return result;
}

// Apply pseudo-inverse placement on the bitstring
bitstring bitstring::pseudo_inv_place(placement &p)
{
    // Output size
    int output_size = p.get_input_size();

    // Check if the placement is valid
    assert(p.get_output_size() == this->size);

    // Create a shallow copy of the bitstring
    bitstring result = bitstring(output_size);
    
    // Iterate over the placement table
    for (int i = 0; i < p.get_output_size(); i++)
    {
        int index = p.get_placement_table()[i];
        result.set_bit(p.get_placement_table()[i], result.get_bit(index) | this->get_bit(i));
    }

    return result;
}


