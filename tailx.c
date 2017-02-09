#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
* Default values
*/
#define DEFAULT_LINESTOSHOW 10
#define DEFAULT_INPUTFILE	"data.txt"
#define DEFAULT_OUTPUTFILE	"output.txt"

/*
* Macros to use for boolean values
*/
#define TRUE	1
#define FALSE	0

#define MAXSTRSIZE 128 /** This string-size limit causes the outer 
** parsing loop to terminate a word after the 128th 'char' is detected.
**/

/** This is a list element...
 **/
 struct listWord{
	char word_str[MAXSTRSIZE];
	struct listWord *nextPtr;
	struct listWord *previousPtr;
};

typedef struct listWord LISTWORD;
typedef LISTWORD *LISTWORDPTR;

static int queued_elements = 0;

/** Function prototypes...
 **/
char * getFileName( char *, char * );

int getDisplayLimit( char * );

int getReverseLinesValue( char * );

struct listWord *createLink( void );

void queueInit( char *, LISTWORDPTR *, LISTWORDPTR *);

int  queueLength( void );

void enqueueItem( char *, LISTWORDPTR *, LISTWORDPTR * );

void rmQueueItem( LISTWORDPTR *, LISTWORDPTR * );

void printListElementsToFile( LISTWORDPTR, LISTWORDPTR , int, char * );

/****************************************************************
 **                                                 
 ** NAME:		main            
 **                                               
 ** ARGUMENTS:	None
 **				 
 ** RETURNS:	None
 **
 ** HELPER FUNCTIONS: 
 **
 ** DESCRIPITON:
 ** 
 ** "Main" extracts user-defined and reserved C word tokens from a sucessfully
 ** compiled .c file, stores them in a dynamically allocated list, and prints
 **/
 

void main(argc, argv)
int argc;
char *argv[];
{
	char *input_filename	=  NULL;
	char *output_filename	=  NULL;
	int   display_limit = 0;
	int   reverse_lines = FALSE;

	LISTWORDPTR head_Ptr, tail_Ptr, current_Ptr;
	char in_word[MAXSTRSIZE];
	int ch_code, j, done, display_line_count;
	FILE *input_fPtr;

	/** List is intially empty... 
 	 **/
	head_Ptr	= NULL;
	current_Ptr = NULL;
	tail_Ptr	= NULL;
	done = 0;
	display_line_count = 0;
	in_word[0] = '\0';

/*
* Takes arguments from the command line, reads from an input file, and writes the last 'n' 
* lines of the file in either standard or reverse order to the output file.
*
* Arguments: argv[1] - name of the input file to read the lines from
*			 argv[2] - number of lines to displayed 
*			 argv[3] - name of the output file to write to 
*		     argv[4] - flag (-r) for reverse order
* Returns:	 nothing
*/
	/*
	* "?" displays command line usage.
	*/
	if ((argv[1] == NULL) || (strcmp(argv[1], "?") == 0))
	{
		printf("Usage:\n");
		printf("%s <inputFile> <numLines> <outputFile> [-r]\n\n", argv[0]);
		printf("   inputFile  - name of file to read in	    (default is \"%s\")\n", DEFAULT_INPUTFILE);
		printf("   numLines   - number of lines to display	(default is %d)\n", DEFAULT_LINESTOSHOW);
		printf("   outputFile - name of file to output to	(default is \"%s\")\n", DEFAULT_OUTPUTFILE);
		printf("   -r         - display lines in reverse order  (default is standard order)\n\n");
		exit(0);
	}

	/* Check for input filename */
	input_filename = getFileName(argv[1], DEFAULT_INPUTFILE);

	/* Check for number of lines to display */
	display_limit = getDisplayLimit(argv[2]);

	/* Check for output filename */
	output_filename = getFileName(argv[3], DEFAULT_OUTPUTFILE);

	/* Check if user wants to reverse the display lines */
	reverse_lines = getReverseLinesValue(argv[4]);

	/** try to open file, otherwise print error message...
 	 **/
	if ((input_fPtr = fopen(input_filename, "r")) == NULL) {
		printf("Can't open input file\n");
		exit(-1);
	}

	/** Look for word tokens until end of file... 
 	 **/
	while (done == 0) {
		in_word[0] = '\0';

		/** Parse a single word token... 
 	 	 **/
		for (j = 0; j < (MAXSTRSIZE - 1); j++){
			if ( (ch_code = getc(input_fPtr)) != EOF) {
					
				if (ch_code == '\n') {
					in_word[j] = '\0';      /*  */
					break;				   /*  */
					
				} else {
					in_word[j] = (char)ch_code;     /*  */	
				}

			} else {                  /* EOF found */
				in_word[j] = '\0';	  /* Terminate char array w/ NULL byte  */
				done = 1;             /* If we were in the process of       */
				break;                /* building word token EOF was found  */
			}                         /* consider EOF as a word delimiter   */
		}   /* for loop */
	
	/** Add a word token to the list if unique; otherwise, visit the 
     ** record and increment the counter to show the match... 
	 **/
		if (queueLength() == display_limit) {
			rmQueueItem(&head_Ptr, &tail_Ptr);
			enqueueItem(in_word, &head_Ptr, &tail_Ptr);
		
		} else {
			enqueueItem(in_word, &head_Ptr, &tail_Ptr);
		}

	} /* end search for words while not end of file */

	    /** The input file has been parsed and the words have been stored, so
         ** close the input file...
         **/
	
	fclose (input_fPtr);

	/** Print a formatted list, unique count, and total count of elements...
	 **/
	
	printListElementsToFile( head_Ptr, 
							 tail_Ptr, 
							 reverse_lines, 
							 output_filename );

} /* End main */

/*********************************************************
 **                                                     **
 **                 Component Functions                 **
 **                                                     **
 *********************************************************/
/*
* Returns the filename to input/output.  If a null value
* is entered, this function will return the default filename.
*
* Arguments: filename - the name of the file to input/output
*			 default_filename - the default value to use
* Returns: the filename to input/output
*/
char * getFileName(char *filename, char *default_filename)
{
	if (filename != NULL)
		return filename;
	else
		return default_filename;
}

/*  
* Returns the maximum number of lines to display (must be greater than 0)
* in the output file.
*
* Arguments: display_limit - the requested number of lines to display.
* Returns: the display_limit entered, if it's a valid number; otherwise,
*			the default number of lines to display will be returned.
*/
int getDisplayLimit(char *display_limit)
{
	int value = 0;

	if (display_limit == NULL)
		return DEFAULT_LINESTOSHOW;

	value = atoi(display_limit);
	if (value <= 0) {
		printf("Negative value is not allowed for display limit; using default value: %d !",
			DEFAULT_LINESTOSHOW);
		return DEFAULT_LINESTOSHOW;
	}

	return value;
} 

/*
* Checks to see if we should return lines in standard order, or in reverse
* order.
*
* Arguments: reverse - if it's "-r", reverse display will be used
* Returns:	TRUE, if user chose to reverse the display;
*			FALSE otherwise.
*/
int getReverseLinesValue(char *reverse)
{
	if ((reverse != NULL) && (strcmp(reverse, "-r") == 0))
		return TRUE;
	else
		return FALSE;
}

/**********************************************************
 **                                                 
 ** NAME:		queueLength             
 **                                               
 ** ARGUMENTS:	None
 **
 ** RETURNS:	listlength (the number of elements waiting in the queue.)
 **/

int  queueLength(void)
{
	return (queued_elements);
}

/**********************************************************
 **                                                 
 ** NAME:		createLink              
 **                                               
 ** ARGUMENTS:	None
 **/

struct listWord *createLink(void)
{
	return ((struct listWord *)
			malloc(sizeof(struct listWord)));
}

void queueInit(char some_word[], 
						  LISTWORDPTR *hPtr, 
						  LISTWORDPTR *tailPtr)
{
	LISTWORDPTR newPtr;
	struct listWord *createLink(void);

	*hPtr = *tailPtr = newPtr = createLink(); /* only one link exists   */
	newPtr->previousPtr = newPtr->nextPtr = NULL; /* no next or prev links  */
	strcpy(newPtr->word_str, some_word);    /* insert item into link  */
	queued_elements = 1;	/* initialize counter for queued elements */
}
/**********************************************************
 **                                                 
 ** NAME:		enqueueItem              
 **                                               
 ** ARGUMENTS:	char some_word[] 	LISTWORDPTR *sPtr
 **				 
 ** RETURNS		void
 **                                        
 ** DESCRIPITON:
 ** 
 ** "store word" expects a char string and a pointer to a list element.
 ** First, it sanity checks that it has been passed a string with at least
 ** one byte of information.  Then, it traverses the list elements to 
 ** find a match.  If no match is found, "store word" dynamically creates
 ** another list element, and inserts the word token into the list.
 **
 ** Note: more detailed comments can be found within the function body.
 **
 ** Because the list contents need not be sorted, store word uses a 
 ** singly-linked list.  Technically it works LIFO, but it has no head
 ** and tail pointer, so it can't be called a stack.  Just a "no frills"
 ** list that works, and that I can reuse/refine in later assignments.
 **/

void enqueueItem(char some_word[], LISTWORDPTR *hPtr, LISTWORDPTR *tailPtr)
{	
	LISTWORDPTR newPtr;
	struct listWord *createLink(void);
	FILE *output_fPtr;
	
	/* If the list is empty, initialize it... */

	if (queueLength() == 0) {
		queueInit(some_word, hPtr, tailPtr);
		return;
	}
				
	/* Otherwise, enqueue the new link... */
		
	newPtr = createLink();
		
	if (newPtr != NULL) {
		strcpy(newPtr->word_str, some_word);
		queued_elements++;

		newPtr->nextPtr		= NULL;  /* end of q, make next NULL */
		(*tailPtr)->nextPtr = newPtr;/* old terminal node points to new */ 
		newPtr->previousPtr = *tailPtr; /* new node points to old */
			
		*tailPtr = newPtr; /*  publish new node as new end of queue */
			
	} else {
		if ((output_fPtr = fopen("output.txt", "w")) == NULL) {
			printf("Can't open output file\n"); 
			exit(1);
		}

	fprintf(output_fPtr, "enqueueItem: No memory available.\n");
	fclose(output_fPtr);
	}
	
} /* end function enqueue item */

/**********************************************************
 **                                                 
 ** NAME:		rmQueueItem              
 **                                               
 ** ARGUMENTS:	*hPtr, *tailPtr
 **/

void rmQueueItem(LISTWORDPTR *hPtr, LISTWORDPTR *tailPtr)
{
	LISTWORDPTR tempPtr;

	tempPtr = *hPtr;
	*hPtr = (*hPtr)->nextPtr;
	(*hPtr)->previousPtr = NULL;

	free(tempPtr);
	queued_elements--;
}

/**********************************************************
 **                                                 
 ** NAME:	printListElementsToFile             
 **                                               
 ** ARGUMENTS:	LISTWORDPTR headPtr, tailPtr, reverse_lines
 **				 
 ** RETURNS	void
 **                                        
 ** DESCRIPITON:
 ** 
 ** This is a list traversal function that visits each member
 ** of a linked list.  It requires a starting point (pointer to
 ** a list member) to be passed to it by the calling routine.
 **
 ** It prints list contents in a user-readable format. 
 **
 ** It requires that the output file pointed to by output_fPtr
 ** is not currently in use by another routine.
 **
 ** It expects to be called when there is something in the list.
 **/

void 
printListElementsToFile( LISTWORDPTR head_Ptr,
						 LISTWORDPTR tail_Ptr,
						 int reverse_lines, 
						 char output_filename[])
{

	FILE *output_fPtr;
	int string_count;
	
	string_count = 0;

	if (head_Ptr == NULL) {
		printf("printListElementsToFile: Nothing to print.\n"); 
		exit(1);
	}
	
	if ((output_fPtr = fopen(output_filename, "w")) == NULL) {
		printf("Can't open output file\n"); 
		exit(1);
	}
	
	if (!reverse_lines) {
		while (head_Ptr != NULL) {
			fprintf(output_fPtr, "%s\n", head_Ptr->word_str);
			string_count++;
			head_Ptr = head_Ptr->nextPtr;
	
		}   /* while word_Ptr != NULL */

	} else {
		while (tail_Ptr != NULL) {
			fprintf(output_fPtr, "%s\n", tail_Ptr->word_str);
			string_count++;
			tail_Ptr = tail_Ptr->previousPtr;
		}
	}
	
	/* print trailer infomation */

	fprintf(output_fPtr, "\nTotal lines \t= %10d\n", string_count);
	fprintf(output_fPtr, "\n----------------Program Done--------------\n\n");

	fclose(output_fPtr);

}