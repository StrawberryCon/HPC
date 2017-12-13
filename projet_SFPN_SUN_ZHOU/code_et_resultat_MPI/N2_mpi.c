#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
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

long long calculer_table_surface_n2(Point* table_points,int n,int size,int rank)
{
    int i, j;
	long long max_surface=0,surface=0,surface2=0,max_surface1=0;
    int min_y;

	/*le cas pour point du gauche,point i et point du droite*/
	if(rank != size-1)
	{    	
		
		for(i=rank*n/size;i<(rank+1)*n/size;i++)
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
	    		
	    		surface2 = (long long)(table_points[i+1].x - table_points[i].x) * (long long)table_points[0].y;
		    max_surface = max(max_surface1,surface2);
		}
   	}
	else
	{
		
		for(i=rank*n/size;i<n-1;i++)
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

	/* Initialiser les variables MPI  */
	int rank;	
	int size;
	MPI_Status status;
	/*Initialiser MPI */
	MPI_Init(&argc ,&argv);
/*récupérer le rang et le nombre de processus */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	int i,j;
	FILE * fp;
	Point *table_points;
	if(rank == 0)
	{
	
		if (argc < 2){
	    fprintf(stderr, "Usage: a.out n");
	  	}

		fp=fopen(argv[1],"r");


	

	    if (fp == NULL)
	    {
		return 0;
	    }
	 
	    fscanf(fp,"%d %d\n",&l,&h);
	    fscanf(fp,"%d\n",&n);
	/*initialiser la tableau du point*/	
	table_points = malloc(n*sizeof(Point));
	
	    gen_Point(table_points,fp,l,h,n);

		for(i=1;i<size;i++)
		{
			MPI_Send(table_points,2*n,MPI_INT,i,98,MPI_COMM_WORLD);
		}
		
	}
	else
	{
	
		MPI_Probe(0,98,MPI_COMM_WORLD,&status);
		MPI_Get_count(&status,MPI_INT,&n);
		n = n/2;
		table_points = malloc(n*sizeof(Point));
		MPI_Recv(table_points,2*n,MPI_INT,0,98,MPI_COMM_WORLD,&status);	
		
		
	}
	debut = my_gettimeofday();

	long long max_surface = calculer_table_surface_n2(table_points,n,size,rank);

	long long *surface = malloc(size*sizeof(long long));

	
	if(rank == 0)
	{
		for(i=1;i<size;i++)
		{
		
			MPI_Recv(&surface[i],1,MPI_LONG_LONG_INT,i,99,MPI_COMM_WORLD,&status);
		
			if(surface[i] > max_surface)
			{
				max_surface = surface[i];
			}
		}
		
	}
	else
	{
		MPI_Send(&max_surface,1,MPI_LONG_LONG_INT,0,99,MPI_COMM_WORLD);
	
	}
	printf(" le plus grand rectangle est: %lld\n ",max_surface);

    	fin = my_gettimeofday();
	
	if(rank == 0)
	{
		fclose(fp);
	}
	free(table_points);

    fprintf( stdout, "%g\n", fin - debut);
    MPI_Finalize();
}
