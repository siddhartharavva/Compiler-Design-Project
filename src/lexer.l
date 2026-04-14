%option noyywrap yylineno nounput noinput

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gen/parser.tab.h"
%}

digit      [0-9]
letter     [A-Za-z_]
identifier {letter}({letter}|{digit})*
number     ({digit}+(\.{digit}+)?|\.[0-9]+)

%%

[ \t\r]+              ;
";"+                  { return SEMICOLON; }
\n+                   { return SEMICOLON; }
"="                   { return ASSIGN; }
"+"                   { return PLUS; }
"-"                   { return MINUS; }
"*"                   { return MUL; }
"/"                   { return DIV; }
"("                   { return LPAREN; }
")"                   { return RPAREN; }
{number}              { yylval.num = atof(yytext); return NUMBER; }
{identifier}          { yylval.str = strdup(yytext); return IDENTIFIER; }
.                     { fprintf(stderr, "LexError at line %d: unexpected character '%s'\n", yylineno, yytext); return INVALID; }

%%
