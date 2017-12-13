#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

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

long long calculer_table_surface_n2(Point* table_points,int n)
{
    int i, j;
	long long max_surface=0,surface=0,max_surface1=0;
    int min_y;

	/*le cas pour point du gauche,point i et point du droite*/
	/*parallel openmp commence*/
	#pragma omp parallel shared(max_surface,table_points) \
	                     private(i,j,surface,min_y,	max_surface1)
	{
    	#pragma omp for schedule(dynamic,10) reduction(max:max_surface)
    	for(i=0;i<n-1;i++)
    	{	
            
    		min_y = table_points[i+1].y;
    		for(j=i+1;j<n-1;j++)
    		{	
    			if(min_y>table_points[j].y){
    				min_y = table_points[j].y;
    			}
    
    			surface = (long long)(table_points[j+1].x-table_points[i].x)*(long long)min_y;
    			
    			max_surface1 = max(max_surface1,surface);
    			
    		}
    		
    		 /* le cas pour (x[i+1]-x[i])*h */
    		
    		surface = (long long)(table_points[i+1].x - table_points[i].x) * (long long)table_points[0].y;
            
            max_surface = max(max_surface1,surface);
    	} 
   	} 
     
    
    return max_surface;
}



int main(int argc, char **argv)
{
	int l,h,n;
    double debut,fin;

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

	
	/*initialiser la tableau du point*/	
	Point *table_points = malloc(n*sizeof(Point));

    gen_Point(table_points,fp,l,h,n);
    debut = my_gettimeofday();
    long long max_surface = calculer_table_surface_n2(table_points,n);

    fin = my_gettimeofday();
	fclose(fp);
	free(table_points);
    fprintf( stdout, "%g\n", fin - debut);
    
}
