/************************************************************************
    vtIRencoder.c
    
    Valiant Turtle IR data encoder emulator
    Copyright (C) 2016 Simon Inns

    The Valiant Turtle IR data encoder emulator is free software: you
    can redistribute it and/or modify it under the terms of the GNU
    General Public License as published by the Free Software Foundation,
    either version 3 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Email: simon.inns@gmail.com

************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// This code emulates the Valiant Turtle Communicator Mark II infra-
// red output encoding.  The timing periods are based on the output
// from P24 of the 89C49 processor.  Note that the actual IR pulses
// and periods are slightly different due to the effect of IC2
// (a 4013N flip-flop).

// Function to test if a bit is set to 1 in a byte
bool isBitSet(uint8_t value, uint8_t bitIndex)
{
	return (value & (1 << bitIndex)) != 0;
}

// Function to calculate the parity of a byte
// Returns 0 if parity is odd and 1 if parity is even
uint8_t parity(uint8_t x)
{
   uint8_t y;
   
   y = x ^ (x >> 1);
   y = y ^ (y >> 2);
   y = y ^ (y >> 4);
   y = y ^ (y >> 8);
   y = y ^ (y >>16);

   return y & 1;
}

// Main function
int main(void)
{
	// The input value between 0 and 127 (7 bits)
	uint8_t value;
	
	// Pointer to the next bit to transmit
	uint8_t bitPointer;
	
	// Note: The data is sent LSB first, so transmission is from
	// bit 0 to bit 6 (7-bit data).  Bit 7 should always be 0
	
	for (value = 0; value < 128; value++)
	{
		// Start of frame
		bitPointer = 0;
		printf("%3d: ", value);
		
		// Send the lead-in period
		//
		// This is 60 if bit 0 is a 0 or 35 if bit 0 is a 1
		if (isBitSet(value, bitPointer)) printf("35,");
		else printf("60,");
		
		// Send the data bit periods
		for (bitPointer = 0; bitPointer < 7; bitPointer++)
		{
			// If the current bit is 0 and the next bit is 0 output 75 uS period
			if (isBitSet(value, bitPointer) == 0 && isBitSet(value, bitPointer+1) == 0)
				printf("75,");
				
			// If the current bit is 0 and the next bit is 1 output 50 uS period
			if (isBitSet(value, bitPointer) == 0 && isBitSet(value, bitPointer+1) == 1)
				printf("50,");
				
			// If the current bit is 1 and the next bit is 0 output 100 uS period
			if (isBitSet(value, bitPointer) == 1 && isBitSet(value, bitPointer+1) == 0)
				printf("100,");
				
			// If the current bit is 1 and the next bit is 1 output 75 uS period
			if (isBitSet(value, bitPointer) == 1 && isBitSet(value, bitPointer+1) == 1)
				printf("75,");
		}
		
		// Send the lead-out periods
		//
		// If the parity is even the lead-out periods are 50, 75 and 75
		// If the parity is odd the lead-out periods are 50, 75 and 100
		if (parity(value) == 1) printf("50,75,75");
		else printf("50,75,100");
		
		// End of frame
		printf("\r\n");
	}
	
	return 0;
}
