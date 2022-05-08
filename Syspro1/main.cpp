#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include<stdint.h>
#include<inttypes.h>
#include <signal.h>
#include "WorkersQueue.hpp"
#include "WorkerBody.hpp"
#include "utilities.hpp"
#define Listening 1
#define Read 0 
#define Write 1
using namespace std; 




void WorkerBody1(char* mypipe){

	printf("[worker] pid %d from [manager] pid  %d\n",getpid(),getppid());

	while(true){


		sleep(5);
		exit(0);
	}
	return;
}






void Assign_to_worker(char* str ){
	char * pch;
	int number_of_files = 0;
	pch = strtok (str," ");
    while (pch != NULL){
    //printf ("%s\n",pch);
    if( strcmp(pch ,"CREATE") == 0 ){
    	//cout<<pch<<endl;
    	number_of_files++;
    }
    pch = strtok (NULL, " ");
  }

}



// ____ Sniffer main function. ____ //

int main (int argc , char* argv[]){
	// ____ Directory to watch ____ //
	char* MonitorDir = "./towatch";   
	struct ListenerManagerCommunication Pipes;
	pid_t sniffer_pid  = getpid();
   printf( "[Sniffer] pid %d \n",getpid());

	// ____ Initialize Listener-Manager Communication. ____ //
   if (pipe(Pipes.Listener_Manager_fds) == -1) { fprintf(stderr, "Listener to Manager Pipe Failed"); return 1; }
   if (pipe(Pipes.Manager_Listener_fds) == -1) { fprintf(stderr, "Manager to Listener Pipe Failed"); return 1; }
	
	// ____ Create Listener & Manager Processes. ____//
  	for(int i=0;i<2;i++){
		if ( getpid() == sniffer_pid ){
			if(fork() == 0){
				if( i == 0 ){

					// ____Listener's  body. ____ //
            	printf("[Listener] pid %d from [Sniffer] pid %d\n",getpid(),getppid());

            	// ___ Close unecessary pipe ends. ___ //
            	close(Pipes.Manager_Listener_fds[1]);
          		close(Pipes.Listener_Manager_fds[0]);

          		// ___ Manager - Listener Handshake ___ //
          		strncpy(Pipes.ListenerMessage, "Hello Manager", 200);
          		write(Pipes.Listener_Manager_fds[1] , Pipes.ListenerMessage , sizeof(Pipes.ListenerMessage) + 1);
          		read(Pipes.Manager_Listener_fds[0] , Pipes.ManagerMessage , sizeof(Pipes.ManagerMessage) + 1);
          		printf("[Listener] read :{%s}\n" ,  Pipes.ManagerMessage);

          		// ___Connect the output of the inotify wait to Listener pipe & exec ___ //.
          		dup2(Pipes.Listener_Manager_fds[1] , 1);
				execlp("/bin/inotifywait" , "/bin/inotifywait",MonitorDir , "-m","-r","-e","create","-e","moved_to" ,NULL);

				}if( i == 1 ){

					// ____ Manager's body. ____ //
					printf("[Manager] pid %d from [Sniffer] pid %d\n",getpid(),getppid());
  					pid_t manager_pid = getpid();
					bool first_listen = true;

         		// ____ Initialize WorkerQueue. ____ //
        			WorkerQueue workers(manager_pid);
		 

		   		// ____Close Unnecessary ends of Listener-Manager pipes. ____ //
		    		close(Pipes.Listener_Manager_fds[1]);
		      	close(Pipes.Manager_Listener_fds[0]);

		     		// ___ Manager - Listener Handshake ___ //
	  				read(Pipes.Listener_Manager_fds[0] , Pipes.ListenerMessage , sizeof(Pipes.ListenerMessage) + 1);
          		printf("[Manager] read :{%s}\n" ,  Pipes.ListenerMessage);
					strncpy(Pipes.ManagerMessage, "ohhh.Hello Listener", 200);
          		write(Pipes.Manager_Listener_fds[1] , Pipes.ManagerMessage , sizeof(Pipes.ManagerMessage) + 1);


	          	// _______________________ START LISTENING ______________________________ //

	          	int new_files = 0 ;
	          	int files_handled = 0;
	          	char** files = 0;
	          	int available_workers;
	          	int new_workers_needed;
	          	int numworkers = 0 ;
	          	while( Listening ){

	          		if ( getpid() == manager_pid){

	          			cout<<endl<<">> Manager["<<manager_pid<<"] waiting for new_files..."<<endl;
	          			memset(Pipes.ListenerMessage,0,strlen(Pipes.ListenerMessage));
	            		read(Pipes.Listener_Manager_fds[Read] , Pipes.ListenerMessage , sizeof(Pipes.ListenerMessage) + 1);
	            		printf("[Manager] read ::  %s \n" ,  Pipes.ListenerMessage);
	            		char* message = (char*)malloc( sizeof(Pipes.ListenerMessage) +1);
	            		strcpy( message , Pipes.ListenerMessage); 
						new_files = get_number_of_new_files(message,MonitorDir,manager_pid);
						free(message);
						cout<<"NUMBER_OF__FILES_LISTENED[ "<<new_files<<" ]"<<endl;
						files_handled = files_handled + new_files ;

					    // __ Handle detected files  ___ //

						//Calculate new Workers needed.
						available_workers =  workers.CheckAvailability();
						if ( available_workers >= new_files){
							new_workers_needed = 0;
						}else{  new_workers_needed = new_files - available_workers ;}
						cout<<endl<<"NEW_WORKERS_NEEDED["<< new_workers_needed<<"]"<<endl;


						for(int i=0;i< new_workers_needed ;i++){
							if ( getpid() == manager_pid){
								numworkers++;
								//__ Create FIFO's of new Workers. __//
								string Pipename = to_string(numworkers) + "_infifo";
								int n = Pipename.length();
		        				char pipename[n + 1];
		        				strcpy(pipename , Pipename.c_str());
								if ( mkfifo (pipename,0777) == -1){if(errno != EEXIST){ cout<<" Could not create" << pipename <<" .."<<endl; return -1 ;}}
							}
							//__ fork needed worker processes __//
	    					pid_t new_worker_pid = fork(); 
	    				    if(new_worker_pid == 0){
	            				
	            				// ___ Call Worker Body ___ //





	            				WorkerBody1();


	        				}else{

	        					workers.AddWorker("test",new_worker_pid);



	        				}
	    				}












					}

					new_files= 0 ;
					if ( files_handled >= 10 ){
						cout<<"BREAK";
						break;
					}

				} //Stop Listening.



				if ( getpid() == manager_pid){
					workers.DisplayWorkers();
         			for(int i=1;i<=numworkers;i++){
						//wait(NULL);
						string Pipename = to_string(i) + "_infifo";
						int n = Pipename.length();
		        		char pipename[n + 1];
		        		strcpy(pipename , Pipename.c_str());

						//__ Delete FIFOS at finish. __//
						if( unlink(pipename) < 0) {
							perror("Manager[]: can't unlink ");
							printf("%d\n",getpid());
						}else{
							cout<<"Manager[" <<getpid()<<"] :deleted pipe [ "<<pipename<<endl;
						}
					}
         		}


         }     		


         
       		} // forked processes end..
		}// sniffer 


	} // Manager - Listener  







	//-- Do not exit before Manager and Listener. --//
    if (  getpid() == sniffer_pid ){
        for(int i=0;i<2;i++) {
     		wait(NULL);
   		}
 	}
	cout<<" I am "<<endl;
	return 0; 
	}