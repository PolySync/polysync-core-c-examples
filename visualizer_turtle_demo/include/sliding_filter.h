/* 
 * File:   sliding_filter.h
 * Author: lbuckland
 *
 * Created on September 15, 2016, 1:49 PM
 */

#ifndef SLIDING_FILTER_H
#define SLIDING_FILTER_H


typedef struct
{
    //
    //
    int slidingFilterArraySize;
    //
    //
    int numInputsToArray;
    //
    //
    double * slidingFilterArray;
} sliding_filter_params_s;


void init_sliding_filter( sliding_filter_params_s * filter, int slidingArraySize );

void delete_sliding_array( sliding_filter_params_s * filter );

double input_to_sliding_filter( sliding_filter_params_s * filter, double input );


#endif /* SLIDING_FILTER_H */

