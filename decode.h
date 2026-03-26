#ifndef DECODE_H
#define DECODE_H

#include "types.h"


#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


typedef struct _DecodeInfo
{
    char *output_name;
    FILE *fptr_output;

    char *stego_image_fname;
    FILE *fptr_stego_image;

    uint ext_file_size;
    char extn_secret_file[10];
    long secret_file_size;

}DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status open_dec_files(DecodeInfo *decInfo);

Status decode_magic_string( DecodeInfo *decInfo);

char decode_byte_from_lsb(FILE *fptr_stego_image);

Status decode_secret_file_size_ext(DecodeInfo *encInfo);

int decode_size_from_lsb(FILE *fptr_stego_image);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_output_file(DecodeInfo *decInfo);

Status decode_secret_file_size(long file_size, DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

#endif