/** This program illustrates the server end of the message queue **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <vector>
#include <list>
#include <string>
#include <fstream>

#include "msg.h"
using namespace std;

/* The record in the hash table */
struct record
{
	/* The record id */
	int id;

	/* The first name */
	string firstName;

	/* The first name */
	string lastName;
};



/**
 * The structure of a hashtable cell
 */
class hashTableCell
{
	/* Public members */
	public:

	/**
 	 * Initialize the mutex
 	 */

	hashTableCell()
	{
		/* Initialize the mutex using pthread_mutex_init() */
		//If successful the pthread_mutex_init() returns zero
		if (pthread_mutex_init(&cell_mutex, NULL) != 0)
		{
			perror("pthread_mutex_init");
			exit(-1);
		}

	}

	/**
 	 * Initialize the mutex
 	 */
	~hashTableCell()
	{
		/* Deallocate the mutex using pthread_mutex_destroy() */
		//If successful the pthread_mutex_destroy() function returns zero
		if (pthread_mutex_destroy(&cell_mutex) != 0)
		{
			perror("pthread_mutex_destroy");
			exit(-1);
		}

	}

	/**
 	 * Locks the cell mutex
 	 */
	void lockCell()
	{
		/*TODO: Add code for locking the cell mutex */

		if(pthread_mutex_lock(&cell_mutex) < 0)
		{
			perror("pthread_mutex_lock");
			exit(-1);
		}
	}

	/**
 	 * Unlocks the cell mutex
 	 */
	void unlockCell()
	{
		/* TODO: Add code for unlocking the cell mutex */

		if(pthread_mutex_unlock(&cell_mutex) < 0)
		{
			perror("pthread_mutex_unlock");
			exit(-1);
		}

	}

	/* The linked list of records */
	list<record> recordList;


	/**
 	 * TODO: declare a cell mutex
 	 */

 	 pthread_mutex_t cell_mutex;

};

/* The number of cells in the hash table */
#define NUMBER_OF_HASH_CELLS 100

/* The number of inserter threads */
#define NUM_INSERTERS 5

/* The hash table */
vector<hashTableCell> hashTable(NUMBER_OF_HASH_CELLS);

/* The number of threads */
int numThreads = 0;

/* The message queue id */
int msqid;

/* The ids that yet to be looked up */
list<int> idsToLookUpList;

/**
 * TODO: Declare and initialize a mutex for protecting the idsToLookUpList.
 */

pthread_mutex_t idsToLookUpListMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * TODO: declare and initialize the condition variable, threadPoolCondVar,
 * for implementing a thread pool.
 */

pthread_cond_t threadPoolCondVar = PTHREAD_COND_INITIALIZER;

/* TODO: Declare the mutex, threadPoolMutex, for protecting the thread pool
 * condition variable.
 */

pthread_mutex_t threadPoolMutex = PTHREAD_MUTEX_INITIALIZER;


/**
 * Prototype for createInserterThreads
 */
void createInserterThreads();

/**
 * A prototype for adding new records.
 */
void* addNewRecords(void* arg);

void wakeUpThread();

/**
 * Deallocated the message queue
 * @param sig - the signal
 */
void cleanUp(int sig)
{

	/* Add code for deallocating the queue */

	if(msgctl(msqid, IPC_RMID, NULL) == -1)
	{
		perror("msgctl");
		exit(-1); 
	}


}

/**
 * Sends the message over the message queue
 * @param msqid - the message queue id
 * @param rec - the record to send
 */
void sendRecord(const int& msqid, const record& rec)
{
	/**
 	 * Convert the record to message
 	 */

	/* The message to send */
	message msg;

	/* Copy fields from the record into the message queue */
	msg.messageType = SERVER_TO_CLIENT_MSG;
	msg.id = rec.id;
	strncpy(msg.firstName, rec.firstName.c_str(), MAX_NAME_LEN);
	strncpy(msg.lastName, rec.lastName.c_str(), MAX_NAME_LEN);

	/* Send the message */
	sendMessage(msqid, msg);
}

/**
 * Prints the hash table
 */
void printHashTable()
{
	/* Go through the hash table */
	for(vector<hashTableCell>::const_iterator hashIt = hashTable.begin();
		hashIt != hashTable.end(); ++hashIt)
	{
		/* Go through the list at each hash location */
		for(list<record>::const_iterator lIt = hashIt->recordList.begin();
			lIt != hashIt->recordList.end(); ++lIt)
		{
			fprintf(stderr, "%d-%s-%s-%d\n", lIt->id,
						lIt->firstName.c_str(),
						lIt->lastName.c_str(),
						lIt->id % NUMBER_OF_HASH_CELLS
						);
		}
	}
}

/**
 * Adds a record to hashtable
 * @param rec - the record to add
 */
void addToHashTable(const record& rec)
{
	/**
 	 * TODO: grab mutex of the hash table cell
 	 */
	hashTable.at(rec.id % NUMBER_OF_HASH_CELLS).lockCell();

	/* Hash, and save the record */
	hashTable.at(rec.id % NUMBER_OF_HASH_CELLS).recordList.push_back(rec);

	/**
 	 * TODO: release mutex of the hashtable cell
 	 */

	hashTable.at(rec.id % NUMBER_OF_HASH_CELLS).unlockCell();


}


/**
 * Adds a record to hashtable
 * @param id the id of the record to retrieve
 * @return - the record from hashtable if exists;
 * otherwise returns a record with id field set to -1
 */
record getHashTableRecord(const int& id)
{
	/* Get pointer to the hash table record */
	hashTableCell* hashTableCellPtr = &hashTable.at(id % NUMBER_OF_HASH_CELLS);

	/**
 	 * The record to return
 	 */
	 record rec = { -1, "", ""};

	/**
 	 * TODO: grab mutex of the cell
 	 */

		hashTableCellPtr->lockCell();

	/* Get the iterator to the list of records hashing to this location */
	list<record>::iterator recIt = hashTableCellPtr->recordList.begin();

	do
	{
		/* Save the record */
		if(recIt->id == id)
		{
			rec = *recIt;

		}

		/* Advance the record it */
		++recIt;
	}
	/* Go through all the records */
	while((recIt != hashTableCellPtr->recordList.end()) && (rec.id != id));



	/**
 	 * TODO: release mutex of the cell. Hint: call unlockCell() to release
     *       mutex protecting the cell.
 	 */

	hashTableCellPtr->unlockCell();

	return rec;
}


/**
 * Loads the database into the hashtable
 * @param fileName - the file name
 * @return - the number of records left.
 */
int populateHashTable(const string& fileName)
{
	/* The record */
	record rec;

	/* Open the file */
	ifstream dbFile(fileName.c_str());

	/* Is the file open */
	if(!dbFile.is_open())
	{
		fprintf(stderr, "Could not open file %s\n", fileName.c_str());
		exit(-1);
	}


	/* Read the entire file */
	while(!dbFile.eof())
	{
		/* Read the id */
		dbFile >> rec.id;

		/* Make sure we did not hit the EOF */
		if(!dbFile.eof())
		{
			/* Read the first name and last name */
			dbFile >> rec.firstName >> rec.lastName;

			/* Add to hash table */
			addToHashTable(rec);
		}
	}

	/* Close the file */
	dbFile.close();
}

/**
 * Gets ids to process from work list
 * @return - the id of record to look up, or
 * -1 if there is no work
 */
int getIdsToLookUp()
{
	/* The id */
	int id = -1;

	/* TODO: Aquire the idsToLookUpListMutex mutex */
	if(pthread_mutex_lock(&idsToLookUpListMutex) < 0)
	{
		perror("pthread_mutex_lock");
		exit(-1);
	}

	/* Remove id from the list if exists */
	if(!idsToLookUpList.empty())
    	{
        	id = idsToLookUpList.front();
	        idsToLookUpList.pop_front();
    	}

	/* TODO: Release idsToLookUpListMutex  */
	if(pthread_mutex_unlock(&idsToLookUpListMutex) < 0)
		{
			perror("pthread_mutex_unlock");
			exit(-1);
		}

	return id;
}

/**
 * Add id of record to look up
 * @param id - the id to process
 */
void addIdsToLookUp(const int& id)
{
	/* TODO: Aquire idsToLookUpListMutex the list mutex */
	if(pthread_mutex_lock(&idsToLookUpListMutex) < 0)
	{
		perror("pthread_mutex_lock");
		exit(-1);
	}

	/* Add the element to look up */
	idsToLookUpList.push_back(id);

	/* TODO: Release the idsToLookUpList mutex */
	if(pthread_mutex_unlock(&idsToLookUpListMutex) < 0)
	{
		perror("pthread_mutex_unlock");
		exit(-1);
	}
}

/**
 * The thread pool function
 * @param thread argument
 */
void* threadPoolFunc(void* arg)
{
	/* The id to process */
	int id = -1;

	/* Sleep until work arrives */
	while(true)
	{

		/* TODO: Lock the mutex protecting threadPoolCondVar from race conditions */
		if(pthread_mutex_lock(&threadPoolMutex) < 0)
		{
			perror("pthread_mutex_lock");
			exit(-1);
		}

		/* Get the id to look up */
		id = getIdsToLookUp();

		/* No work to do */
		while(id == -1)
		{
			/* TODO: Sleep on the condition variable threadPoolCondVar */

			if(pthread_cond_wait(&threadPoolCondVar, &threadPoolMutex) < 0)
			{
				perror("pthread_cond_wait");
				exit(-1);
			}

			/* Get the id to look up */
			id = getIdsToLookUp();

		}

		/* TODO: Release the mutex protecting threadPoolCondVar from race conditions */

		if(pthread_mutex_unlock(&threadPoolMutex) < 0)
		{
			perror("pthread_threadPoolMutex");
			exit(-1);
		}
		/* Look up id */
		record rec = getHashTableRecord(id);


		/* Send the record to the client */
		sendRecord(msqid, rec);

	}

}

/**
 * Wakes up a thread from the thread pool
 */
void wakeUpThread()
{


	/* TODO: Lock the mutex protecting threadPoolCondVar from race conditions */

	if(pthread_mutex_lock(&threadPoolMutex) < 0)
	{
		perror("pthread_threadPoolMutex");
		exit(-1);
	}

	/* TODO: Wake up a thread sleeping on threadPoolCondVar (you can use
 	 * wakeUpThread function which you need to implement.
 	 */
	if(pthread_cond_signal(&threadPoolCondVar) < 0)
	{
		perror("pthread_cond_signal");
		exit(-1);
	}
	/* TODO: Release the mutex protecting threadPoolCondVar from race conditions */
	 if(pthread_mutex_unlock(&threadPoolMutex) < 0)
	 {
	 	perror("pthread_threadPoolMutex");
	 	exit(-1);
	 }

}

/**
 * Creates the threads for looking up ids
 * @param numThreads - the number of threads to create
 */
void createThreads(const int& numThreads)
{
	/** TODO: create numThreads threads that call threadPoolFunc() */
	pthread_t thread[numThreads]; 

	for(int temp_thread = 0; temp_thread < numThreads; temp_thread++)
	{
		if(pthread_create(&thread[temp_thread], NULL, threadPoolFunc, NULL) < 0)
		{
			perror("pthread_create");
			exit(-1);
		}
	}
}

/**
 * Creates threads that update the database
 * with randomly generated records
 */
void createInserterThreads()
{

	/* TODO: create NUM_INSERTERS threads that add new elements to the hashtable
 	 * by calling addNewRecords().
 	 */

	pthread_t thread[NUM_INSERTERS];

	for(int temp_thread = 0; temp_thread < NUM_INSERTERS; temp_thread++)
	{

		if(pthread_create(&thread[temp_thread], NULL, addNewRecords, NULL) < 0)
		{
		 	perror("pthread_create");
		 	exit(-1);
	    }
	}
}


/**
 * Called by parent thread to process incoming messages
 */
void processIncomingMessages()
{
	/* The arriving message */
	message msg;

	/* The id of the record */
	int id = -1;

	/* Wait for messages forever */
	while(true)
	{

		/* Receive the message */
		recvMessage(msqid, msg, CLIENT_TO_SERVER_MSG);

		/* TODO: Add id to the list of ids that should be processed */
		addIdsToLookUp(msg.id);

		/* TODO: Wake up a thread to process the newly received id */
		wakeUpThread();
	}
}

/**
 * Generates a random record
 * @return - a random record
 */
record generateRandomRecord()
{
	/* A record */
	record rec;

	/* Generate a random id */
	rec.id = rand() % NUMBER_OF_HASH_CELLS;

	/* Add the fake first name and last name */
	rec.firstName = "Random";
	rec.lastName = "Record";

	return rec;
}

/**
 * Threads inserting new records to the database
 * @param arg - some argument (unused)
 */
void* addNewRecords(void* arg)
{
	/* A randomly generated record */
	record rec;

	/* Keep generating random records */
	while(true)
	{
		/* Generate a record */
		rec = generateRandomRecord();

		/* Add the record to hashtable */
		addToHashTable(rec);
	}

}

int main(int argc, char** argv)
{
	/**
 	 * Check the command line
 	 */
	if(argc < 3)
	{
		fprintf(stderr, "USAGE: %s <DATABASE FILE NAME> <NUMBER OF THREADS>\n", argv[0]);
		exit(-1);
	}

	/* TODO: install a signal handler for deallocating the message queue */

	if(signal(SIGINT, cleanUp) == SIG_ERR)
	{
		perror("signal");
		exit(-1);
	}

	//signal(SIGINT, cleanUp);


	/* Populate the hash table */
	populateHashTable(argv[1]);


	printHashTable();

	/* Get the number of lookup ID threads */
	numThreads = atoi(argv[2]);

	/* Use a random file and a random character to generate
	 * a unique key. Same parameters to this function will
	 * always generate the same value. This is how multiple
	 * processes can connect to the same queue.
	 */
	key_t key = ftok("/bin/ls", 'O');

	/* Error checks */
	if(key < 0)
	{
		perror("ftok");
		exit(-1);
	}

	/* Connect to the message queue */
	msqid = createMessageQueue(key);

	/* Instantiate a message buffer for sending/receiving msgs
 	 * from msg queue
 	 */
	message msg;

	/* Create the lookup ID threads */
	createThreads(numThreads);

	/* Create the inserter threads */
	createInserterThreads();

	/* Process incoming requests */
	processIncomingMessages();


	return 0;
}
