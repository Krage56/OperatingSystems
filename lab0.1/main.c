#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
extern char *optarg;
extern int optind, opterr, optopt;
#define _GNU_SOURCE
typedef enum
{
    false = 0,
    true = 1
} bool;
const size_t path_max_size = 256;
/*Получить путь до файла*/
char *concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1); // +1 for the null-terminator
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
    return result;
}
void getTime(time_t t)
{
    char buff[20];
    struct tm *timeinfo;
    timeinfo = localtime(&t);
    strftime(buff, sizeof(buff), "%b %d %H:%M", timeinfo);
    printf("%s ", buff);
}
void printFileData(const char *path, const char *name)
{
    struct stat sb;
    if (stat(path, &sb) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    switch (sb.st_mode & S_IFMT)
    {
        case S_IFDIR:
            printf("d");
            break;
        case S_IFLNK:
            printf("l");
            break;
        case S_IFREG:
            printf("-");
            break;
        default:
            perror("unknown type of file");
            exit(EXIT_FAILURE);
            break;
    }
    //rwx by owner
    mode_t tmp = sb.st_mode & S_IRWXU;
    if (tmp & S_IRUSR)
    {
        printf("%s", "r");
    }
    else
    {
        printf("%s", "-");
    }
    if (tmp & S_IWUSR)
    {
        printf("%s", "w");
    }
    else
    {
        printf("%s", "-");
    }
    if (tmp & S_IXUSR)
    {
        printf("%s", "x");
    }
    else
    {
        printf("%s", "-");
    }
    //rwx by group
    tmp = sb.st_mode & S_IRWXG;
    if (tmp & S_IRGRP)
    {
        printf("%s", "r");
    }
    else
    {
        printf("%s", "-");
    }
    if (tmp & S_IWGRP)
    {
        printf("%s", "w");
    }
    else
    {
        printf("%s", "-");
    }
    if (tmp & S_IXGRP)
    {
        printf("%s", "x");
    }
    else
    {
        printf("%s", "-");
    }
    //rwx for others
    tmp = sb.st_mode & S_IRWXO;
    if (tmp & S_IROTH)
    {
        printf("%s", "r");
    }
    else
    {
        printf("%s", "-");
    }
    if (tmp & S_IWOTH)
    {
        printf("%s", "w");
    }
    else
    {
        printf("%s", "-");
    }
    if (tmp & S_IXOTH)
    {
        printf("%s", "x");
    }
    else
    {
        printf("%s", "-");
    }
    printf(" %ld ", sb.st_nlink);
    struct passwd *pwd;
    pwd = getpwuid(sb.st_uid);
    if(pwd == NULL){
        printf("      ");
    }
    else{
        printf("%s ", pwd->pw_name);
    }
    struct group *gr = getgrgid(sb.st_gid);
    if(gr == NULL){
        printf("      ");
    }
    else{
        printf("%s ", gr->gr_name);
    }
    printf("%ld ", sb.st_size);
    getTime(sb.st_mtime);
    printf(" %s\n", name);
}
void getBlocksCount(const char *path)
{
    struct dirent *entry;
    DIR *dp;
    blksize_t fullBlockSize = 0;
    dp = opendir(path);
    char *tmpStr = ".";
    while ((entry = readdir(dp)))
    {
        if (entry->d_name[0] != tmpStr[0] &&
            (strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
        {
            char *tmp = malloc(2);
            memset(tmp, 0, sizeof(*tmp));
            strcpy(tmp, "/");
            char *fileName = concat(path, tmp);
            free(tmp);
            tmp = fileName;
            fileName = concat(tmp, entry->d_name);
            struct stat sb;
            if (stat(fileName, &sb) == -1)
            {
                perror("stat");
                exit(EXIT_FAILURE);
            }
            fullBlockSize += (sb.st_blocks);
            free(fileName);
        }
    }
    closedir(dp);
    printf("%s %ld\n", "итого", fullBlockSize);
}
int listdir(const char *path, bool long_listing)
{
    struct dirent *entry;
    DIR *dp;
    dp = opendir(path);
    if (dp == NULL)
    {
        perror("opendir");
        return -1;
    }
    char *tmpStr = ".";
    while ((entry = readdir(dp)))
    {
        if (entry->d_name[0] != tmpStr[0] &&
        (strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
        {
            if (!long_listing)
            {
                puts(entry->d_name);
            }
            else
            {
                char *tmp = malloc(2);
                memset(tmp, 0, sizeof(*tmp));
                strcpy(tmp, "/");
                char *fileName = concat(path, tmp);
                free(tmp);
                tmp = fileName;
                fileName = concat(tmp, entry->d_name);
                printFileData(fileName, entry->d_name);
                free(fileName);
            }
        }
    }
    closedir(dp);
    return 0;
}

int main(int argc, char **argv)
{
    int c = 0;
    bool l = false;
    while ((c = getopt(argc, argv, "l")) != -1)
    {
        switch (c)
        {
            case 'l':
                l = true;
        }
    }
    if (argc == 1)
    {
        listdir(".", l);
    }
    if (argc == 2)
    {
        if (l)
        {
            getBlocksCount(".");
            listdir(".", l);
        }
        else
        {
            listdir(argv[1], l);
        }
    }
    if (argc == 3)
    {
        getBlocksCount(argv[2]);
        listdir(argv[2], l);
    }
    return 0;
}