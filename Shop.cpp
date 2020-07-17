/* Notes About File
 * @File: Shop.cpp
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
 *    threads.  In this program, a barber shop has nWating chairs, nService 
 *    chairs
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
#include <iostream> //cout
#include <sstream>  //stringstream
#include <unistd.h> //usleep
#include <cmath>    //absolute value for print member
#include <string>   //string
#include "Shop.h"   //shop header

/* init
 * @Description: Initializes mutexes and conditions
 * @Pre: Shop is insantiated but conditions and mutexes are named and not init
 * @Post: all conditions and mutex init
 */
void Shop::init()
{
    //intialize mutex for Critical Sections (CS)
    pthread_mutex_init(&mutex_barber, NULL);           //for barber CS**********
    pthread_mutex_init(&mutex_customer, NULL);         //for customer CS********
    
    //initialize condition variables
    pthread_cond_init(&cond_paid, NULL);                //payment status
    pthread_cond_init(&customer_waiting_cond, NULL);    //for waiting customer
    barber_waiting_conds = new pthread_cond_t[nBarbers];//Barber conditions
    for(int i = 0; i < nBarbers; i++)
        pthread_cond_init(&barber_waiting_conds[i], NULL);

    //initialize barber service chair vector
    for(int i = 0; i < nBarbers; i++)
        who_is_in_my_service_chair[i] = -1;
}//end init

/* int2string
 * @Description: Takes an integer as an argument and turns it to a string
 * @Pre:An integer has been created
 * @Post: The integer is returned as a string
 * @Param: i is an integer to be converted to a string
 */
string Shop::int2string(int i)
{
    stringstream out;
    out <<i;
    return out.str();
}//end int2string

/* print
 * @Description: Takes an integer value and a string prints a corresponding 
 * message.  If number is negative prints barber, else customer.
 * @Pre: barber / customer integer created and message
 * @Post: Message passed to std out
 * @Param: person is an integer value to determine barber or customer
 * @Param: message is a string 
 */
void Shop::print(int person, string message)  
{
     cout << ((person > 0) ? "customer[": "barber [")
             <<abs(person) << "]: " <<message <<endl;
}//end print

/* visitShop
 * @Description: Takes in integer id and determines if the waiting chairs are
 * full in the shop.  If there is waiting room, id waits.  If a service chair is
 * open, id gets serviced and wakes the barber.
 * @Pre: Barber, waiting, and servicing chairs assigned
 * @Post: id is serviced or leaves, bool value passed
 * @Param: id is an integer representing a thread customer
 */
int Shop::visitShop(int customerId)
{
    pthread_mutex_lock(&mutex_customer);  //lock CS ****************************
   //speical case if max is 0
    if(max == 0){
       if (sleepingBarbers.empty())  //waiting chairs all occupied
       {
           print
           (customerId, " leaves the shop because of no available waiting chairs");
           nDropsOff += 1;  //add one customer that didn't get service

           pthread_mutex_unlock(&mutex_customer);  //unlock CS ********************
           return -1;
       }//end if all waiting chairs are full
    }//end if special case

    //regular case for max > 0
    if(max != 0){
       if (waiting_chairs.size() == max)  //waiting chairs all occupied
       {
           print
           (customerId, " leaves the shop because of no available waiting chairs");
           nDropsOff += 1;  //add one customer that didn't get service

           pthread_mutex_unlock(&mutex_customer);  //unlock CS ********************
           return -1;
       }//end if all waiting chairs are full
    }//end if special case

    //must add all customers to the queue for synchronization to work
    waiting_chairs.push(customerId);  //add customer id to waiting queue
     
    while (sleepingBarbers.empty())  //when all barbers are busy
    {
       //wait for signal from hellowCustomer ***********************************
        pthread_cond_wait(&customer_waiting_cond, &mutex_customer);
        print(customerId, " takes a waiting chair. # waiting seats available = "
             + int2string(max - waiting_chairs.size()));
       
    }//end while the barbers are all busy
    
    waiting_chairs.pop();  //cust leaves waiting_chair to being serviced

    //race condition for barbers exists this fixes it!!   
    pthread_mutex_lock(&mutex_barber);  //when barber avail lock CS ************
    
    int barberId = sleepingBarbers.front();  //get barberID
    sleepingBarbers.pop_front(); //barber is busy, remove from deque
    
    //Make condition for this customer to wait to be serviced
    //Shop doesn't know how many customers from driver due to SoW
    //Therefore, must create and init these here not in init!!!!
    pthread_cond_t cond_customer_waiting;
    pthread_cond_init(&cond_customer_waiting, NULL);

    //Store customer id and condition in hashmap
    hmap_cust_wait_conds.insert
       (make_pair(customerId, cond_customer_waiting));

    //Store customer id in this barber's service chair
    who_is_in_my_service_chair[barberId] = customerId;

    print(customerId, " moves to a service chair["
       +int2string(barberId)+ "], # waiting seats available = " 
       + int2string(max - waiting_chairs.size()));
    
    //customer signals barberId to cut hair
    pthread_cond_signal(&barber_waiting_conds[barberId]);//to helloCustomer ***

    //leave the barber and customer critical sections
    pthread_mutex_unlock(&mutex_barber);
    pthread_mutex_unlock(&mutex_customer);
    return barberId;
}//end visitShop

/* leaveShop
 * @Description: Takes a thread id and waits for a haircut, recieves haircut and
 * pays, saying goodbye.
 * @Pre: Shop created, customer id created
 * @Post: Customer id serviced
 * @Param: id is an integer representing a customer thread
 */
void Shop::leaveShop(int customerId, int barberId)
{
    pthread_mutex_lock(&mutex_barber);  //lock barber CS ***********************
    
    print(customerId, " wait for barber[" 
       + int2string(barberId) + "] to be done with hair-cut." );
    
    //wait for barber to signal they are done from byeCustomer *****************
    while(who_is_in_my_service_chair[barberId] == customerId)
    {
        pthread_cond_wait
           (&hmap_cust_wait_conds[customerId], &mutex_barber);
    print(customerId," says good-by to barber["+ int2string(barberId) +"]");
    money_paid = true;
    pthread_cond_signal(&cond_paid);
    who_is_in_my_service_chair[barberId] = -1; //set no customer in my chair 
    }//end wait till barber done    

    pthread_mutex_unlock(&mutex_barber); //unlock barber CS *******************
}//end leave shop

/* helloCustomer
 * @Description: Checks if no customers, then barber sleeps, else customer sits
 * down and barber starts haircut.
 * @Pre: Barber and shop created
 * @Post: If customer exists, hair cut starts
 */
void Shop::helloCustomer(int barberId)
{
    pthread_mutex_lock(&mutex_barber);  //lock barber CS ***********************
    
    //Shop has no more customers waiting    
    if (waiting_chairs.empty() && who_is_in_my_service_chair[barberId] == -1)
    {
        print(-barberId, " sleeps because of no customers.");
    }//end if no customers
   
    //loop while this barber has no customers
    while (who_is_in_my_service_chair[barberId] == -1) //I have no customers
    {
        bool assigned_state = false;   //create boolean state for if barber is busy
        
        //use iterator and search to see if barber assigned to customer
        for(deque<int>::iterator it = sleepingBarbers.begin(); 
              it != sleepingBarbers.end(); it++)
        {
            if (*it == barberId)  //barber assigned a customer
            {
                assigned_state = true;  //barber is not sleeping
                break;
            }//end if barberId is in deque
        }//end check if customer sit down in my chair
        
        //The first if statement needs barber to be put into deque else
        //hangs... so, lets make sure barberis in deque
        if (!assigned_state)  //put barberId back in deque if not sleeping
        {
            sleepingBarbers.push_back(barberId);
        }//end reset

        //signal the customer that is waiting for barber to get service
        pthread_cond_signal(&customer_waiting_cond);// signal to visitShop *****
           
        //wait for customer to signal they are waiting -- from visitShop *******
        pthread_cond_wait(&barber_waiting_conds[barberId], &mutex_barber);
    }//end while i have no customers
    
    print(-barberId, " starts a hair-cut service for customer[" 
       + int2string(who_is_in_my_service_chair[barberId]) +"].");
    pthread_mutex_unlock(&mutex_barber);  //unlock barber CS *******************
}//end helloCustomer

/* byCustomer
 * @Description: completes the hair cut process for one customer and calls
 * for another customer
 * @Pre:Customer is receiving haircut
 * @Post: Customer leaves, new if there comes in for service
 */
void Shop::byeCustomer(int barberId)
{
    pthread_mutex_lock(&mutex_barber);  //lock barber CS ***********************
    
    money_paid = false;  //need to synchronize exit messages with leaveShop ****
     
    //send signal to customer sitting in my chair  in leaveShop ****************
    pthread_cond_signal
       (&hmap_cust_wait_conds[who_is_in_my_service_chair[barberId]]);
    
    print(-barberId, " says he's done with a hair-cut service for customer["
       + int2string(who_is_in_my_service_chair[barberId]) + "].");
    
    //Barber prints his message goodbye before customer  synch with leaveShop***
    while(money_paid == false)
       pthread_cond_wait(&cond_paid, &mutex_barber);
    
    sleepingBarbers.push_back(barberId); //put barber into sleepingBarbers
    who_is_in_my_service_chair[barberId] = -1; //set no customer in my chair 
    print(-barberId, " calls in another customer.");
    pthread_cond_signal(&customer_waiting_cond); //signal leaveShop ************
    pthread_mutex_unlock(&mutex_barber);  //unlock barber CS *******************
}//end byeCustomer

