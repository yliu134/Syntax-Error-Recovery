/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.16.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
    Michael L. Scott, 2008-2017.
*/
#include <stdexcept>
#include <iostream>
#include "scan.h"
#include <cstdlib>
#include <vector>
using namespace std;
#include <string>

class SyntaxErrorException : public exception{
    public:
        const char * what(token token, const char* nonterminal) const throw()
        {
            return "Found token "+names[token]+" in " +nonterminal+" "+"\n";
        }
};

const char* names[] = {"read", "write", "id", "literal", "gets", "if",
    "fi", "do", "od", "equal", "colonequal", "doubleequal", "notequal", "smaller",
    "greater", "smallerequal","greaterequal",
    "add", "sub", "mul", "div", "lparen", "rparen", "eof", "eps"};

static token input_token;

// Hard-coded first and follow sets
typedef vector<token> token_set;
token_set first_stmt {t_id, t_read, t_write, t_if;
token_set follow_stmt {};

//Check if a token is in first or follow set of some category
bool contains(token input, token_set theSet){
    return find(begin(theSet), end(theSet), input != end(theSet));
}

void error () {
    cout << "Syntax error" << endl;
    exit (1);
}

void match (token expected) {
    if (input_token == expected) {
        cout << "matched" << names[input_token]<< endl;
        //printf ("matched %s", names[input_token]);
        // cout << "match " <<  names[input_token];
        if (input_token == t_id || input_token == t_literal)
            //printf (": %s", token_image);
            cout << ": " <<  token_image;
        //printf ("\n");
        cout << "\n" << endl;
        input_token = scan ();
    }
    else error ();
}

void program ();
void stmt_list ();
void stmt ();
void expr ();
void expr_tail();
void term_tail ();
void term ();
void factor_tail ();
void factor ();
void relation_op();
void add_op ();
void mul_op ();
void relation();

void program () {
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            cout << "predict program -->stmt_list eof" << endl;
            //printf ("predict program --> stmt_list eof\n");
            stmt_list ();
            match (t_eof);
            break;
        default: error ();
    }
}

void stmt_list () {
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
            //printf ("predict stmt_list --> stmt stmt_list\n");
            cout << "predict stmt_list --> stmt stmt_list" << endl;
            stmt ();
            stmt_list ();
            break;
        case t_eof:
            //printf ("predict stmt_list --> epsilon\n");
            break;          /*  epsilon production */
        default: error ();
    }
}

void stmt () {
    switch (input_token) {
        case t_id:
            //printf ("predict stmt --> id gets expr\n");
            match (t_id);
            match (t_gets);
            expr ();
            break;
        case t_read:
            //printf ("predict stmt --> read id\n");
            match (t_read);
            match (t_id);
            break;
        case t_write:
            //printf ("predict stmt --> write expr\n");
            match (t_write);
            expr ();
            break;
        case t_if:
            relation();
            stmt_list ();
            match(t_fi);
            break;
        case t_do:
            stmt_list();
            match(t_od);
            break;
        case t_check:
            relation();
            break;
        default: //SyntaxErrorException e; throw e; //Throw the exception
            while(!contains(input_token, follow_stmt)
                ||input_token != t_eof){
                input_token = scan();
            }
            if(contains(input_token, follow_stmt)){
                return;
            }else{

            } //If having reached eof
    }
}

void expr () {
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            //printf ("predict expr --> term term_tail\n");
            term ();
            term_tail ();
            break;
        default: error ();
    }
}
// the new built method by us

void expr_tail(){
    switch (input_token) {
        case t_doubleequal:
        case t_notequal:
        case t_smaller:
        case t_greater:
        case t_smallerequal:
        case t_greaterequal:
            expr();
            break;
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            break;
        default: error ();
        //predict set ET -> epsilon = SL
    }
}

void term_tail () {
    switch (input_token) {
        case t_add:
        case t_sub:
            //printf ("predict term_tail --> add_op term term_tail\n");
            add_op ();
            term ();
            term_tail ();
            break;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            //printf ("predict term_tail --> epsilon\n");
            break;          /*  epsilon production */
        default: error ();
    }
}

void term () {
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            //printf ("predict term --> factor factor_tail\n");
            factor ();
            factor_tail ();
            break;
        default: error ();
    }
}

void factor_tail () {
    switch (input_token) {
        case t_mul:
        case t_div:
            //printf ("predict factor_tail --> mul_op factor factor_tail\n");
            mul_op ();
            factor ();
            factor_tail ();
            break;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            //printf ("predict factor_tail --> epsilon\n");
            break;          /*  epsilon production */
        default: error ();
    }
}

void factor () {
    switch (input_token) {
        case t_id :
            //printf ("predict factor --> id\n");
            match (t_id);
            break;
        case t_literal:
            //printf ("predict factor --> literal\n");
            match (t_literal);
            break;
        case t_lparen:
            //printf ("predict factor --> lparen expr rparen\n");
            match (t_lparen);
            expr ();
            match (t_rparen);
            break;
        default: error ();
    }
}
// the new built one
void relation_op(){
    switch(input_token){
        case t_doubleequal:
        match(t_doubleequal);
            break;
        case t_notequal:
            match(t_notequal);
            break;
        case t_smaller:
            match(t_smaller);
            break;
        case t_greater:
            match(t_greater);
            break;
        case t_smallerequal:
            match(t_smallerequal);
            break;
        case t_greaterequal:
            match(t_greaterequal);
            break;
        default: error();
    }
}

void add_op () {
    switch (input_token) {
        case t_add:
            //printf ("predict add_op --> add\n");
            match (t_add);
            break;
        case t_sub:
            //printf ("predict add_op --> sub\n");
            match (t_sub);
            break;
        default: error ();
    }
}

void mul_op () {
    switch (input_token) {
        case t_mul:
            //printf ("predict mul_op --> mul\n");
            match (t_mul);
            break;
        case t_div:
            //printf ("predict mul_op --> div\n");
            match (t_div);
            break;
        default: error ();
    }
}

void relation(){
    expr();
    expr_tail();
}

int main () {
    input_token = scan ();
    program ();
    return 0;
}
