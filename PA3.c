#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

// You can declare your shared variables here.
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// You should fill this function.
void *display(void* args)
{
    pthread_mutex_lock(&lock);
    printf("---- %ld\n", pthread_self());
    char myStr[100000] ;
    read(((int *) args)[0], myStr, 100001);
    printf("%s",myStr);
    printf("---- %ld\n", pthread_self());
    fflush(stdout);
    pthread_mutex_unlock(&lock);
}

// You can modify the main except for the parser.
int main(int argc, char *argv[])
{
    //mutex init
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init has failed\n");
        return 0;
    }

    FILE *fptr;

    // You can use these variables for creating and manipulating shell threads.
    pthread_t threads[50];
    int thrCount = 0;
    int pointThread = 0;

    fptr = fopen("./commands.txt", "r");

    if (fptr == NULL)
    {
        exit(1);
    }

    char line[256];

    while (fgets(line, sizeof(line), fptr))
    {

        char redirection = '-';
        char background = 'n';

        int i;
        for( i = 0; i < strlen(line); i++)
        {
            if( line[i] == '<' )
            {
                redirection = '<';
            }
            else if( line[i] == '>' )
            {
                redirection = '>';
            }
            if (line[i] == '&')
            {
                background = 'y';
            }
        }

        // PARSER STARTS
        // DO NOT MODIFY THE PARSER!
        char *myargs[10];

        int fileFlag = 0;

        int argNumber = 0;

        char *redirFile;

        int onWord = 0;
        char wordLength = 0;

        int redirFilelen = 0;

        for(i = 0; i < strlen(line) && line[i] != '&' && line[i] != '\n'; i++)
        {
            if (line[i] != ' ' && line[i] != '<' && line[i] != '>')
            {
                onWord = 1;
                wordLength++;

                if ( (i + 1 < strlen(line) && line[i + 1] == '\n') || i + 1 == strlen(line) )
                {
                    char *word = (char*)malloc(sizeof(char)*(wordLength + 1));
                    memcpy(word, line + i + 1 - wordLength, wordLength);
                    myargs[argNumber] = strdup(word);
                    argNumber++;
                    break;
                }
            }

            else if (onWord == 1 && line[i] == ' ')
            {
                onWord = 0;
                char *word = (char*)malloc(sizeof(char) * (wordLength + 1));
                memcpy(word, line + i - wordLength, wordLength);
                myargs[argNumber] = strdup(word);
                argNumber++;
                wordLength = 0;
            }

            else if (line[i] == '<' || line[i] == '>')
            {
                int k;
                for(k = i + 1; k < strlen(line) && line[k] != '\n'; k++)
                {
                    if (line[k] != ' ')
                    {
                        fileFlag = 1;
                        redirFilelen++;
                        if (k + 1 == strlen(line))
                        {
                            break;
                        }
                    }
                    else if (fileFlag == 1 && line[k] == ' ')
                    {
                        break;
                    }
                }
                redirFile = (char*)malloc(sizeof(char) * (redirFilelen + 1));
                memcpy(redirFile, line + k - redirFilelen, redirFilelen);
                break;
            }
        }
        // PARSER ENDS
        // All the modifications for MAIN comes after this point.

        myargs[argNumber] = NULL;

        // You should fill this 'if' block for handling wait commands.
        if (myargs[0][0] == 'w' && myargs[0][1] == 'a')
        {
            //close all threads
            for ( i = 0; i < thrCount; ++i) {
                //printf("%d - %ld\n",i,&threads[i]);
                pthread_join(threads[i],NULL);
                pointThread--;
            }

            thrCount = pointThread;
        }

        /*
        for(int i = 0; i<=argNumber;i++){
            printf("%s ",myargs[i]);

        }
        printf("%c", background);
         */

        //pipe creation
        int fd[2];
        pipe(fd);

        int rc = fork();
        if (rc < 0)
        {
            exit(1);
        }
        else if (rc == 0)
        {
            if (redirection == '-')
            {
                //inside child
                close(fd[0]);
                dup2(fd[1],STDOUT_FILENO); //writes to the pipe
                //close(fd[0]);
            }
            else if (redirection == '<')
            {
                close (STDIN_FILENO);
                int fileNo = open(redirFile, O_RDONLY);
                //printf("%s\n",redirFile);

                if (fileNo <0 ){ //Error couldn't open file at location
                    printf("The file couldn't be loaded!");
                    return -2;
                }
                //dup2(fileNo, STDIN_FILENO);
            }
            else
            {
                close(STDOUT_FILENO);
                //printf("%s\n",redirFile);
                open(redirFile, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
            }

            execvp(myargs[0], myargs);
            fflush(stdout);
            close(fd[1]);

        }
        else //main shell process
        {
            if(redirection == '-'){
                pthread_t curr;
                pthread_create(&curr,NULL, display, fd);
                threads[pointThread] = curr;
                pointThread++;
                thrCount++;
            }
            close(fd[1]);
            //close(fd[0]);
            //dup2(fd[1], STDOUT_FILENO);
            if (background == 'n')
            {
                waitpid(rc, NULL, 0);
                // finish non-background task
                if (redirection == '-') {
                    //printf("Closing - %ld\n",&threads[pointThread]);
                    pointThread--;
                    pthread_join(threads[pointThread], NULL);
                    //fflush(stdout);

                    thrCount = pointThread;
                }
            }
        }

    }



    //close all threads
    int i;
    for (i=0; i < pointThread; i++) {
        //printf("%d - %ld\n",i,threads[i]);
        pthread_join(threads[i],NULL);
        fflush(stdout);
        //pointThread--;
    }
    wait(NULL);
    thrCount = pointThread;

    return 0;
}
