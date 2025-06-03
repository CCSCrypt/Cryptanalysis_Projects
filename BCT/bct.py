import numpy as np
import collections

print("Which S-box do you want to use?")
print("1. AES S-box -- type: 'aes'")
print("2. Twine S-box -- type: 'twine'")
sbox_type = input("Enter your choice: ").strip().lower()

# import 'aes_sbox' if sbox_type == 'aes' else 'twine_sbox'
if sbox_type == 'aes':
    import aes_sbox as _sbox_
else:
    import twine_sbox as _sbox_

sbox = _sbox_.sbox

ddt_filename = f"{sbox_type}_ddt.csv"
bct_filename = f"{sbox_type}_bct.csv"

print("sbox.dtype: ", sbox.dtype)
print("sbox.shape: ", sbox.shape)

def sub_bytes(byte):
    """Substitutes bytes in the 'byte' using the S-box."""
    return sbox[byte]

length_of_sbox = len(sbox)
print(f"length_of_sbox: ", length_of_sbox)

inv_sbox = np.zeros(length_of_sbox, dtype=int)
for i in range(length_of_sbox):
    inv_sbox[sbox[i]] = i

def inv_sub_bytes(byte):
    """Substitutes bytes in the 'byte' using the inverse S-box."""
    return inv_sbox[byte]

# Example usage of the sub_bytes function:
print("# Example usage of the sub_bytes function:")
num1 = hex(sub_bytes(0))
num2 = hex(sub_bytes(0xFF)) if length_of_sbox == 256 else hex(sub_bytes(0xF))
print(num1)  # Example usage
print(num2[2:])  # Example usage

# differential distribution table of AES S-box
ddt = np.zeros((length_of_sbox, length_of_sbox), dtype=int)
for x in range(length_of_sbox):
    for y in range(length_of_sbox):
        x1 = sub_bytes(x)
        y1 = sub_bytes(y)
        z = x1 ^ y1
        ddt[(x^y), z] += 1

# print first and second row of the differential distribution table
# for i in range(256):
print(ddt[0])
print(ddt[1])


# print the differential distribution table in a csv file
np.savetxt(ddt_filename, ddt, delimiter=",", fmt="%d")


# We will now use an alternative method to generate the boomerang connectivity table
bct = np.zeros((length_of_sbox, length_of_sbox), dtype=int)
for delta_i in range(length_of_sbox):
    for nabla_o in range(length_of_sbox):
        for x in range(length_of_sbox):
            y = x ^ delta_i
            first_entity = inv_sub_bytes(sub_bytes(x) ^ nabla_o)
            second_entity = inv_sub_bytes(sub_bytes(y) ^ nabla_o)
            
            if (first_entity ^ second_entity) == delta_i :
                bct[delta_i, nabla_o] += 1
            # ((first_entity ^ second_entity) == delta_i) ? bct[delta_i, nabla_o] += 1 : bct[delta_i, nabla_o] += 0
            # print(bct[delta_i, nabla_o])

# print the boomerang connectivity table in a csv file
np.savetxt(bct_filename, bct, delimiter=",", fmt="%d")


ddt_counts = collections.Counter(ddt.flatten())
bct_counts = collections.Counter(bct.flatten())

print("ddt_counts: ", ddt_counts)
print("bct_counts: ", bct_counts)

print("\n--- Frequency map of DDT ---")
for val, count in sorted(ddt_counts.items()):
    print(f"Value {val}: {count} times")

print("\n--- Frequency map of BCT ---")
for val, count in sorted(bct_counts.items()):
    print(f"Value {val}: {count} times")
