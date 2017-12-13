#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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
    int i, left, right;
	long long max_surface=0,surface=0,max_surface1=0,surface1=0;
    int min_y = table_points[0].y;

	/*le cas pour point du gauche,point i et point du droite*/
		for(i=1;i<n-1;i++)
		{
			//printf(" thread = %d\n",omp_get_thread_num());
			/*chercher le point du gauche*/        
			left = i-1;

			while(table_points[left].x == table_points[i].x)
		    {
		        left--;
		    }
		    while(table_points[i].y <= table_points[left].y && left>0)
		    {    
		        left--;       
		    }
		    min_y = table_points[i].y;


			/*chercher le point du droite*/  
		    right = i+1;
		    while(table_points[right].x == table_points[i].x)
		    {
		        right++;
		    }
		    while(table_points[i].y <= table_points[right].y && right<n-1)
		    {
		        right++;   
		    }

			/*calcule le surface*/
		    surface = (long long)min_y * (long long)(table_points[right].x - table_points[left].x);
		    
			/* le cas pour (x[i+1]-x[i])*h */
			surface1 = (long long)(table_points[i+1].x - table_points[i].x) * (long long)table_points[0].y;
			
			max_surface1 = max(surface,surface1);
	
			max_surface = max(max_surface,max_surface1);
		}	 
		max_surface = max(max_surface,(long long)(table_points[1].x - table_points[0].x) * (long long)table_points[0].y);
    return max_surface;
}



int main(int argc, char** argv)
{
	int l,h,n;
    double debut,fin;

	if (argc < 2){
    fprintf(stderr, "Usage: a.out l h n");
	}

	FILE* fp=fopen(argv[1],"r");

    debut = my_gettimeofday();
    if (fp == NULL)
    {
        return 0;
    }
 
    fscanf(fp,"%d %d\n",&l,&h);
    fscanf(fp,"%d\n",&n);
    //printf("%d %d %d\n",l,h,n);
	/*initialiser la tableau du point*/	
	Point *table_points = malloc((n)*sizeof(Point));

    gen_Point(table_points,fp,l,h,n);
    long long max_surface = calculer_table_surface_n2(table_points,n);
	printf(" le plus grand rectangle est: %lld\n ",max_surface);

    fin = my_gettimeofday();
	fclose(fp);
	free(table_points);
    /*fprintf( stderr, "Temps total de calcul : %g sec\n",
            fin - debut);*/
    fprintf( stdout, "%g\n", fin - debut);
    
}
