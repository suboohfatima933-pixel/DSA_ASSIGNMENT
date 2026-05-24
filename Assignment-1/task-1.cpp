#include <iostream>
#include <stack>
#include <vector>
#include <string>
using namespace std;
enum TokenType
{
    NUMBER,
    VARIABLE,
    OPERATOR,
    OPEN_BRACKET,
    CLOSE_BRACKET
};
class Token
{
public:
    string value;
    TokenType type;

    Token(string v = "", TokenType t = NUMBER)
    {
        value = v;
        type = t;
    }
};
class Variable
{
public:
    string name;
    int value;
};
bool isDigit(char ch)
{
    return ch >= '0' && ch <= '9';
}
bool isLetter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}
bool isAlphaNumeric(char ch)
{
    return isLetter(ch) || isDigit(ch);
}
bool isWhiteSpace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}
bool isOperator(char ch)
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}
bool isOpeningBracket(char ch)
{
    return ch == '(' || ch == '[' || ch == '{';
}
bool isClosingBracket(char ch)
{
    return ch == ')' || ch == ']' || ch == '}';
}
bool bracketsMatch(char open, char close)
{
    return (open == '(' && close == ')') ||
        (open == '[' && close == ']') ||
        (open == '{' && close == '}');
}
int precedence(char op)
{
    if (op == '*' || op == '/')
        return 2;

    if (op == '+' || op == '-')
        return 1;

    return 0;
}
void syntaxError(string message)
{
    cerr << "Syntax Error: " << message << "\n";
    exit(1);
}
void runtimeError(string message)
{
    cerr << "Runtime Error: " << message << "\n";
    exit(2);
}

void logicalError(string message)
{
    cerr << "Logical Error: " << message << "\n";
    exit(3);
}

int stringToInt(string str)
{
    int num = 0;
    for (int i = 0; i < str.length(); i++)
    {
        num = num * 10 + (str[i] - '0');
    }
    return num;
}
vector<Token> tokenize(string expression)
{
    vector<Token> tokens;

    int i = 0;

    while (i < expression.length())
    {
        if (isWhiteSpace(expression[i]))
        {
            i++;
            continue;
        }

        if (isDigit(expression[i]))
        {
            string number = "";

            while (i < expression.length() && isDigit(expression[i]))
            {
                number += expression[i];
                i++;
            }

            tokens.push_back(Token(number, NUMBER));
            continue;
        }

        if (isLetter(expression[i]) || expression[i] == '_')
        {
            string variable = "";

            while (i < expression.length() &&
                (isAlphaNumeric(expression[i]) || expression[i] == '_'))
            {
                variable += expression[i];
                i++;
            }

            tokens.push_back(Token(variable, VARIABLE));
            continue;
        }

        if (isOperator(expression[i]))
        {
            string op = "";
            op += expression[i];

            tokens.push_back(Token(op, OPERATOR));
            i++;
            continue;
        }

        if (isOpeningBracket(expression[i]))
        {
            string bracket = "";
            bracket += expression[i];

            tokens.push_back(Token(bracket, OPEN_BRACKET));
            i++;
            continue;
        }

        if (isClosingBracket(expression[i]))
        {
            string bracket = "";
            bracket += expression[i];

            tokens.push_back(Token(bracket, CLOSE_BRACKET));
            i++;
            continue;
        }

        syntaxError("Invalid character detected");
    }

    if (tokens.size() == 0)
    {
        syntaxError("Expression is empty");
    }

    return tokens;
}

void validateSyntax(vector<Token> tokens)
{
    stack<char> brackets;

    for (int i = 0; i < tokens.size(); i++)
    {
        Token current = tokens[i];

        if (current.type == OPEN_BRACKET)
        {
            brackets.push(current.value[0]);
        }

        if (current.type == CLOSE_BRACKET)
        {
            if (brackets.empty())
            {
                syntaxError("Closing bracket without opening bracket");
            }

            char open = brackets.top();
            brackets.pop();

            if (!bracketsMatch(open, current.value[0]))
            {
                syntaxError("Mismatched brackets detected");
            }
        }

        if (current.type == OPERATOR)
        {
            if (i == 0 || i == tokens.size() - 1)
            {
                syntaxError("Operator at invalid position");
            }

            if (tokens[i - 1].type == OPERATOR ||
                tokens[i + 1].type == OPERATOR)
            {
                syntaxError("Consecutive operators detected");
            }
        }
    }

    if (!brackets.empty())
    {
        syntaxError("Unclosed bracket detected");
    }
}

vector<Token> infixToPostfix(vector<Token> tokens)
{
    vector<Token> postfix;
    stack<Token> operators;

    for (int i = 0; i < tokens.size(); i++)
    {
        Token token = tokens[i];

        if (token.type == NUMBER || token.type == VARIABLE)
        {
            postfix.push_back(token);
        }
        else if (token.type == OPEN_BRACKET)
        {
            operators.push(token);
        }
        else if (token.type == CLOSE_BRACKET)
        {
            while (!operators.empty() &&
                operators.top().type != OPEN_BRACKET)
            {
                postfix.push_back(operators.top());
                operators.pop();
            }

            if (operators.empty())
            {
                syntaxError("Bracket mismatch");
            }

            operators.pop();
        }
        else if (token.type == OPERATOR)
        {
            while (!operators.empty() &&
                operators.top().type == OPERATOR &&
                precedence(operators.top().value[0]) >= precedence(token.value[0]))
            {
                postfix.push_back(operators.top());
                operators.pop();
            }

            operators.push(token);
        }
    }

    while (!operators.empty())
    {
        postfix.push_back(operators.top());
        operators.pop();
    }

    return postfix;
}

string postfixToString(vector<Token> postfix)
{
    string result = "";

    for (int i = 0; i < postfix.size(); i++)
    {
        result += postfix[i].value;

        if (i != postfix.size() - 1)
        {
            result += " ";
        }
    }

    return result;
}

int findVariable(vector<Variable> variables, string name)
{
    for (int i = 0; i < variables.size(); i++)
    {
        if (variables[i].name == name)
        {
            return i;
        }
    }

    return -1;
}

vector<Variable> getVariableValues(vector<Token> tokens)
{
    vector<Variable> variables;

    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].type == VARIABLE)
        {
            if (findVariable(variables, tokens[i].value) == -1)
            {
                Variable v;
                v.name = tokens[i].value;

                cerr << "Enter value for " << v.name << ": ";

                if (!(cin >> v.value))
                {
                    runtimeError("Invalid numeric input");
                }

                variables.push_back(v);
            }
        }
    }

    return variables;
}

int getVariableValue(vector<Variable> variables, string name)
{
    for (int i = 0; i < variables.size(); i++)
    {
        if (variables[i].name == name)
        {
            return variables[i].value;
        }
    }

    logicalError("Variable value not found");
    return 0;
}

int evaluatePostfix(vector<Token> postfix, vector<Variable> variables)
{
    stack<int> st;

    for (int i = 0; i < postfix.size(); i++)
    {
        Token token = postfix[i];

        if (token.type == NUMBER)
        {
            st.push(stringToInt(token.value));
        }
        else if (token.type == VARIABLE)
        {
            st.push(getVariableValue(variables, token.value));
        }
        else if (token.type == OPERATOR)
        {
            if (st.size() < 2)
            {
                runtimeError("Insufficient operands");
            }

            int right = st.top();
            st.pop();

            int left = st.top();
            st.pop();

            int result = 0;

            switch (token.value[0])
            {
            case '+':
                result = left + right;
                break;

            case '-':
                result = left - right;
                break;

            case '*':
                result = left * right;
                break;

            case '/':
                if (right == 0)
                {
                    runtimeError("Division by zero");
                }

                result = left / right;
                break;
            }

            st.push(result);
        }
    }

    if (st.size() != 1)
    {
        logicalError("Invalid expression evaluation");
    }

    return st.top();
}

int main()
{
    string expression;

    getline(cin, expression);

    vector<Token> tokens = tokenize(expression);

    validateSyntax(tokens);

    vector<Token> postfix = infixToPostfix(tokens);

   

    vector<Variable> variables = getVariableValues(tokens);

    int result = evaluatePostfix(postfix, variables);

    cerr << "Postfix Expression: "
        << postfixToString(postfix)
        << endl;

    cout << result << endl;

    return 0;
}
