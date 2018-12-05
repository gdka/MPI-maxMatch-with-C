#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>    
#include <string.h>
#include <math.h>
#include "mpi.h"
#define MAXN 10000
#define MAXP 17

int AB[MAXN][MAXN];
int vis[MAXN];
int matchA[MAXN], matchB[MAXN];

int match(int a, int n){
	if(vis[a]) return 0;
	vis[a]=1;
	int b;
	for( b=0; b<n; b++){
		if(!AB[a][b]) continue;
		if(b==matchA[a]) continue;
		if(matchB[b]==-1 || match(matchB[b],n)){
			matchB[b]=a;
			matchA[a]=b;
			return 1;
		}
	}
	return 0;
}

int maxMatch(int n){
	int a,i,MBM=0;
	for(i = 0; i<n; i++) matchA[i]=-1,matchB[i]=-1;
	for(a=0; a<n; a++){
		for(i = 0; i<n; i++) vis[i]=0;
		if(match(a,n)) MBM++;
	}
	return MBM;
}

int is_prime(long long num){
	int sq_root = sqrt(num);
	int i;
	for( i = 2; i<=sq_root; i++) if(num%i==0) return 0;
	return 1; 
}

void verify(char **second_half,char *number, int cnt, int num_p){
	char str[20];
	long long num;
	int i;
	for(i = 0; i< cnt; i++){
		strcpy(str,second_half[i]);
		strcat(str,number);
		num = atoll(str);
		int arr[2];
		arr[0] = -1;
		arr[1] = num_p;
		//printf("Armo el numero %d\n",num);
		if(is_prime(num)){
			arr[0] = i; 
			//printf("Armo el numero primo %d\n",num);
		}
		MPI_Send(&arr,2,MPI_INT,0,0,MPI_COMM_WORLD);
	}
	
}
void esclavo(){
	int num_elem;
	char **second_half;
	int i;
	int me;
	MPI_Status estado, estado_string;
	MPI_Bcast(&num_elem,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Comm_rank(MPI_COMM_WORLD, &me);
	//printf("Soy el hijo %d y recibo %d\n",me,num_elem);
	second_half = (char**)malloc(sizeof(char*)*num_elem);

	
	for(i = 0; i < num_elem; i++){
		int length;
		MPI_Bcast(&length,1,MPI_INT,0,MPI_COMM_WORLD);
		//printf("Soy el hijo %d y recibo un numero con %d digitos\n",me,length);
		second_half[i] = (char*)malloc(sizeof(char)*length);
		MPI_Bcast(second_half[i],length+1,MPI_CHAR,0,MPI_COMM_WORLD);
		//printf("Soy el hijo %d y recibo el numero %s\n",me,second_half[i]);
	}

	int cnt_elements,cnt_process;
	MPI_Comm_size(MPI_COMM_WORLD,&cnt_process);
	cnt_process--;
	
	MPI_Bcast(&cnt_elements,1,MPI_INT,0,MPI_COMM_WORLD);
	
	//printf("Soy el hijo %d y recibire %d elementos\n",me,cnt_elements);

    for( i = 0; i<=cnt_elements; i++){
    	int length_first;
    	MPI_Recv(&length_first,1,MPI_INT,0,0,MPI_COMM_WORLD, &estado);
    	//printf("Soy el hijo %d y recibo un numero con %d digitos\n",me,length_first);
    	if(length_first==0) break;
    	length_first++;
    	char *number = (char*)malloc(sizeof(char)*length_first);
    	MPI_Recv(number,length_first,MPI_CHAR,0,0,MPI_COMM_WORLD,&estado);
    	//printf("Soy el hijo %d y recibo el numero %s\n",me,number);
    		
     	if(i==cnt_elements) verify(second_half,number,num_elem,cnt_process*cnt_elements-1+me);
    	else verify(second_half,number,num_elem,(me-1)*cnt_elements+i);
    	free(number);
    }
    
	

}
//­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­
void maestro(){
	int num_elements;
	MPI_Status estado;
	int i,j;
	scanf("%d",&num_elements);
	
	int cnt_process;
	
	MPI_Comm_size(MPI_COMM_WORLD,&cnt_process);
	
	//printf("Soy el padre y voy a leer %d elementos\n",num_elements);
	MPI_Bcast(&num_elements,1,MPI_INT,0,MPI_COMM_WORLD);
	char str[MAXN][2][MAXP];


	for(i=0; i<num_elements; i++){
		int length;
		scanf("%s",str[i][0]);
		length = strlen(str[i][0]);
		MPI_Bcast(&length,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(str[i][0],length+1,MPI_CHAR,0,MPI_COMM_WORLD);
	}

	
	cnt_process--;

	int cnt = num_elements / cnt_process; 
	
 
	MPI_Bcast(&cnt,1,MPI_INT,0,MPI_COMM_WORLD);
	
	for(i = 1; i<=cnt_process; i++){
		for(j = 0; j<cnt; j++){
			int length;
			scanf("%s",str[(i-1)*cnt+j][1]);
			length = strlen(str[(i-1)*cnt+j][1]);
			MPI_Send(&length,1,MPI_INT,i,0,MPI_COMM_WORLD);
			MPI_Send(str[(i-1)*cnt+j][1],length+1,MPI_CHAR,i,0,MPI_COMM_WORLD);
		}
	}
	
	int mod = num_elements % cnt_process; 
	
	for(i=1; i<=mod; i++){
		int length;
		scanf("%s",str[cnt*cnt_process-1+i][1]);
		length = strlen(str[cnt*cnt_process-1+i][1]);
		MPI_Send(&length,1,MPI_INT,i,0,MPI_COMM_WORLD);
		MPI_Send(str[cnt*cnt_process-1+i][1],length+1,MPI_CHAR,i,0,MPI_COMM_WORLD);
	}
	
	for(i=mod+1; i<=cnt_process; i++){
		int length = 0;
		MPI_Send(&length,1,MPI_INT,i,0,MPI_COMM_WORLD);
	}
	
	for( i = 0; i<num_elements*num_elements; i++){
		int arr[2];
		MPI_Recv(&arr,2,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&estado);
		int a = arr[0], b = arr[1];
		if(a!=-1 && b!=-1) AB[a][b]=1;
		//printf("Recibo que el numero %d y el %d hacen un primo\n",arr[0],arr[1]);
	}
	int maxM = maxMatch(num_elements);
	printf("El max match es: %d\n",maxM);
	int a;
	
	for( a=0; a<num_elements; a++){
		int b=matchA[a];
		printf("%s%s\n",str[a][0],str[b][1]);
	}



}

//­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­­
int main(int argc, char *argv[]){
	int me;
	MPI_Init(&argc,&argv);
	setbuf(stdout, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &me);
	if (me == 0) maestro();
	else esclavo();
	MPI_Finalize();
	return 0;
}
