# mid-pSquare
Public implementations of the *mid-pSquare* tweakable block cipher.

## Publication
This repository contains source code associated with a TCHES 2025 publication titled "mid-pSquare: Leveraging the Strong Side-Channel Security of Prime-Field Masking in Software" authored by Brieuc Balon, Lorenzo Grassi, Pierrick Méaux, Thorben Moos, François-Xavier Standaert and Matthias Johann Steiner.

Links:
- [ePrint](https://eprint.iacr.org/2025/519)

## FPM, small-pSquare and mid-pSquare
At Eurocrypt 2024, the FPM (Feistel for Prime Masking) family of tweakable block ciphers has been introduced together with its first hardware-oriented instance *small-pSquare*. A new software-oriented instance of the family, optimized for 32-bit processors, has been introduced in a follow-up publication (linked above) by the name *mid-pSquare*. Both, the family and the instances are based on a Type-II generalized Feistel network and have been designed for the efficient application of prime-field masking. *small-pSquare* leverages the hardware efficiency of a small Mersenne prime (hence, "small-p"), while *mid-pSquare* exploits the optimized 32-bit arithmetic available on most modern (micro-)processors by operating over a mid-sized 31-bit Mersenne prime field (hence, "mid-p"). Both instances rely on squaring as their only non-linear component (hence, "Square").

## Content of the Repository
We provide source code for different masked and unmasked software implementations of *mid-pSquare* (and competitors). The C code is optimized for high-performance constant-time execution on ARM Cortex-M4 and similar devices with single-cycle 32-bit multiplication. Additionally, we provide the corresponding implementations of the *SKINNY* and *small-pSquare* tweakable block ciphers for equivalent parametrization. Makefiles for gcc compilation of all platform-independent code are provided. The implementations located in `masked/TRNG randomness/` use the on-board True Random Number Generator (TRNG) of the Cortex-M4 and are thus platform-dependent (no Makefile provided). All implementations have been tested and evaluated (details in the paper) on the STM32F415RGT6 microcontroller which is part of the STM32F4 Target board for the [ChipWhisperer CW308 UFO board](https://www.newae.com/products/nae-cw308).

## Contact and Support
Please contact Thorben Moos (thorben.moos@uclouvain.be) if you have any questions, comments or if you found a bug that should be fixed.

## Licensing
Please see `LICENSE.txt` for licensing instructions.