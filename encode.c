#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2], ".bmp") != NULL)
    {
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp file does not found\n");
        return e_failure;
    }

    if(strstr(argv[3], ".txt") != NULL)
    {
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        printf(".txt file does not found\n");
        return e_failure;
    }


    if( argv[4] == NULL)
    {
        encInfo -> stego_image_fname = "stego.bmp";
    }
    else if(strstr(argv[4], ".bmp") != NULL )
    {
        encInfo -> stego_image_fname = argv[4];
    }
    else
    {
        encInfo -> stego_image_fname = "stego.bmp";
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        printf("All files are open now\n");
    }
    else
    {
        printf("Files are not open\n");
        return e_failure;
    }

    if(check_capacity(encInfo) == e_success)
    {
        printf("Check capacity is successful\n");
    }
    else
    {
        printf("Check capacity is unsuccessful\n");
    }

    if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
        printf("Headers are copied\n");
    }
    else
    {
        printf("Headers are not copied\n");
        return e_failure;
    }

    if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("Magic string encoded successfully\n");
    }
    else
    {
        printf("Magic string encoding Unsuccessful\n");
        return e_failure;
    }

    strcpy( encInfo -> extn_secret_file, strchr(encInfo -> secret_fname, '.'));   // ext secret file has address of .txt

    if(encode_secret_file_size_ext(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
    {
        printf("File ext size encoded successfully\n");
    }
    else
    {
        printf("File ext size encoding unsuccessful\n");
        return e_failure;
    }

    if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
    {
        printf("Secret file ext encoded successfully\n");
    }
    else
    {
        printf("Secret file ext encoding Unsuccessful\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
    {
        printf("Secret file size encoded successfully\n");
    }
    else
    {
        printf("Secret file size encoding Unsuccessful\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret data encoded successfully\n");
    }
    else
    {
        printf("Secret data encoding Unsuccessful\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo) == e_success)
    {
        printf("Remaining data copied successfully\n");
    }
    else
    {
        printf("Remaining data copying Unsuccessfull\n");
    }

    return e_success;
}


Status open_files(EncodeInfo *encInfo)
{
    encInfo -> fptr_src_image = fopen(encInfo -> src_image_fname, "r");

    if(encInfo -> fptr_src_image == NULL)
    {
        perror(" ");
        return e_failure;
    }
    encInfo -> fptr_secret = fopen(encInfo -> secret_fname, "r");
    if(encInfo -> fptr_secret == NULL)
    {
        perror(" ");
        return e_failure;
    }

    encInfo -> fptr_stego_image = fopen(encInfo -> stego_image_fname, "w");
    return e_success;
}



Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);

    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    if(encInfo -> image_capacity > (16 + 32 + 32 + 32 + encInfo -> size_secret_file * 8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, 4, 1, fptr_image);
    printf("width is %u\n", width);

    fread(&height, 4, 1, fptr_image);
    printf("height is %u\n", height);

    return width * height * 3;

}


uint get_file_size(FILE *fptr)
{ 
   fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image, 0, SEEK_SET);     // changing the pointer to zero

    char str[54];
    fread(str, 54, 1, fptr_src_image);

    fwrite(str, 54, 1, fptr_dest_image);

    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    if(encode_data_to_image(magic_string, strlen(magic_string), encInfo) == e_success)
    {
        printf("Magic string encoded successfully\n");
    }
    else
    {
        printf("Magic string does not  encoded successfully\n");
        return e_failure;
    }
    return e_success;
}

Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    for(int i = 0; i < size; i++)
    {
        fread(encInfo -> image_data, 8, 1, encInfo -> fptr_src_image);
        encode_byte_to_lsb(data[i], encInfo -> image_data);
        fwrite(encInfo -> image_data, 8, 1, encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);
    }
    return e_success;
}

Status encode_secret_file_size_ext(int size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(size, encInfo);
    
    return e_success;
}

Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 32, 1, encInfo -> fptr_src_image);
    for(int i = 0; i < 32; i++)
    {
        buffer[i] = (buffer[i] & 0xFE) | ((size >> i) & 1);
    }
    fwrite(buffer, 32, 1, encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size, encInfo);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo -> fptr_secret, 0, SEEK_SET);
    
    char str[encInfo -> size_secret_file];

    fread(str, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);

    encode_data_to_image(str, encInfo -> size_secret_file, encInfo);

    return e_success;
}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    char ch;
    
    while(fread(&ch, 1, 1, encInfo -> fptr_src_image) > 0)
    {
        fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
    }
    return e_success;
}