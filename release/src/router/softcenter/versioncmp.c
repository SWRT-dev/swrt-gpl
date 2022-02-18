//gcc versioncmp.c -o versioncmp
//versioncmp v1 v2
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int readline(char* filename, char* buffer, int size) {
    int pos = 0, c;

    FILE *f = fopen(filename, "r");
    if(f) {
          c = fgetc(f);
          while((c == ' ' || c == '\n') && c != EOF) {
              c = fgetc(f);
          }
          if (c != EOF) {
              while(c != '\n' && c != EOF) {
                  buffer[pos++] = c;
                  c = fgetc(f);
              }
          }

          buffer[pos] = '\0';
          fclose(f);           
    } else {
        return -1;
    }
    return pos;
}

int str_split(char* buf, char** s, int s_size) {
    int curr = 0;

    char* token = strtok(buf, ".");
    while(token && curr < s_size) {
        s[curr++] = token;
        token = strtok(NULL, ".");
    }

    return curr;
}

#define BUF_SIZE 1024
#define S_LEN 10
int main(int argc, char * argv[], char * envp[]) {
    char buf1[BUF_SIZE] = {0}, buf2[BUF_SIZE] = {0};
    char* s1[S_LEN] = {0};
    char* s2[S_LEN] = {0};
    char* end;
    int i, n1, n2, s1_len, s2_len, s_max, rlt;

    if (argc != 3) {
        fprintf(stderr, "versioncmp str1 str2\n");
        return -1;
    }

    n1 = readline(argv[1], buf1, BUF_SIZE-1);
    n2 = readline(argv[2], buf2, BUF_SIZE-1);
    if (-1 == n1) {
        strncpy(buf1, argv[1], BUF_SIZE-1);
    }
    if (-1 == n2) {
        strncpy(buf2, argv[2], BUF_SIZE-1);
    }

    //fprintf(stderr, "%s\n", buf1);
    //fprintf(stderr, "%s\n", buf2);

    s1_len = str_split(buf1, s1, S_LEN);
    s2_len = str_split(buf2, s2, S_LEN);

    //fprintf(stderr, "s1=%d, s2=%d\n", s1_len, s2_len);

    s_max = (s1_len > s2_len ? s1_len: s2_len);

    rlt = 0;
    for(i = 0; i < s_max; i++) {
        if(!s1[i]) {
            s1[i] = "0";
        }
        if(!s2[i]) {
            s2[i] = "0";
        }

        n1 = strtol(s1[i], &end, 10);
        if(*end) {
            //parse error, but not break hear
            rlt = 1;
        }

        n2 = strtol(s2[i], &end, 10);
        if(*end) {
            if (1 == rlt) {
                //both error
                rlt = 0;
                break;
            }
            rlt = -1;
            break;
        }

        if(1 == rlt) {
            break;
        }

        if (n1 == n2) {
            continue;
        } else if(n1 < n2) {
            rlt = 1;
            break;
        } else {
            rlt = -1;
            break;
        }
    }

    fprintf(stdout, "%d", rlt);

    return 0;
}
