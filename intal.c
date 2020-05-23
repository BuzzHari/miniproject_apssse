/* 
   Written by: Hari Om - PES1201700250.

   Simple Big Integer library with few extra funcitons.
   
   This library is memory leak free--wew.

 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "intal.h"

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))


// Frequently used digits. Actually needed it for zero, one and two only. lol
static const char zero[2]     = "0";
static const char one[2]      = "1";
static const char two[2]      = "2";
/*
 *static const char three[2]    = "3";
 *static const char four[2]     = "4";
 *static const char five[2]     = "5";
 *static const char six[2]      = "6";
 *static const char seven[2]    = "7";
 *static const char eight[2]    = "8";
 *static const char nine[2]     = "9";
 *
 */

static void quick_sort(char **arr, int l, int n);
static int quick_sort_partition(char **arr, int l, int n);
static char* strip_zeros(char* intal, int len);

// Helper function to strip zeros from the front.
static char* strip_zeros(char* intal, int len) {
    
    if (!intal) {
        printf("Internal Error:: strip_zeros - intal empty");
        exit(0);
    }
     
    // counting number of leading zeros.
    int i = 0;
    while(i < len && intal[i] == '0')
        ++i;
    
    char *temp;
    //printf("i: %d\n len: %d\n", i, len);
    // No leading zeros.
    if (i == 0 )
        return intal;
    // All zeros.
    if (i == len) {
        temp = strdup(zero);
    }
    else {
        int new_len = len - i;
        temp = (char*) malloc(sizeof(char) * new_len + 1);
        strcpy(temp, intal+i);
    }

    free(intal);
    
    return temp;
}

char* intal_add(const char* intal1, const char* intal2) {

    if ( intal1 == NULL || intal2 == NULL ) {
        printf("Intal1 or Intal2 is empty\n");
        exit(0);
    }

    int len_1 = strlen(intal1);
    int len_2 = strlen(intal2);
    
    int max_len = MAX(len_1, len_2);
    
    // Allocating memory for the result.
    // +2 to accomodate the '\0' character.
    char *result = (char*) malloc((max_len+2) * sizeof(char));
    if (!result) {
        printf("Malloc Error\n");
        exit(0);
    }
   
    int i = len_1 - 1;  // Index for intal1.
    int j = len_2 - 1;  // Index for intal2.
    int k = max_len;    // Index for intal3.
    int temp = 0; 

    while( i >= 0 && j >= 0) {
        // intal1[i] + intal2[j] + carry.
        temp = (intal1[i] - '0') + (intal2[j] - '0') + temp/10;
        // storing the one's digit into result.
        result[k--] = (temp%10) + '0';
        --i; --j;
    }
    // If Intal1's len is greater.
    while ( i > -1) {
        temp = (intal1[i] - '0') + temp/10;
        result[k--] = (temp%10) + '0';
        --i;
    } 
    // If Intal2's len is greater. 
    while ( j > -1 ) {
        temp = (intal2[j] - '0') + temp/10;
        result[k--] = (temp%10) + '0';
        --j;
    }

    // Adding '\0'.
    result[max_len+1] = '\0';
    
    // Handling the extra carry case.
    if (temp / 10) {
        result[k--] = (temp / 10) + '0';

    }
    // Removing the leading '0' if there.
    else {
        char *temp = (char*) malloc(sizeof(char) * (max_len+1));
        strcpy(temp, result+1);
        free(result);
        result = temp;

        // Adding '\0'.
        result[max_len] = '\0';
    }

    return result;
}

int intal_compare(const char* intal1, const char* intal2) {
     
    if ( intal1 == NULL || intal2 == NULL ) {
        printf("Intal1 or Intal2 is empty\n");
        exit(0);
    }
    
    int len_1 = strlen(intal1);
    int len_2 = strlen(intal2);
    
    // Checking lengths.
    if (len_1 > len_2)
        return 1;
    else if (len_1 < len_2)
        return -1;
    else {
        // Lenghts equal, checking individual digits.
        for (int i = 0; i < len_1; ++i){
            if (intal1[i] > intal2[i])
                return 1;
            else if (intal1[i] < intal2[i])
                return -1;
        }
    }
    // They are equal.
    return 0;
}

char* intal_diff(const char* intal1, const char* intal2) {
    
    if ( intal1 == NULL || intal2 == NULL ) {
        printf("Intal1 or Intal2 is empty\n");
        exit(0);
    }

    int len_1 = strlen(intal1);
    int len_2 = strlen(intal2);
    int max_len = MAX(len_1, len_2);

    int cmp = intal_compare(intal1, intal2);
    char *result;    
    const char *big_int, *small_int;
    
    // Equal case. 
    if (cmp == 0) {
        result = strdup(zero);
        return result;
    }
    else if (cmp == 1) {
        // Intal1 greater.
        result = (char*) malloc(sizeof(char) * (len_1+1));
        big_int = intal1;
        small_int = intal2;
    }
    else {
        // Intal2 greater.
        result = (char*) malloc(sizeof(char) * (len_2+1));
        big_int = intal2;
        small_int = intal1;
    }
    
    if (!result) {
        perror("Malloc Error\n");
        exit(0);
    }

    int i = max_len - 1;            // Index for big_int.
    int j = MIN(len_1, len_2) - 1 ; // Index for small_int.
    int k = max_len - 1;            // Index for result.

    // Carry flag to simulate carry over without 
    // actually modifying the actual intals.
    int carry_flag = 0, temp;

    //printf("%s %s\n", big_int, small_int);
    
    while( j > -1  && i > -1) {
        temp = big_int[i] - '0';
        
        // If 1 was borrowed from this digit,
        // then subtract 1 from temp 
        // and reset the carry flag.
        if (carry_flag) {
            temp -= 1;
            carry_flag = 0;
        }
        
        // Add +10 to simulate the borrow, and
        // set the carry flag.
        if (temp < (small_int[j] - '0')) {
            temp += 10;
            carry_flag = 1;
        }
        
        // Perform digit subtraction and store.
        result[k--] = (temp - (small_int[j] - '0')) + '0';
        
        --i; --j; 
    }
    
    // Case where big_int is greater in length too.
    while( i > -1) {
        temp = big_int[i] - '0';
        
        if (carry_flag) {
            temp -= 1;
            carry_flag = 0;
        }

        // Add +10 to simulate the borrow, and
        // set the carry flag.
        if (temp < 0) {
            temp += 10;
            carry_flag = 1;
        }
        
        // Perform digit subtraction and store.
        // Subtracting 0, as small_int is over.
        result[k--] = temp + '0';
        --i;
    }
    
    // Adding '\0' to end.
    result[max_len] = '\0';
    // Striping zeros from the front if there.
    result = strip_zeros(result, max_len);
    return result;
}

char* intal_multiply(const char* intal1, const char* intal2) {

    if ( intal1 == NULL || intal2 == NULL ) {
        printf("Intal1 or Intal2 is empty\n");
        exit(0);
    }

    int len_1 = strlen(intal1);
    int len_2 = strlen(intal2);
    int max_len = MAX(len_1, len_2);

    char *result;

    // Handling the zero case.
    if ((len_1 == 1 && intal1[0] == '0') || (len_2 == 1 && intal2[0] == '0')) {
        result = strdup(zero);
        return result;
    }
    
    // Handling the '1' case.
    if (len_1 == 1 && intal1[0] == '1'){
        result = strdup(intal2);
        return result;
    }
    if (len_2 == 1 && intal2[0] == '1') {
        result = strdup(intal1);
        return result;
    }
    
    const char *big_int, *small_int;

    // Assiging big_int and small_int, according to len.
    if (len_1 >= len_2) {
        // Intal1 greater.
        result = (char*) malloc(sizeof(char) * (2*len_1+1));
        memset(result, '0', 2*len_1+1);
        big_int = intal1;
        small_int = intal2;
    }
    else {
        // Intal2 greater.
        result = (char*) malloc(sizeof(char) * (2*len_2+1));
        memset(result, '0', 2*len_2+1);
        big_int = intal2;
        small_int = intal1;
    }

    int i = max_len - 1;           // Index of big_int.
    int j_min = MIN(len_1, len_2) - 1, j; // Indexs of small_int.
    int k = 2*max_len - 1;         // Index of result.
    
    int temp = 0;
    int carry = 0;
    /*printf("BigInt: %s\n",big_int);*/
    /*printf("SmallInt: %s\n",small_int);*/
    /*printf("i: %d\n", i);*/
    /*printf("j_min: %d\n", j_min);*/
    /*printf("k: %d\n", k);*/
    // Implementing the naive paper pen method of multiplication.
    // O(n^2) time complexity.
   for (i = max_len - 1; i >= 0; --i ) {
        carry = 0;
        k = max_len + i;
        for (j = j_min; j >= 0; --j) {
            temp = (result[k]-'0') + ((big_int[i]-'0')*(small_int[j]-'0')) + carry;
            result[k] = (temp % 10) + '0';
            carry = temp / 10;
            k -= 1;
        }
        result[k] = ((result[k] - '0') + carry) + '0';
    }

    result[2*max_len] = '\0';
    //printf("reults: %s\n", result);
    result = strip_zeros(result, 2*max_len+1);
    //printf("reult_2: %s\n", result);
    return result;
}

char* intal_mod(const char* intal1, const char* intal2) {

    if ( intal1 == NULL || intal2 == NULL ) {
        printf("Intal1 or Intal2 is empty\n");
        exit(0);
    }

    //int len_1 = strlen(intal1);
    //int len_2 = strlen(intal2);
    
    char *result;
    
    //Zero Case.
    if(!strcmp(intal1,zero)) {
        result = strdup(zero);
        return result;
    }

    //Intal1 lesser than Intal2
    if(intal_compare(intal1,intal2) == -1) {
        result = strdup(intal1);
        return result;
    }
    
    // Creating duplicates, as we would
    // need to modify these numbers.
    // Don't forget to free them lollol.
    char *dup_intal1 = strdup(intal1);
    char *dup_intal2 = strdup(intal2);
    // Just a temp, to avoid to those memory leaks.
    char *temp;      
    // Idea is to subtract intal1 = intal1 * 2, from 
    // intal1, till it's smaller than intal2.
    while( intal_compare(dup_intal1, intal2) >= 0   ) {
        
        //printf("dup_intal1 %s\n", dup_intal1);
        //printf("dup_intal2 %s\n", dup_intal2);

        temp = dup_intal1; 
        dup_intal1 = intal_diff(dup_intal1, dup_intal2);
        // Deallocating the old dup_intal1.
        free(temp);

        temp = dup_intal2;
        dup_intal2 = intal_multiply(dup_intal2, two);
        //printf("dup_intal2 * 1 %s\n", dup_intal2);
        // Deallocating the old dup_intal2.
        free(temp);
    
        // If after multipling with 2, dup_intal2 becomes greater than
        // dup_intal1 but dup_intal1 is still greater than intal2, then
        // reset dup_intal2 to intal2.
        if ( intal_compare(dup_intal1, dup_intal2) == -1 &&
             intal_compare(dup_intal1, intal2) == 1 ) {

            temp = dup_intal2;
            dup_intal2 = strdup(intal2);
            //printf("dup_intal2 after reseting %s\n", dup_intal2);
            free(temp);
        }
    }
    
    free(dup_intal2);

    // This is final result.
    return dup_intal1;
}

char* intal_pow(const char* intal1, unsigned int n) {
    
    if ( intal1 == NULL ) {
        printf("Intal1 is empty\n");
        exit(0);
    }

    //int len_1 = strlen(intal1);
   
    // Intal1 = zero case.
    if (!strcmp(intal1, zero)) {
        char *result = strdup(zero);
        return result;
    }

    // N = 1 and intal1 = 1case.
    if (n == 1 || strcmp(intal1,one) == 0) {
        char *result = strdup(intal1);
        return result;
    }
    
    // Spliting 'n' by dividing by 2 and then taking pow.
    // a^b where b is even.
    if (n % 2 == 0) {
        char *temp = strdup(intal1);
        char *temp2 = intal_multiply(temp, temp);
        char *result = intal_pow(temp2, n/2);
        free(temp);
        free(temp2);
        
        return result;
    }
    else {
        // (a^b-1)*a where b is odd.
        char *temp = strdup(intal1);
        char *temp2 = intal_multiply(temp, temp);
        char *temp3 = intal_pow(temp2, n/2);
        char *result = intal_multiply(temp3, temp);
        free(temp);
        free(temp2);
        free(temp3);

        return result;
    }
}


char* intal_gcd(const char* intal1, const char* intal2) {
    
    if ( intal1 == NULL || intal2 == NULL ) {
        printf("Intal1 or Intal2 is empty\n");
        exit(0);
    }
    // intal1 = 0 and intal2 = 0 case.
    if (strcmp(intal1, zero) == 0 && strcmp(intal2, zero) == 0) {
        char *result = strdup(zero);
        return result;
    }

    if (strcmp(intal1, zero) == 0) {
        char *temp = strdup(intal2);
        return temp;
    }

    char *temp = intal_mod(intal2, intal1);
    char *result = intal_gcd((const char*)temp, intal1);

    free(temp);
    return result;
}


char* intal_fibonacci(unsigned int n) {
    
    // Intialiazing with '0', '1';
    char *prev = strdup(zero);
    char *curr = strdup(one);

    if (n == 0) {
        free(curr);
        return prev;
    }
    if (n == 1 || n == 2) {
        free(prev);
        return curr;
    }
    
    char *temp;
    for (unsigned i = 2; i <= n; ++i) {
        
        temp = prev; 
        prev = curr;
        curr = intal_add(temp, curr);
        free(temp);

    }
    
    free(prev);

    return curr;
}

char* intal_factorial(unsigned int n) {
    
    // 0! = 1.
    if (n == 0) 
        return strdup(one);

    char *result = strdup(one);
    char *multiplier = strdup(one); 
    char *temp, *temp2;
    while(n > 1) { 

        temp = result;
        temp2 = multiplier;
        multiplier = intal_add(multiplier, one); 
        free(temp2);
        result = intal_multiply(result, multiplier);
        free(temp);
        --n;
    }
    
    free(multiplier);
    return result;
}

char* intal_bincoeff(unsigned int n, unsigned int k) {
    
    // Returning 0, even though it's not mathematically defined.
    if (k > n) {
        printf("Invalid Input\n");
        exit(0);
    }

    if (n == 0)
        return strdup(zero);


    // C(0,0) and C(n,n)
    if ((n == 0 && k == 0) || n==k)
        return strdup(one);

    if (k > n/2)
        k = n - k;
    
    char **result = (char**) malloc(sizeof(char*) * (k+1));
    for (unsigned int i = 1; i < k+1; ++i) {
        result[i] = strdup(zero);
    }
    
    
    unsigned int temp;
    char *temp2;
    result[0] = strdup(one);
    for(unsigned int i = 0; i <= n; ++i) {
        if (i < k)
            temp = i;
        else
            temp = k;

        while(temp > 0) {
            temp2 = result[temp];
            result[temp] = intal_add(result[temp], result[temp-1]);
            free(temp2);
            --temp;
        }
    }
    // Freeing all allcoated memory.
    // ZERO-MEMORY LEAK- THAT'S THE WAY!!!
    char *final_result = strdup(result[k]);
    for(unsigned int i = 0; i < k+1; ++i) 
        free(result[i]);
    free(result);
    return final_result;
}

int intal_max(char **arr, int n) {

    if (arr == NULL) {
        printf("Arr Empty\n");
        exit(0);
    }

    int offset = -1;
    char *curr_max = arr[0];

    
    for (int i = 0; i < n; ++i) {
        if (intal_compare(curr_max, arr[i]) == -1) {
            curr_max = arr[i];
            offset = i;
        }
    }
    return offset;
}

int intal_min(char **arr, int n) {

    if (arr == NULL) {
        printf("Arr Empty\n");
        exit(0);
    }

    int offset = -1;
    char *curr_min = arr[0];

    
    for (int i = 0; i < n; ++i) {
        if (intal_compare(curr_min, arr[i]) == 1) {
            curr_min = arr[i];
            offset = i;
        }
    }
    return offset;
}

int intal_search(char **arr, int n, const char* key) {

    if (arr == NULL) {
        printf("Arr Empty\n");
        exit(0);
    }
    
    for (int i = 0; i < n; i++) {
        if (strcmp(key,arr[i]) == 0)
            return i;
    }
    return -1;
}

int intal_binsearch(char **arr, int n, const char* key) {

    if (arr == NULL) {
        printf("Arr Empty\n");
        exit(0);
    }
    // Implementing iterative binsearch. 
    int left = 0, right = n-1;
    int offset = -1;

    while (left <= right) {
        int mid = left + (right-left)/2;
        int cmp = intal_compare(arr[mid], key);
        
        // If equal set offset to mid.
        // And go further left to check for
        // duplicates.
        if (cmp == 0) {
            offset = mid;
            // the number on the left isn't same,
            // no point looking further..return offset.
            if (intal_compare(arr[mid], key) != 0)
                return offset;
            right = mid - 1;
        }
        // If smaller than key.
        // Go right.
        // else go left.
        if (cmp == -1)
            left = mid + 1;
        else 
            right = mid - 1;

    }
    return offset;
}

static int quick_sort_partition(char **arr, int l, int n) {
    
    char *pivot = arr[n];

    int i = l-1;

    for(int j = l; j <= n-1; ++j) {
        int cmp = intal_compare(pivot, arr[j]);
        // if arr[i] <= pivot
        if( cmp == 0 || cmp == 1 ) {
            ++i;
            // swapping.
            char* temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    // Swapping the last elment into the right postion.
    char* temp = arr[i+1];
    arr[i+1] = arr[n];
    arr[n] = temp;

    return (i+1);
}

static void quick_sort(char **arr, int l, int n) {
    if(l<=n) {
        int split = quick_sort_partition(arr, l, n);
        quick_sort(arr, l, split-1);
        quick_sort(arr, split+1, n);
    }
}

void intal_sort(char **arr, int n) {
    if (arr == NULL) {
        printf("Arr Empty\n");
        exit(0);
    }
    
    // Implementing Quick Sort - O(n log n) algorithm.
    quick_sort(arr, 0, n-1);
    
}

char* coin_row_problem(char **arr, int n) {

    if (arr == NULL) {
        printf("Arr Empty\n");
        exit(0);
    }

    char *prev = strdup(zero);
    char *curr = strdup(arr[0]);
    char *temp; 
    char *sum;
    for (int i = 1; i < n; ++i) {
        sum = intal_add(arr[i], prev);
        int cmp = intal_compare(curr, sum);

        if(cmp == 1) {
            free(prev);
            prev = strdup(curr);
            if(sum)
                free(sum);
        }
        else {
            temp = prev;
            prev = curr;
            curr = sum;
            if(temp)
                free(temp);
        }
        
    }
    free(prev);
    return curr;
}



