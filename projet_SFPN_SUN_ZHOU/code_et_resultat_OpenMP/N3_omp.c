#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

/* Definir la structure du point */
typedef struct Point
{
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
Point* gen_Point(Point* table_points, FILE* fp,int l,int h,int n)
{
    int i;
	for(i=0;i<n;i++)
    {
        fscanf(fp,"%d %d\n",&table_points[i].x,&table_points[i].y);
    }

    return table_points;
}

/* Calculer la plus grande surface entre 2 points p1 et p2 */

long long calculer_surface(Point p1,Point p2,Point* table_points,int n)
{
    int i,j,x1=p1.x,x2=p2.x,y1=p1.y,y2=p2.y;
    int min_y = table_points[0].y;
  
    /* Tester s'il y a des points dans l'axe x1 et x2 et /* Trouver le min_y des points internes */
    for(i=0;i<n;i++)
    {
        if(table_points[i].x > x1 && table_points[i].x < x2)
        {
			if((table_points[i].y < min_y) && (table_points[i].y!=0))
        	{
            	min_y = table_points[i].y;
        	}		                       
        }
    }
						
    return (long long)min_y * (long long)(x2 - x1);
}




long long calculer_table_surface(Point* table_points,int n)
{
	int i, j;
	long long surface=0, max_surface = 0;
    /* Commencer à calculer toutes les surfaces possibles */
   #pragma omp parallel shared(max_surface,table_points,n) \
	                 private(j,surface)
			
	{
	#pragma omp for schedule(dynamic,10) reduction(max:max_surface)
	for(i=0;i<n;i++)
    {
        for(j=i+1;j<n;j++)
        {
            surface = calculer_surface(table_points[i],table_points[j],table_points,n);
            max_surface = max(max_surface,surface);
        }
    }
	}
    return max_surface;
}


int main(int argc, char* argv[])
{
    int l,h,n;
	double debut,fin;
    
	if(argc<2){
    fprintf(stderr, "Usage: a.out  n");
	}
	
	FILE* fp=fopen(argv[1],"r");
   
	if (fp == NULL)
    {
        return 0;
    }
	debut = my_gettimeofday();
    fscanf(fp,"%d %d\n",&l,&h);
    fscanf(fp,"%d\n",&n);

    Point* table_points = malloc(n*sizeof(Point));
	gen_Point(table_points,fp,l,h,n);
   	long long max_surface = calculer_table_surface(table_points,n);
	printf(" le plus grand rectangle est: %lld\n ",max_surface);
	fin = my_gettimeofday();
	fclose(fp);
	free(table_points);

    fprintf( stdout, "%g\n", fin - debut);
}
