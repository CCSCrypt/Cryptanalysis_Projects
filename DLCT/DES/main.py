from collections import Counter
import numpy as np
from DES.DES_S_Boxes import S_BOXES  # Import the modified S-boxes




'''def parity(x):
    result = 0
    while x:
        result ^= (x & 1)
        x >>= 1
    return result'''

   

def extract_bits(value):
    """Extract row and column indices from a 6-bit input value."""
    binary_six_bits = f"{value:06b}"  # Convert to 6-bit binary
    msb = int(binary_six_bits[0])  # Extract MSB
    lsb = int(binary_six_bits[-1])  # Extract LSB
    row_index = (msb << 1) | lsb  # Compute row index (2-bit value) ..first lefy shift then logical OR
    col_index = int(binary_six_bits[1:5], 2)  # Compute column index (4-bit value)
    return row_index, col_index

def compute_dlct(S_BOX):
    DLCT = np.zeros((64, 16), dtype=int)  # Initialized DLCT with zeros

    for delta in range(64):  # Iterate over all possible input differentials
        for X1 in range(64):   # Iterate over all possible input values
            X2 = X1 ^ delta  # Compute the second input (XOR with delta_X)
            
            # Get the S-Box outputs for X1 and X2
            row_X1, col_X1 = extract_bits(X1)
            row_X2, col_X2 = extract_bits(X2)
            Y1 = S_BOX[row_X1][col_X1]
            Y2 = S_BOX[row_X2][col_X2]

            for lambdas in range(16):
                result1=bin(Y1&lambdas).count('1')%2 #  integer dot product converted to binary and no. of 1's counted.
                result2=bin(Y2&lambdas).count('1')%2 
                if result1==result2: # parity should be same in both results in order to be the x that satisfy the DLCT'entry conditon
                    # Update the DLCT table
                    DLCT[delta][lambdas] += 1  
    
    return DLCT


# some designs for table and writing up table
def save_dlct_to_file(DLCT, sbox_index, file):
    """Save the formatted DLCT table for an S-Box to a file."""
    file.write(f"\nDLCT Table for S-Box {sbox_index + 1}:\n")
    file.write("     " + "  ".join(f"{i:2}" for i in range(16)) + "\n")
    file.write("    " + "-" * 64 + "\n")
    
    for delta in range(64):
        row_values = "  ".join(f"{DLCT[delta][l]:2}" for l in range(16))
        file.write(f"{delta:2} | {row_values}\n")


# Compute and save the DDT for all 8 S-boxes to a file
with open("DLCT_tables.txt", "w") as file:
    for i, S_BOX in enumerate(S_BOXES):
        DLCT = compute_dlct(S_BOX)
        save_dlct_to_file(DLCT, i, file)

print("DLCT tables have been saved to 'DLCT_tables.txt'.")