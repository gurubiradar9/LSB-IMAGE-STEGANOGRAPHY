#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status open_decode_files(DecodeInfo *decInfo)
{

    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return d_failure;
    }
}

Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    // step1:check argv[2] is .bmp file or not , if yes goto step 2
    // stpe2:store the file name into src_image_fname
    // step3:return e_failure

    int len1 = strlen(argv[2]);
    if (strstr(argv[2], ".bmp") != NULL)
    {

        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        return d_failure;
    }

    if (argc > 3)
    {
        strtok(argv[3], ".");
        decInfo->secret_fname = argv[3];
    }
    else
    {
        decInfo->secret_fname = "decoded";
        printf("INFO  :  Output file not mentioned,Default file name is added\n");
        printf("\n");
    }
    return d_success;
}
Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
        int bit = image_buffer[i] & 1;
        *data = *data << 1 | bit;
    }
    return d_success;
}
Status decode_size_from_lsb(int *size, char *image_buffer)
{
    *size = 0;
    for (int i = 0; i < 32; i++)
    {
        int bit = image_buffer[i] & 1;
        *size = *size << 1 | bit;
    }
    return d_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    int len = strlen(MAGIC_STRING); // length of the magic string to decode exactly that much
    decInfo->magic_string_len = len;

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    char buffer[8];

    for (int i = 0; i < len; i++)
    {
        if (fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR:Unable to read magic string bytes\n");
            return d_failure;
        }
        if (decode_byte_from_lsb(buffer, &decInfo->magic_string[i]) != d_success)
        {
            printf("ERROR:failed to decode byte\n");
            return d_failure;
        }
    }
    decInfo->magic_string[len] = '\0';
   // printf("Decoded magic string: %s\n", decInfo->magic_string);
    return d_success;
}
Status decode_secret_file_extn_size(int *extn_size, DecodeInfo *decInfo)
{
    char buffer[32];
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        return d_failure;
    }
    if (decode_size_from_lsb(extn_size, buffer) != d_success)
    {
        return d_failure;
    }

    return d_success;
}
Status decode_secret_file_extn(int extn_size, DecodeInfo *decInfo)
{
    char buffer[8];

    for (int i = 0; i < extn_size; i++)
    {
        if (fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR:Unable to read the extension bytes\n");
            return d_failure;
        }
        if (decode_byte_from_lsb(buffer, &decInfo->extn_secret_file[i]) != d_success)
        {
            printf("ERROR:failed to decode byte\n");
            return d_failure;
        }
    }
    decInfo->extn_secret_file[extn_size] = '\0';
   // printf("Decoded extn is : %s\n", decInfo->extn_secret_file);

    return d_success;
}
Status decode_secret_file_size(long *secret_file_size, DecodeInfo *decInfo)
{
    int temp_size;
    char buffer[32];
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        return d_failure;
    }
    if (decode_size_from_lsb(&temp_size, buffer) != d_success) // because my size from lsb expects int * so i am sending the int type after finishing i will convert to long
    {
        return d_failure;
    }

    *secret_file_size = (long)temp_size; // converting int to long

    return d_success;
}
Status decode_secret_file_data(long secret_file_size, DecodeInfo *decInfo)
{

    char buffer[8];
    if (decInfo->fptr_secret == NULL)
    {
        printf("Error: File is not opened\n");
        return d_failure;
    }

    for (long i = 0; i < secret_file_size; i++)
    {
        if (fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR:Unable to read file data \n");
            return d_failure;
        }
        char data;

        if (decode_byte_from_lsb(buffer, &data) != d_success)
        {
            printf("ERROR:failed to decode byte\n");
            return d_failure;
        }
        fwrite(&data, 1, 1, decInfo->fptr_secret);
    }
    return d_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_decode_files(decInfo) == d_failure)
    {
        printf("ERROR : Files are not opened\n");
        printf("\n");
        return e_failure;
    }
    else
    {
        printf("INFO  :  Files are opened successfully\n");
        printf("\n");
    }

    if (decode_magic_string(decInfo) == d_success)
    {
        if (strcmp(decInfo->magic_string, MAGIC_STRING) == 0)
        {
            printf("INFO  :  Magic string decoded successfully\n");
            printf("\n");
        }
        else
        {
            printf("ERROR: Decoding magic string failed\n");
            return d_failure;
        }
    }

    int extn_len;
    if (decode_secret_file_extn_size(&extn_len, decInfo) != d_success)
    {
        printf("ERROR:Decoding size of the extension is failed\n");
        return d_failure;
    }
    else
    {
        printf("INFO  :  Decode file extension size is completed\n");
        printf("\n");
    }
    //printf("extn length is %d\n", extn_len);

    if (decode_secret_file_extn(extn_len, decInfo) != d_success)
    {
        printf("ERROR:Decoding extension is failed\n");
        return d_failure;
    }
    else
    {
        printf("INFO  :  Decode file extension is completed\n");
        printf("\n");
    }

    char output_file[100];
    strcpy(output_file, decInfo->secret_fname);
    strcat(output_file, decInfo->extn_secret_file);
    printf("Full output file: %s\n", output_file);
    decInfo->fptr_secret = fopen(output_file, "w");
    if (!decInfo->fptr_secret)
    {
        printf("ERROR: Unable to create secret file\n");
        return d_failure;
    }

    long file_size;
    if (decode_secret_file_size(&file_size, decInfo) != d_success)
    {
        printf("ERROR:Decoding file size is failed\n");
        return d_failure;
    }
    else
    {
        printf("INFO  :  Decode size of the file is completed\n");
        printf("\n");
    }
    //printf("file  size is %ld\n", file_size);

    if (decode_secret_file_data(file_size, decInfo) != d_success)
    {
        printf("ERROR:Decoding secret file data  is failed\n");
        return d_failure;
    }
    else
    {
        printf("INFO  :  Decode file data is completed\n");
        printf("\n");
    }

    return d_success;
}
