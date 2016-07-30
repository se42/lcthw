#include <stdio.h>

int main(int argc, char *argv[])
{
    if(argc != 2) {
        printf("ERROR: You need 1 argument.\n");
        // this is how you abort a program
        return 1;
    }

    int i = 0;
    for(i = 0; argv[1][i] != '\0'; i++){
        char letter = argv[1][i];

        switch(letter){
            case 'a': // fallthrough
            case 'A':
                printf("%d: 'A'\n", i);
                break;

            case 'e': // fallthrough
            case 'E':
                printf("%d: 'E'\n", i);
                break;

            case 'i': // fallthrough
            case 'I':
                printf("%d: 'I'\n", i);
                break;

            case 'o': // fallthrough
            case 'O':
                printf("%d: 'O'\n", i);
                break;

            case 'u': // fallthrough
            case 'U':
                printf("%d: 'U'\n", i);
                break;

            case 'y': // fallthrough
            case 'Y':
                if(i > 2){
                    printf("%d: 'Y'\n", i);
                    break;
                }
                // break; NOTE: break here didn't send leading y to default case

            default:
                printf("%d: %c is not a vowel\n", i, letter);
        }
    }


    for(i=0; argv[1][i] != '\0'; i++){
        char lt = argv[1][i];
        if(lt >=65 && lt <= 90){
            lt += 32;
        }

        switch(lt){
            case 'a':
                printf("%d: '%c' --> 'A'\n", i, lt);
                break;

            case 'e':
                printf("%d: '%c' --> 'E'\n", i, lt);
                break;

            case 'i':
                printf("%d: '%c' --> 'I'\n", i, lt);
                break;

            case 'o':
                printf("%d: '%c' --> 'O'\n", i, lt);
                break;

            case 'u':
                printf("%d: '%c' --> 'U'\n", i, lt);
                break;

            case 'y':
                if(i > 2){
                    printf("%d: 'Y'\n", i);
                    break;
                }
                // break; NOTE: break here didn't send leading y to default case

            default:
                printf("%d: %c is not a vowel\n", i, lt);
        }
    }

    return 0;
}
