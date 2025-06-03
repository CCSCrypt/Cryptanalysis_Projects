import numpy as np

# Midori S-boxes as hex strings, convert to integers
Sb0_hex = ['c', 'a', 'd', '3', 'e', 'b', 'f', '7', '8', '9', '1', '5', '0', '2', '4', '6']
Sb1_hex = ['1', '0', '5', '3', 'e', '2', 'f', '7', 'd', 'a', '9', 'b', 'c', '8', '4', '6']

# Convert hex string values to integers
Sb0 = [int(x, 16) for x in Sb0_hex]
Sb1 = [int(x, 16) for x in Sb1_hex]
S_BOXES = [Sb0, Sb1]

def compute_midori(S_BOX):
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

def save_midori_to_file(DLCT, sbox_index, file):  
    file.write(f"\nDLCT Table for Midori S-Box {sbox_index + 1}:\n")
    file.write("     " + "  ".join(f"{i:X}" for i in range(16)) + "\n")
    file.write("    " + "-" * 55 + "\n")

    for delta in range(16):
        row = "  ".join(f"{DLCT[delta][l]:2}" for l in range(16))
        file.write(f"{delta:X} | {row}\n")  

# Generate and save DLCT tables
with open("midori_dlct_table.txt", "w") as file:
    for i, S_BOX in enumerate(S_BOXES):
        DLCT = compute_midori(S_BOX)
        save_midori_to_file(DLCT, i, file)

print("Midori DLCT tables have been saved to 'midori_dlct_table.txt'.")
