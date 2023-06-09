#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 16
#define ID 257
#define NUM 258

FILE* in;
int token = 0;
long long num = 0;
char id[MAXLEN] = { 0 };

void error(const char* s)
{
	printf("%s\n", s);
	exit(-1);
}

struct var
{
	struct var* next;
	const char* name;
	long long val;
} *vars = NULL;

long long* getvar(char* name)
{
	struct var* it;
	for(it = vars; it != NULL; it = it->next)
	{
		if (!strcmp(it->name, name))
		{
			return &it->val;
		}
	}
	return NULL;
}

void setvar(char* name, long long val)
{
	long long* ref = getvar(name);
	if (ref != NULL)
	{
		*ref = val;	
	}
	else
	{
		struct var* v = malloc(sizeof(struct var));
		v->next = NULL;
		v->name = strdup(name);
		v->val = val;

		if (vars != NULL)
		{
			v->next = vars;
		}
		vars = v;
	}
}

long long getval(char* name)
{
	long long* ref = getvar(name);
	if (ref == NULL)
	{
		error("undefined vairbale");
	}
	return *ref;
}

void next()
{
	// peek
	int c = getc(in);
	// skip white spaces
	while(c == ' ' || c == '\t' || c == '\r') { c = getc(in); }

	// opeators
	if (c == '~'
		|| c == '%'
		|| c == '^'
		|| c == '&'
		|| c == '*'
		|| c == '('
		|| c == ')'
		|| c == '-'
		|| c == '+'
		|| c == '='
		|| c == '|'
		|| c == ':'
		|| c == '/'
			// \n and EOF are special tokens
		|| c == '\n'
		|| c == EOF
	)
	{
		token = c;
	}
	else if (c == '0')
	{
		c = getc(in);
		if (c == 'b' || c == 'B')
		{
			// binary literal
			long long val = 0;
			while(1)
			{
				c = getc(in);
				if (c == '0' || c == '1')
				{
					val <<= 1;
					val += c-'0';
				}	
				else
				{
					ungetc(c, in);
					break;
				}
			}
			num = val;
			token = NUM;
		}
		else if (c == 'o' || c == 'O')
		{
			// octal literal
			long long val = 0;
			while(1)
			{
				c = getc(in);
				if ('0' <= c && c <= '7')
				{
					val <<= 3;
					val += c-'0';
				}	
				else
				{
					ungetc(c, in);
					break;
				}
			}
			num = val;
			token = NUM;
		}
		else if (c == 'x' || c == 'X')
		{
			fscanf(in, "%llx", &num);
			token = NUM;
		}
		else
		{
			ungetc(c, in);
			ungetc('0', in);
			// try reading it as a decimal integer literal
			fscanf(in, "%lld", &num);
			token = NUM;
		}
	}
	else if ('1' <= c && c <= '9')
	{
		// decimal integer literal
		ungetc(c, in);
		fscanf(in, "%lld", &num);
		token = NUM;
	}
	else if (('a' <= c && c <= 'z')
		|| ('A' <= c && c <= 'Z')
		|| c == '_')
	{
		ungetc(c, in);
		int i = 0;
		for(; i < (MAXLEN-1); i++)
		{
			c = getc(in);
			if (('a' <= c && c <= 'z')
				|| ('A' <= c && c <= 'Z')
				|| ('0' <= c && c <= '9')
				|| c == '_')
			{
				id[i] = c;
			}
			else
			{
				id[i] = 0;
				ungetc(c, in);
				break;
			}
		}
		token = ID;
	}
	else
	{
		error("unknown character");
	}
}

void match(int t)
{
	if (t == token)
	{
		next();
	}
	else
	{
		error("token mismatch");
	}
}

long long expression();

long long factor()
{
	long long val;
	if (token == '(') {
        match('(');
        val = expression();
        match(')');
	}
	else if (token == '-')
	{
		match('-');
		val = -expression();
	}	
	else if (token == '~')
	{
		match('~');
		val = ~expression();
	}
	else if (token == ID)
	{
		char name[MAXLEN];
		strcpy(name, id);
		match(ID);
		val = getval(name);
	}
	else if (token == NUM)
	{
		val = num;
		match(NUM);
	}
	else
	{
		error("unexpected expression");
	}
	return val;
}

long long term5()
{
	long long lhs = factor();
	while (token == '*' || token == '/' || token == '%')
	{
		if (token == '*')
		{
			match('*');
			lhs *= factor();
		}
		else if (token == '/')
		{
			match('/');
			lhs /= factor();
		}
		else
		{
			match('%');
			lhs %= factor();
		}
	}
	return lhs;
}

long long term4()
{
	long long lhs = term5();
	while (token == '+' || token == '-')
	{
		if (token == '+')
		{
			match('+');
			lhs += term5();
		}
		else
		{
			match('-');
			lhs -= term5();
		}
	}
	return lhs;
}

long long term3()
{
	long long lhs = term4();
	while (token == '&')
	{
		match('&');
		lhs &= term4();
	}
	return lhs;
}

long long term2()
{
	long long lhs = term3();
	while (token == '^')
	{
		match('^');
		lhs ^= term3();
	}
	return lhs;
}

long long term1()
{
	long long lhs = term2();
	while (token == '|')
	{
		match('|');
		lhs |= term2();
	}
	return lhs;
}

long long expression()
{
	return term1();
}

void printvar()
{
	match(':');
	if (token == '\n')
	{
		struct var* it;
		for(it = vars; it != NULL; it = it->next)
		{
			printf("%s = %lld, 0x%llX, 0o%llo\n",
				it->name, it->val, it->val, it->val);
		}
	}
	else
	{
		while(token == ID)
		{
			char name[MAXLEN];
			strcpy(name, id);
			match(ID);
		
			long long val = getval(name);
			printf("%s = %lld, 0x%llX, 0o%llo\n",
				name, val, val, val);
		}
	}
}

long long opexpr(char* name)
{
	long long val = getval(name);
	if (token == '+')
	{
		match('+');
		val += expression();
	}
	else if (token == '-')
	{
		match('-');
		val -= expression();
	}
	else if (token == '*')
	{
		match('*');
		val *= expression();
	}
	else if (token == '/')
	{
		match('/');
		val /= expression();
	}
	else if (token == '%')
	{
		match('%');
		val %= expression();
	}
	else if (token == '&')
	{
		match('&');
		val &= expression();
	}
	else if (token == '^')
	{
		match('^');
		val ^= expression();
	}
	else if (token == '|')
	{
		match('|');
		val |= expression();
	}
	else if (token == '\n')
	{
		// DO NOTHING
	}
	else
	{
		error("unexpected expression");
	}
	return val;
}

void program()
{
	printf("> ");	
	next();

	while(token != EOF)
	{
		long long val;
		if (token == '\n')
		{
			// empty statement
			// DO NOTHING
		}
		else if (token == ':')
		{
			// print-var statement
			printvar();
		}
		else if (token == ID)
		{
			char name[MAXLEN];
			strcpy(name, id);
			match(ID);

			if (token == '=')
			{
				// assignment
				match('=');
				val = expression();
				setvar(name, val);
			}
			else
			{
				// expression starting with an ID
				val = opexpr(name);
				printf("%lld\n", val);
			}
		}
		else
		{
			// expression starting with an NUM
			val = expression();
			printf("%lld\n", val);
		}
		printf("> ");	
		match('\n');
	}
}

void printtokens()
{
	while(token != EOF)
	{
		next();
		switch(token)
		{
		case ID: printf("ID %s\n", id); break;
		case NUM: printf("NUM %lld\n", num); break;
		default: printf("SYM %c\n", token); break;
		}
	}
}

int main()
{
	in = stdin;	
#if 0
	printtokens();
#else
	program();
#endif
}
