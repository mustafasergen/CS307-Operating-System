#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

struct Person
{
    char name[128];
    char surname[128];
    char gender[128];
};


int co = 0;


void listdir(const char *name, int indent, int array_size, struct Person users[co])
{

    int countX = 0;
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name))){
        return;
    }


    while ((entry = readdir(dir)) != NULL)
    {

        if (entry->d_type == DT_DIR) {
            char path[1024];
            int dot = strcmp(entry->d_name, ".");
            int twodot=strcmp(entry->d_name, "..");

            if ( dot == 0 ||  twodot== 0) {
                continue;
            }

            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);

            listdir(path, indent + 2, array_size, users);
        }

        else {

            // printf("%*s- %s\n", indent, "", entry->d_name);
            char *filename = entry->d_name;
            const size_t len = strlen(entry->d_name);
            char newname[1024];
            snprintf(newname, sizeof(newname), "%s/%s", name, entry->d_name);
            char *file_see = "./database.txt";

            if (strcmp(file_see, newname) != 0 && len > 4 && entry->d_name[len - 4] == '.' && entry->d_name[len - 3] == 't' && entry->d_name[len - 2] == 'x' && entry->d_name[len - 1] == 't') // just to open txt files
            {
                FILE *file = fopen(newname, "r+");
                char word[20];
                if (!file)
                {
                    printf("File not opened\n");
                }

                while (!feof(file))
                {

                    fscanf(file, "%s", word);
                    int k;
                    for (k = 0; k < array_size; k++)
                    {
                        char *word_name = strdup(word);
                        if (strcmp(word_name, users[k].name) == 0)
                        {

                            char *realGen;
                            if (strcmp(users[k].gender, "m") == 0)
                                realGen = "Mr.";
                            else
                                realGen = "Ms.";
                            int offset1 = strlen(word_name) + 4;
                            fseek(file, -offset1, SEEK_CUR);
                            fputs(realGen, file);

                            fseek(file, offset1 - 3, SEEK_CUR);
                            fseek(file, 1, SEEK_CUR);

                            fputs(users[k].surname, file);
                            int lll = strlen(users[k].surname);
                            fseek(file, -( lll+ 1), SEEK_CUR);
                        }
                    }
                }
                fclose(file);
            }
        }
    }
    closedir(dir);
}

int main(void)
{
    char *filenameX = "database.txt";

    FILE *file = fopen(filenameX, "r");

    if (file == NULL)
    {
        printf("The file name/path is incorrect \n");
        return -25;
    }

    co = 0;
    char c;
    for (c = getc(file); c != EOF; c = getc(file))
    {
        if (c=='\n') {
            co = co + 1;
        }
    }
    co += 1;
    fseek(file, 0, SEEK_SET);

    struct Person users[co];

    char namex[256]; char surnamex[256]; char genderx[256]; // TODO check size

    int lcount = 0;
    while (!feof(file))
    {
        fscanf(file, " %s %s %s ", genderx, namex, surnamex);
        strcpy(users[lcount].name, namex);strcpy(users[lcount].surname, surnamex);strcpy(users[lcount].gender, genderx);
        lcount += 1;
    }
    
    fclose(file);

    listdir(".", 0, co, users);

    return 0;
}
