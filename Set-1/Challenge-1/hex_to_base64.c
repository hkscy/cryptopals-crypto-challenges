#include <stdio.h> //printf
#include <stdlib.h> //strol
#include <errno.h> //errno
#include <string.h> //strlen

int paramFailure() 
{
    printf ("Specify one (only) hex number as input, i.e. hex_to_base64 4fd3\n");
    exit(EXIT_FAILURE);
}

char* hexToB64(char* b16String, char *base64_num, int bSize)
{
    //list(string.ascii_uppercase+string.ascii_lowercase+string.digits+'+'+'/')
    char base64_digits[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 
                              'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 
                              'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 
                              'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 
                              's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', 
                              '3', '4', '5', '6', '7', '8', '9', '+', '/'};
                                  
    char * endptr; 
    errno = 0;                 
    unsigned long base10_num = strtoul(b16String, &endptr, 16);
    int strtolErr = errno;
    if (*endptr == '\0') //entire string converted
    {   
        if(strtolErr!=0) 
        {
            perror("");
            paramFailure();
        }
        
        //printf("Base 10 intermediary: %d\n", base10_num);
        //How many bits are contained in one base64 digit? log(64)/log(2). 6 bits
        //*bit-masking: If you want the k-th bit of n, then do (n & ( 1 << k )) >> k
        //base64_num[strlen(base64_num)-1] = '\0'; //Null terminate strings returned
        int k = 0, l = bSize-1;
        int b64bits = (bSize)*6; //6 bits per 4-bit hex digit
        //printf("l: %d, b64bits: %d\n", l, b64bits);
        for(k = 0; k < b64bits; k+=6)
        {
            unsigned int b64_word = base10_num >> k & 0x3f;
            base64_num[l--] = base64_digits[b64_word];
        }
        return base64_num;
    } 
    else 
    {
        paramFailure();
    }
}

int main(int argc, char** argv)
{
    if(argc != 2) //No params specified
    {
        paramFailure();
    } 
    else 
    {    
        //Split input into LCM(4,6) bit blocks (should verify first)
        //LCM(4,6) = 12,24.. i.e. 3 hex characters -> 2 b64 characters
        int b16BlockSize = 3;
        int b16Strlen = strlen(argv[1]);
        int b16Blocks = b16Strlen/b16BlockSize;
        int b16Padding = b16Strlen%b16BlockSize;
        //printf("len: %d, n blocks: %d, padding: %d\n", b16Strlen, b16Blocks, b16Padding);
        int b64padding = b16Padding > 0 ? 3:0;
        char b64PadString[3] = {'=','=','='};
        char * base64_num = malloc((b16Blocks*2)+b64padding+1); //Assign memory for building output string
        base64_num[sizeof(base64_num)] = '\0';
        
        //Convert whole blocks from hex to b64
        int bCount;
        char base16_word[3], base64_word[2];
        
        for(bCount = 0; bCount < b16Blocks; bCount++)
        {   
            memcpy(base16_word, argv[1]+(bCount*3), 3); //Copy 3 digits of hex into register
            hexToB64(base16_word, base64_word, sizeof(base64_word)); //Convert register contents into 2 digits of b64
            memcpy(base64_num+(bCount*sizeof(base64_word)), base64_word, sizeof(base64_word)); //Copy 2 digits b64 to output string
        }

        if(b16Padding > 0) //blocks of input < 3 characters; padding.
        {
            //Need to append padding block b64PadString
            memcpy(base16_word, argv[1]+(bCount*3), 3);
            hexToB64(base16_word, base64_word, sizeof(base64_word));
            memcpy(b64PadString, base64_word, sizeof(base64_word)); //Copy 2 digits b64 to padding string
            memcpy(base64_num+(bCount*sizeof(base64_word)), b64PadString, sizeof(b64PadString)); //Copy pad string to output
        }
        
        printf("%s\n", base64_num);
        free(base64_num);
    }
    
    exit(EXIT_SUCCESS);
}
