/*
    Simple Calculator
        Code based on B. STROUSTRUP's book: 
        "PROGRAMING Principles and Practice using C++"

    Hist:
        Reconstruction: cxnajder            summer 2023
        Update: Bjorn Stroustrup    august 2007
        ...
        Original author: Bjorne Stroustrup
            (bs@cs.tamu.edu)        spring 2004
        
    This code implements calculator with basic expresions
    Imoprts input data form std::cin stream, outputs to std::cout

    Grammar:
        Statement:
            expression
            print
            quit

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

*/

#include <iostream>
#include <string>
#include <cmath>


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
 

const char quit = 'q';
const char QUIT = 'Q';
const char print = ';';

const std::string prompt = "> ";
const std::string result = "= ";

const char number = 'n';


class Token{
public:
    char kind;
    double value;
};

class Token_stream{
public:
    Token_stream();
    Token get();
    void putback(Token t);
private:
    Token buffer;
    bool full;
};

Token_stream::Token_stream(){}

void Token_stream::putback(Token t){
    if (full) error("Cannot call putback() when buffer is full.");
    this->buffer = t;
    this->full = true;
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
        return Token{ch};
    case '.':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        {
            std::cin.putback(ch);
            double val;
            std::cin >> val;
            return Token{number, val};
        }
    
    default:
        break;
    }
}

double expression();
double term();
double primary();

Token_stream ts;

double expression(){
    double left = term();
    Token t = ts.get();
    while(true) {
        switch (t.kind)
        {
        case '+':
            left += term();
            t = ts.get();
            break;
        case '-':
            left -= term();
            t = ts.get();
            break;
        default:
            ts.putback(t);
            return left;
        }
    }
}

double term(){
    double left = primary();
    Token t = ts.get();
    while (true) {
        switch (t.kind)
        {
        case '*':
            left *= primary();
            t = ts.get();
            break;
        case '/':
            left /= primary();
            t = ts.get();
            break;
        case '%':
            {
                double d = primary();
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

double primary() {
    Token t = ts.get();
    switch (t.kind)
    {
    case '(':
        {
            double d = expression();
            t = ts.get();
            if (t.kind != ')') error("Expected ')'.");
            return d;
        }
    case number:
        return t.value;
    case '-':
        return -primary();
    case '+':
        return primary();
    default:
        error("Invalid factor.");
    }
}

void calculate() {
    while(std::cin){
        std::cout << prompt;
        Token t = ts.get();
        while(t.kind == print) 
            t = ts.get();
        if (t.kind == quit || t.kind == QUIT) 
            break;
        ts.putback(t);
        std::cout<<result<<expression()<<'\n';
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
