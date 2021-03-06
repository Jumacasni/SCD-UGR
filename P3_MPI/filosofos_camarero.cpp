#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

using namespace std;

void Filosofo( int id, int nprocesos);
void Tenedor ( int id, int nprocesos);
void Camarero(int id, int nprocesos);

// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int rank, size;
   
   srand(time(0));
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   if( size!=11)
   {
      if( rank == 0) 
         cout<<"El numero de procesos debe ser 11" << endl << flush ;
      MPI_Finalize( ); 
      return 0; 
   }
   
   if ((rank%2) == 0 && rank < 10)  
      Filosofo(rank,size); // Los pares son Filosofos 
   else if (rank == 10)
      Camarero(rank, size);
   else 
      Tenedor(rank,size);  // Los impares son Tenedores 
   
   MPI_Finalize( );
   return 0;
}  
// ---------------------------------------------------------------------

void Filosofo( int id, int nprocesos )
{
   int izq = (id+1) % (nprocesos-1);
   int der = (id+nprocesos-2) % (nprocesos-1);   
   MPI_Status status;

   while(1)
   {
      
      MPI_Ssend(&id, 1, MPI_INT, 10, 2, MPI_COMM_WORLD);
      cout << "Filosofo " << id << " solicita sentarse" << endl << flush;

      MPI_Recv(&id, 1, MPI_INT, 10, 2, MPI_COMM_WORLD, &status);
      cout << "Filosofo " << id << " se sienta en la mesa" << endl << flush;


      MPI_Ssend(&id, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);
      cout << "Filosofo "<<id<< " solicita tenedor izquierdo " << izq << endl << flush;
      
      MPI_Ssend(&id, 1, MPI_INT, der, 0, MPI_COMM_WORLD);
      cout <<"Filosofo "<<id<< " coge tenedor derecho " << der << endl << flush;


      cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
      sleep((rand() % 3)+1);


      MPI_Ssend(&id, 1, MPI_INT, izq, 1, MPI_COMM_WORLD);
      cout <<"Filosofo "<<id<< " suelta tenedor izquierdo " << izq << endl << flush;
      
      MPI_Ssend(&id, 1, MPI_INT, der, 1, MPI_COMM_WORLD);
      cout <<"Filosofo "<<id<< " suelta tenedor derecho " << der << endl << flush;

      
      MPI_Ssend(&id, 1, MPI_INT, 10, 3, MPI_COMM_WORLD);
      cout << "Filosofo " << id << " se levanta de la mesa" << endl << flush;

      cout << "Filosofo " << id << " PENSANDO" << endl << flush;
      sleep((rand() % 3)+1);

      usleep( 1000U * (100U+(rand()%900U)) );
 }
}
// ---------------------------------------------------------------------

void Tenedor(int id, int nprocesos)
{
  int buf; 
  MPI_Status status; 
  int Filo;
  
  while( true )
  {
    MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    Filo = status.MPI_SOURCE;

    cout << "Tenedor " << id << " recibe petición de " << Filo << endl << flush;
    
    MPI_Recv(&buf, 1, MPI_INT, Filo, 1, MPI_COMM_WORLD, &status);

    cout << "Tenedor " << id << " recibe liberación de " << Filo << endl << flush; 
  }
}
// ---------------------------------------------------------------------

void Camarero(int id, int nprocesos){
  int buf, n_sentados = 0, Filo;
  MPI_Status status;

  while(true){

    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    if(n_sentados < 4 && status.MPI_TAG == 2){
      MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
      n_sentados++;

      Filo = status.MPI_SOURCE;

      MPI_Ssend(&buf, 1, MPI_INT, Filo, 2, MPI_COMM_WORLD);
      cout << "El camarero le dice al filósofo " << Filo << " que puede sentarse en la mesa. Hay " << n_sentados << " filósofos sentados en la mesa" << endl << flush;
    }

    else if(n_sentados < 4 && status.MPI_TAG == 3){
      MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
      Filo = status.MPI_SOURCE;
      cout << "El filósofo " << Filo << " puede retirarse de la mesa" << endl << flush;
      n_sentados--;
    }
  }
}