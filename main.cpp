//Code based on B. STROUSTRUP's book PROGRAMING Principles and Practice using C++

#include <iostream>
#include <string>


void error(std::string s1, std::string s2 = ""){
    throw std::runtime_error(s1+s2);
}

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
    case 'q': case 'Q':
        return Token{'q'};
    case ';':
    case '(': case ')':
    case '*': case '/':
    case '+': case '-':
        return Token{ch};
    case '.':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        {
            std::cin.putback(ch);
            double val;
            std::cin >> val;
            return Token{'n', val};
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
    case 'n':
        return t.value;
    default:
        error("Expected factor.");
    }
}

int main(int argc, char const *argv[])
{
    double val = 0;
    std::cout << "> ";
    while(std::cin){
        Token t = ts.get();
        if (t.kind=='q') 
            break;
        if (t.kind==';') {
            std::cout<<"= "<<val<<'\n';
            std::cout << "> ";
        }
        else
            ts.putback(t);
        val = expression();
    }
    return 0;
}
