
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int main (int argc, char * argv[]){
    regex_t regex;
    int reti;
    char msgbuf[100];
    char * string, * pattern;

    string = "123,1234";
    pattern = "^[0-9]\{1,\},[0-9]\{1,\}$";

    if(regcomp(&regex, pattern, 0))
    {
            fprintf(stderr, "Could not compile regex\n");
            exit(107);
    }

    if(!(reti = regexec(&regex, string, 0, NULL, 0)))
    {
            printf("MATCH\n");
    }
    else if(reti == REG_NOMATCH)
    {
            printf("NO MATCH\n");
    }
    else
    {
            regerror(reti, &regex, msgbuf, sizeof(msgbuf));
            fprintf(stderr, "Regex match failed: %s\n", msgbuf);
            exit(107);
    }
