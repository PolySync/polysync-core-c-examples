/*
 * Copyright (c) 2016 PolySync
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file sliding_filter.h
 * @brief Simple sliding average filter.
 *
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



/**
 * @brief Initialize the sliding filter. 
 *
 * @param [out] filter Pointer to sliding filter.
 * @param [in] sliding_array_size Size of sliding array.
 *
 */
void init_sliding_filter( sliding_filter_params_s * filter, const int sliding_array_size );

/**
 * @brief Delete the sliding filter. 
 *
 * @param [out] filter Pointer to sliding filter.
 *
 */
void delete_sliding_array( sliding_filter_params_s * filter );

/**
 * @brief Input value to sliding filter. 
 *
 * @param [out] filter Pointer to sliding filter.
 * @param [in] input Input to add to sliding filter.
 * 
 * @return Result of doing a sliding average on the sliding filter array.
 *
 */
double input_to_sliding_filter( sliding_filter_params_s * const filter, const double input );




#endif /* SLIDING_FILTER_H */

