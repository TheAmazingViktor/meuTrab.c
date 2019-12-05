#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#define TAMANHO 17 // Tamanho do problema
#define DISPONIVEL 10 //Medida de Volume da Mochila
#define MAXVOLUME 9
#define MAXVALOR 9

int volume[TAMANHO];
int valor[TAMANHO];
int mochila[TAMANHO];
int mochilaFinal[TAMANHO];
int nproc;

int preencheCusto(){
  int cont;
  printf("\nProdutos da LOJA  :\n");
  printf("VOLUME: ");
  for(cont=0;cont<TAMANHO;cont++){
	volume[cont]= (rand() % MAXVOLUME)+1;
	printf("%d ",volume[cont]);
  }
  printf("\nVALOR:  ");
  for(cont=0;cont<TAMANHO;cont++){
	valor[cont]= (rand() % MAXVALOR)+1;
	printf("%d ",valor[cont]);
  }
  printf("\n");
  return 0;
}

int imprimeMochila(int pid,int *m, int lucro, int carga){
  int cont;
  printf("\nPID: %d MOCHI:  ", pid);
  for(cont=0; cont<TAMANHO;cont++){
     printf("%d ",m[cont]);
  }
  printf ("  LUCRO: %d VOLUME: %d\n", lucro, carga);
}

int preencheMochila(int pid){
  MPI_Status status;
  int cont,cont2;
  int lucro=0,maiorLucro=0;
  int carga=0, maiorCarga=0;
  int baseCalc;
  int distancia = TAMANHO / pid;
  
  int mochilaFilho[TAMANHO];
  int valorFilho, volumeFilho;
  
  if(pid==0){
  	for(cont=0;(pid+1) * distancia;cont++){
        	baseCalc=cont;
		//printf("\n"); //Comentar
		if(lucro>maiorLucro && carga<=DISPONIVEL){
			for(cont2=0;cont2<TAMANHO;cont2++)
				mochilaFinal[cont2]=mochila[cont2];
			maiorLucro=lucro;
			maiorCarga=carga;
		}
		lucro=0;
		carga=0;
		for(cont2=0;cont2<TAMANHO;cont2++){
  			mochila[cont2]=baseCalc % 2;// Convertendo o número 
			baseCalc = baseCalc / 2;    // em binário!
			//printf("%d ",mochila[cont2]); //Comentar
			lucro += mochila[cont2]*valor[cont2];
			carga += mochila[cont2]*volume[cont2];
		}
		//printf(" VALOR: %d  VOLUME:%d",lucro,carga); //Comentar
  	}
  	
  	
	int mochilaTemp[nproc][TAMANHO];
	int valores[nproc];
	int volumes[nproc];
	
	//atribuido o resultado do pid 0 no vetor
	for(int i=0;i<TAMANHO; i++){		
		mochilaTemp[0][i] = mochilaFinal[i];		
	}	
	
	valores[0] = maiorLucro;
	volumes[0] = maiorCarga;
	
	
	for(int i=1; i<=pid; i++){
		MPI_Recv(&mochilaFilho, TAMANHO, MPI_INT, i, 1001, MPI_COMM_WORLD, &status); 
		MPI_Recv(&valorFilho, 1, MPI_INT, i, 1001, MPI_COMM_WORLD, &status); 
		MPI_Recv(&volumeFilho, 1, MPI_INT, i, 1001, MPI_COMM_WORLD, &status); 
			
		for(int j=0;j<TAMANHO; j++){		
			mochilaTemp[i][j] = mochilaFilho[j];		
		}	
	
		valores[i] = valorFilho;
		volumes[i] = volumeFilho;	
	}
	
	//descobrindo qual é o melhor lucro 
	int melhorLucro = 0;
	int melhorPid;
	for(int i=0; i<nproc; i++){
		if(valores[i]>melhorLucro){
			melhorLucro = valores[i];
			melhorPid = i;
		}	
	}
	
	for(int i=0; i<TAMANHO; i++){
		mochilaFilho[i] = mochilaTemp[melhorPid][i];
	}
	
	valorFilho = valores[melhorPid];
	volumeFilho = volumes[melhorPid];	
	  	imprimeMochila(1, mochilaFilho, valorFilho, volume[melhorPid]);
   }else{
  	int final = (pid + 1) * distancia;
  	if(pid == nproc - 1){
		final = TAMANHO;
	}	
  	
  	for(cont = (pid*distancia)+1;final;cont++){

        	baseCalc=cont;
		//printf("\n"); //Comentar
		if(lucro>maiorLucro && carga<=DISPONIVEL){
			for(cont2=0;cont2<TAMANHO;cont2++)
				mochilaFinal[cont2]=mochila[cont2];
			maiorLucro=lucro;
			maiorCarga=carga;
		}
		lucro=0;
		carga=0;
		for(cont2=0;cont2<TAMANHO;cont2++){
  			mochila[cont2]=baseCalc % 2;// Convertendo o número 
			baseCalc = baseCalc / 2;    // em binário!
			//printf("%d ",mochila[cont2]); //Comentar
			lucro += mochila[cont2]*valor[cont2];
			carga += mochila[cont2]*volume[cont2];
		}
		//printf(" VALOR: %d  VOLUME:%d",lucro,carga); //Comentar
  	}
  	//imprimeMochila(pid, mochilaFinal, maiorLucro, maiorCarga);
	//Enviar os dados para o processo 0
	MPI_Send(&mochilaFinal, TAMANHO, MPI_INT, 0, 1001, MPI_COMM_WORLD);
	MPI_Send(&maiorLucro, 1, MPI_INT, 0, 1001, MPI_COMM_WORLD);
	MPI_Send(&maiorCarga, 1, MPI_INT, 0, 1001, MPI_COMM_WORLD);
  }
  
}

int main(int argc, char *argv[]){
	int pid;
	srand(time(NULL));
	//Gerando a loja
	preencheCusto();
	MPI_Init(&argc, &argv);
      MPI_Comm_size(MPI_COMM_WORLD,&nproc);
      MPI_Comm_rank(MPI_COMM_WORLD,&pid);
      
   	preencheMochila(pid);
   MPI_Finalize();
}
