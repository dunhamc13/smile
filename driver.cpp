#include <iostream>    //cout
#include <sys/time.h>  //gettimeofday
#include <unistd.h>    //usleep
#include <cstdlib>     //pthread_create
#include <stdlib.h>    //standard 
#include "Shop.h"      //Shop header
using namespace std;

//______________________________________________________________________________
//Operation 1 : Forward Declarations; Supporting Class Creation

//Forward declations of thread functions
void *barber( void * );    //barber thread function
void *customer( void * );  //customer thread function

/* ThreadParam
 * @General Description: This class hold data paramaters to help make the threads
 * for the shop class.
 * 
 * Each Param holds a shop object pointer, id, and service time.  These are the 
 * essential parameters thread creation in the shop program.
*/
class ThreadParam {
public:
  /* Overloaded Constructor
   * Takes shop pointer, thread id, and usleep serviece time
  */
  ThreadParam( Shop *shop, int id, int serviceTime ) :
    shop( shop ), id( id ), serviceTime( serviceTime ) { };
  Shop *shop;               //pointer to the Shop object
  int id;                   //thread identifier
  int serviceTime;          //time for barber service, 0 to a customer
};//end ThreadParam


//______________________________________________________________________________
//Operation 1 : Recieve, Initialize, and Execute Program

/* main
 * @General Description: This is the main driver for the sleepingBarbers
 * program.  It accepts 5 arguments.  The first is the executable.  The next
 * four are used to create the paramaters of the sleepingBarber problem.  Once,
 * nBarbers, nChairs, nCustomers, and serviceTime are assigned; the program 
 * intitializes a shop and executes the synchronization problem.
 * 
 * The main drive executes 3 operations:
 * 2(a) Checks parameters, assigns values, and confrims values
 * 2(b) Creates Threads
 * 2(c) Joins and cancels threads
*/
int main( int argc, char *argv[] ) 
{

//______________________________________________________________________________
//Operation 2(a) : Check Passed Paramaters; assign values; confirm values

  // validate the number of arguments
  if ( argc != 5 ) 
  {
    cerr << 
      "usage: sleepingBarber [nBarbers] [nChairs] [nCustomers] [serviceTime]" 
       << endl;                      //error message
    return -1;
  }//end if arguments passed not 5

  int nBarbers = atoi(argv[1]);      //number of Barbers
  int nChairs = atoi(argv[2]);       //number of waiting chairs
  int nCustomers = atoi(argv[3]);    //number of customers
  int serviceTime = atoi(argv[4]);   //how long each barber takes

//______________________________________________________________________________
//Operation 2(b) : Create Threads

  pthread_t barber_threads[nBarbers];     //array of barber threads
  pthread_cond_t end_barbers[nBarbers];
  pthread_t customer_threads[nCustomers]; //array of customer threads
  Shop shop( nBarbers, nChairs );         //instantiate a barbershop
  
  //Create barber threads
  for ( int i = 0; i < nBarbers; i++ ) 
  { 
     //use param class to collect essential information and pass it
     ThreadParam *barber_param = new ThreadParam(&shop, i, serviceTime);
     pthread_create( &barber_threads[i], NULL, barber, (void *)barber_param);
  }//end barber thread creation
  
  //Create conditions to join barbers at end of solution
  for ( int i = 0; i < nBarbers; i++ ) 
  { 
     //use param class to collect essential information and pass it
     pthread_cond_init( &end_barbers[i], NULL);
  }//end barber conds creation

  //create customer threads
  for ( int i = 0; i < nCustomers; i++ ) 
  {
     //use param class to collect essential informaiton and pass it 
     usleep( rand( ) % 1000 );              //at random times created
     ThreadParam *customer_param = new ThreadParam( &shop, i + 1, 0 );
     pthread_create( &customer_threads[i], NULL, customer, (void *)customer_param );
  }//end customer thread creation

//______________________________________________________________________________
//Operation 2(c) : Join & Cancel Threads

  //Make sure all customers went to shop
  for ( int i = 0; i < nCustomers; i++ ) 
    pthread_join( customer_threads[i], NULL );
/*  
  //Make sure all barbers are given a pink slip (terminated)
  for ( int i = 0; i < nBarbers; i++ ) {
    pthread_cond_signal(&end_barbers[i]);    
    pthread_join( barber_threads[i], NULL );
  }//end join barbers
*/
  //Make sure all barbers are given a pink slip (terminated)
  for ( int i = 0; i < nBarbers; i++ )    
    pthread_cancel( barber_threads[i] );

  //Let user know how effecient implementation was:
  cout << "# customers who didn't receive a service = " << shop.nDropsOff
       << endl;

  return 0;
}//end main

//______________________________________________________________________________
//Operation 3 : Thread Functions

/* barber
 * @General Description: Function is passed to pthread_create in order to make
 * barber threads.  
 *
 * Pre: main initialized, barber params created and passed as void pointer.
 * Post: barber thread in while loop waiting on customer threads
 * @Param: arg, a void pointer that holds a ThreadParam object.  Each 
 * threadParam holds a pointer to a shop object, id, and serviceTime.
*/
void *barber( void *arg ) 
{

  //extract parameters
  ThreadParam &param = *(ThreadParam *)arg;  //void pointer
  Shop &shop = *(param.shop);                //shop object
  int id = param.id;                         //thread id for conditions
  int serviceTime = param.serviceTime;       //barber service time
  delete &param;

  //Work till canceled in main
  while( true ) 
  {
    shop.helloCustomer(id);  //wait for customer
    usleep(serviceTime);     //how long I cut hair
    shop.byeCustomer(id);    //release customer 
  }//end while
}//end barber thread

/* customer
 * @General Description: Function is passed to pthread_create in order to make
 * customer threads.  
 *
 * Pre: main initialized, customer params created and passed as void pointer.
 * Post: Customer thread uses barber thread for hair cut or leaves shop
 * @Param: arg, a void pointer that holds a ThreadParam object.  Each 
 * threadParam holds a pointer to a shop object, id, and serviceTime.
*/
void *customer( void *arg ) {

  //extract parameters
  ThreadParam &param = *(ThreadParam *)arg; //void pointer
  Shop &shop = *(param.shop);               //shop object
  int id = param.id;                        //thread id for conditions
  delete &param;

  //if assigned a barber i then wait for service to finish
  int barber = -1;                          //-1 means didn't get barber
  if ( ( barber = shop.visitShop( id ) ) != -1 )
    shop.leaveShop( id, barber );                //wait till service is finished
}//end customer thread


