//Q. Write a function that takes two equal-length buffers and produces their XOR combination.
//A. This solution pads the inputs to be a unit of the block length (8) without changing the 
//   logical value of the buffer.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define DEBUG 1 

//Should we find non-hexadecimal characters at the input buffer
int paramFailure() 
{
    printf ("Specify two (only) hex numbers as input, i.e. fixed_xor 4fd3 43fe\n");
    exit(EXIT_FAILURE);
}

//Convert hexadecimal characters to byte values. 
uint8_t hexCharToBin(char c)
{
    uint8_t afHex[6] = {10,11,12,13,14,15};
    
    if ( (int)c >= 48 && (int)c <=57 ) //0-9 characters
    {
        return (int)c-48;
    } 
    else if ( (int)c >= 65 && (int)c <=70 ) //A-Z
    {
        return afHex[(int)c-65];
    } 
    else if ( (int)c >= 97 && (int)c <=102 ) //a-z
    {
        return afHex[(int)c-97];
    } 
    else 
    {
        printf("Found non-hexadecimal value '%c' in input.\n", c);
        paramFailure();
    }
}

//Convert hexadecimal characters to byte values. 
char* binToHexWord(unsigned int bin, char* hexWord, unsigned int hexWordLen)
{
    uint8_t afHex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    int n = 0;
    for(n = 28; n >= 0; n-=4)
    {
        unsigned int nibble = bin >> n & 0xf;
        printf("%c", afHex[nibble]);
    }
}

//Initialise the size first elements of cArr to val
void initCArr(char* cArr, char val, int size)
{
    int i = 0;
    for(i = 0; i < size; i++)
    {
        cArr[i] = val;
    }
}

void hexBufferToBase2(char * buffer, unsigned int* base2_num, 
                                     unsigned int b16BlockSize, 
                                     unsigned int b16Blocks, 
                                     unsigned int b16Padding, 
                                     unsigned int b2padding)
{
    //printf("%s\n", buffer);
    unsigned long long b2PadString = 0;                   
    
    //Convert blocks from hex to b2
    int bCount = 0;
    char base16_word[b16BlockSize+1];
    initCArr(base16_word, '0', sizeof(base16_word));
    base16_word[b16BlockSize] = '\0';
    unsigned int base2_word = 0;
    unsigned int base2_padBlockCount = 0;


    //Do padding first, we want leading zeroes so that logical value is unchanged
    if(b2padding > 0) //blocks of input < sizeof(int) characters; padding.
    {
        memcpy(base16_word+(b16BlockSize-b16Padding), buffer, b16Padding); //Copy partial block from argv to word
        if(DEBUG) printf( "%s ", base16_word );
        
        //Each character of base16_word
        int iNibble = 0;
        for(iNibble = 0; iNibble < b16BlockSize; iNibble++)
        {   
            //For each nibble, shift the nibble representation into the base2_word register in the correct location.
            base2_word = base2_word | ( (hexCharToBin(base16_word[iNibble])&0xf)<<((4*(b16BlockSize-1))-(4*iNibble)) );
                                                                   //f->00001111<<((4*7)-(4*7)) (4*6) (4*5)
            //if(DEBUG) printf( "%lu ", base2_word );
        }
        if(DEBUG) printf( "base2_num[%lu++] = %lu, ", bCount, base2_word );
        base2_num[bCount++] = base2_word; //Offset by leading 0 padding blocks
    }
     
    for(bCount = bCount; bCount < b16Blocks+b2padding; bCount++)
    {   
        base2_word = 0;

        memcpy(base16_word, buffer+(((bCount-b2padding)*b16BlockSize)+b16Padding), b16BlockSize); //Copy block from buffer to word
        if(DEBUG) printf( "%s ", base16_word );
        
        //Each character of base16_word
        int iNibble = 0;
        for(iNibble = 0; iNibble < b16BlockSize; iNibble++)
        {   
            //For each nibble, shift the nibble representation into the base2_word register in the correct location.
            base2_word = base2_word | ( (hexCharToBin(base16_word[iNibble])&0xf)<<((4*(b16BlockSize-1))-(4*iNibble)) );
            //if(DEBUG) printf( "%lu ", base2_word );
        }
        if(DEBUG) printf( "base2_num[%lu] = %lu, ", bCount, base2_word );
        base2_num[bCount] = base2_word;
    }
    if(DEBUG) printf("\n");
}

int main(int argc, char** argv)
{
    if(argc != 3) //No params specified
    {
        paramFailure();
    } 
    
    if(DEBUG) printf("Input buffer 1 length: %d, buffer 2 length %d\n", strlen(argv[1]), strlen(argv[2]));
    //Each hex character represents 4 bits, can read 8 of them into a 32 bit register
    
    unsigned int bufferInSize = strlen(argv[1])>strlen(argv[2])?strlen(argv[1]):strlen(argv[2]);
    //Need to pad both strings to the same length without changing their value, should also probably warn user.
    char *inb16_1, *inb16_2;
    if (strlen(argv[1])>strlen(argv[2]))
    {
        inb16_1 = argv[1];
        inb16_2 = malloc(bufferInSize); 
        unsigned int pad2 = 0;
        for(pad2 = 0; pad2 < bufferInSize-strlen(argv[2]); pad2++) //Pad leading zeroes
        {        
            inb16_2[pad2] = '0';
        }
        unsigned int argData = 0;
        for(pad2 = pad2; pad2 < bufferInSize; pad2++) //Copy remainder
        {        
            inb16_2[pad2] = argv[2][argData++];
        }
        inb16_2[pad2] = '\0';
        printf("WARNING, buffer 2 padded to: %s \n", inb16_2);
    } 
    else if (strlen(argv[1])<strlen(argv[2]))
    {
        inb16_2 = argv[2];
        inb16_1 = malloc(bufferInSize); 
        unsigned int pad1 = 0;
        for(pad1 = 0; pad1 < bufferInSize-strlen(argv[1]); pad1++) //Pad leading zeroes
        {        
            inb16_1[pad1] = '0';
        }
        unsigned int argData = 0;
        for(pad1 = pad1; pad1 < bufferInSize; pad1++) //Copy remainder
        {        
            inb16_1[pad1] = argv[1][argData++];
        }
        inb16_1[pad1] = '\0';
        printf("WARNING, buffer 1 padded to: %s \n", inb16_1);
    }
    else
    {
        inb16_1 = argv[1];
        inb16_2 = argv[2];
        if(DEBUG) printf("Equal length buffers at input.\n");
    }
    
    unsigned int b16BlockSize = 2*sizeof(unsigned int)/(sizeof(char)); //read block size
    unsigned int b2BlockSize = sizeof(unsigned int);
    unsigned int b16Blocks = bufferInSize/b16BlockSize;                   //how many blocks fit the input bufffer
    unsigned int b16Padding = bufferInSize%b16BlockSize;                  //how many chars overflow the blocks and need padding  
    unsigned int b2padding = b16Padding > 0 ? 1:0;                        //Is buffer a multiple of the block size?
    unsigned int bufferOutBlocks = 1 + ((bufferInSize - 1) / b16BlockSize); //ceil bufferSize/b16BlockSize
        
    unsigned int base2_num_1[bufferOutBlocks];
    hexBufferToBase2(inb16_1, base2_num_1, b16BlockSize, b16Blocks, b16Padding, b2padding);
    
    unsigned int base2_num_2[bufferOutBlocks];
    hexBufferToBase2(inb16_2, base2_num_2, b16BlockSize, b16Blocks, b16Padding, b2padding);
     
    //XOR the two numbers together, lengths are equal and multiple of block length.
    unsigned int base2_result[bufferOutBlocks];
    int iXOR = 0;
    for(iXOR = 0; iXOR < bufferOutBlocks; iXOR++ ){
        base2_result[iXOR] = base2_num_1[iXOR]^base2_num_2[iXOR];
        if(DEBUG) printf("base2_num_1[%d] = %lu, base2_num_2[%d] = %lu, result: %lu\n", iXOR, base2_num_1[iXOR], iXOR, base2_num_2[iXOR],  base2_result[iXOR]);
    }
    
    char hexWord[8];
    //Print the result niceley to stdout
    for(iXOR = 0; iXOR < bufferOutBlocks; iXOR++ ){
        binToHexWord(base2_result[iXOR], hexWord, 8);    
    }
    printf("\n");
   
}
