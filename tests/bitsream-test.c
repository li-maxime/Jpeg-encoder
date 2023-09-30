#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <my_bitstream.h>

int main()
{
 
    FILE * file;
    file = fopen("try_my_bitstream.txt", "w");
    struct my_bitstream * stream =  my_bitstream_create(file);
    my_bitstream_write_bits(stream, 30,  5);
    my_bitstream_write_bits(stream, 90,  7);
    my_bitstream_write_bits(stream, 0,  2);
    my_bitstream_write_bits(stream, 1,  1);
    my_bitstream_write_bits(stream, 26,  5);
    my_bitstream_destroy(stream);
    fclose(file);
  
    return EXIT_SUCCESS;
}