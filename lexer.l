%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

DIGIT      [0-9]
LETTER     [a-zA-Z_]
ID         {LETTER}({LETTER}|{DIGIT})*
INT        {DIGIT}+


%%
"//".*                      ;   /* single-line comment */
"/*"([^*]|\*+[^*/])*\*+"/"  ;   /* multi-line comment */
[ \t\n]+                   ;   /* skip whitespace */
{INT}                      { printf("Token(INTEGER, %s)\n", yytext); 
                             yylval.ival = atoi(yytext); 
                             return INTEGER; }

{ID}                       { printf("Token(IDENTIFIER, %s)\n", yytext);
                             yylval.sval = strdup(yytext);
                             return IDENTIFIER;}
"+"                        { printf("Token(PLUS, %s)\n", yytext); 
                             return PLUS; }
"-"                        { printf("Token(MINUS, %s)\n", yytext);
                             return MINUS; }
"*"                        { printf("Token(MUL, %s)\n", yytext);
                             return MUL; }
"/"                        { printf("Token(DIV, %s)\n", yytext);
                             return DIV; }
"="                        { printf("Token(ASSIGN, %s)\n", yytext);
                             return ASSIGN; }
"("                        { printf("Token(LPAREN, %s)\n", yytext);
                             return LPAREN; }
")"                        { printf("Token(RPAREN, %s)\n", yytext); 
                             return RPAREN; }
";"                        { printf("Token(SEMICOLON, %s)\n", yytext);
                             return SEMICOLON; }
.                          { printf("Unexpected character: %s\n", yytext);
                             exit(1); }
%%


