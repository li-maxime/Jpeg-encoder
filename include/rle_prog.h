#ifndef _RLE_PROG_H_
#define _RLE_PROG_H_

#include <stdlib.h>
#include <stdio.h>
#include <mcu.h>
#include <rle.h>
#include <my_bitstream.h> 
#include <stdint.h>
#include <stdbool.h>
#include <me_jpeg_writer.h>

extern void encode_AC_prog(struct my_bitstream* AC1_5, struct my_bitstream* AC6_63, int32_t *matrix, uint8_t cc);



#endif /* RLE_PROG_H */
