#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>

/* Definir la structure du point */
typedef struct Point{
	int x;
	int y;
}Point;

long long max(long long a, long long b){
	return((a)>(b)?(a):(b));
}

double my_gettimeofday(){
    struct timeval tmp_time;
    gettimeofday(&tmp_time, NULL);
    return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

/* Generation du tableau des points qui viennent du fichier */
void gen_Point(Point* table_points, FILE* fp,int l,int h,int n)
{   
    int i;
    
    for(i=0;i<n;i++)
    {
        fscanf(fp,"%d %d\n",&table_points[i].x,&table_points[i].y);
    }
    
}

long long surface1_max=0,max_surface=0;
long long calculer_table_surface_diviser(Point* table_points,int h,int left, int right, int milieu)
{
    int i,j,min_y=h,m=left,length = 0;
    long long surface1,surface2,surface3;
	
	length=right-left;
	if(length>8192)
	{	
		if(length == 1)
		{
	
			return h * (table_points[right].x - table_points[left].x);
		}
		else if(length==0)
		{
			return 0;
		}
		else
		{
		  	
			if(length == 2)
			{
				min_y = table_points[left+1].y;
				m = left+1;
			}
			else
			{
				for(i=left;i<right;i++)
				{

					if((i!= milieu)&& (min_y >= table_points[i].y) && (table_points[i].y != 0))
					{
						min_y = table_points[i].y;
						m = i;
					}
				}
			}
		
			surface1 = (long long)min_y * (table_points[right].x - table_points[left].x);
			surface1_max = max(surface1,surface1_max);
			if(m-left>4096)
			{
				#pragma omp task untied firstprivate(left,m,surface1,surface1_max)
				{
				surface2 = (long long)calculer_table_surface_diviser(table_points,h,left,m,m);
				}
			}
			else
			{
				surface2 = (long long)calculer_table_surface_diviser(table_points,h,left,m,m);
			}
			if(right-m>4096)
			{
				#pragma omp task untied firstprivate(right,m,surface1,surface1_max)
				{
				surface3 = (long long)calculer_table_surface_diviser(table_points,h,m,right,m);
			
				}
			}
			else
			{
				surface3 = (long long)calculer_table_surface_diviser(table_points,h,m,right,m);
			}
		  	#pragma omp taskwait
		max_surface = max(surface2,surface3);
		  	max_surface = max(max_surface,surface1_max);
		 		return max_surface;

		  }
		}
		else{
				
			if(length == 1)
			{
				
	
				return h * (table_points[right].x - table_points[left].x);
			}
			else if(length==0)
			{
				return 0;
			}
			else
			{
					
				if(length == 2)
				{
					min_y = table_points[left+1].y;
					m = left+1;
				}
				else
				{
					for(i=left;i<right;i++)
					{

						if((i!= milieu)&& (min_y >= table_points[i].y) && (table_points[i].y != 0))
						{
							min_y = table_points[i].y;
							m = i;
						}
					}
				}
		
		
				surface1 = (long long)min_y * (table_points[right].x - table_points[left].x);
				surface1_max = max(surface1,surface1_max);

				surface2 = (long long)calculer_table_surface_diviser(table_points,h,left,m,m);
				surface3 = (long long)calculer_table_surface_diviser(table_points,h,m,right,m);


				max_surface = max(surface2,surface3);
				max_surface = max(max_surface,surface1_max);
			 	return max_surface;
			}
		}   
}

int main(int argc, char** argv)
{
	int l,h,n;
    double debut,fin;
	long long max_surface;

	if (argc < 2){
    fprintf(stderr, "Usage: a.out n");
  	}

	FILE* fp=fopen(argv[1],"r");

    debut = my_gettimeofday();
    if (fp == NULL)
    {
        return 0;
    }
 
    fscanf(fp,"%d %d\n",&l,&h);
    fscanf(fp,"%d\n",&n);
    printf("%d %d %d\n",l,h,n);
	
	/*initialiser la tableau du point*/	
    Point *table_points = malloc(n*sizeof(Point));
    gen_Point(table_points,fp,l,h,n);
	

	#pragma omp parallel
	#pragma omp single
    max_surface = calculer_table_surface_diviser(table_points,h,0,n-1,0);
	printf(" le plus grand rectangle est: %lld\n ",max_surface);

    fin = my_gettimeofday();
	fclose(fp);
	free(table_points);
    fprintf( stdout, "%g\n", fin - debut);
    return 0;
}
