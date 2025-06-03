// S-Box Class Definitions
#include "s_box.h"

// Import Libraries
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>

// Bitwise Dot-product
int dot_product(int a, int b, int n)
{
  // Assert that the inputs are valid
  int N = (1 << n);
  assert(a < N);
  assert(b < N);

  // Compute 
  int res = 0;
  for (int i = 0; i < n; i++) res ^= ((a >> i) & 1) & ((b >> i) & 1);
  return res;
}

// Comparision function for sorting LAT entries
bool gt_lat(const lat_entry &a, const lat_entry &b)
{
  int abs_a = (a.bias < 0) ? -a.bias : a.bias;
  int abs_b = (b.bias < 0) ? -b.bias : b.bias;
  return (abs_a > abs_b);
}

// Constructors and Destructors
s_box::s_box(int in, int out, std::vector<int> table)
{
    // Check if the input and output sizes are valid
    assert(in > 0 && out > 0);
    assert(table.size() == (1 << in));
    for (int i = 0; i < table.size(); i++) assert(table[i] >= 0 && table[i] < (1 << out));

    // Populate
    this->in = in; 
    this->out = out;
    this->table = table;

    // Generate LAT and store
    this->gen_lat();
}

s_box::~s_box()
{
    // Destructor
    // this->table.clear();
    // this->lat.clear();
}

// Accessors
int s_box::eval(int input)
{
    // Check if the input is valid
    assert(input >= 0 && input < (1 << this->in));

    // Return the S-Box output
    return this->table[input];
}

std::vector<int> s_box::get_table()
{
    // Return the S-Box table
    return this->table;
}

std::vector<lat_entry> s_box::get_lat()
{
    // Return the Linear Approximation Table
    return this->lat;
}

// Linear Analysis
void s_box::gen_lat()
{
    // Resize the LAT
    int N = (1 << this->in);
    int M = (1 << this->out);
    std::vector<float> values(N * M, 0);

    // Compute the LAT
    for (int x = 0; x < N; x++)
    {
        int y = this->eval(x);
        for (int a = 0; a < N; a++)
        {
           int xa = dot_product(x, a, this->in); 
           for (int b = 0; b < M; b++)
           {
               int yb = dot_product(y, b, this->out);
               values[b*N + a] += ((xa ^ yb) ? -1 : 1);
           }
        }
    }

    // Create LAT
    std::vector<lat_entry> lat;
    for (int a = 0; a < N; a++)
    {
        for (int b = 0; b < M; b++)
        {
            lat_entry entry;
            entry.bias = values[b*N + a]/2;
            entry.a = a;
            entry.b = b;
            lat.push_back(entry);
        }
    }

    // Sort the LAT
    std::sort(lat.begin(), lat.end(), gt_lat);

    // Store the sorted LAT
    this->lat = lat;
}

// Get LAT entries above a threshold
std::vector<lat_entry> s_box::get_lat_thr(int thr)
{
    // Create a vector to store the entries above the threshold
    std::vector<lat_entry> lat_thr;
    for (int i = 0; i < this->lat.size(); i++)
    {
        int abs_bias = (this->lat[i].bias < 0) ? -this->lat[i].bias : this->lat[i].bias;
        if (abs_bias >= thr)
        {
            lat_thr.push_back(this->lat[i]);
        }
        else break;
    }
    return lat_thr;
}

// Get the top N LAT entries
std::vector<lat_entry> s_box::get_lat_top(int top)
{
    // Create a vector to store the top N entries
    std::vector<lat_entry> lat_top;
    for (int i = 0; i < top && i < this->lat.size(); i++)
    {
        lat_top.push_back(this->lat[i]);
    }
    return lat_top;
}

// Get the top LAT entry for a given input mask 
lat_entry s_box::get_lat_top_inp(int input_mask)
{
    // Create a vector to store the top entry
    lat_entry lat_top;
    for (int i = 0; i < this->lat.size(); i++)
    {
        if ((this->lat[i].a == input_mask))
        {
            lat_top = this->lat[i];
            break;
        }
    }
    return lat_top;
}

// Get the top LAT entry for a given output mask
lat_entry s_box::get_lat_top_out(int output_mask)
{
    // Create a vector to store the top entry
    lat_entry lat_top;
    for (int i = 0; i < this->lat.size(); i++)
    {
        if ((this->lat[i].b == output_mask))
        {
            lat_top = this->lat[i];
            break;
        }
    }
    return lat_top;
}

// Get the LAT entries for a given output mask
std::vector<lat_entry> s_box::get_lat_outs(int output_mask)
{
    // Create a vector to store the entries
    std::vector<lat_entry> lat_outs;
    for (int i = 0; i < this->lat.size(); i++)
    {
        if ((this->lat[i].b == output_mask))
        {
            lat_outs.push_back(this->lat[i]);
        }
    }
    return lat_outs;
}
