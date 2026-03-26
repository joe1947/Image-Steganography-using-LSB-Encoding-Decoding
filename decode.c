#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2], ".bmp") != NULL)
    {
        decInfo -> stego_image_fname = argv[2];
    }
    else
    {
        printf(".bmp file does not found\n");
        return e_failure;
    }


    if(argv[3] == NULL)
    {
        decInfo -> output_name = "output.txt";
    }
    else if(strstr( argv[3], ".txt") != NULL)
    {
        decInfo -> output_name = argv[3];
    }
    else
    {
        decInfo -> output_name = "output.txt";
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_dec_files(decInfo) == e_success)
    {
        printf("All files are open now\n");
    }
    else
    {
        printf("Files are not open\n");
        return e_failure;
    }

    if(decode_magic_string(decInfo) == e_success)
    {
        printf("Magic string decoded succesfully\n");
    }
    else
    {
        printf("Magic string not decoded\n");
        return e_failure;
    }

    if(decode_secret_file_size_ext(decInfo) == e_success)
    {
        printf("Secret file ext size Decoded successfully\n");
    }
    else
    {
        printf("Secret file ext size decoding Unsuccessful\n");
        return e_failure;
    }

    if(decode_secret_file_extn(decInfo) == e_success)
    {
        printf("Secret file extn data decoded asauccessfully\n");
    }
    else
    {
        printf("Secret file extn data decoding Unsuccessful\n");
        return e_failure;
    }

    if(decode_output_file(decInfo) == e_success)
    {
        printf("Output file extn added sucessfully\n");
    }
    else
    {
        printf("Output file ext adding Unsuccessfu\n");
        return e_failure;
    }

    if(decode_secret_file_size(decInfo -> secret_file_size, decInfo) == e_success)
    {
        printf("Secret file size decoded successfully\n");
    }
    else
    {
        printf("Secret file size dicoding unsuccessful\n");
        return e_failure;
    }

    if(decode_secret_file_data(decInfo) == e_success)
    {
        printf("Secret file data decoded successfully\n");
    }
    else
    {
        printf("Secret file data decoding Unsuccessful\n");
        return e_failure;
    }

    return e_success;
}

Status open_dec_files(DecodeInfo *decInfo)
{
    decInfo -> fptr_stego_image = fopen( decInfo -> stego_image_fname, "r");

    if(decInfo -> fptr_stego_image == NULL)
    {
        perror(" ");
        return e_failure;
    }

    decInfo -> fptr_output = fopen( decInfo -> output_name, "w");
    return e_success;
}

Status decode_magic_string(  DecodeInfo *decInfo)
{
   fseek(decInfo -> fptr_stego_image, 54, SEEK_SET);
   
   char magic[strlen(MAGIC_STRING) + 1];
   int i = 0;
   for(i = 0; i < strlen(MAGIC_STRING); i++)
   {
    magic[i] = decode_byte_from_lsb(decInfo -> fptr_stego_image);
   }
   magic[i] = '\0';
    if( strcmp(magic, MAGIC_STRING) ==  0)
    {
        printf("Magic sting is obtained : %s\n",magic);
    }
    else
    {
        printf("Magic string not obtained : %s\n", magic);
        return e_failure;
    }
    return e_success;
}

char decode_byte_from_lsb(FILE *fptr_stego_image)
{
    unsigned char lsb = 0;

    for (int i = 0; i < 8; i++)
    {
        unsigned char buffer;
        fread(&buffer, 1, 1, fptr_stego_image);
        lsb = lsb | (buffer & 1) << i;
    }

    return lsb;
}

Status decode_secret_file_size_ext(DecodeInfo *decInfo)
{
    decInfo -> ext_file_size = decode_size_from_lsb(decInfo -> fptr_stego_image);

    printf("The decoded file extn size is %u\n", decInfo -> ext_file_size);
    return e_success;
}

int decode_size_from_lsb(FILE *fptr_stego_image)
{
    unsigned char buffer;
    int lsb = 0;

    for (int i = 0; i < 32; i++)
    {
        unsigned char buffer;
        fread(&buffer, 1, 1, fptr_stego_image);
        lsb = lsb | (buffer & 1) << i;
    }

    return lsb;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int i;
    for(i = 0; i < decInfo -> ext_file_size; i++)
    {
    decInfo -> extn_secret_file[i] = decode_byte_from_lsb(decInfo -> fptr_stego_image);
    }
    decInfo -> extn_secret_file[i] = '\0';
    printf("The extn decoded was %s\n", decInfo -> extn_secret_file);
    return e_success;
}

Status decode_output_file(DecodeInfo *decInfo)
{
    char *buffer = strchr(decInfo -> output_name, '.');
    *buffer = '\0';
    strcat(decInfo -> output_name, decInfo -> extn_secret_file);
    printf("The output file name is %s\n", decInfo -> output_name);

    return e_success;
}

Status decode_secret_file_size(long file_size, DecodeInfo *decInfo)
{
    decInfo -> secret_file_size = decode_size_from_lsb(decInfo -> fptr_stego_image);

    printf("The decoded file size is %ld\n", decInfo -> secret_file_size);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    int i;
    char data_file[decInfo -> secret_file_size];
    for(i = 0; i < decInfo -> secret_file_size; i++)
    {
        data_file[i] = decode_byte_from_lsb(decInfo -> fptr_stego_image);
    }
    data_file[i] = '\0';
    fwrite(data_file, decInfo -> secret_file_size, 1, decInfo -> fptr_output);

    return e_success;
}
