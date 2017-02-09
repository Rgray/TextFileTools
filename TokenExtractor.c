/************************************************************************/
/*                                                                      */
/*  Name:       Reid Gray                                               */
/*															            */
/*  Class:      Data Structures											*/
/*                                                                      */
/*  Date:       8 April 2000                                            */
/*                                                                      */
/*  General description of the program:                                 */
/*  ----------------------------------- 
/* - Prompts user for an input file.
/* - Any ASCII text file can be used for input if there is one word
/*   per line.
/* - Reads the data from the input file and stores it a the HASH table.
/* - Produces a report for each non-empty cell in the HASH table and
/*   while listing all reserved words that occur in each chain.
/* - Handles collisions using quadratic probing. 
/* - When probing fails, chaining results.
/* - Has a search engine and prompts user to find a reserved word 
/*   in the HASH table.
/* 
/*  Description of the hash algorithm:                                       
/*  -----------------------------------
/*  A simple hash function that adds (8 + number of chars) factored
/*  by the first character and added to the last character weights the
/*  differences between the words sufficiently to have only about 6
/*  repeats when hash size is 37 (quadratic probing disabled).
/*  So I used this hash function as my 'first time' hash.  It also
/*  provides a satisfactory spread. I came up with this method through
/*  the honorable, trial and error way, although some one at work offered
/*  to show me the JRE code 'perfect hash' solution for this. Right now, 
/*  I'm quite curious how they do it.
/* 
/*  If this function does not come up with a free cell on the first
/*  try, I employ quadratic probing.
/*
/*  If quadratic probing fails, I revert to chaining.
/*
/*  To find an item I employ almost the same sequence of steps.  
/*  -First, I look for the item in the most obvious place (simple hash index)
/*  -Then, I check the chain because it is a quick solution with only thirty
/*   two words.
/*  -Then I step back quadratically.
/*
/*  Description of the functions:                                       
/*  ----------------------------    
/* findHashEntry()                         
/* Finds a 'hash entry' (a node that was stored by way of
/* the hashing algorithms of this program) and returns 1, or returns an 
/* indication that the entry does not exist in the table (0).
/*
/* The following cases return 1:
/* - The item is found where expected (first hashing of the key renders
/*   the index).
/* - The item found in the list pointed to by the index.  In other words,
/*   due to a collision, it was chained.
/* - The item was found in a rehashed index by stepping through the 
/*   probing algorithm.
/* 
/* The following case retuns 0:
/* The function is not found in the expected location, and it is not
/* found in the chained list of the expected location, and it is not
/* found elsewhere when stepping through each of the possible other
/* indices using the probing algorithmn.
/*
/* This function calls hashKey(), sequentialSearch(), findRehashKey().
*/
/* sequentialSearch()
/* Searches list for target item until the end list indicated by NULL.
/* 
/* -Expects the calling function to pass it a pointer to the head 
/*  of the list and item to search for.
/* -If successful, this function returns 1.
/* -If the item is not found, this function returns 0.
/* -It has no side effects.
*/
/* hashKey()
/* -hashes a character to string to an integer hash table index.
/* -expects the caller pass an ascii string and make the hash table
/*  size available by way of a macro.
/* -Adds the ascii integer equivalents to the last char in the string
/*  Adds it to the factor of the number of letters in the string + 8
/*  and the last character's ascii equivalent. 
/* 
/* It passes this key hashed as an index to the caller.
*/
/* hashKeyQuad()
/* - Recalculates the key quadratically.
/* - Expects the calling function will pass the hashed key (of type int)
/*   from on the first (unsuccessful) attempt to find an empty bucket. 
/* - Returns a new key so that the caller can use the key to make a step 
/*   through the hash table quadratically.
/* - Expects that the integer size of the hash table will be 
/*   available by way of a macro.
*/ 
/* initHashTable();
/* - Initializes the buckets in the hashtable to NULL.
/* - Expects the calling function will pass the address array of pointers.
/* - Nothing is returned.
*/
/* processInputFile()
/* - Reads the reserved words from the input file (one word per line)
/* - Makes a node for the linked list.
/* - Adds nodes to the linked list if not previously entered. 
/* - Expects address to array of pointers (hash table) and an
/*   input file name.
/* Calls findHashEntry() and addHashEntry()       
*/
/* addHashEntry()
/* - Gets an appropriate key and insert the node into a any one of 
/*   the lists pointed to by the hash table. 
/* - Resolves collisions through a call to rehashKey()
/* Expects the caller to pass to it:
/*  - An array of pointers, or that is to say, a "hash table."
/*  - A pointer to a new node containing arbitrary data.
/*
/* Calls hashKey(), rehashKey()
*/
/* rehashKey()
/* Performs a quadratic search for an empty bucket until either the 
/* empty bucket is found or an overflow indication is detected.
/*
/* If an overflow indication is detected, this function will return 
/* -1 back to the caller to indicate that the search for an empty
/* bucket failed.
/* 
/* If the function finds an empty bucket, the key for that bucket is 
/* return to the caller.
/*  
/* This function expects to be passed:
/* - A empty, full, or partially full hash table.
/* - Any key for that hash table.
/* Calls hashKeyQuad()
*/
/* findRehashKey()
/* Uses quadratic probing to backtrack 
/* over the buckets to find a key not in the expected (hashed) position.
*/
/* makeNode()
/* Makes a new node for the linked list and pass the pointer
/* back to caller.  If an error occurs during space allocation
/* print an appropriate message and exit from the program.
/*
/* Expects the caller pass it a character array that it
/* can store in the newly allocated node.
/*
/* If the char array is real data and there is enough memory for
/* the node, it initializes the next ptr to NULL and returns
/* a pointer to the node to the caller.
*/

/* printHashEntries()
/* Expects the caller to pass it the address of the hash table.
/* It returns nothing.
/* 
/* Step through the hash table and display any cell's linked list 
/* entries when the cell contains a nonNULL value. 
/* 
/* It expects the caller pass it the address of an array of pointers 
/* to structures, i.e., a hash table.
/* 
/* queryHashTable()
/* -Prompts the user for search criteria
/* -Searches for the specified string.
/* -Prints the results to the screen.
/*
*/

#define HASHSIZE  67

typedef struct node {
        char   *line_text;		/* For variable length lines from any file */
        struct node *next_ptr;  /* For singly linked list. */
} NODE_ENTRY;

typedef NODE_ENTRY *NODE_PTR;

typedef NODE_PTR HASH_TAB[HASHSIZE];

/** Pre-processor definitions
 ****************************/

/* Defines the maximum number of characters that can make up a
 * line. Defines character array size used to handle and store 
 * lines from input file.
 */
#define MAXARRAY  80

/** Function prototypes
 ***********************/

int /* Hashes key to an integer value */
hashKey(char *);

int /* Recalculates key index for quadratic probing method */ 
hashKeyQuad(int);

int /* Probes table until empty cell is found. Passes back index value */
rehashKey(int, HASH_TAB);

int /* Finds empty cell to add a node to the table based on algorithm */
findRehashKey(int, HASH_TAB, char *);

void /* Initializes hash table so all buckets are empty */
initHashTable(HASH_TAB);

void /* Adds a node to the table */
addHashEntry(HASH_TAB, NODE_PTR);

void /* Prints non-empty hash table cells */
printHashEntries(HASH_TAB);

int /* Finds a string in hash table; returns true or false indicator */
findHashEntry(HASH_TAB, char *);

int /* Walks chains searching entries for a specified string */
sequentialSearch(NODE_PTR, char *);  

NODE_PTR  /* Allocates memory for a node structure */
node_alloc(void);

NODE_PTR /* Allocates memory for character array */
makenode(char *);

void /* Pulls lines from the input file and inserts them to hash table */
processInputFile(char *, HASH_TAB);

char * /* Creates a handle for the character array */
stringDup(char *);

void  /* Prints non-empty cells in the hashtable to file */
printNodeEntry(NODE_PTR);

void /* Allows user to query the hash table for a reserved word */
queryHashTable(HASH_TAB); 

void /* Prompts user for an input file */
getInputFile(char *);

/* Beginning of main() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Main():
/* - Prompts user for an input file.
/* - Reads the data from the input file and stores it a the HASH table.
/* - Produces a report for each non-empty cell in the HASH table and
/*   while listing all reserved words that occur in each chain.
/* - Handles collisions using quadratic probing. When probing fails, 
/*   chaining results.
/* - Has a search engine and prompts user to find a reserved word 
/*   in the HASH table.
 */
void 
main(void)
{ 
	HASH_TAB hash_tab;
	char input_filename[MAXARRAY]; 
	getInputFile(input_filename); 

	/** Initialize buckets to NULL... 
	 **/							
	initHashTable(hash_tab);

	/** Process the input file and make the hash table... 
	 **/
	processInputFile(input_filename, hash_tab); 
	
	printf("\n\n");

	/** Access buckets and visit chained nodes and print contents... 
	 **/
	printHashEntries(hash_tab);

	/** Find a reserved word as specified by the user... 
	 **/
	queryHashTable(hash_tab);

} /* End main. */

/* This function finds a 'hash entry' (a node that was stored by way of
/* the hashing algorithms of this program) and returns 1, or returns an 
/* indication that the entry does not exist in the table (0).
/*
/* The following cases return 1:
/* - The item is found where expected (first hashing of the key renders
/*   the index).
/* - The item found in the list pointed to by the index.  In other words,
/*   due to a collision, it was chained.
/* - The item was found in a rehashed index by stepping through the 
/*   probing algorithm.
/* 
/* The following case retuns 0:
/* The function is not found in the expected location, and it is not
/* found in the chained list of the expected location, and it is not
/* found elsewhere when stepping through each of the possible other
/* indices using the probing algorithmn.
/*
/* This function calls hashKey(), sequentialSearch(), findRehashKey ().
*/
int 
findHashEntry(HASH_TAB hash_tab, char *key)
{ 
	int i = 0;
	NODE_PTR target_node_ptr;
	
	/**  Get converted or "hashed" key from hash function...
	 **/
	i = hashKey(key);

	/**  Use hashed key as array index to get list pointer...
	 **/
	target_node_ptr = hash_tab[i]; 

	/** Check if bucket is NOT empty...
     **/
	if( target_node_ptr != NULL) {    
		
		/** Check if item matches the item in current bucket...  
		 **/
		if ( strcmp(target_node_ptr->line_text, key) == 0) 
			return 1;  /* Return match indicator...*/
		
		/** Check to see if item was placed on list due to overflow
		 ** condition...  
		 **/
		if ( sequentialSearch(target_node_ptr, key) == 1)
			return 1;	/* Return match indicator...*/
		
		else
			return 0;	/* Return new item indicator... */
	}

	if (target_node_ptr == NULL) {
		
		/** Check to see if item was placed in rehashed bucket
		 ** due to collision and the alternate item that mapped 
		 ** to the key (and caused the collision) has since been deleted.
		 **/
		if ( findRehashKey (i, hash_tab, key) == 1)
			return 1;

		else
			return 0;	/* Return Empty bucket indicator... */
	}
	
	else return 0;

} /* End findHashEntry. */

/* The purpose of this function is to search a list for a target item.
/* It searches the list until the end of the list is indicated by NULL.
/* 
/* It expects the calling function will pass it a pointer to the head 
/* of the list and search item.
/* 
/* If successful, this function returns 1.
/* 
/* If the item is not found, this function returns 0.
/*
/* It has no side effects.
*/
int 
sequentialSearch(NODE_PTR current_ptr, char *search_item)
{
	while (current_ptr != NULL) {
	
		if ( strcmp(current_ptr->line_text, search_item) == 0)
		return 1;
		
		current_ptr = current_ptr->next_ptr;
	}
	return 0;
}

/* This function hashes a character to string to an integer hash table
/* index.
/* It expects the caller pass it an ascii string and make the hash table
/* size available by way of a macro.
/* It adds the ascii integer equivalents to the last char in the string
/* and adds it to the factor of the number of letters in the string + 8
/* and the last character's ascii equivalent. 
/* 
/* It passes this key hashed as an index to the caller.
*/
int 
hashKey(char *key)
{ 
	int i;
	int value = 0;
	char *hold_key = key;  
	
	i = strlen(key);

	if (key != NULL) {
		value =  (key[i-1]) + key[0] * (i + 8) ;   /* A little this and that. */
	}

	return abs(value % HASHSIZE);  /* To bound values within the array. */
} /* End hash. */

/* This function recalculate the key quadratically.
/* It expects the calling funciton will pass the hashed key (of type int)
/* calculated on the first unsuccessful attempt to find an empty bucket. 
/* 
/* The function returns a new key so that the caller can use the key to
/* make a step through the hash table quadratically.
/*
/* This function expects that the integer size of the hash table will be 
/* available by way of a macro.
*/
int hashKeyQuad(int i)
{
	i = (i + (i * i)) % HASHSIZE;  /* Rehash based on original key */
	
	return i;
}

/* This function initializes the buckets in the hashtable to NULL.
/* It expects the calling function will pass the address array of pointers.
/* 
/* Nothing is returned.
/*
/* There are no side effects to calling this function.
*/
void 
initHashTable(HASH_TAB hash_tab)
{ 
	int i;
 
	for(i = 0; i <= HASHSIZE - 1; i++)     
		hash_tab[i] = NULL;

} /* End initHashTable. */

/* Read the reserved words from the input file ( one word per line), make a node
for the linked list, add the node to the linked list if the word has not been
previously entered.  */

void 
processInputFile(char *input_filename, HASH_TAB hash_tab )
{
	char file_buffer[MAXARRAY];
	FILE *fptr;     /* Pointer to input file */ 
	NODE_PTR node_ptr;
 
	/** Open input file. Test for good file name. 
	 **/
	if ((fptr = fopen(input_filename,"r")) == NULL) {
		printf("Can't find input file: %s\n",input_filename);
		exit(-1);    /* Stop processing. */ 
	}
 
	/** Read lines from input file until end of file
	 ** Limit line length to a specified size... 
	 ** Store lines in an array... 
	 **/
	while( fgets(file_buffer, MAXARRAY , fptr) != NULL ) {

		/**  Replace the the array's terminating <NL> with a null byte...
		 **/
		file_buffer[strlen(file_buffer) - 1] = '\0'; /* Replace cr with null */
		
		/** Call function to allocate memory and insert array into node...
		 ** Assign pointer to the new node...
		 **/
		node_ptr = makenode (file_buffer);
		
		/** If a node with the same key either does NOT already exist
		 ** --or does not match, drop our new node in...
		 **/
		if (!findHashEntry(hash_tab, node_ptr->line_text)) {
			addHashEntry(hash_tab, node_ptr); 
		}	
		else
			printf("\"%s\" has already been entered into the hash table\n",
		node_ptr->line_text); 
	} /* End while. */
} /* End  processInputFile. */

/* Add an entry to the hash table, if an entry already exists at that cell,
   then insert the new node first in the list since all reserved words in
   the input file are unique. */

/* This function adds an entry into the chained hash table.
/* 
/* It EXPECTS the caller to pass to it:
/*  - An array of pointers, or that is to say, a "hash table."
/*  - A pointer to a new node containing arbitrary data.
/* 
/* The purpose of this function is to get an appropriate key and
/* insert the node into a any one of the lists pointed too by the
/* hash table.  
*/
void 
addHashEntry(HASH_TAB hash_tab, NODE_PTR new_node_ptr)
{
	int h = 0; /* */
	int r; /* Rehashed key */
	char *key;
	
	NODE_PTR target_node_ptr;

	key = new_node_ptr->line_text;
	
	/**  Get converted or "hashed" key from hash function...
	 **/
	h = hashKey(key);

	/**  Use hashed key as array index to get intended list pointer...
	 **/
	target_node_ptr = hash_tab[h]; 

	/** Bucket EMPTY...
	 ** Assign the address of new node...
     **/
	if (target_node_ptr == NULL)     
		hash_tab[h] = new_node_ptr;
	
	/** Bucket NOT empty...
	 ** Rehash key...
     **/
	if (target_node_ptr != NULL) {
		r = rehashKey(h, hash_tab);
		
		/** If rehashKey indicates assumed overflow conditions exist
		 ** --add node to list using original key...
		 **/
		if (r == -1) {	
			target_node_ptr = hash_tab[h];
			new_node_ptr->next_ptr = target_node_ptr;
			hash_tab[h] = new_node_ptr;
		
		/** If rehashKey returns a key, an empty bucket was found...
		 ** Add node to the table using rehashed key...
		 **/
		} else    
			hash_tab[r] = new_node_ptr;
	}

} /* End addHashEntry. */

/* The purpose of this function to perform a quadratic search for an
/* empty bucket until either the empty bucket is found or an overflow
/* indication is detected.
/*
/* If an overflow indication is detected, this function will return 
/* -1 back to the caller to indicate that the search for an empty
/* bucket failed.
/* 
/* If the function finds an empty bucket, the key for that bucket is 
/* return to the caller.
/*  
/* This function expects to be passed:
/* - A empty, full, or partially full hash table.
/* - Any key for that hash table.
*/
int 
rehashKey(int h, HASH_TAB hash_tab)
{
	NODE_PTR target_node_ptr;

	int assume_over_flow = 0;

	target_node_ptr = hash_tab[h];

	while (target_node_ptr != NULL) {
		if ( assume_over_flow == (HASHSIZE + 1) / 2) {
			/** Indicate to caller probing failed to find empty bucket... 
			 **/
			return -1; 
		}
		
		h = hashKeyQuad(h); /* Rehash based on original key */
		assume_over_flow++;
		target_node_ptr = hash_tab[h]; /* Get pointer at new key position */
			
	}
	/** Return successful rehash to caller... 
	 **/
	return h;
}

/* The purpose of this function is to use quadratic probing to backtrack 
/* over the buckets to find a key not in the expected (hashed) position.
/*
*/
int 
findRehashKey(int h, HASH_TAB hash_tab, char *search_item)
{
	NODE_PTR target_node_ptr;

	int assume_all_buckets_searched = 0;
	int match = 0;

	target_node_ptr = hash_tab[h];

	while ( assume_all_buckets_searched != (HASHSIZE + 1) / 2) {

		while (target_node_ptr == NULL) {
			h = hashKeyQuad(h);				/* Rehash based on original key */
			assume_all_buckets_searched++;
			target_node_ptr = hash_tab[h]; /* Get pointer at new key position */
		
			if ( assume_all_buckets_searched == (HASHSIZE + 1) / 2) {
				/** Indicate to caller probing found no match... 
				 **/
				return 0; 
			}
		}
		
		if (target_node_ptr->line_text == search_item) {
			return 1; 
		}	
		
		h = hashKeyQuad(h);				/* Rehash based on original key */
		assume_all_buckets_searched++;
		target_node_ptr = hash_tab[h]; /* Get pointer at new key position */		
	}
	/** Search unsuccessful.  Indicate 'item not found' to caller... 
	 **/
	return 0;
}

/* Makes a new node for the linked list and pass the pointer
   back to caller.  If an error occurs during space allocation
   print an appropriate message and exit from the program.

   It expects the caller pass it a character array that it
   can store in the newly allocated node.

   If the char array is real data and there is enough memory for
   the node, it initializes the next ptr to NULL and returns
   a pointer to the node to the caller.

   This function has no side effects.
*/

NODE_PTR 
makenode(char *w)
{
	
	NODE_PTR ptr = NULL; 
	
	/** Get pointer to node and initialize terminal node...
	 **/
	if  ((ptr = node_alloc()) != NULL) { /* If memory exists. */
        ptr->next_ptr = NULL;            /* Initialize next to NULL. */
		
		/** Copy item to node and sanity check...
		 ** Check if string function returns valid argument...
		 **/
		if  ((ptr->line_text = stringDup(w)) == NULL) {
              printf("Error: Unable to allocate line text storage\n");
              exit(-1);        
		}  
	}  else {
        printf("Error: Unable to allocate linked node storage\n");
        exit(-1);  
	}  
	return ptr;  /* Which can't be NULL! */
} /* End makenode. */

/* Allocate memory for the node, cast pointer , return NULL if malloc fails. */
NODE_PTR 
node_alloc(void)
{
  return (NODE_PTR) malloc(sizeof(NODE_ENTRY));

}

/* Copy the string w to a safe place in memory, return the location
   pointer to caller.*/
char *
stringDup(char *w)
{  
	char *p;

	p = (char *)malloc(strlen(w) + 1); /* 1 added for null terminator. */
  
	if (p != NULL)      
		strcpy(p, w);  
	return p;
}

/* This function prompts user for an input file 
/* It expects a character array.
/* It returns nothing. 
*/
void 
getInputFile(char *input_filename)
{
 
	printf("Please type the name of the file you want hashed!\n");
	scanf("%s", input_filename);

} /* End getInputFile. */

  
/* Step through the hash table and display any cell's linked list 
/* entries when the cell contains a nonNULL value. 
/* 
/* It expects the caller pass it the address of an array of pointers 
/* to structures, i.e., a hash table.
/* 
/* It returns nothing.
/* 
/* It has no side effects.
*/

void 
printHashEntries(HASH_TAB hash_tab)
{ 

	FILE *output_fptr;
	int i = 0;
	int first_while_string_count = 0;
	NODE_PTR head_ptr;
	
	/** Open output file...
	 **/
	if ((output_fptr = fopen("output.txt", "w")) == NULL) {
		printf("Can't open output file\n"); 
		exit(1);
	}

	fprintf(output_fptr, "Table Index\tStored word(s)\n");
	fprintf(output_fptr, "===========\t==============\n");

	for (i = 0; i <= (HASHSIZE - 1); i++) {
		
		head_ptr = hash_tab[i];

		if ( head_ptr != NULL)
			fprintf(output_fptr, "\nAt address [%d]: ", i);

		while ( head_ptr != NULL) {	
			fprintf(output_fptr, "%s ", head_ptr->line_text);
			first_while_string_count++;
			head_ptr = head_ptr->next_ptr;
		}
	
	} /* End For */
	
	/* print trailer infomation */

	fprintf(output_fptr, "\n<p>Total words \t= %10d\nHASHSIZE\t= %10d", 
		first_while_string_count, HASHSIZE);
	fprintf(output_fptr, "\n<p>----------------Program Done--------------\n\n");

	fclose(output_fptr);
	
} /* End printHashEntries */

/* This function:
/* -Prompts the user for search criteria
/* -Searches for the specified string.
/* -Prints the results to the screen.
/* 
/* It expects the caller to pass it the address of the hash table.
/* 
/* It returns nothing.
/* 
/* It has no side effects.
*/
void
queryHashTable(HASH_TAB hash_tab)
{
	char search_item[MAXARRAY]; 
	int i = 0;
	
	/** Prompt user for seach item...
	 **/
	printf("Enter a search item:\n");
		scanf("%s", search_item);
		printf("\n");

	/** Seach for item in table...
	 **/
	i = findHashEntry(hash_tab, search_item);

	/** Print results...
	 **/
	printf("Found %d occurance(s) of %s.\n\n", i, search_item);
}