
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define COUNT_PATIENT 100 // maximum number of patients 99 start from 1
#define COUNT_STAFF 9     // maximum number of staff 8 start from 1
#define MAX_CAPACITY 3


struct roomSeats {
int numOne;
int numTwo;
int numThree;

}roomSeat;


void *patient(void *num);  // Definitions of functions
void *staff(void *);
void randwait(int secs);

sem_t waitingArea;
sem_t roomChair; //allows three patients to sit on seats in the room. 
sem_t staffVentilation; //allow rooms to be ventilated until a patient arrives
sem_t seatBelt;  //it is used to keep patients waiting until staff complete the testing process.


int allDone = 0; 
int waitingAreaValue=-1; //use it to send staff home when all patients are finished
int temp=0;

int main(int argc, char *argv[]) {

pthread_t btid[COUNT_STAFF]; //staff thread id, identification
pthread_t tid[COUNT_PATIENT]; //patient thread id, identification

int i, numpatients, numStops, numStaff;
int Number[COUNT_PATIENT];
int NumberroomChair [COUNT_STAFF];
numpatients = 100;
numStops = 100; //Simultaneous standby capacity in the waiting room

numStaff = 9;
waitingAreaValue=numStops;

for (i=1; i<COUNT_PATIENT; i++) {
Number[i] = i; //  array assigns patient no.
}
for (i=1; i<COUNT_STAFF; i++) {
NumberroomChair[i] = i;  // array assigns room chairs no.
}

sem_init(&waitingArea, 0, numStops); // waiting area with space up to numStops


// each numStaff contains roomChair up to MAX_CAPACITY.
sem_init(&roomChair, 0, MAX_CAPACITY);
sem_init(&staffVentilation, 0, 0);
sem_init(&seatBelt, 0, 0);

// create staffs
for(i=1;i<numStaff;i++){
pthread_create(&btid[i], NULL, staff,(void *)&NumberroomChair[i]);
}

// create patients
for (i=1; i<numpatients; i++) {
pthread_create(&tid[i], NULL, patient, (void *)&Number[i]);
}

for (i=1; i<numpatients; i++) {
pthread_join(tid[i],NULL); 
}

allDone = 1;
//sem_post 
waitingAreaValue=1; 
for(i=1;i<numStaff;i++){
sem_post(&staffVentilation);
}
for(i=0;i<numStaff;i++){
pthread_join(btid[i],NULL); 
}
printf("THE PROGRAM FINISHED.\n");
}

void *patient(void *number) {
int num = *(int *)number;


randwait(2);
printf("Patient %d has arrived at the hospital and has been waiting for the test.\n", num);

sem_wait(&waitingArea); // sem_wait fonk for three patients. locked with.
waitingAreaValue-=1;

sem_wait(&roomChair);  //sem_wait fonk for a patient. locked with.

sem_post(&waitingArea); //after locking, sem_post fonk. the lock at waitingArea semaphore is being removed.

waitingAreaValue+=1;
int value;
sem_getvalue(&roomChair, &value);
if(value==3)roomSeat.numOne=num;
else if(value==2)roomSeat.numTwo=num;
else if(value==1)roomSeat.numThree=num;
//else taxiSeat.numFour=num;
sem_post(&staffVentilation);
sem_wait(&seatBelt);
printf("Patient %d has arrived to the HOSPİTAL.\n", num);
// removing the lock on the roomChair semaphore.


sem_post(&roomChair);
randwait(3);
if(value==0){
printf("Room %d has finished testing and clear the room.\n", temp);
printf("Staff %d is ventilating.\n", temp);
}

}

void *staff(void *junk) {
int jun = *(int *)junk;
int value; 
printf("Staff %d is ventilating.\n", jun);
while (!allDone) {
sem_wait(&staffVentilation); // Air until someone arrives and wakes the person who arrives.

if(!allDone){// staff is being warned.
printf("Staff %d was warned by Patient %d.\n", jun,roomSeat.numOne);
value=1;
while(value<MAX_CAPACITY){
if(value==0)
printf("[Room %d>>|X|X|X|]",jun,roomSeat.numOne);
else if(value==1)
printf("[Room %d>>|%d|X|X|]",jun,roomSeat.numOne);
else if(value==2)
printf("[Room %d>>|%d|%d|%d|X|]",jun,roomSeat.numOne,roomSeat.numTwo);

printf("The last %d patient, let's begin!\n",(MAX_CAPACITY-value));
value++;
sem_wait(&staffVentilation);
}
}


// if the patient capacity to be tested is full, the testing process begins.
if (!allDone) {

printf("[Room %d>>|%d|%d|%d|]The test of patient begins...\n", jun,roomSeat.numOne,roomSeat.numTwo,roomSeat.numThree);
randwait(1);
temp=jun;
// patients whose test has been completed are released.
int freeAll=MAX_CAPACITY;
while(freeAll>0){
sem_post(&seatBelt);
freeAll--;
}

}
// if patient all done  staff can go home.
else {
printf("Staff %d is going to the home for the day.\n", jun);
}
}

}

void randwait(int secs) {
int len = 1; // wait delay
sleep(len);
}
