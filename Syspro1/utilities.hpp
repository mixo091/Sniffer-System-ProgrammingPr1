#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include<stdint.h>
#include<inttypes.h>
#include <signal.h>
using namespace std;

struct ListenerManagerCommunication{
  int Listener_Manager_fds[2];
  int Manager_Listener_fds[2];
  char ListenerMessage[200];
  char ManagerMessage[200];
};




int get_number_of_new_files(char* message , char* dir ,pid_t p ){
    cout<<"Manager [ "<< p << " ] called me"<<endl;
	int number_of_files = 0;
	char * token;
    token = strtok (message," ");

    	while (token != NULL){
        	printf ("%s\n",token);
        	if( strcmp(token ,"CREATE") == 0 ){
        		cout<<token<<endl;
        		number_of_files++;
                token = strtok (NULL, " ");
                cout<<"FILE [" <<token << " ]" <<endl;

        }
        token = strtok (NULL, " ");
      }

      return number_of_files;

};



char* get_file_name(char* message , char* dir , int file_pos){
	char * token;
	//cout<<"KOMPRAAAAAAAA "<<  message  <<"FILE POS"<< file_pos<<endl; 
	int file_counter = 0;
	char* filename;
    token = strtok (message," ");
    while (token != NULL){
        //printf ("%s\n",token);
        if( strcmp(token ,"CREATE") == 0  ){
        	

        	//cout<<token<<endl;
        	if ( file_counter == file_pos){

        		filename = strtok (NULL, " ");
        		//cout<<"skata "<<filename;
        		return filename;
        	}
        	file_counter++;

        }
        token = strtok (NULL, " ");
     }





};










