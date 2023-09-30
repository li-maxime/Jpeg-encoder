#include <stdlib.h>
#include <stdio.h>
#include <mcu.h>
#include <my_bitstream.h> 
#include <stdint.h>
#include <huffman.h>
#include <htables.h>
#include <me_jpeg_writer.h>
#include <math.h>
#include <rle.h>
/*
void encod_EOBRUN(uint16_t* EOBRUN, struct my_bitstream* stream, struct huff_table* ht)
{
	if(EOBRUN != 0)
	{
		uint8_t mag = magnetude(*EOBRUN);
		uint16_t I = ((uint8_t) 16)*mag;
		uint8_t code_length;
		uint32_t code_mag = huffman_table_get_path(ht, I, &code_length);
		my_bitstream_write_bits(stream, code_mag, code_length, false);
		my_bitstream_write_bits(stream, *EOBRUN, mag, false);
		*EOBRUN = 0;
	}
}

void encod_ZRL(uint8_t* R, struct my_bitstream* stream, struct huff_table* ht)
{
	my_bitstream_write_bits(stream, 0xF0, 16, false);
	*R -= 16;
}

void encod_ZRN(uint8_t* R, int32_t value, struct my_bitstream* stream, struct huff_table* ht)
{
	uint8_t mag = magnetude(value);
	uint16_t I = ((uint8_t) 16)*(*R) + mag;
	uint8_t code_length;
	uint32_t code_mag = huffman_table_get_path(ht, I, &code_length);
	my_bitstream_write_bits(stream, code_mag, code_length, false);

	code_mag = huffman_table_get_path(ht, abs(value), &code_length);
	my_bitstream_write_bits(stream, code_mag, code_length, false); //long(I) ?? index ? 
	*R = 0;
}


void encode_AC_prog(struct my_bitstream* AC1_5, struct my_bitstream* AC6_63, int32_t *matrix, uint8_t cc, uint16_t *EOBRUN)
{
	struct huff_table* ht = huffman_table_build(htables_nb_symb_per_lengths[AC][cc], htables_symbols[AC][cc], htables_nb_symbols[AC][cc]);
	uint8_t R = 0;
	uint8_t nb_bits_code;
	uint32_t code_mag;
	uint16_t bit_seq = 0;
	int end_seq = 0;
	uint16_t index;
	uint8_t k = 1;
	struct my_bitstream* current_stream;   
	
	select:
	k++;
	current_stream = (k < 6) ? AC1_5 : AC6_63;
		if(matrix[k] == 0)
		{
			R++;
			if(k == BLOC_SIZE*BLOC_SIZE)
			{
				(*EOBRUN)++;
				if(*EOBRUN == 0x7fff)
				{
						encod_EOBRUN(EOBRUN, current_stream, ht);
				} 
				else
				{
						goto end;
				} 
			}
			else
			{
				goto select;
			}
		}
		else
		{	
			encod_EOBRUN(EOBRUN, current_stream, ht);
			r_test:
			if(R < 16)
			{
				encod_ZRL(&R, current_stream, ht);
				goto r_test;
			}
			else
			{
				encod_ZRN(&R, matrix[k], current_stream, ht);
				if(k == BLOC_SIZE*BLOC_SIZE)
				{
					goto end;
				}
				else
				{
					goto select;
				}
			}

		}
end:
k++;
}

*/
