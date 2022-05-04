#include "a2.h"

int bitwise_xor(int value)
{
    return value ^ KEY;
}

char *xor_encrypt(char c)
{
    int value_to_encrypt = bitwise_xor(c);
    char *binary_result = (char *)malloc(sizeof(char) * 8);
    binary_result[7] = '\0';
    for (int i = 7; i > 0; i--)
    {
        binary_result[i - 1] = value_to_encrypt % 2 + '0';
        value_to_encrypt /= 2;
    }
    return binary_result;
}

char xor_decrypt(char *s)
{
    int ascii_of_binary = 0;
    for (int i = 0; i < 7; i++)
    {
        ascii_of_binary += (s[i] - '0') * pow(2, 6 - i);
    }

    return bitwise_xor(ascii_of_binary);
}

char *gen_code(char *msg)
{
    // initializing the SC Code matrix
    char *code_string = (char *)malloc(sizeof(char) * 257);
    code_string[256] = '\0';
    for (int i = 0; i < 256; i++)
    {
        code_string[i] = '0';
    }
    int restricted_indicies[256] = {0};

    for (int i = 0; i < 5; i++)
    {
        code_string[i] = '1';
        restricted_indicies[i] = 1;

        code_string[i + 11] = '1';
        restricted_indicies[i + 11] = 1;

        code_string[i + 64] = '1';
        code_string[i + 75] = '1';
        restricted_indicies[i + 64] = 1;
        restricted_indicies[i + 75] = 1;

        code_string[i + 176] = '1';
        code_string[i + 240] = '1';
        restricted_indicies[i + 176] = 1;
        restricted_indicies[i + 240] = 1;
    }
    for (int i = 0; i < 5; i += 2)
    {
        code_string[i + 32] = '1';
        code_string[i + 43] = '1';
        code_string[i + 208] = '1';
        restricted_indicies[i + 32] = 1;
        restricted_indicies[i + 43] = 1;
        restricted_indicies[i + 208] = 1;
    }
    for (int i = 0; i < 5; i += 4)
    {
        code_string[i + 16] = '1';
        code_string[i + 27] = '1';
        restricted_indicies[i + 16] = 1;
        restricted_indicies[i + 27] = 1;

        code_string[i + 48] = '1';
        code_string[i + 59] = '1';
        restricted_indicies[i + 48] = 1;
        restricted_indicies[i + 59] = 1;

        code_string[i + 192] = '1';
        code_string[i + 224] = '1';
        restricted_indicies[i + 192] = 1;
        restricted_indicies[i + 224] = 1;
    }

    // restriced indicies
    for (int i = 0; i < 3; i++)
    {
        restricted_indicies[i + 17] = 1;
        restricted_indicies[i + 33] = 1;
        restricted_indicies[i + 49] = 1;

        restricted_indicies[i + 28] = 1;
        restricted_indicies[i + 44] = 1;
        restricted_indicies[i + 60] = 1;

        restricted_indicies[i + 193] = 1;
        restricted_indicies[i + 209] = 1;
        restricted_indicies[i + 225] = 1;
    }
    code_string[255] = '1';
    restricted_indicies[255] = 1;

    // filling the SC Code Matrix
    for (int i = 0; i < strlen(msg) + 1; i++)
    {
        char *encrypted_message = xor_encrypt(msg[i]);

        for (int j = 0; j < strlen(encrypted_message); j++)
        {
            for (int k = 0; k < 256; k++)
            {
                if (restricted_indicies[k] != 1)
                {
                    code_string[k] = encrypted_message[j];
                    restricted_indicies[k] = 1;
                    break;
                }
            }
        }
        free(encrypted_message);
    }
    return code_string;
}

char *read_code(char *code)
{
    // making list of restriced indicies
    int restricted_indicies[256] = {0};
    for (int i = 0; i < 5; i++)
    {
        restricted_indicies[i] = 1;
        restricted_indicies[i + 16] = 1;
        restricted_indicies[i + 32] = 1;
        restricted_indicies[i + 48] = 1;
        restricted_indicies[i + 64] = 1;

        restricted_indicies[i + 11] = 1;
        restricted_indicies[i + 27] = 1;
        restricted_indicies[i + 43] = 1;
        restricted_indicies[i + 59] = 1;
        restricted_indicies[i + 75] = 1;

        restricted_indicies[i + 176] = 1;
        restricted_indicies[i + 192] = 1;
        restricted_indicies[i + 208] = 1;
        restricted_indicies[i + 224] = 1;
        restricted_indicies[i + 240] = 1;
    }
    restricted_indicies[255] = 1;

    char *decrypted_message = (char *)malloc(sizeof(char) * 100); // idk exact max change this later
    int decrypted_message_index = 0;
    int count = 0;
    while (1)
    {
        char encrypted_character[8];
        // getting binary of one encrypted character
        int index = 0;
        while (index < 7)
        {
            if (restricted_indicies[count] == 1)
            {
                count++;
                continue;
            }
            encrypted_character[index] = code[count];
            index++;
            count++;
        }

        // decrypt character
        decrypted_message[decrypted_message_index] = xor_decrypt(encrypted_character);
        if (decrypted_message[decrypted_message_index] == '\0')
        {
            break;
        }
        decrypted_message_index++;
    }

    return decrypted_message;
}

char *compress(char *code)
{
    // compressed string
    char *compressed_sc_code = (char *)malloc(sizeof(char) * 65);
    compressed_sc_code[64] = '\0';
    int compressed_sc_code_index = 0;
    for (int i = 0; i < 256; i += 4)
    {
        int temp = 0;
        for (int j = 0; j < 4; j++)
        {
            // temp stores the decimal associate with every 4 binary nums
            temp += ((code[i + j] - '0') * pow(2, 3 - j));
        }

        char hex[2];
        sprintf(hex, "%X", temp);
        compressed_sc_code[compressed_sc_code_index] = hex[0];
        compressed_sc_code_index++;
    }
    return compressed_sc_code;
}

char *decompress(char *code)
{
    char *decompressed_hexadecimal = (char *)malloc(sizeof(char) * 257);
    decompressed_hexadecimal[256] = '\0';
    int decompressed_hexadecimal_index = 0;
    for (int i = 0; i < 64; i++)
    {
        // convert to decimal
        int val;
        if (code[i] >= '0' && code[i] <= '9')
        {
            val = code[i] - 48;
        }
        else if (code[i] >= 'A' && code[i] <= 'F')
        {
            val = code[i] - 65 + 10;
        }

        // convert to binary
        char binary_result[5];
        for (int j = 4; j > 0; j--)
        {
            binary_result[j - 1] = val % 2 + '0';
            val /= 2;
        }
        // store into decompressed hexadecimal
        for (int j = 0; j < 4; j++)
        {
            decompressed_hexadecimal[decompressed_hexadecimal_index] = binary_result[j];
            decompressed_hexadecimal_index++;
        }
    }

    return decompressed_hexadecimal;
}

int calc_ld(char *sandy, char *cima)
{

    // initialize matrix
    int matrix[strlen(sandy) + 1][strlen(cima) + 1];

    // initalize first row and first column
    matrix[0][0] = 0;
    for (int i = 1; i < strlen(sandy) + 1; i++)
    {
        matrix[i][0] = matrix[i - 1][0] + 1;
    }
    for (int j = 1; j < strlen(cima) + 1; j++)
    {
        matrix[0][j] = matrix[0][j - 1] + 1;
    }

    // start at [1][1] and check row + column
    // move to next diagonal [2][2] and repeat steps until end
    for (int i = 1; i < strlen(sandy) + 1; i++)
    {
        for (int j = 1; j < strlen(cima) + 1; j++)
        {
            // check if characters are equal
            if (sandy[i - 1] == cima[j - 1])
            {
                matrix[i][j] = matrix[i - 1][j - 1];
                continue;
            }

            // find min of substitution, deletion, and insertion
            int min_value;

            // substitution
            min_value = matrix[i - 1][j - 1];

            // deletion
            if (min_value > matrix[i - 1][j])
            {
                min_value = matrix[i - 1][j];
            }

            // insertion
            if (min_value > matrix[i][j - 1])
            {
                min_value = matrix[i][j - 1];
            }

            // assigning value
            matrix[i][j] = 1 + min_value;
        }
    }

    // // print matrix
    // for (int i = 0; i < strlen(sandy) + 1; i++)
    // {
    //     for (int j = 0; j < strlen(cima) + 1; j++)
    //     {
    //         printf("%d ", matrix[i][j]);
    //     }
    //     printf("\n");
    // }

    return matrix[strlen(sandy)][strlen(cima)];
}

