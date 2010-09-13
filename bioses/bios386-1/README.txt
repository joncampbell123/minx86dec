Experimental BIOS.
The BIOS code itself runs in flat 32-bit protected mode, calling down to 16-bit
real mode (with 4GB limits) when possible. Obviously, there might be some
compatibility or slowdown issues with thunking to/from protected mode, a more
reasonable implementation would be the next iteration (bios386-2)

