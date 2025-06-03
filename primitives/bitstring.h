// Class to represent bitstrings of arbitrary size

#ifndef BITSTRING_H
#define BITSTRING_H

// Standard Library Imports
#include <iostream>
#include <vector>
#include <string>

// Custom Library Imports
#include "placement.h"

#define CHUNK_SIZE sizeof(int) * 8 // Size of each chunk in bits

// Class def
class bitstring
{
  public:
    // Member variables
    int size;                   // Size of the bitstring
    std::vector<int> chunks;    // Chunks of the bitstring
    int num_chunks;             // Number of chunks

    // Constructors & Destructors
    bitstring(int size);
    ~bitstring();

    // Setters
    void set_bit(int index, int value);             // Set bit in big-endian
    void set_slice(int start, int end, int value);  // Set slice in big-endian

    // Getters
    int get_size() const { return this->size; }                 // Get size
    int get_bit(int index) const;                               // Get bit in big-endian
    bitstring get_slice(int start, int end) const;              // Get slice in big-endian
    int get_slice_int(int start, int end) const;                // Get small slice in big-endian (int)
    
    // Get/print in string format
    std::string get_string();                       // Get string in big-endian
    void print();                                   // Print in big-endian

    // Bitwise operations to be overloaded
    bitstring operator&(const bitstring &other);     // Bitwise AND
    bitstring operator|(const bitstring &other);     // Bitwise OR
    bitstring operator^(const bitstring &other);     // Bitwise XOR

    // Assignment overload
    bitstring &operator=(const bitstring &other);    // Assignment    

    // Concatenation overload
    bitstring operator+(const bitstring &other);     // Concatenation
    
    // Product overload
    int operator*(const bitstring &other);     // Inner product

    // Hamming weight
    int hamming_weight() const;                      // Hamming weight

    // Apply placement on the bitstring
    bitstring place(placement &p); // Apply placement on the bitstring
    bitstring inv_place(placement &p); // Apply inverse placement on the bitstring
    bitstring pseudo_inv_place(placement &p); // Apply pseudo-inverse placement on the bitstring
};
   
#endif
