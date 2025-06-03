// Main Function
// Scratchpad for now

// StdLibs
#include <iostream>
#include <vector>
#include <cstdlib>
#include <tuple>

// CustomLibs
#include "primitives/s_box.h"
#include "primitives/placement.h"
#include "primitives/bitstring.h"
#include "primitives/feistel.h"
#include "primitives/attack.h"

// Main
int main()
{
  // DES Initialization
  // DES IP
  std::vector<int> des_ip = {
      57, 49, 41, 33, 25, 17, 9, 1,
      59, 51, 43, 35, 27, 19, 11, 3,
      61, 53, 45, 37, 29, 21, 13, 5,
      63, 55, 47, 39, 31, 23, 15, 7,
      56, 48, 40, 32, 24, 16, 8, 0,
      58, 50, 42, 34, 26, 18, 10, 2,
      60, 52, 44, 36, 28, 20, 12, 4,
      62, 54, 46, 38, 30, 22, 14, 6};

  // DES FP
  std::vector<int> des_fp = {
      39, 7, 47, 15, 55, 23, 63, 31,
      38, 6, 46, 14, 54, 22, 62, 30,
      37, 5, 45, 13, 53, 21, 61, 29,
      36, 4, 44, 12, 52, 20, 60, 28,
      35, 3, 43, 11, 51, 19, 59, 27,
      34, 2, 42, 10, 50, 18, 58, 26,
      33, 1, 41, 9 ,49 ,17 ,57 ,25,
      32, 0, 40, 8, 48, 16, 56, 24};

  // DES Key Schedule
  std::vector<int> ks1 = {13, 16, 10, 23, 0, 4, 2, 27, 14, 5, 20, 9, 22, 18, 11, 3, 25, 7, 15, 6, 26, 19, 12, 1, 40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47, 43, 48, 38, 55, 33, 52, 45, 35, 28, 31, 37, 34};
  std::vector<int> ks2 = {12, 15, 9, 22, 63, 3, 1, 26, 13, 4, 19, 8, 21, 17, 10, 2, 24, 6, 14, 5, 25, 18, 11, 0, 39, 50, 29, 35, 45, 53, 28, 38, 49, 43, 31, 46, 42, 47, 37, 54, 32, 51, 44, 34, 27, 30, 36, 33};
  std::vector<int> ks3 = {10, 13, 7, 20, 61, 1, 63, 24, 11, 2, 17, 6, 19, 15, 8, 0, 22, 4, 12, 3, 23, 16, 9, 62, 37, 48, 27, 33, 43, 51, 26, 36, 47, 41, 29, 44, 40, 45, 35, 52, 30, 49, 42, 32, 25, 28, 34, 31};
  std::vector<int> ks4 = {8, 11, 5, 18, 59, 63, 61, 22, 9, 0, 15, 4, 17, 13, 6, 62, 20, 2, 10, 1, 21, 14, 7, 60, 35, 46, 25, 31, 41, 49, 24, 34, 45, 39, 27, 42, 38, 43, 33, 50, 28, 47, 40, 30, 23, 26, 32, 29};
  std::vector<int> ks5 = {6, 9, 3, 16, 57, 61, 59, 20, 7, 62, 13, 2, 15, 11, 4, 60, 18, 0, 8, 63, 19, 12, 5, 58, 33, 44, 23, 29, 39, 47, 22, 32, 43, 37, 25, 40, 36, 41, 31, 48, 26, 45, 38, 28, 21, 24, 30, 27};
  std::vector<int> ks6 = {4, 7, 1, 14, 55, 59, 57, 18, 5, 60, 11, 0, 13, 9, 2, 58, 16, 62, 6, 61, 17, 10, 3, 56, 31, 42, 21, 27, 37, 45, 20, 30, 41, 35, 23, 38, 34, 39, 29, 46, 24, 43, 36, 26, 19, 22, 28, 25};
  std::vector<int> ks7 = {2, 5, 63, 12, 53, 57, 55, 16, 3, 58, 9, 62, 11, 7, 0, 56, 14, 60, 4, 59, 15, 8, 1, 54, 29, 40, 19, 25, 35, 43, 18, 28, 39, 33, 21, 36, 32, 37, 27, 44, 22, 41, 34, 24, 17, 20, 26, 23};
  std::vector<int> ks8 = {1, 4, 62, 11, 52, 56, 54, 15, 2, 57, 8, 61, 10, 6, 63, 55, 13, 59, 3, 58, 14, 7, 0, 53, 28, 39, 18, 24, 34, 42, 17, 27, 38, 32, 20, 35, 31, 36, 26, 43, 21, 40, 33, 23, 16, 19, 25, 22};
  std::vector<int> ks9 = {0, 3, 61, 10, 51, 55, 53, 14, 1, 56, 7, 60, 9, 5, 62, 54, 12, 58, 2, 57, 13, 6, 63, 52, 27, 38, 17, 23, 33, 41, 16, 26, 37, 31, 19, 34, 30, 35, 25, 42, 20, 39, 32, 22, 15, 18, 24, 21};
  std::vector<int> ksA = {62, 2, 60, 9, 50, 54, 52, 13, 0, 55, 6, 59, 8, 4, 61, 53, 11, 57, 1, 56, 12, 5, 62, 51, 26, 37, 16, 22, 32, 40, 15, 25, 36, 30, 18, 33, 29, 34, 24, 41, 19, 38, 31, 21, 14, 17, 23, 20};
  std::vector<int> ksB = {60, 0, 58, 7, 48, 52, 50, 11, 62, 53, 4, 57, 6, 2, 59, 51, 9, 55, 63, 54, 10, 3, 60, 49, 24, 35, 14, 20, 30, 38, 13, 23, 34, 28, 16, 31, 27, 32, 22, 39, 17, 36, 29, 19, 12, 15, 21, 18};
  std::vector<int> ksC = {58, 62, 56, 5, 46, 50, 48, 9, 60, 51, 2, 55, 4, 0, 57, 49, 7, 53, 61, 52, 8, 1, 58, 47, 22, 33, 12, 18, 28, 36, 11, 21, 32, 26, 14, 29, 25, 30, 20, 37, 15, 34, 27, 17, 10, 13, 19, 16};
  std::vector<int> ksD = {56, 60, 54, 3, 44, 48, 46, 7, 58, 49, 0, 53, 2, 62, 55, 47, 5, 51, 59, 50, 6, 63, 56, 45, 20, 31, 10, 16, 26, 34, 9, 19, 30, 24, 12, 27, 23, 28, 18, 35, 13, 32, 25, 15, 8, 11, 17, 14};
  std::vector<int> ksE = {55, 59, 53, 2, 43, 47, 45, 6, 57, 48, 63, 52, 1, 61, 54, 46, 4, 50, 58, 49, 5, 62, 55, 44, 19, 30, 9, 15, 25, 33, 8, 18, 29, 23, 11, 26, 22, 27, 17, 34, 12, 31, 24, 14, 7, 10, 16, 13};
  std::vector<int> ksF = {54, 58, 52, 1, 42, 46, 44, 5, 56, 47, 62, 51, 0, 60, 53, 45, 3, 49, 57, 48, 4, 61, 54, 43, 18, 29, 8, 14, 24, 32, 7, 17, 28, 22, 10, 25, 21, 26, 16, 33, 11, 30, 23, 13, 6, 9, 15, 12};
  std::vector<int> ks0 = {53, 57, 51, 0, 41, 45, 43, 4, 55, 46, 61, 50, 63, 59, 52, 44, 2, 48, 56, 47, 3, 60, 53, 42, 17, 28, 7, 13, 23, 31, 6, 16, 27, 21, 9, 24, 20, 25, 15, 32, 10, 29, 22, 12, 5, 8, 14, 11};

  std::vector<std::vector<int>> key_schedule = {ks1, ks2, ks3, ks4, ks5, ks6, ks7, ks8, ks9, ksA, ksB, ksC, ksD, ksE, ksF, ks0};

  // Create expansion layer
  std::vector<int> exp = {31, 0, 1, 2, 3, 4,
                          3, 4, 5, 6, 7, 8,
                          7, 8, 9, 10, 11, 12,
                          11, 12, 13, 14, 15, 16,
                          15, 16, 17, 18, 19, 20,
                          19, 20, 21, 22, 23, 24,
                          23, 24, 25, 26, 27, 28,
                          27, 28, 29, 30, 31, 0};

  // Create post-S-Box layer (permutation)
  std::vector<int> pos = {15, 6, 19, 20, 28, 11, 27, 16,
                          0, 14, 22, 25, 4, 17, 30, 9,
                          1, 7, 23, 13, 31, 26, 2, 8,
                          18, 12, 29, 5, 21, 10, 3, 24};

  // Create S-Boxes
  std::vector<int> sbox1 = {
    14, 0, 4, 15, 13, 7, 1, 4,
    2, 14, 15, 2, 11, 13, 8, 1,
    3, 10, 10, 6, 6, 12, 12, 11,
    5, 9, 9, 5, 0, 3, 7, 8,
    4, 15, 1, 12, 14, 8, 8, 2,
    13, 4, 6, 9, 2, 1, 11, 7,
    15, 5, 12, 11, 9, 3, 7, 14,
    3, 10, 10, 0, 5, 6, 0, 13
  };

  std::vector<int> sbox2 = {
    15, 3, 1, 13, 8, 4, 14, 7,
    6, 15, 11, 2, 3, 8, 4, 14,
    9, 12, 7, 0, 2, 1, 13, 10,
    12, 0, 0, 10, 1, 6, 10, 13,
    3, 13, 14, 8, 4, 7, 7, 4,
    15, 2, 2, 14, 8, 5, 5, 6,
    12, 9, 0, 11, 6, 3, 11, 5,
    10, 11, 9, 1, 13, 8, 15, 6
  };

  std::vector<int> sbox3 = {
    10, 13, 0, 7, 9, 0, 14, 9,
    6, 3, 3, 4, 15, 6, 5, 10,
    1, 2, 13, 8, 12, 5, 7, 14,
    11, 12, 4, 11, 2, 15, 8, 1,
    13, 1, 6, 10, 4, 13, 9, 0,
    8, 7, 15, 14, 3, 8, 0, 7,
    11, 4, 1, 15, 2, 3, 12, 5,
    5, 10, 10, 6, 14, 11, 7, 12
  };

  std::vector<int> sbox4 = {
    7, 13, 13, 8, 14, 11, 3, 5,
    0, 6, 6, 15, 9, 0, 10, 3,
    1, 4, 2, 7, 8, 2, 5, 12,
    11, 1, 12, 10, 4, 14, 15, 9,
    10, 3, 6, 15, 9, 0, 0, 6,
    12, 10, 11, 1, 7, 13, 13, 8,
    15, 9, 1, 4, 3, 5, 14, 11,
    5, 2, 2, 12, 8, 7, 4, 14
  };

  std::vector<int> sbox5 = {
    2, 14, 12, 11, 4, 2, 1, 12,
    7, 4, 10, 7, 11, 13, 6, 1,
    8, 5, 5, 0, 3, 15, 15, 10,
    13, 3, 0, 9, 14, 8, 9, 6,
    4, 11, 2, 13, 1, 12, 11, 7,
    10, 8, 13, 1, 7, 14, 8, 2,
    15, 6, 9, 15, 12, 5, 5, 10,
    6, 0, 3, 9, 0, 7, 14, 3
  };

  std::vector<int> sbox6 = {
    12, 10, 1, 15, 10, 9, 15, 2,
    9, 4, 2, 7, 6, 12, 8, 14,
    0, 6, 13, 11, 3, 1, 4, 5,
    14, 13, 7, 0, 5, 8, 11, 3,
    9, 4, 14, 3, 15, 2, 5, 12,
    2, 1, 8, 15, 12, 10, 3, 7,
    7, 11, 0, 6, 4, 9, 10, 5,
    1, 0, 13, 14, 6, 11, 13, 8
  };

  std::vector<int> sbox7 = {
    4, 13, 11, 0, 2, 11, 14, 7,
    15, 4, 0, 9, 8, 1, 13, 10,
    3, 10, 12, 3, 9, 5, 7, 12,
    5, 6, 10, 15, 6, 8, 1, 6,
    1, 6, 4, 11, 11, 13, 13, 8,
    12, 1, 7, 4, 14, 10, 2, 7,
    10, 9, 15, 5, 6, 0, 8, 15,
    0, 3, 5, 14, 3, 2, 9, 12
  };

  std::vector<int> sbox8 = {
    13, 1, 2, 15, 8, 13, 4, 8,
    6, 10, 15, 3, 11, 7, 1, 4,
    10, 12, 9, 5, 3, 6, 14, 11,
    5, 0, 0, 14, 12, 9, 7, 2,
    7, 2, 11, 1, 4, 14, 1, 7,
    9, 4, 12, 10, 3, 15, 2, 13,
    0, 6, 6, 15, 10, 8, 13, 3,
    15, 5, 8, 0, 5, 11, 14, 9
  };

  // Create S-Box objects
  s_box sbox1_obj(6, 4, sbox1);
  s_box sbox2_obj(6, 4, sbox2);
  s_box sbox3_obj(6, 4, sbox3);
  s_box sbox4_obj(6, 4, sbox4);
  s_box sbox5_obj(6, 4, sbox5);
  s_box sbox6_obj(6, 4, sbox6);
  s_box sbox7_obj(6, 4, sbox7);
  s_box sbox8_obj(6, 4, sbox8);

  // Create vector of S-Boxes
  std::vector<s_box> sboxes = {sbox1_obj, sbox2_obj, sbox3_obj, sbox4_obj,
                                sbox5_obj, sbox6_obj, sbox7_obj, sbox8_obj};

  // Create key
  bitstring key(64);
  for (int i = 0; i < 8; i++)
  {
    int parity = 0;
    int bit = 0;
    for (int j = 0; j < 7; j++)
    {
      bit = rand() % 2;
      key.set_bit(i * 8 + j, bit);
      parity ^= bit;
    }
    key.set_bit(i * 8 + 7, parity);
  }

  // Create Feistel object
  feistel des(64, 16, des_ip, des_fp, 8, 6, 4, sboxes,
              exp, pos, 64, key_schedule, key);

  /* // Plaintext
  bitstring plaintext(64);
  for (int i = 0; i < 64; i++)
  {
    plaintext.set_bit(i, rand() % 2);
  } */

  // Print plaintext
  // std::cout << "Plaintext: ";
  // plaintext.print();



  // Print key
  // std::cout << "Key: ";
  // key.print();

  /* // Encrypt
  bitstring ciphertext = des.encrypt(plaintext, key, 16);
  std::cout << "Ciphertext: ";
  ciphertext.print();

  // Decrypt
  bitstring decrypted_text = des.decrypt(ciphertext, key, 16);
  std::cout << "Decrypted Text: ";
  decrypted_text.print(); */

  // Print LAT table
  /* std::cout << "LAT Table: " << std::endl;
  std::vector<lat_entry> lat_table = sbox1_obj.get_lat();
  for (int i = 0; i < lat_table.size(); i++)
  {
    std::cout << "Input Mask: " << lat_table[i].a; 
    std::cout << ", Output Mask: " << lat_table[i].b;
    std::cout << ", Bias: " << lat_table[i].bias << std::endl;
  } */

  // Get round approximation
  /* std::tuple<bitstring, bitstring, bitstring> approx_triv = des.round_approx(0, 0x0, 0x0);
  std::cout << "Approximation (trivial) : " << std::endl;
  std::cout << "Input Mask: ";
  std::get<0>(approx_triv).print();
  std::cout << "Key Mask: ";
  std::get<1>(approx_triv).print();
  std::cout << "Output Mask: ";
  std::get<2>(approx_triv).print();

  std::tuple<bitstring, bitstring, bitstring> approx_triv2 = des.round_approx(4, 16, 15);
  std::cout << "Approximation: " << std::endl;
  std::cout << "Input Mask: ";
  std::get<0>(approx_triv2).print();
  std::cout << "Key Mask: ";
  std::get<1>(approx_triv2).print();
  std::cout << "Output Mask: ";
  std::get<2>(approx_triv2).print();

  // Linear Trail
  trail_state trail;
  trail = des.find_linear_trail(trail, 3, true); */

  // Attack
  bitstring input_mask(64);
  input_mask.set_bit(32+16, 1);
  input_mask.set_bit(24, 1);
  input_mask.set_bit(13, 1);
  input_mask.set_bit(7, 1);
  input_mask.set_bit(2, 1);

  bitstring output_mask(64);
  output_mask.set_bit(32+16, 1);
  output_mask.set_bit(24, 1);
  output_mask.set_bit(13, 1);
  output_mask.set_bit(7, 1);
  output_mask.set_bit(2, 1);

  // Guess bits for attack 1
  int k1 = key_schedule[0][47-22];
  int k2 = key_schedule[2][47-22];

  // Launch attack
  matsui des_attack(des, 3);
  int rhs = des_attack.attack_1(75, input_mask, output_mask, 1.56/8, placement(64, 64, des_ip), placement(64, 64, des_fp));

  // Get actual key bits
  int k1_actual = key.get_bit(k1);
  int k2_actual = key.get_bit(k2);
  std::cout << "Key bit 1: " << k1 << " = " << k1_actual << std::endl;
  std::cout << "Key bit 2: " << k2 << " = " << k2_actual << std::endl;
  std::cout << "RHS: " << rhs << std::endl;

  // Launch attack 2
  /* matsui des_attack2(des, 4);
  bitstring partial_key = des_attack.attack_2(50, input_mask, output_mask, input_mask, 1.56/8, placement(64, 64, des_ip), placement(64, 64, des_fp), placement(32, 32, pos), placement(32, 48, exp));
  std::cout << "Partial key: ";
  partial_key.print();
  std::cout << "Actual key: ";
  key.print(); */

  return 0;
}
