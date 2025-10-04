#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

//NAME   : GURU
//DATE   : 3/10/2025
//PROJECT : LSB IMAGE STEGANOGRAPHY
//DESCRIPTION : TAKING ONE BMP IMG FILE ENCODING IT WITH SECRET DATA  AND DECODING THE ENCODED IMAGE TO GET SECRET DATA .


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo)
{
    // step1:check argv[2] is .bmp file or not , if yes goto step 2
    // stpe2:store the file name into src_image_fname
    // step3:return e_failure
    if(argc<2)
    {
        printf("INFO  :  Please pass valid arguments\n");
        return 1;

    }

    int len1 = strlen(argv[2]);
    if (argv[2][len1 - 4] == '.' && argv[2][len1 - 3] == 'b' && argv[2][len1 - 2] == 'm' && argv[2][len1 - 1] == 'p')
    {

        encInfo->src_image_fname= argv[2];
    }
    else
    {
        return e_failure;
    }

    // step4:check argv[3] is .txt or not , if yes goto step 5  if not goto step 6
    // step5:
    // i)store the file name into the secret _fname
    // ii)fetch the extension and store into the extn_sec_file(strcpy)
    // step6: return e_failure

    char *dot = strrchr(argv[3], '.');  //if we use strchr then it will for first dot in the file 
    if (dot != NULL &&  (strcmp(dot, ".txt") == 0 || strcmp(dot, ".csv") == 0 || strcmp(dot, ".py") == 0))
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, dot);
    }
    else
    {
        return e_failure;
    }

    // step7:check argv[4] is !=NULL  , if yes goto step 8 , if no goto step 12
    // step8 :if yes ,goto step 9
    // step9:check the file is .bmp or not , if yes goto step 10,if no goto step 11
    // step 10:store the file name into the stego_image_fname, return e_sucess
    // step 11: return  e_failure

    // step 12: store the default file name [stego.bmp] into the stego_image_fname

    if (argc > 4)
    {
        int len3 = strlen(argv[4]);
        if (argv[4][len3 - 4] == '.' && argv[4][len3 - 3] == 'b' && argv[4][len3 - 2] == 'm' && argv[4][len3 - 1] == 'p')
        {

            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
    encInfo->stego_image_fname = "stego_img.bmp";
        printf("INFO  :  Output file not mentioned, default file name is added\n");
        printf("\n");
    }
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);


    if (encInfo->image_capacity > 54 + strlen(MAGIC_STRING) * 8 + 32 + strlen(encInfo->extn_secret_file) * 8 + 32 + encInfo->size_secret_file * 8)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
uint get_file_size(FILE *fptr_secret)
{

    // stpe1: move
    // return ftell
    fseek(fptr_secret, 0l, SEEK_END);
    long size= ftell(fptr_secret);
    rewind(fptr_secret);
    return size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char image_buffer[54];
    rewind(fptr_src_image);
    // step1:read
    // step2:write
    // step3:return e_sucess
    if (fread(image_buffer, 54, 1, fptr_src_image) != 1)
    {
        return e_failure;
    }
    if (fwrite(image_buffer, 54, 1, fptr_dest_image) != 1)
    {
        return e_failure;
    }
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        // step1:get the msb bit in data
        // step2:clear the lsb bit in imgage_buffer[i]
        ////step3:step1|step2
        char bit=(data>>(7-i)) & 1;
        image_buffer[i]=image_buffer[i]&0xFE;
        image_buffer[i]= image_buffer[i] | bit;    

    }
    return e_success;
}
Status encode_size_to_lsb(int data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        // step1:get the msb bit in data
        // step2:clear the lsb bit in imgage_buffer[i]
        ////step3:step1|step2

        int bit = (data>>(31-i))&1;
        image_buffer[i] = image_buffer[i] & 0xFE;
        image_buffer[i]= image_buffer[i] | bit;

       
    }
    return e_success;
}

Status encode_magic_string(const  char *magic_str, EncodeInfo *encInfo)
{
    if(encode_data_to_image(magic_str, strlen(magic_str), encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_data_to_image(const char *data, int size, FILE *src, FILE *dest)
{
     char image_buffer[8];
    for (int i = 0; i < size; i++)
    {
        // step1:read 8 bytes from the src
        // step2:call the byte_to_lsb(data[0],image_buffer)
        // step3: write 8 bytes to the dest

        if(fread(image_buffer,1,8,src)!=8)
        {
            return e_failure;
           
        }
        if( encode_byte_to_lsb(data[i],image_buffer)!=e_success)
        {
            return e_failure;
        }
        if(fwrite(image_buffer,1,8,dest)!=8)
        {
            return e_failure;
        }
    }
     return e_success;
}

Status encode_secret_file_extn_size(long extn_size, EncodeInfo *encInfo)
{
    char image_buffer[32];
    // step1:read 32 bytes from the src_image
    // step2:call size to lsb function (extn,size,image_buffer)
    // step3:write 32 bytes to the dest_image

    if(fread(image_buffer , 1,32,encInfo->fptr_src_image)!=32)
    {
        return e_failure;
    }
    if(encode_size_to_lsb(extn_size,image_buffer)==e_failure)
    {
        return e_failure;
    }
    if(fwrite(image_buffer , 1,32,encInfo->fptr_stego_image)!=32)
    {
        return e_failure;
    }
    return e_success;
}

Status encode_secret_file_extn(const char *extn, EncodeInfo *encInfo) // call generic function
{
    if(encode_data_to_image(extn,strlen(extn),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    else
    {
        return e_success;
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
     char image_buffer[32];
    // step1 :read 32 bytes fromm the src
    // step2: call size_to_lsb(file_size,image_buffer);
    // step3: write 32 bytes to the dest
     if(fread(image_buffer,1,32,encInfo->fptr_src_image)!=32)
     {
        return e_failure;
     }
     if(encode_size_to_lsb(file_size,image_buffer)==e_failure)
     {
        return e_failure;
     }
     if(fwrite(image_buffer,1,32,encInfo->fptr_stego_image)!=32)
     {
        return e_failure;
     }
     return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char data[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    // read the contents from the file
    if (fread(data, 1, encInfo->size_secret_file, encInfo->fptr_secret) != encInfo->size_secret_file)
    {
       printf("secret file contents are not read correctly\n");
        return e_failure;
    }
    if(encode_data_to_image(data, encInfo->size_secret_file, encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
    return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status copy_remaining_img_data(FILE *src, FILE *dest)
{
    // char ch;
    // while(//read byte by byte upto eof)
    //     {
    //     // write byte  by byte to the dest_img
    //     }

    int ch;
    while ((ch = fgetc(src)) != EOF)  // read one byte
    {
        if (fputc(ch, dest) == EOF)   // write one byte
        {
            return e_failure;         // write error
        }
    }
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
        printf("error\n");
         return e_failure;
    }
    else
    {
         printf("INFO  :  Files are opened successfully\n");
         printf("\n");
    }

    if (check_capacity(encInfo) == e_failure)
    {
         printf("ERROR: checking file  capacity is failed\n");
         return e_failure;
    }
    else
    {
         printf("INFO  :  Checking file capacity successfully completed\n");
         printf("\n");
    }

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
         printf("Error:header is not copied correctly\n");
         return e_failure;
    }
    else
    {
        printf("INFO  :  Copy bmp header is successful\n");
        printf("\n");
    }
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_failure)
    {
        printf("Error : Magic string is not encoded correctly\n");
        return e_failure;
    }
    else 
    {
        printf("INFO  :  Magic string encoded successfully\n");
        printf("\n");
    }

    if (encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_failure)
    {
       
         printf("Error  :  secret file extension size  is not encoded correctly\n");
         return e_failure;
    }
    else
    {
       printf("INFO  :  Secret file extension size is Encoded successfully\n");
       printf("\n");
   }
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("Error:secret file extention is not  encoded correctly\n");
        return e_failure;
    }
    else
    {
       printf("INFO  :  Secret file extenstion  Encoded successfully\n");
       printf("\n");
    }

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("Error:secret file size is not  encoded correctly\n");
        return e_failure;
    }
    else
    {
        printf("INFO  :  Secret file size  Encoded successfully\n");
        printf("\n");
    }
    if (encode_secret_file_data(encInfo) == e_failure)
    {
         printf("Error:secret file data  is not  encoded correctly\n");
        return e_failure;
    }
    else
    {
        printf("INFO  :  Secret file data  Encoded successfully\n");
        printf("\n");
    }
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf( "Error : the remaining data is not copied successfully\n");
        return e_failure;
    }
    else
    {
        printf("INFO  :  Remaining data successfully copied\n");
        printf("\n");
    }
    return e_success;
    
 }
