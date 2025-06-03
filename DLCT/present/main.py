PRESENT_SBOX_HEX = ['C', '5', '6', 'B', '9', '0', 'A', 'D', '3', 'E', 'F', '8', '4', '7', '1', '2']
PRESENT_SBOX = [int(x, 16) for x in PRESENT_SBOX_HEX]

import numpy as np

def compute_present(S_BOX):
    dlct = np.zeros((16, 16), dtype=int)
    
    for delta in range(16):
        for X1 in range(16):
            X2 = X1 ^ delta
            Y1 = S_BOX[X1]
            Y2 = S_BOX[X2]

            for lambdas in range(16):  # lambda is a 4-bit mask
                result1 = bin(Y1 & lambdas).count('1') % 2 
                result2 = bin(Y2 & lambdas).count('1') % 2
                if result1 == result2:
                    dlct[delta][lambdas] += 1
                    
    return dlct

def save_present_to_file(DLCT, sbox_index, file):  
    file.write(f"\nDLCT Table for PRESENT S-Box {sbox_index + 1}:\n")
    file.write("     " + "  ".join(f"{i:X}" for i in range(16)) + "\n")
    file.write("    " + "-" * 55 + "\n")

    for delta in range(16):
        row = "  ".join(f"{DLCT[delta][l]:2}" for l in range(16))
        file.write(f"{delta:X} | {row}\n")  

# Generate and save DLCT table
with open("present_dlct_table.txt", "w") as file:
    DLCT = compute_present(PRESENT_SBOX)
    save_present_to_file(DLCT, 0, file)

print("PRESENT DLCT table has been saved to 'present_dlct_table.txt'.")
