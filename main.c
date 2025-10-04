#include<stdio.h>
#include<string.h>
#include"encode.h"
#include"decode.h"
#include"types.h"
#include"common.h"

//NAME   : GURU
//DATE   : 3/10/2025
//PROJECT : LSB IMAGE STEGANOGRAPHY
//DESCRIPTION : TAKING ONE BMP IMG FILE ENCODING IT WITH SECRET DATA  AND DECODING THE ENCODED IMAGE TO GET SECRET DATA .

 OperationType check_operation_type(char*argv)
{
    //step1:check argv is -e or  not ,  if yes goto step2,if not goto step 3 
    //step 2:return e_encode
    //step 3: check argv is -d or not if yes goto step4,if not goto step 5
    //step 4 : return e_decode
    //step 5: return e_unsuppoerted
    if(strcmp(argv,"-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(argv,"-d")==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
int main(int argc ,  char*argv[])
{ 
    EncodeInfo encoInfo;
    DecodeInfo decInfo;
    if(argc<2)
    {
        printf("INFO  :  Please pass valid arguments.\n");
        printf("INFO  :  Encoding - minimum 4 arguments.\n");
        printf("INFO  :  Decoding - minimum 3 arguments.\n"); 
        return 1;
    }
   OperationType res= check_operation_type(argv[1]);
   if(res==e_encode)
   {
        if(argc < 4)   
        {
            printf("ERROR: Encoding requires at least 4 arguments.\n");
            printf("\n");
            printf("INFO  :  Please enter like this %s -e <source_image> <secret_file> <stego_image>\n", argv[0]);
            return 1;
        }

        printf("INFO  :  Selected Encoding , Encoding Started\n");
        printf("\n");
        
        if(read_and_validate_encode_args(argc,argv,&encoInfo)==e_success)
        {
            printf("INFO  :  Read and validate is  successully completed for Encoding\n");
            printf("\n");
            if(do_encoding (&encoInfo)==e_success)
            {
                 printf("..........................................\n");
                printf("INFO  :  Encoding Completed successfully\n");
                printf("..........................................\n");

                
            }
            else
            {
                printf("encoding is not done correctly\n");
            }
           
        }
   }
   else if(res==e_decode)
   {
         if(argc < 3)   
        {
            printf("ERROR: decoding requires at least 3 arguments.\n");
            printf("\n");
            printf("INFO  :  Please enter like this %s -d <stego_image> <optional>\n", argv[0]);
            return 1;
        }
        printf("INFO  :  Selected Decoding , Decoding Started\n");
        printf("\n");
        if(read_and_validate_decode_args(argc,argv,&decInfo)==d_success)
        {
            printf("INFO  :  Read and validate is  successully completed for Decoding\n");
            printf("\n");
            if(do_decoding (&decInfo)==d_success)
            {
                printf("..........................................\n");
                printf("INFO  :  Decoding Completed successfully\n");
                printf("..........................................\n");
    
            }
            else
            {
                printf("decoding is not done correctly\n");
            }
            //return
        }
   }
   else
   {
   printf("encoding and decoding is not correctly done\n") ; 
   return e_failure;
   }
return 0;

}