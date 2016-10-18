/**
 * @file sliding_filter.c
 * @brief Sliding filter source.
 *
 */




#include <math.h>
#include <stdlib.h>

#include "sliding_filter.h"



void init_sliding_filter( sliding_filter_params_s * filter, int sliding_array_size )
{
    filter->slidingFilterArray = malloc( sizeof( double ) * sliding_array_size );
    
    filter->slidingFilterArraySize = sliding_array_size;
    
    filter->numInputsToArray = 0;
}


void delete_sliding_array( sliding_filter_params_s * filter )
{
    free( filter->slidingFilterArray );
}


double input_to_sliding_filter( sliding_filter_params_s * filter, double input )
{
    int currentArrayIndex = 
            filter->numInputsToArray % filter->slidingFilterArraySize;
    
    filter->slidingFilterArray[ currentArrayIndex ] = input;
    
    int currentArraySize = 
            fmin( filter->numInputsToArray, filter->slidingFilterArraySize );
    
    double currentAvg = 0;
    
    for( int i = 0; i < currentArraySize; i++ )
    {
        currentAvg += filter->slidingFilterArray[ i ];
    }
    
    filter->numInputsToArray++;
   
    return currentAvg/currentArraySize;
}