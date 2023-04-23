#include <stdio.h>
#include <string.h>

// Usage
// g++ child.cpp

#define QIT_FORM "Canceled by demand. in %s\n"
#define MSG_FORM "Can you see this message \"%s\" in %s?\n"

int main()
{
    FILE *fp = fopen("_child.log","w+");
    char buf[80];
    char *ptr;
    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        ptr = strpbrk(buf,"\r\n");
        if (ptr) *ptr = '\0';
        if (strcmp(buf, "exit") == 0 || strcmp(buf, "quit") == 0)
        {
            break;
        }
        fprintf(stdout, MSG_FORM, buf, "STDOUT");
        fprintf(stderr, MSG_FORM, buf, "STDERR");
        fprintf(fp, MSG_FORM, buf, "_child.log");
    }
    fprintf(stdout, QIT_FORM, "STDOUT");
    fprintf(stderr, QIT_FORM, "STDERR");
    fprintf(fp, QIT_FORM, "_child.log");
    fclose(fp);
    return 0;
}
