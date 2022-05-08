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
#define SUCCESS 1
#define FAILURE 1

using namespace std;

//Worker Structure.
class Worker{
  public:
    bool available ;
    int id ;
    char* file ;
    pid_t process_id;

    Worker(){
      bool available =true;
      int id = -1;
      char* file = NULL;
    }

    void Display(){
      printf("\nWorker [%d] : { \n  File : %s\n  Availability : %d\n}\n",process_id,file,available);
      return;
    }
};

//WorkerNode Structure.
typedef struct WorkerNode{
  Worker worker;
  WorkerNode* next;
}WorkerNode;

//WorkerQueue Srtucture.
class WorkerQueue{

  public:

    WorkerNode* front;
    WorkerNode* rear;
    int WorkerNum ;
    int available_workers;
    pid_t manager;

    //Initialize WorkerQueue.
    WorkerQueue(pid_t manager_pid){
      front = NULL;
      rear = NULL;
      manager = manager_pid;
      WorkerNum = 0;
      available_workers = 0 ;
      printf("\n WorkerQueue initialized SuccesFully\n");
    }

    //Add Worker to Queue.
    void AddWorker(char* FilePath , pid_t _id){
      WorkerNode* newWorker = (struct WorkerNode*) malloc (sizeof(struct WorkerNode));
      if(newWorker == NULL)
      {
          printf("\nError:Cannot Allocate newWorker\n");
          return;
      }
      else{
        newWorker->worker.id = WorkerNum;
        newWorker->worker.process_id = _id;
        newWorker->worker.file = (char* ) malloc ( sizeof (FilePath));
        strcpy( newWorker->worker.file , FilePath);
        newWorker->worker.available = false;
        if(front == NULL)
        {
          front = newWorker;
          rear = newWorker;
          front -> next = NULL;
          rear -> next = NULL;
        }else{
          rear -> next = newWorker;
          rear = newWorker;
          rear->next = NULL;
        }
        WorkerNum++;
        return;
      }
    }



  int get_number_of_workers(){
    return WorkerNum;
  }


  int AssignToFirstAvailable(char* FilePath){
      struct WorkerNode * temp;
      temp = front;
      while(temp != NULL)
          {
              if(temp->worker.available == true ){

                free(temp->worker.file);
                temp->worker.file = (char* ) malloc ( sizeof (FilePath));
                strcpy( temp->worker.file , FilePath);

                temp->worker.available =  false ;
                return SUCCESS;
              }
              temp = temp -> next;
          }
      return FAILURE;

    }


    //Display Workers.
    void DisplayWorkers( ){
      struct WorkerNode * temp;
      temp = front;
      if(front == NULL)
      {
          printf("\nEmpty queue\n");
      }
      else
      {
          printf("-WORKERS- \n");
          while(temp != NULL)
          {
              temp->worker.Display();
              temp = temp -> next;
          }
      }

      return;
    }


    int CheckAvailability( ){
      struct WorkerNode * temp;
      int available_workers = 0;
      temp = front;
      if(front == NULL)
      {
          printf("\nEmpty queue\n");
      }
      else
      {
         
          while(temp != NULL)
          {
              if(temp->worker.available == true){
                available_workers++;

              }
              temp = temp -> next;
          }
      }

      return available_workers;


    }



    /* function to handle the message the Manager got from Listener 
      and either assign work to a worker or create one and then assign .*/

    void HandleListenedMessage(char* message , char* dir, bool first_listen){


      char* filename;
      cout<<"Directory "<<dir<<endl;
      int number_of_files = 0;
      char * token;
      token = strtok (message," ");

      while (token != NULL){
        printf ("%s\n",token);
        if( strcmp(token ,"CREATE") == 0 ){
        cout<<token<<endl;
        number_of_files++;
        filename = strtok (NULL, " ");
        strcat(filename,dir);
        cout << "Filename : "<<filename<<endl;
        number_of_files++;






        }
        token = strtok (NULL, " ");
      }

      return ;
    }


    //Delete Queue.
    ~WorkerQueue(){};
};