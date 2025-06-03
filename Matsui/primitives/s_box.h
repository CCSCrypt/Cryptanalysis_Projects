// Class to represent S-Boxes of arbitrary input
// and output size

#ifndef S_BOX_H
#define S_BOX_H

// Library Imports
#include <iostream>
#include <vector>
#include <utility>

// Struct definition for LAT entries
struct lat_entry
{
  float bias = 0.0;
  int a = 0;   // Input mask
  int b = 0;   // Output mask
  lat_entry() = default; 
};

// Comparision function for sorting LAT entries
bool gt_lat(const lat_entry &a, const lat_entry &b);

// Class definition for S-Box
class s_box
{
  private:
    std::vector<int> table;         // S-Box table
    int in;                    // Input size
    int out;                   // Output size
    std::vector<lat_entry> lat;              // Linear Approximation Table

  public:
    // Constructors & Destructors
    s_box(int in, int out, std::vector<int> table);
    ~s_box();

    // Accessors
    int eval(int input);
    std::vector<int> get_table();
    std::vector<lat_entry> get_lat();

    // Linear Analysis
    void gen_lat();
    std::vector<lat_entry> get_lat_thr(int thr);
    std::vector<lat_entry> get_lat_top(int top);
    lat_entry get_lat_top_inp(int input_mask);
    lat_entry get_lat_top_out(int output_mask);
    std::vector<lat_entry> get_lat_outs(int output_mask);
};

#endif
