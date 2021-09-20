#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
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
    printf("%s %s ", getpwuid(sb.st_uid)->pw_name, getpwuid(sb.st_gid)->pw_name);
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
    while ((entry = readdir(dp)))
    {
        if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
        {
            char *tmp = malloc(2);
            tmp[0] = '/';
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
    blksize_t fullBlockSize = 0;
    dp = opendir(path);
    if (dp == NULL)
    {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)))
    {
        if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
        {
            if (!long_listing)
            {
                puts(entry->d_name);
            }
            else
            {
                char *tmp = malloc(2);
                tmp[0] = '/';
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
    // int c = 0;
    // while ((c = getopt(argc, argv, "ab:c")) != -1)
    // {
    //     switch (c)
    //     {
    //     case 'a':
    //         printf("%s\n", "a-option entered");
    //     case 'b':
    //         if (optarg)
    //         {
    //             printf("%s\n", optarg);
    //         }
    //         else
    //         {
    //             printf("%s\n", "b-option entered");
    //         }
    //     }
    // }
    // int counter = 1;

    if (argc == 1)
    {
        getBlocksCount(".");
        listdir(".", true);
    }
    // // else if (argc > 1)
    // {
    // }
    // while (++counter <= argc)
    // {
    //     listdir(argv[counter - 1]);
    // }

    return 0;
}

/*#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

int main(int argc, char *argv[])
{
    struct stat sb;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (lstat(argv[1], &sb) == -1)
    {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    printf("ID of containing device:  [%jx,%jx]\n",
           (uintmax_t)major(sb.st_dev),
           (uintmax_t)minor(sb.st_dev));

    printf("File type:                ");

    switch (sb.st_mode & S_IFMT)
    {
    case S_IFBLK:
        printf("block device\n");
        break;
    case S_IFCHR:
        printf("character device\n");
        break;
    case S_IFDIR:
        printf("directory\n");
        break;
    case S_IFIFO:
        printf("FIFO/pipe\n");
        break;
    case S_IFLNK:
        printf("symlink\n");
        break;
    case S_IFREG:
        printf("regular file\n");
        break;
    case S_IFSOCK:
        printf("socket\n");
        break;
    default:
        printf("unknown?\n");
        break;
    }

    // printf("I-node number:            %ju\n", (uintmax_t)sb.st_ino);

    // printf("Mode:                     %jo (octal)\n",
    //        (uintmax_t)sb.st_mode);

    // printf("Link count:               %ju\n", (uintmax_t)sb.st_nlink);
    // printf("Ownership:                UID=%ju   GID=%ju\n",
    //        (uintmax_t)sb.st_uid, (uintmax_t)sb.st_gid);

    // printf("Preferred I/O block size: %jd bytes\n",
    //        (intmax_t)sb.st_blksize);
    // printf("File size:                %jd bytes\n",
    //        (intmax_t)sb.st_size);
    // printf("Blocks allocated:         %jd\n",
    //        (intmax_t)sb.st_blocks);

    // printf("Last status change:       %s", ctime(&sb.st_ctime));
    // printf("Last file access:         %s", ctime(&sb.st_atime));
    // printf("Last file modification:   %s", ctime(&sb.st_mtime));

    exit(EXIT_SUCCESS);
}
*/