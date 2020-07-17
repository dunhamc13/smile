/* Notes About File
 * @File: Shop.h
 * @Name: Christian Dunham
 * @Number: 1978955
 * @Date: 18April2020
 * @Program Name:  Barber Shop
 *
 * **** Seed Program created by Professor Dimpsey
 * ***  MultiThreaded version designed by Dunham
 *
 * Program Purpose:
 *    This Barber Shop is to practice using multithreading techniques to manage
 *    threads.  In this program, a barber shop has nWating chairs, mService chairs
 *    and thread pools help to split thw ork between barbers and the number of 
 *    customers.
 *
 * Design Rational:  One importance feature is that I chose to implement was a
 *    the utilization of thread pooling.  This will help to prevent dead locks
 *    and protect system resources.
 *
 * Dynamic Memory:
 *    There are no dynamic memory considerations for this simple driver.
 *
 *******************************************************************************
 *******************************************************************************
 *
 *                        Special Cases Identified
 *
 * * None at this time : xyz
 * 
 *******************************************************************************
 *
 *******************************************************************************
 *Product BackLog :
 *                 1) Await for Instructor Comments
 *
 *******************************************************************************
 *
 *******************************************************************************
 * Code Outline :
 *
 *                Driver    : main()
 *                Shop      : init()
 *                          : int2string(int i)
 *                          : print(int person, string message)
 *                          : visitShop(int id)
 *                          : leaveShop(int id)
 *                          : helloCustomer()
 *                          : byeCustomer()
 *
 ******************************************************************************* 
 *
 *                        Included Libraries
 *
 *******************************************************************************
 *******************************************************************************
*/
#ifndef __SHOP_H__
#define __SHOP_H__
#include <stdlib.h>       //Standard Lib
#include <iostream>       //cout 
#include <sstream>        //for in2string member
#include <pthread.h>      //posix threads 
#include <queue>          //for waiting_chairs storage
#include <deque>          //for sleepingBarbers storage
#include <unordered_map>  //to make hashmap of customerId & customerCondition
#include <string>         //strings
using namespace std;

#define DEFAULT_CHAIRS 3  //default number of chairs 
#define DEFAULT_BARBERS 1 //defualt number of barbers
class Shop 
{

   public:
   /* Overload Constuctor
    * @Description: Initializes shop with number of barbers and chairs.
    * @Pre: driver accepted nBarbers and nChairs
    * @Post: Shop initialized with all members to defualts/params
    * @Param: nBarbers, int for number of barber threads.
    * @Param: nChairs, int for number of waiting chairs
   */
   Shop(int nBarbers, int nChairs) : 
      max((nChairs >= 0) ? nChairs : DEFAULT_CHAIRS), money_paid(false),
      nBarbers(nBarbers), nChairs(nChairs), nDropsOff(0)
     {who_is_in_my_service_chair.resize(nBarbers, -1); init();};
   
   /* Constuctor
    * @Description: Initializes shop with number of barbers and chairs.
    * @Pre: driver accepted nBarbers and nChairs
    * @Post: Shop initialized with all members to defualts/params
   */
   Shop() : 
      max(DEFAULT_CHAIRS), money_paid(false),
      nBarbers(DEFAULT_BARBERS), nChairs(DEFAULT_CHAIRS), 
      nDropsOff(0)
     {who_is_in_my_service_chair.resize(nBarbers, -1); init();};
      
   /* Destructor
    * @Description: Destroys all heap memory
    * @Pre: Shop created
    * @Post: All dynamic memory destroyed
   */
   ~Shop()
   {
     pthread_cond_destroy(&customer_waiting_cond);
     for(int i = 0; i < nBarbers; ++i)
     {
         pthread_cond_destroy(&barber_waiting_conds[i]);
     }

     pthread_mutex_destroy(&mutex_barber);
     pthread_mutex_destroy(&mutex_customer);
     delete []barber_waiting_conds;
   }
    
   /* visitShop
    * @Description: Takes in integer id and determines if the waiting chairs are
    * full in the shop.  If there is waiting room, id waits.  If a service chair is
    * open, id gets serviced and wakes the barber.
    * @Pre: Barber, waiting, and servicing chairs assigned
    * @Post: id is serviced or leaves, bool value passed
    * @Param: id is an integer representing a thread customer
   */
   int visitShop(int customerId);

   /* leaveShop
    * @Description: Takes a thread id and waits for a haircut, recieves haircut and
    * pays, saying goodbye.
    * @Pre: Shop created, customer id created
    * @Post: Customer id serviced
    * @Param: id is an integer representing a customer thread
   */
   void leaveShop(int customerId, int barberId);

   /* helloCustomer
    * @Description: Checks if no customers, then barber sleeps, else customer sits
    * down and barber starts haircut.
    * @Pre: Barber and shop created
    * @Post: If customer exists, hair cut starts
   */
   void helloCustomer(int barberId);
  
   /* byCustomer
    * @Description: completes the hair cut process for one customer and calls
    * for another customer
    * @Pre:Customer is receiving haircut
    * @Post: Customer leaves, new if there comes in for service
   */
   void byeCustomer(int barberId);
   
   int nDropsOff; //customer that leaves without service
    
   private:

   //Private data members
   const int max;           //maximum number of threads that can wait
   bool money_paid;         //state of payment for haircut
   int nBarbers;            //number of barbers
   int nChairs;             //number of waiting chairs
   
   //Private data structures
   deque<int> sleepingBarbers;      //barbers waiting for customer thread
   queue<int> waiting_chairs;   //holds ids of customers in waiting chairs

   //Vector index represents barberId and vector[barberId] holds customer id
   vector<int> who_is_in_my_service_chair;  

   //Private Thread & Synchronization members
   pthread_cond_t customer_waiting_cond; //Condition for Customer threads
   pthread_cond_t* barber_waiting_conds;//Condition for Barber Threads
   pthread_cond_t cond_paid; //Condition for Customer threads

   //Map to tie customer id and customer condition together
   unordered_map<int,pthread_cond_t> hmap_cust_wait_conds; 
   pthread_mutex_t mutex_barber;   //enter barber critical section
   pthread_mutex_t mutex_customer; //enter customer critical section

   //Private members
   
   /* init
    * @Description: Not sure what this does yet
    * @Pre:
    * @Post:
   */
   void init();
   
   /* int2string
    * @Description: Takes an integer as an argument and turns it to a string
    * @Pre:An integer has been created
    * @Post: The integer is returned as a string
    * @Param: i is an integer to be converted to a string
   */
   string int2string(int i);                //inline to reduce overhead
   
   /* print
    * @Description: Takes an integer value and a string prints a corresponding 
    * message.  If number is negative prints barber, else customer.
    * @Pre: barber / customer integer created and message
    * @Post: Message passed to std out
    * @Param: person is an integer value to determine barber or customer
    * @Param: message is a string 
   */
   void print(int person, string message);  //inline to reduce overhead
};//end Shop
#endif
