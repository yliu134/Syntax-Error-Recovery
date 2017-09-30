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
const char* names[] = {"check", "read", "write", "id", "literal", "gets", "if",
    "fi", "do", "od", "equal", "notequal", "smaller",
    "greater", "smallerequal","greaterequal",
    "add", "sub", "mul", "div", "lparen", "rparen", "eof", "eps", "end"};

static token input_token;
int tabNum = 0;
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
    cout << "Ahhh Syntax error" << endl;
    exit (1);
}

string match (token expected) {
    if (input_token == expected) {
        cout << "expected: " << names[expected] << "; actual: " << names[input_token] <<endl;
        cout << "matched " << endl;
        cout << "lololol\n";
        // cout << "match " <<  names[input_token];
        if (input_token == t_id || input_token == t_literal)
            cout << ": " <<  token_image;
        input_token = scan ();
    }
    else{
      cout << "match problem\n";
      error ();
    }

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
    cout << "input token: " << names[input_token] << endl;  switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_end:{
            cout << "predict program -->stmt_list eof" << endl;
            string str1 = "( program \n" ;
            str1 += stmt_list ();
            match (t_end);
            str1 += " ";
            str1 += names[t_end];
            return str1 + " )";
          }
        default:
        cout << "program wrong\n";
        error (); return " ";
    }
}

string stmt_list () {
  tabNum++;
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:{
            cout << "predict stmt_list --> stmt stmt_list" << endl;
            string str1 = "[ " + stmt ();
            string str2 = stmt_list ();
            str1 += str2;
            for(int i = 0; i <= tabNum; i++){
              str1 = "    " + str1;
            }
            tabNum--;
            return str1 + " ]\n";
          }
        case t_end:
            return "Ha ";          /*  epsilon production */
        default:
        cout << "statement list wrong\n";
        error ();
        return " ";
    }
}

string stmt () {
  tabNum++;
    switch (input_token) {
        case t_id:{
            match (t_id);
            match (t_gets);
            string str1 = relation();//Used to be expr()
            str1 = "(id := " + str1 + " )";
            for(int i = 0; i <= tabNum; i++){
              str1 = "    " + str1;
            }
            tabNum--;
            return str1;
          }
        case t_read:
            match (t_read);
            match (t_id);
            tabNum--;
            return "(read id) ";;
        case t_write:{
            match (t_write);
            string str1 = relation();//Used to be expr()
            for(int i = 0; i <= tabNum; i++){
              str1 = "    " + str1;
            }
            tabNum--;
            return "(write " + str1 + " )";
          }
        case t_if:{
            string str1 = relation();
            string str2 = stmt_list ();
            match(t_fi);
            str1 = "( " + str1 + str2 + ")";
            for(int i = 0; i <= tabNum; i++){
              str1 = "    " + str1;
            }
            tabNum--;
            return str1;
          }
        case t_do:{
            string str1 = stmt_list();
            match(t_od);
            tabNum--;
            return "( do "+ str1 + "od) ";;
          }
        case t_check:{
            string str1 = relation();
            tabNum--;
            return "( check "+str1+") ";;
          }
        default: //SyntaxErrorException e; throw e; //Throw the exception
            cout << "statement wrong\n";
            error();
            tabNum--;
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
  tabNum++;
    cout << "input token: " << names[input_token] << endl;
            string str1 = term ();
            str1 += term_tail ();
            tabNum--;
            return "( "+ str1 +")";
        cout << "expr wrong\n";
        error ();
        tabNum--;
        return " ";

}
// the new built method by us

string expr_tail(){
  tabNum++;
    cout << "input token: " << names[input_token] << endl;  switch (input_token) {
        case t_equal:
        case t_notequal:
        case t_smaller:
        case t_greater:
        case t_smallerequal:
        case t_greaterequal:{
            string str1 = relation_op();
            string str2 = expr();
            tabNum--;
            return "( "+str1+str2+" ) ";;
          }
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
        tabNum--;
            return " ";;
        default:
        cout << "expr tail wrong\n";
        error ();
        tabNum--;
        return " ";
        //predict set ET -> epsilon = SL
    }
}

string term_tail () {
  tabNum++;
    cout << "input token: " << names[input_token] << endl;  switch (input_token) {
        case t_add:
        case t_sub:{
            string str1 = add_op ();
            str1 += term ();
            str1 += term_tail ();
            tabNum--;
            return "( "+str1+" ) ";;
          }
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
        tabNum--;
            return " ";;          /*  epsilon production */
        default:
        cout << "term tail wrong\n";
        error ();
        tabNum--;
        return " ";
    }
}

string term () {
  tabNum++;
            string str1 = factor ();
            str1 + factor_tail ();
            tabNum--;
            return "( "+str1+" ) ";;
        cout << "term wrong\n";
        error ();
        tabNum--;
        return " ";

}

string factor_tail () {
  tabNum++;
    switch (input_token) {
        case t_mul:
        case t_div:{
            string str1 = mul_op ();
            str1 += factor ();
            str1 += factor_tail ();
            tabNum--;
            return "( "+str1+" ) ";;
          }
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
        tabNum--;
            return "Ha ";          /*  epsilon production */
        default:
        cout << "factor tail wrong\n";
        error ();
        tabNum--;
        return " ";
    }
}

string factor () {
  tabNum++;
    switch (input_token) {
        case t_id :
            match (t_id);
            tabNum--;
            return "Ha ";
        case t_literal:
            match (t_literal);
            tabNum--;
            return "Ha ";
        case t_lparen:
            match (t_lparen);
            expr ();
            match (t_rparen);
            tabNum--;
            return "Ha ";
        default:
        cout << "factor wrong\n";
        error (); tabNum--;
        return " ";
    }
}
// the new built one
string relation_op(){
  tabNum++;
    switch(input_token){
        case t_equal:
        match(t_equal);
        tabNum--;
            return "Ha ";
        case t_notequal:
            match(t_notequal);
            tabNum--;
            return "Ha ";
        case t_smaller:
            match(t_smaller);
            tabNum--;
            return "Ha ";
        case t_greater:
            match(t_greater);
            tabNum--;
            return "Ha ";
        case t_smallerequal:
            match(t_smallerequal);
            tabNum--;
            return "Ha ";
        case t_greaterequal:
            match(t_greaterequal);
            tabNum--;
            return "Ha ";
        default:
        cout << "relatioon op wrong\n";
        error (); tabNum--; return " ";
    }
}

string add_op () {
  tabNum++;
    switch (input_token) {
        case t_add:
            match (t_add);
            tabNum--;
            return "Ha ";
        case t_sub:
            match (t_sub);
            tabNum--;
            return "Ha ";
        default:
        cout << "add op wrong\n";
        error ();
        tabNum--;
        return " ";
    }
}

string mul_op () {
  tabNum++;
    cout << "input token: " << names[input_token] << endl;  switch (input_token) {
        case t_mul:
            printf ("predict mul_op --> mul\n");
            match (t_mul);
            tabNum--;
            return "Ha ";
        case t_div:
            printf ("predict mul_op --> div\n");
            match (t_div);
            tabNum--;
            return "Ha ";
        default:
        tabNum--;
        cout << "mul op wrong\n";
        error ();
        return " ";
    }
}

string relation(){
  tabNum++;
    expr();
    expr_tail();
    return " relation finished \n";
    tabNum--;
    cout << "relation wrong\n";
    //should be catching exception here
}

int main () {
    input_token = scan ();
    cout << program ();
    return 0;
}
