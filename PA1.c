#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void empty_string(char * str, int strlen)
{
    for (int i = 0; i < strlen; i++)
    {
        str[i] = '\0';
    }
    
}

void parse_input(char * buff, int bufflen)
{
    int part = 0;
    char cmd_name[102]="";
    char input[102]="";
    char option[102]="";
    char optional_string[102]="";
    char redirection ='\0';
    char file_name[102]="";
    int background = 0;
    char temp[102]="";
    int buf_start = 0;
    for (int i = 0; i < bufflen; i++)
    {
        int end_word= 0;
        char ch = buff[i];
        if (ch == ' ' || ch == '\n' )
        {
            buf_start = i+1;
            end_word = 1;
        }
        else if(i == bufflen-1)
        {
            temp[i-buf_start] = ch;
            end_word = 1;
        }
        else
            temp[i-buf_start] = ch;

        if (end_word==1)
        {
            if(part == 0)//first word
            {
                strcpy(cmd_name,temp);
                part=1;

            }
            else if(part == 1)//2nd word options: input, option, redirection
            {
                int optionlen = strlen(option);
                char val_ch = temp[0];
                if (val_ch == '-')
                {
                    strcpy(option,temp);
                }
                else if (val_ch == '<' || val_ch == '>')
                {
                    redirection = val_ch;
                    part=2;
                }
                else if(val_ch == '&')
                {
                    background = 1;
                }
                else if (optionlen==0) //if the temp is full string
                {
                    strcpy(input,temp);
                }
                else
                {
                    strcpy(optional_string,temp);
                }
                
            }
            else if(part==2)
            {
                if(strlen(file_name)==0)
                {
                    strcpy(file_name,temp);
                }
                else
                {
                    background=1;
                }
                
            }
            //temp[0] = '\0';
            empty_string(temp, 102);
            end_word = 0;
            buf_start = i+1;
        }

    }
    printf("----------\n");
    printf("Command: %s\n",cmd_name);
    printf("Inputs: %s\n",input);
    printf("Options: %s\n",option);
    printf("Redirection: %c\n",redirection);
    if(background==1)
        printf("Background Job: y\n");
    else
        printf("Background Job: n\n");
    printf("----------\n");
    
}


int main(int argc, char *argv[]) {
    /* fork a child process 
    int rc = fork();
    if (rc < 0) { 
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (rc == 0) { //chils
        execvp("/bin/ls","ls",NULL);

    }
    else { //parent
         //parent will wait for the child to complete 
        wait(NULL);
        printf("Child Complete");
    }
    */
    
    char * txt_name = "commands.txt";
    FILE *in_file  = fopen(txt_name, "r"); // read only
    char buff[1024];
    while ( fgets(buff, 1024, in_file ))  //reads a single line / every iteration
    { 
        printf("%s\n", buff); 
        int len = strlen(buff);
        //printf("%c",buff[len-1]);
        
        parse_input(buff,len);
    }  

    fclose(in_file);


    return 0;
} // end of main