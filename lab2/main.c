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

#define MASK_HELP 100000
#define MASK_GROUP 10000
#define MASK_ACTION 1000
#define MASK_PERM1 100
#define MASK_PERM2 10
#define MASK_PERM3 1

// const size_t path_max_size = 256;
// /*Получить путь до файла*/
// char *concat(const char *s1, const char *s2)
// {
//     const size_t len1 = strlen(s1);
//     const size_t len2 = strlen(s2);
//     char *result = malloc(len1 + len2 + 1); // +1 for the null-terminator
//     memcpy(result, s1, len1);
//     memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
//     return result;
// }
// void getTime(time_t t)
// {
//     char buff[20];
//     struct tm *timeinfo;
//     timeinfo = localtime(&t);
//     strftime(buff, sizeof(buff), "%b %d %H:%M", timeinfo);
//     printf("%s ", buff);
// }
// void printFileData(const char *path, const char *name)
// {
//     struct stat sb;
//     if (stat(path, &sb) == -1)
//     {
//         perror("stat");
//         exit(EXIT_FAILURE);
//     }
//     switch (sb.st_mode & S_IFMT)
//     {
//     case S_IFDIR:
//         printf("d");
//         break;
//     case S_IFLNK:
//         printf("l");
//         break;
//     case S_IFREG:
//         printf("-");
//         break;
//     default:
//         perror("unknown type of file");
//         exit(EXIT_FAILURE);
//         break;
//     }
//     //rwx by owner
//     mode_t tmp = sb.st_mode & S_IRWXU;
//     if (tmp & S_IRUSR)
//     {
//         printf("%s", "r");
//     }
//     else
//     {
//         printf("%s", "-");
//     }
//     if (tmp & S_IWUSR)
//     {
//         printf("%s", "w");
//     }
//     else
//     {
//         printf("%s", "-");
//     }
//     if (tmp & S_IXUSR)
//     {
//         printf("%s", "x");
//     }
//     else
//     {
//         printf("%s", "-");
//     }
//     //rwx by group
//     tmp = sb.st_mode & S_IRWXG;
//     if (tmp & S_IRGRP)
//     {
//         printf("%s", "r");
//     }
//     else
//     {
//         printf("%s", "-");
//     }
//     if (tmp & S_IWGRP)
//     {
//         printf("%s", "w");
//     }
//     else
//     {
//         printf("%s", "-");
//     }
//     if (tmp & S_IXGRP)
//     {
//         printf("%s", "x");
//     }
//     else
//     {
//         printf("%s", "-");
//     }
//     //rwx for others
//     tmp = sb.st_mode & S_IRWXO;
//     if (tmp & S_IROTH)
//     {
//         printf("%s", "r");
//     }
//     else
//     {
//         printf("%s", "-");
//     }
//     if (tmp & S_IWOTH)
//     {
//         printf("%s", "w");
//     }
//     else
//     {
//         printf("%s", "-");
//     }
//     if (tmp & S_IXOTH)
//     {
//         printf("%s", "x");
//     }
//     else
//     {
//         printf("%s", "-");
//     }
//     printf(" %ld ", sb.st_nlink);
//     struct passwd *pwd;
//     pwd = getpwuid(sb.st_uid);
//     if (pwd == NULL)
//     {
//         printf("      ");
//     }
//     else
//     {
//         printf("%s ", pwd->pw_name);
//     }
//     struct group *gr = getgrgid(sb.st_gid);
//     if (gr == NULL)
//     {
//         printf("      ");
//     }
//     else
//     {
//         printf("%s ", gr->gr_name);
//     }
//     printf("%ld ", sb.st_size);
//     getTime(sb.st_mtime);
//     printf(" %s\n", name);
// }
// void getBlocksCount(const char *path)
// {
//     struct dirent *entry;
//     DIR *dp;
//     blksize_t fullBlockSize = 0;
//     dp = opendir(path);
//     char *tmpStr = ".";
//     while ((entry = readdir(dp)))
//     {
//         if (entry->d_name[0] != tmpStr[0] &&
//             (strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
//         {
//             char *tmp = malloc(2);
//             memset(tmp, 0, sizeof(*tmp));
//             strcpy(tmp, "/");
//             char *fileName = concat(path, tmp);
//             free(tmp);
//             tmp = fileName;
//             fileName = concat(tmp, entry->d_name);
//             struct stat sb;
//             if (stat(fileName, &sb) == -1)
//             {
//                 perror("stat");
//                 exit(EXIT_FAILURE);
//             }
//             fullBlockSize += (sb.st_blocks);
//             free(fileName);
//         }
//     }
//     closedir(dp);
//     printf("%s %ld\n", "итого", fullBlockSize);
// }
// int listdir(const char *path, bool long_listing)
// {
//     struct dirent *entry;
//     DIR *dp;
//     dp = opendir(path);
//     if (dp == NULL)
//     {
//         perror("opendir");
//         return -1;
//     }
//     char *tmpStr = ".";
//     while ((entry = readdir(dp)))
//     {
//         if (entry->d_name[0] != tmpStr[0] &&
//             (strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
//         {
//             if (!long_listing)
//             {
//                 puts(entry->d_name);
//             }
//             else
//             {
//                 char *tmp = malloc(2);
//                 memset(tmp, 0, sizeof(*tmp));
//                 strcpy(tmp, "/");
//                 char *fileName = concat(path, tmp);
//                 free(tmp);
//                 tmp = fileName;
//                 fileName = concat(tmp, entry->d_name);
//                 printFileData(fileName, entry->d_name);
//                 free(fileName);
//             }
//         }
//     }
//     closedir(dp);
//     return 0;
// }
bool checker(unsigned int seq)
{
    if ((seq / MASK_HELP == 1) && (seq % MASK_HELP == 0))
    {
        return true;
    }
    else if (seq > MASK_HELP)
    {
        return false;
    }
    else if (((seq % MASK_HELP) / MASK_GROUP == 0) ||
             ((seq % MASK_HELP) / MASK_GROUP > 3))
    {
        return false;
    }
    else if ((((seq % MASK_HELP) % MASK_GROUP) / MASK_ACTION == 0) ||
             (((seq % MASK_HELP) % MASK_GROUP) / MASK_ACTION > 2))
    {
        return false;
    }
    unsigned int permisions = ((seq % MASK_HELP) % MASK_GROUP) % MASK_ACTION;
    if (permisions == 0)
    {
        return false;
    }
    unsigned int tmp = 0;
    bool isEmpty = true;
    while (permisions)
    {
        tmp = permisions % 10;
        if (tmp != 0)
        {
            isEmpty = false;
        }
        if (tmp > 3)
        {
            return false;
        }
        permisions /= 10;
    }
    if (isEmpty)
    {
        return false;
    }
    return true;
}

void show_man()
{
    printf("chmod [flags] [file]\n");
    printf("Flags must be [h][ugo][ar][rwx][rwx][rwx]:\n -h -- print the reference\n");
    printf("-u -- define attribute group flag 'user'\n");
    printf("-g -- define attribute group flag 'group'\n");
    printf("-o -- define attribute group flag 'other'\n");
    printf("-a -- add permision\n");
    printf("-r -- remove permision\n");
    printf("-r -- permision to read\n");
    printf("-w -- permision to write\n");
    printf("-x -- permision to execute\n");
}

int main(int argc, char **argv)
{
    int c = 0;
    unsigned int flags = 0;
    bool shutDown = false;
    unsigned int step = MASK_PERM1;
    //Закодируем флаги в числовой формат
    while (((c = getopt(argc, argv, "hugoadrwx")) != -1) && !shutDown)
    {
        switch (c)
        {
        case 'h':
            flags += (MASK_HELP);
            break;
        case 'u':
            flags += (1 * MASK_GROUP);
            break;
        case 'g':
            flags += (2 * MASK_GROUP);
            break;
        case 'o':
            flags += (3 * MASK_GROUP);
            break;
        case 'a':
            flags += (1 * MASK_ACTION);
            break;
        case 'r':
            if (((flags % MASK_HELP) % MASK_GROUP) / MASK_ACTION == 0)
            {
                flags += (2 * MASK_ACTION);
            }
            else
            {
                flags += (1 * step);
                step /= 10;
            }
            break;
        case 'w':
            flags += (2 * step);
            step /= 10;
            break;
        case 'x':
            flags += (3 * step);
            step /= 10;
            break;
        default:
            printf("Incorrect flag\n");
            shutDown = true;
            break;
        }
    }
    bool isConsistant = checker(flags);
    printf("%d %d\n", isConsistant, flags);
    // if (argc == 1)
    // {
    //     listdir(".", l);
    // }
    // if (argc == 2)
    // {
    //     if (l)
    //     {
    //         getBlocksCount(".");
    //         listdir(".", l);
    //     }
    //     else
    //     {
    //         listdir(argv[1], l);
    //     }
    // }
    // if (argc == 3)
    // {
    //     getBlocksCount(argv[2]);
    //     listdir(argv[2], l);
    // }
    return 0;
}