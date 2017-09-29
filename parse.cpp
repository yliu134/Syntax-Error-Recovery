/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.16.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
    Michael L. Scott, 2008-2017.
*/
#include <stdexcept>
#include <iostream>
#include <ios>
#include "scan.h"
#include <cstdlib>
#include <vector>
#include <string>
//using this instead of namespace std to avoid allocating for those unused
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::noskipws;

/*
class SyntaxErrorException : public exception{
    public:
        const char * what(token token, const char* nonterminal) const throw()
        {
            // return "Found token "+names[token]+" in " +nonterminal+" "+"\n";
        }
};*/
//replace colonequal and doubleequal
const char* names[] = {"read", "write", "id", "literal", "gets", "if",
    "fi", "do", "od", "equal", "notequal", "smaller",
    "greater", "smallerequal","greaterequal",
    "add", "sub", "mul", "div", "lparen", "rparen", "eof", "eps"};

static token input_token;

// Hard-coded first and follow sets
// typedef vector<token> token_set;
// token_set first_stmt {t_id, t_read, t_write, t_if;
// token_set follow_stmt {};

//Check if a token is in first or follow set of some category
int contains(token t, token set[]){
  int i = 0;
  while(set[i]){
    if (set[i] == t) return 1;
    i++;
  }
  return 0;
}

void error () {
    cout << "Syntax error" << endl;
    exit (1);
}

string match (token expected) {
    if (input_token == expected) {
        cout << "matched " << names[input_token]<< endl;
        // cout << "match " <<  names[input_token];
        if (input_token == t_id || input_token == t_literal)
            cout << ": " <<  token_image;
        printf ("\n");
        cout << "\n";
        input_token = scan ();
    }
    else error ();
    return " ";
}

string program ();
string stmt_list ();
string stmt ();
string expr ();
string expr_tail();
string term_tail ();
string term ();
string factor_tail ();
string factor ();
string relation_op();
string add_op ();
string mul_op ();
string relation();

string program () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_eof:{
            cout << "predict program -->stmt_list eof" << endl;
            string str1 = "( program \n" ;
            str1 += stmt_list ();
            match (t_eof);
            str1 += " ";
            str1 += names[t_eof];
            return str1 + " )";
          }
        default: error (); return " ";
    }
}

string stmt_list () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_id:
        case t_read:
        case t_write:{
            cout << "predict stmt_list --> stmt stmt_list" << endl;
            string str1 = "[ " + stmt ();
            string str2 = stmt_list ();
            str1 += str2;
            return str1 + " ]";
          }
        case t_eof:
            printf ("predict stmt_list --> epsilon\n");
            return "Ha ";          /*  epsilon production */
        default: error (); return " ";
    }
}

string stmt () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_id:{
            printf ("predict stmt --> id gets expr\n");
            match (t_id);
            match (t_gets);
            string str1 = relation();//Used to be expr()
            string str2 = "(id := " + str1 + " )";
            return str2;
          }
        case t_read:
            printf ("predict stmt --> read id\n");
            match (t_read);
            match (t_id);
            return "(read id) ";;
        case t_write:{
            printf ("predict stmt --> write expr\n");
            match (t_write);
            string str1 = relation();//Used to be expr()
            return "(write " + str1 + " )";
          }
        case t_if:{
            string str1 = relation();
            string str2 = stmt_list ();
            match(t_fi);
            return "( " + str1 + str2 + ")";;
          }
        case t_do:{
            string str1 = stmt_list();
            match(t_od);
            return "( do "+ str1 + "od) ";;
          }
        case t_check:{
            string str1 = relation();
            return "( check "+str1+") ";;
          }
        default: //SyntaxErrorException e; throw e; //Throw the exception
            error();
            return " ";
            /*
            while(!contains(input_token, follow_stmt)
                ||input_token != t_eof){
                input_token = scan();
            }
            if(contains(input_token, follow_stmt)){
                return;
            }else{

            } //If having reached eof
            */
    }
}

string expr () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:{
            printf ("predict expr --> term term_tail\n");
            string str1 = term ();
            str1 += term_tail ();
            return "( "+ str1 +")";;
          }
        default: error (); return " ";
    }
}
// the new built method by us

string expr_tail(){
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_equal:
        case t_notequal:
        case t_smaller:
        case t_greater:
        case t_smallerequal:
        case t_greaterequal:{
            string str1 = relation_op();
            string str2 = expr();
            return "( "+str1+str2+" ) ";;
          }
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            return " ";;
        default: error (); return " ";
        //predict set ET -> epsilon = SL
    }
}

string term_tail () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_add:
        case t_sub:{
            printf ("predict term_tail --> add_op term term_tail\n");
            string str1 = add_op ();
            str1 += term ();
            str1 += term_tail ();
            return "( "+str1+" ) ";;
          }
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            printf ("predict term_tail --> epsilon\n");
            return " ";;          /*  epsilon production */
        default: error (); return " ";
    }
}

string term () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:{
            printf ("predict term --> factor factor_tail\n");
            string str1 = factor ();
            str1 + factor_tail ();
            return "( "+str1+" ) ";;
          }
        default: error (); return " ";
    }
}

string factor_tail () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_mul:
        case t_div:{
            printf ("predict factor_tail --> mul_op factor factor_tail\n");
            string str1 = mul_op ();
            str1 += factor ();
            str1 += factor_tail ();
            return "( "+str1+" ) ";;
          }
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            printf ("predict factor_tail --> epsilon\n");
            return "Ha ";;          /*  epsilon production */
        default: error (); return " ";
    }
}

string factor () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_id :
            printf ("predict factor --> id\n");
            match (t_id);
            return "Ha ";;
        case t_literal:
            printf ("predict factor --> literal\n");
            match (t_literal);
            return "Ha ";;
        case t_lparen:
            printf ("predict factor --> lparen expr rparen\n");
            match (t_lparen);
            expr ();
            match (t_rparen);
            return "Ha ";;
        default: error (); return " ";
    }
}
// the new built one
string relation_op(){
    switch(input_token){
        case t_equal:
        match(t_equal);
            return "Ha ";;
        case t_notequal:
            match(t_notequal);
            return "Ha ";;
        case t_smaller:
            match(t_smaller);
            return "Ha ";;
        case t_greater:
            match(t_greater);
            return "Ha ";;
        case t_smallerequal:
            match(t_smallerequal);
            return "Ha ";;
        case t_greaterequal:
            match(t_greaterequal);
            return "Ha ";;
        default: error (); return " ";
    }
}

string add_op () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_add:
            printf ("predict add_op --> add\n");
            match (t_add);
            return "Ha ";;
        case t_sub:
            printf ("predict add_op --> sub\n");
            match (t_sub);
            return "Ha ";;
        default: error (); return " ";
    }
}

string mul_op () {
    cout << "input token: " << input_token << endl;  switch (input_token) {
        case t_mul:
            printf ("predict mul_op --> mul\n");
            match (t_mul);
            return "Ha ";;
        case t_div:
            printf ("predict mul_op --> div\n");
            match (t_div);
            return "Ha ";;
        default: error (); return " ";
    }
}

string relation(){
    expr();
    expr_tail();
    return " ";
}

int main () {
    input_token = scan ();
    program ();
    return 0;
}
