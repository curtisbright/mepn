#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#define MAXSTRING 1200000

struct data {
	int level;
	int size;
	int width;
	int unsolved;
	int searchheight;
};

struct data* levels = NULL;
int count = 0;

int compare(const void* a, const void* b)
{	return ((struct data*)a)->level - ((struct data*)b)->level;
}

int main(int argc, char** argv)
{
	DIR *dp;
	struct dirent *ep;

	dp = opendir("./data");
	if(dp != NULL)
	{	while(ep = readdir(dp))
		{	char filename[100];
			strcpy(filename, ep->d_name);
			filename[7] = '\0';
			if(strcmp(filename, "minimal")==0)
			{	count++;
				strcpy(strchr(filename+8, '.'), "\0");
				int n = atoi(filename+8);
				sprintf(filename, "data/%s", ep->d_name);
				FILE* in;
				int numlines = 0;
				char line[MAXSTRING];
				if(in = fopen(filename, "r"))
				{	char** lines = NULL;
					int maxsize = 0;

					while(fgets(line, MAXSTRING, in)!=NULL)
					{	numlines++;
						lines = realloc(lines, sizeof(char*)*numlines);
						if(lines==NULL)
						{	printf("couldn't allocate %zu bytes\n", sizeof(char*)*numlines);
							exit(1);
						}
						lines[numlines-1] = malloc(strlen(line)+1);
						if(lines[numlines-1]==NULL)
						{	printf("couldn't allocate %zu bytes\n", strlen(line)+1);
							exit(1);
						}
						strcpy(lines[numlines-1], line);
						lines[numlines-1][strlen(line)-1] = '\0';
						if(strlen(lines[numlines-1])>maxsize)
							maxsize = strlen(lines[numlines-1]);
					}

					//printf("%d\t size %d\tlength %d\n", n, numlines, maxsize);
					levels = realloc(levels, sizeof(struct data)*count);
					if(levels==NULL)
					{	printf("couldn't allocate %zu bytes\n", sizeof(struct data)*count);
						exit(1);
					}
					levels[count-1].level = n;
					levels[count-1].size = numlines;
					levels[count-1].width = maxsize;

					fclose(in);
					for(int i=0; i<numlines; i++)
						free(lines[i]);
					free(lines);
				}

				numlines = 0;
				sprintf(filename, "data/unsolved.%d.txt", n);
				if(in = fopen(filename, "r"))
				{	while(fgets(line,MAXSTRING,in)!=NULL)
						numlines++;
					fclose(in);
					levels[count-1].unsolved = numlines;
				}

				sprintf(filename, "data/sieve.%d.txt", n);
				levels[count-1].searchheight = 0;
				if(in = fopen(filename, "r"))
				{	while(fgets(line,MAXSTRING,in)!=NULL)
					{	if(strchr(line,'*')==NULL && strchr(line,'=')==NULL)
							if(levels[count-1].searchheight==0 || levels[count-1].searchheight>atoi(line))
								levels[count-1].searchheight = atoi(line)-1;
					}
					fclose(in);
				}
			}
		}
		(void)closedir(dp);
	}
	else
		perror("Couldn't open the directory");

	qsort(levels, count, sizeof(struct data), compare);

	printf("level\tsize\twidth\tunsolved\tsearched height\n");
	for(int i=0; i<count; i++)
	{	char str[100];
		sprintf(str, "%d", levels[i].searchheight);
		printf("%d\t%d\t%d\t%d\t\t%s\n", levels[i].level, levels[i].size, levels[i].width, levels[i].unsolved, levels[i].searchheight==0 ? "-" : str);
	}

	return 0;
}
