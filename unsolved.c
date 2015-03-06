#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <gmp.h>

int main(int argc, char** argv)
{
	DIR *dp;
	//struct dirent *ep;

	printf("Current unsolved families:\n");

	char filename[100];

	dp = opendir("./data");
	if(dp != NULL)
	/*{	while(ep = readdir(dp))
		{	strcpy(filename, ep->d_name);*/
	{	for(int base=2; base<=30; base++)
		{	sprintf(filename, "data/unsolved.%d.txt", base);
			/*filename[8] = '\0';
			if(strcmp(filename, "unsolved")==0)*/
			{	/*strcpy(strchr(filename+9, '.'), "\0");
				int base = atoi(filename+9);
				sprintf(filename, "data/%s", ep->d_name);*/
				FILE* in = fopen(filename, "r");
				if(in==NULL)
					continue;
				char line[100];
				printf("BASE %d:\n", base);
				char start[100];
				char middle[2];
				char end[100];
				while(fgets(line, 100, in)!=NULL)
				{	int l = (int)(strchr(line, '*')-line);
					middle[0] = line[l-1];
					middle[1] = '\0';
					line[strlen(line)-1] = '\0';
					line[l-1] = '\0';
					strcpy(start, line);
					strcpy(end, line+l+1);

					int zlen = strlen(end);
					mpz_t x, y, z, temp, temp2, temp3, temp10;
					mpz_inits(x, y, z, temp, temp2, temp3, temp10, NULL);
					mpz_set_str(x, start, base);
					mpz_set_str(y, middle, base);
					mpz_set_str(z, end, base);
					mpz_gcd_ui(temp10, y, base-1);
					int g = mpz_get_ui(temp10);
					mpz_divexact_ui(temp, y, g);
					mpz_set(temp2, temp);
					mpz_addmul_ui(temp, x, (base-1)/g);
					mpz_ui_pow_ui(temp3, base, zlen);
					mpz_mul(temp, temp, temp3);
					mpz_mul(temp2, temp2, temp3);
					mpz_submul_ui(temp2, z, (base-1)/g);
					mpz_neg(temp3, temp2);

					char astr[100] = {'\0'};
					char cstr[100];
					char nstr[10];
					mpz_t an, cn;
					mpz_inits(an, cn, NULL);
					mpz_set(an, temp);
					mpz_set(cn, temp3);
					if(mpz_cmp_ui(cn, 0)>0)
						sprintf(cstr, "+");
					else
					{	sprintf(cstr, "-");
						mpz_abs(cn, cn);
					}
					int n = 2;
					int s = 0;
					while(mpz_cmp_ui(an, 1)!=0)
					{	if(mpz_divisible_ui_p(an, n)!=0)
						{	mpz_divexact_ui(an, an, n);
							sprintf(nstr, "%d*", n);
							if(s==0)
								strcat(astr, nstr);
							s++;
						}
						else
						{	n++;
							if(s>1)
							{	astr[strlen(astr)-1] = '\0';
								sprintf(nstr, "^%d*", s);
								strcat(astr, nstr);
							}
							s = 0;
						}
					}
					n = 2;
					s = 0;
					while(mpz_cmp_ui(cn, 1)!=0)
					{	if(mpz_divisible_ui_p(cn, n)!=0)
						{	mpz_divexact_ui(cn, cn, n);
							sprintf(nstr, "%d*", n);
							if(s==0)
								strcat(cstr, nstr);
							s++;
						}
						else
						{	n++;
							if(s>1)
							{	cstr[strlen(cstr)-1] = '\0';
								sprintf(nstr, "^%d*", s);
								strcat(cstr, nstr);
							}
							s = 0;
						}
					}
					mpz_clears(an, cn, NULL);

					if(strcmp(cstr, "+")==0)
						sprintf(cstr, "+1");
					else if(strcmp(cstr, "-")==0)
						sprintf(cstr, "-1");
					else
						cstr[strlen(cstr)-1] = '\0';

					// Print family
					if((base-1)/g==1)
						printf("%s(%s)^n%s = %s%d^n%s\n", start, middle, end, astr, base, cstr);
					else
						printf("%s(%s)^n%s = (%s%d^n%s)/%d\n", start, middle, end, astr, base, cstr, (base-1)/g);

					mpz_clears(x, y, z, temp, temp2, temp3, temp10, NULL);
				}
				fclose(in);
			}
		}
	}

	return 0;
}
