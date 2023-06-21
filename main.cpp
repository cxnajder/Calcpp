//Code based on B. STROUSTRUP's book PROGRAMING Principles and Practice using C++

#include <iostream>
#include <string>


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
        error("Invalid factor.");
    }
}

int main(int argc, char const *argv[])
{
    try
    {
        while(std::cin){
            std::cout << "> ";
            Token t = ts.get();
            if (t.kind=='q') 
                break;
            while(t.kind==';') 
                t = ts.get();
            if (t.kind=='q' || t.kind=='Q') 
                break;
            ts.putback(t);
            std::cout<<"= "<<expression()<<'\n';
        }
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
