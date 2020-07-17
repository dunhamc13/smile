/* Notes About File
 * @File: Smile.cpp
 * @Name: Sophia He, Melinda Johnson, Christian Dunham
 * @Date: 17July2020
 * @Program Name: Smile
 *
 * Program Purpose:
 *
 * Design Rational:  
 *
 * Dynamic Memory:
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
 *                 1) 
 *
 *******************************************************************************
 *
 *******************************************************************************
 * Code Outline :
 *
 *                Driver    : main()
 *                Smile     : init()
 *
 *******************************************************************************
 *
 *                        Included Libraries
 *
 *******************************************************************************
 *******************************************************************************
*/
#include <iostream> //cout
#include <string>   //string
#include "Smile.h"   //Smile header

/* print
 * @Description:
 * @Pre:
 * @Post: 
 * @Param:
 */
void Smile::print(int person, string message)  
{
     cout << ((person > 0) ? "customer[": "barber [")
             <<abs(person) << "]: " <<message <<endl;
}//end print
