/*
    Simple Calculator
        Code based on B. STROUSTRUP's book: 
        "PROGRAMING Principles and Practice using C++"

    Hist:
        Reconstruction: cxnajder        summer 2023
        Update: Bjarne Stroustrup       august 2007
        ...
        Original author: Bjaorne Stroustrup
            (bs@cs.tamu.edu)            spring 2004
        
    This code implements calculator with basic expresions
    Imoprts input data form std::cin stream, outputs to std::cout

    Grammar:

        calculator:
            statement
            print
            quit

            contains: token_stream

        Statement:
            expression
            declaration

        print:
            ;

        quit:
            q or Q

        expression:
            term
            expression + term
            expression - term
        
        term:
            primary
            term * primary
            term / primary
            term % primary
        
        primary:
            number
            ( expression )
            - primary
            + primary

        number:
            floating-point-literal


            global:
                Variables and const chars/strs

*/

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <cctype>

void error(std::string s1, std::string s2 = ""){
    throw std::runtime_error(s1+s2);
}

 void keep_window_open()
{
	std::cin.clear();
	std::cout << "Please enter a character to exit\n";
	char ch;
	std::cin >> ch;
	return;
}

 void keep_window_open(std::string s)
{
	if (s == "") return;
	std::cin.clear();
	std::cin.ignore(120, '\n');
	for (;;) {
		std::string ss;
        do {
			std::cout << "Please enter " << s << " to exit\n";
        } while (std::cin >> ss && ss != s);
        
		return;
	}
}
 
//CONSTANT VALUES: *******************************************
//TODO: TURN INTO DICT IN FUTURE
const char QUIT = 'Q';
const char quit = 'q';
const char print = ';';
const std::string prompt = "> ";
const std::string result = "= ";
const char number = 'n';
const char let = 'l';
const char vname = 'v';
const std::string dec_key = "let";

//TOKEN ******************************************************
class Token {
public:
    char kind;
    double value;
    std::string varname;

    Token();
    Token(char ch);
    Token(char ch, double val);
    Token(char ch, std::string vn);

};

Token::Token() {}
Token::Token(char ch) : kind(ch), value(0) {}
Token::Token(char ch, double val) : kind(ch), value(val) {} 
Token::Token(char ch, std::string vn) : kind(ch), varname(vn) {}

//TOKEN_STREAM **********************************************
class Token_stream{
public:
    Token_stream();
    Token get();
    void putback(Token t);
    void ignore(char c); //will ignore cin till c (including)
private:
    Token buffer;
    bool full;
};

Token_stream::Token_stream() {}

void Token_stream::putback(Token t){
    if (full) error("Cannot call putback() when buffer is full.");
    this->buffer = t;
    this->full = true;
}

void Token_stream::ignore(char c){
    //check buffor first
    if (full && c == buffer.kind) {
        full=false;
        return;
    }
    full = false;
    //check cin input
    char ch = 0;
    while(std::cin>>ch)
        if (ch==c) return;
}

Token Token_stream::get(){
    if (full) {
        full = false;
        return buffer;
    }
    char ch;
    std::cin >> ch;
    switch (ch) 
    {
    case quit: 
    case QUIT:
    case print:
    case '(': 
    case ')':
    case '*': 
    case '/': 
    case '%':
    case '+': 
    case '-':
    case '=':
        return Token(ch);
    case '.':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        {
            std::cin.putback(ch);
            double val;
            std::cin >> val;
            return Token(number, val);
        }
    default:
        if (std::isalpha(ch)) {
            std::string s = "";
            s += ch;
            while (std::cin.get(ch) &&  std::isalpha(ch) || std::isdigit(ch))
                s += ch;
            std::cin.putback(ch);
            if (s == dec_key)
                return Token(let);
            return Token(vname, s);
        }
        error("Invalid token.");
    }
}

//VARIABLES **********************************************************
class Variable {
public:
    std::string name;
    double value;
};

std::vector<Variable>  var_table;

    //var_tab debug:
void check_table(){
    for (auto v: var_table)
        std::cout<<v.name<<" = "<<v.value<<'\n';
}

double get_value(const std::string & s){
    for (int i = 0; i < var_table.size(); ++i)
            if (var_table[i].name == s )
                return var_table[i].value;
    error("Accessing: undefined variable ", s);    
}
void set_value(const std::string & s, const double & d){
    for (int i = 0; i < var_table.size(); ++i)
            if (var_table[i].name == s ){
                var_table[i].value = d;
                return;
            }
    error("Changing: undefined variable ", s);    
}
bool is_declared (const std::string & varname){
    for (auto var: var_table)
        if (var.name == varname)
            return true;
    return false;
}
void define_var(const std::string & name, const double & val){
    if (is_declared(name))
        error(name, " - multiplyied declaration.");
    var_table.push_back(Variable{name, val});
    return;
}



//REST OF FUN ***************************************************************************
double statement(Token_stream & ts);
double declaration(Token_stream & ts);
double expression(Token_stream & ts);
double term(Token_stream & ts);
double primary(Token_stream & ts);

double statement(Token_stream & ts)
{
    Token t = ts.get();
    switch (t.kind)
    {
    case let:
        return declaration(ts);
    
    default:
        ts.putback(t);
        return expression(ts);
    }
}

double declaration(Token_stream & ts){
    Token t = ts.get();
    if (t.kind != vname)
        error("Expected name in declaration.");
    std::string varname = t.varname; 
    t = ts.get();
    if (t.kind != '=')
        error("Missing '=' sign in declaration.");
    double d = expression(ts);
    define_var(varname, d);
    return d;
}

double expression(Token_stream & ts){
    double left = term(ts);
    Token t = ts.get();
    while(true) {
        switch (t.kind)
        {
        case '+':
            left += term(ts);
            t = ts.get();
            break;
        case '-':
            left -= term(ts);
            t = ts.get();
            break;
        default:
            ts.putback(t);
            return left;
        }
    }
}

double term(Token_stream & ts){
    double left = primary(ts);
    Token t = ts.get();
    while (true) {
        switch (t.kind)
        {
        case '*':
            left *= primary(ts);
            t = ts.get();
            break;
        case '/':
            left /= primary(ts);
            t = ts.get();
            break;
        case '%':
            {
                double d = primary(ts);
                if (d == 0) 
                    error("Division by zero.");
                left = fmod(left, d);
                t = ts.get();
                break;
            }
        default:
            ts.putback(t);
            return left;
        }
    }
    
}

double primary(Token_stream & ts) {
    Token t = ts.get();
    switch (t.kind)
    {
    case '(':
        {
            double d = expression(ts);
            t = ts.get();
            if (t.kind != ')') error("Expected ')'.");
            return d;
        }
    case number:
        return t.value;
    case '-':
        return -primary(ts);
    case '+':
        return primary(ts);
    case vname:
        return get_value(t.varname);
    default:
        error("Invalid factor.");
    }
}

void clean_up(Token_stream & ts) {
    ts.ignore(print);
}

void calculate() {

    Token_stream ts; // TODO: MAKE IT A SINGLETON (TOKEN_STREAM)
    std::vector<Variable> var_table;
    while(std::cin){
        try {
            std::cout << prompt;
            Token t = ts.get();
            while(t.kind == print) 
                t = ts.get();
            if (t.kind == quit || t.kind == QUIT) 
                break;
            ts.putback(t);
            std::cout<<result<<statement(ts)<<'\n';
            check_table(); // <-- DIsplaying List of variables each prompt.
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            clean_up(ts);
        }
    }
    
    
}

int main(int argc, char const *argv[])
{
    
    try
    {
        calculate();   
        keep_window_open();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        keep_window_open("~~");
        return 1;
    }
    catch(...)
    {
        std::cerr << "Exception \n";
        keep_window_open("~~");
        return 2;
    }
    
    
}