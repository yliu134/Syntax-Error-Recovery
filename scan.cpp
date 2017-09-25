/* Simple ad-hoc scanner for the calculator language.
    Michael L. Scott, 2008-2017.
*/

#include<iostream>
//#include "stdio.h"
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "scan.h"
using namespace std;

char token_image[100];

token scan() {
    static int c = ' ';
        /* next available char; extra (int) width accommodates EOF */
    int i = 0;              /* index into token_image */

    /* skip white space */
    while (isspace(c)) {
        c = getchar();
    }
    if (c == EOF)
        return t_eof;
    if (isalpha(c)) {
        do {
            token_image[i++] = c;
            c = getchar();
        } while (isalpha(c) || isdigit(c) || c == '_');
        token_image[i] = '\0';
        if (!strcmp(token_image, "read")) return t_read;
        else if (!strcmp(token_image, "write")) return t_write;
        //Added do, od, if and fi case
        else if (!strcmp(token_image, "do")) return t_do;
        else if (!strcmp(token_image, "od")) return t_od;
        else if (!strcmp(token_image, "if")) return t_if;
        else if (!strcmp(token_image, "fi")) return t_fi;
        else return t_id;
    }
    else if (isdigit(c)) {
        do {
            token_image[i++] = c;
            c = getchar();
        } while (isdigit(c));
        token_image[i] = '\0';
        return t_literal;
    } else switch (c) {
        case ':':
            if ((c = getchar()) != '=') {
                //fprintf(stderr, "error\n");
                cout << "error\n" << endl;
                exit(1);
            } else {
                c = getchar();
                return t_gets;
            }
            break;
        case '+': c = getchar(); return t_add;
        case '-': c = getchar(); return t_sub;
        case '*': c = getchar(); return t_mul;
        case '/': c = getchar(); return t_div;
        case '(': c = getchar(); return t_lparen;
        case ')': c = getchar(); return t_rparen;
        //Relation operators added
        case '<': c = getchar();
            if (c == '>'){
                c = getchar();
                return t_notequal;
            }else if(c == '='){
                c = getchar();
                return t_smallerequal;
            }else return t_smaller;
        case '>': 
            if(getchar() == '='){
                c = getchar();
                return t_greaterequal;
            }else return t_greater;
        case '=': 
            if(getchar() == '='){
                c = getchar();
                return t_doubleequal;
            }else return t_equal;
        default:
            //printf("error\n");
            cout << "error\n" << endl;
            exit(1);
    }
}
