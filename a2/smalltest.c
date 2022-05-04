#include "a2.h"

int main()
{
    char *test_code = gen_code("Hello World");
    char *test_string = compress(test_code);
    printf("%s\n", test_string);
    free(test_code);
    free(test_string);
    // Task 1
    printf("%d\n", bitwise_xor(67));
    char *bin_string = xor_encrypt('C');
    printf("%c\n", xor_decrypt(bin_string));
    free(bin_string);

    // Task 2
    char *code = gen_code("Program in C!");
    printf("%s\n", code);
    char *msg = read_code(code);
    printf("%s\n", msg);
    free(msg);

    // Task 3
    char *hexcode = compress(code);
    printf("%s\n", hexcode);
    free(code);

    code = decompress(hexcode);
    printf("%s\n", code);
    free(hexcode);
    char *samehexcode = compress(code);
    printf("%s\n", samehexcode);
    free(code);
    free(samehexcode);

    // Task 4
    printf("%d\n", calc_ld("COMMENCE", "CODING"));
    printf("%d\n", calc_ld("COMMENCE", "PROCRASTINATING"));
    return 0;
}