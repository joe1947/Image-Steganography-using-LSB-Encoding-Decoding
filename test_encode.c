#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Invalid arguments\n");
        printf("For encoding -->  ./a.out -e beutiful.bmp sectret.txt [stego.bmp]\n");
        printf("For decoding -->  ./a.out -d stego.bmp [output.bmp]\n");
        return e_failure;
    }
    if(strcmp(argv[1], "-e") == 0)
    {
        if(argc < 5 || argc > 6)
        {
            printf("Invalid arguments\n");
            printf("For encoding -->  ./a.out -e beutiful.bmp sectret.txt [stego.bmp]\n");
            return e_failure;
        }
    }

    if(strcmp(argv[1], "-d") == 0)
    {
        if(argc < 3 || argc > 4)
        {
            printf("Invalid arguments\n");
            printf("For decoding -->  ./a.out -d stego.bmp [output.bmp]\n");
            return e_failure;
        }
    }


    if(check_operation_type(argv) == e_encode)
    {
        printf("We chose encoding\n");
        EncodeInfo encode;
        if(read_and_validate_encode_args(argv, &encode) == e_success) 
        {
            printf("Read and Validate is successful\n");
        }
        else
        {
            printf("Read and validate is unsuccessful\n");
            return e_failure;
        }
        
        if(do_encoding(&encode) == e_success)
        {
            printf("Encoding Successful\n");
        }
        else
        {
        printf("Encoding Unsuccessful\n");
        return e_failure;
        }

    }
    else if(check_operation_type(argv) == e_decode)
    {
        printf("We chose decoding\n");
        DecodeInfo decode;

        if(read_and_validate_decode_args(argv, &decode) == e_success)
        {
            printf("Read and validate successfull\n");
        }
        else
        {
            printf("Read and validate Unsuccessful\n");
            return e_failure;
        }
        if(do_decoding(&decode) == e_success)
        {
            printf("Decoding Successful\n");
        }
        else
        {
            printf("Decoding Unsuccessful\n");
            return e_failure;
        }
    }
    else 
        printf("Neither encoding nor decoding\n");
}




OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
         return e_encode;
    else if(strcmp(argv[1], "-d") == 0)
         return e_decode;
    else
         return e_unsupported;
}