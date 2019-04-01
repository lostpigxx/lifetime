
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

#define K 512	//链的长度
#define LEN 6		//key的长度
#define X (4 * 1024 * 1024) //链的条数
#define SS 36   //明文的字符种类
typedef unsigned long long int UINT64;

//selv.txt pass.txt

char alpha[] = "abcdefghijklmnopqrstuvwxyz0123456789";

UINT64 N;
BIGNUM *bn0, *bn1, *bn2;

void R_function(UINT64 n, unsigned char *bufin, unsigned char *bufout)
{
	//a-z 0-9
	//将160位hash值映射到[0, N-1]的空间中，并且对于不同的n，相同的hash值映射不同
	UINT64 bias = 0;
	int i;
	int t;
	for(i=0; i<20; ++i)
	{
		bias += bufin[i];
		bias = bias << 3;
	}
	for(i=0; i<LEN; ++i)
	{
		bias += n;
		t = bias % SS;
		bufout[i] = alpha[t];
		bias /= SS;
	}
	bufout[LEN] = 0;
	//printf("-->%s\n", bufout);
}

void initial_start_point(unsigned char *buf)
{
	FILE *fp;
	UINT64 i = 0, k;
	unsigned char t;
	puts("Initialing start point!");
	if((fp = fopen("pass.txt", "r+")) == NULL)
	{
		puts("error!");
		exit(0);
	}

	while(fscanf(fp, " %s", buf) != EOF && i < X)
	{
		buf += LEN + 1;
		i++;
	}
	if(i < X)
	{
		for(; i<X; ++i)
		{
			if(i % 10000 == 0)
				printf("%llu\n", i);
			for(k=0; k<LEN; ++k)
			{
				 RAND_bytes(&t, 1);
				 t %= SS;
				 buf[k] = alpha[t];
			}
			buf[LEN] = 0;
			buf += (LEN + 1);
		}
	}
	fclose(fp);
	puts("Start point initial done!");
}

void generate_table()
{
	UINT64 i, j, k, l, m;
	UINT64 fsize = X * LEN * 2;
	unsigned char sp[LEN+1], ep[LEN+1];
	unsigned char th[21];
	unsigned char t;
	unsigned char thx[41];
	FILE *fp;
	unsigned char *sps;
	unsigned char *pt;

	sps = (unsigned char *)malloc(sizeof(unsigned char) * X * (LEN + 1));
	pt = sps;
	initial_start_point(sps);

	if((fp = fopen("rainbowtable.txt", "w+")) == NULL)
	{
		puts("error!");
		exit(0);
	}
	for(i=0; i<X; ++i)
	{
		//生成起始点
		memcpy(sp, pt, LEN+1);
		pt += (LEN + 1);
		memcpy(ep, sp, LEN+1);
		for(j=0; j<K; ++j)
		{
			SHA1(ep, LEN, th);
			R_function(j, th, ep);
		}
		if(i % 1000 == 0)
			printf("Generate chain %llu\n", i);
		fprintf(fp, "%s %s\n", (char *)sp, (char *)ep);
	}
	free(sps);
	fclose(fp);
}


void crack_password()
{
	unsigned char *ps;
	unsigned char *pe;
	FILE * fp;
	UINT64 i;
	int count, j, k, l, m, mm, ll;
	int lian, huan;
	unsigned char hbuf[41];
	unsigned char buf0[21], buf1[LEN+1], buf2[21];
	int flag, flag1;
	UINT64 success, fail;

	unsigned char sp[LEN+1], ep[LEN+1];
	unsigned char th[21];
	unsigned char t;
	unsigned char thx[41];

	vector<string> v;
	if((fp = fopen("rainbowtable.txt", "r+")) == NULL)
	{
		puts("error!");
		exit(0);
	}
	ps = (unsigned char *)malloc(sizeof(char) * X * (LEN + 1));
	pe = (unsigned char *)malloc(sizeof(char) * X * (LEN + 1));

	for(i=0; i<X; ++i)
	{
		fscanf(fp, " %s %s", ps + i*(LEN+1), pe + i*(LEN+1));
		string t0((char *)ps + i*(LEN+1));
		string t1((char *)pe + i*(LEN+1));
		t1 = t1 + t0;
		v.push_back(t1);
	}
	fclose(fp);
	//puts("Input number of SHA1");
	//cout << v[0] << ' ' << v[1] << endl;
	sort(v.begin(), v.end());
	//cout << v[1086110] << ' ' << v[1086111] << ' ' << v[1086112] << endl;
	for(i=0; i<X; ++i)
	{
		memcpy(pe + i*(LEN+1), v[i].c_str(), LEN);
		memcpy(ps + i*(LEN+1), v[i].c_str() + LEN, LEN);
		ps[i*(LEN+1) + LEN] = 0;
		pe[i*(LEN+1) + LEN] = 0;
	}

	//printf("%s %s\n", ps+3*(LEN+1), pe+3*(LEN+1));

	puts("Cracker initialing done.");


	//479407
	fp = fopen("selv1.txt", "r+");
	count = 5;
	success = 0;
	fail = 0;
	for(j=0; j<count; ++j)
	{
		lian = -1;
		huan = -1;
		fscanf(fp, " %s", buf1);
		SHA1(buf1, LEN, buf2);
		buf2[20] = 0;
		flag = 0;
		for(k=K-1; k>=0; --k)//k第几环
		{
			for(m=0; m<20; ++m)
				buf0[m] = buf2[m];
			buf0[20] = 0;

			for(l=k; l<K; ++l)
			{
				R_function(l, buf0, buf1);
				if(l == K - 1)
					break;
				SHA1(buf1, LEN, buf0);
			}

			l = X / 2;
			int dl = 0, dr = X-1;
			string a((char *)buf1);
			while(abs(dr - dl) > 1)
			{
				//cout << dr << ' ' << dl << ' ' << l << endl;
				l = (dl + dr) / 2;
				string b((char *)pe + (LEN + 1) * l);
				//cout << a << '*' << b << ' ' << dl << ' ' << l << ' ' << dr << endl;
				if(a > b)
				{
					dl = l;
					continue;
				}
				else if(a < b)
				{
					dr = l;
					continue;
				}
				else //find
				{
					int el = l, er = l;
					while(1)
					{
						el--;
						string c((char *)pe + (LEN + 1) * el);
						if(c != a || el < 0)
							break;
					}
					el++;
					while(1)
					{
						er++;
						string c((char *)pe + (LEN + 1) * er);
						if(c != a || er >= X)
							break;
					}
					er--;
					//cout << l << ' ' << el << ' ' << er << endl;
					for(m=el; m<=er; ++m)
					{
						//检查是否是false alarm
						flag1 = 0;
						for(mm=0; mm<LEN; ++mm)
							sp[mm] = ps[m * (LEN + 1) + mm];
						sp[LEN] = 0;
						memcpy(ep, sp, LEN+1);
						for(ll=0; ll<k; ++ll)
						{
							SHA1(ep, LEN, th);
							R_function(ll, th, ep);
						}
						SHA1(ep, LEN, th);
						for(mm=0; mm<20; ++mm)
						{
							if(buf2[mm] != th[mm])
								flag1 = 2;
						}
						if(flag1 == 0)
						{
							lian = m;
							flag = 1;
							break;
						}
					}
					break;
				}
				if(flag == 1)
					break;
			}

			if(flag == 1) //find
			{
				huan = k;
				break;
			}
		}

		if(lian == -1 || huan == -1)
		{
			printf("Can not crack!  ");
			fail++;
			printf("The %d (success:%llu fail:%llu) \n", j, success, fail);
			continue;
		}
		//printf("In %d , %d position\n", lian, huan);	
		printf("Crack answer: %s  ", ep);
		success++;
		printf("The %d (success:%llu fail:%llu) \n", j, success, fail);
	}

	fclose(fp);
	free(ps);
	free(pe);
}

int main()
{
	int i;
	bn0 = BN_new();
	bn1 = BN_new();
	bn2 = BN_new();
	N = pow(SS, LEN); 
	puts("1:generate 2:crack");
	scanf(" %d", &i);
	if(i == 1)
	{
		generate_table();
	}
	else
		crack_password();
	BN_free(bn0);
	BN_free(bn1);
	BN_free(bn2);
	return 0;
}



