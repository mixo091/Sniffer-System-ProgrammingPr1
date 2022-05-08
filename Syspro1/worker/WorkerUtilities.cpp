#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <regex>



#define BUFFSIZE 2056
using namespace std;
typedef string::iterator iter;
typedef regex_iterator<iter> regIter;


typedef struct url{
    int appearances;
    string _url;
    struct url *next;
}url;


class url_list{
	public:
    	url *head,*tail;
    	int size;
    url_list(){
        head = NULL;
        tail = NULL;
        size = 0;
    }


     void add_url(string x)
    {
        url *tmp = new url;
        tmp->_url = x;
        tmp->next = NULL;

        if(head == NULL)
        {
            head = tmp;
            tail = tmp;
        }
        else
        {
            tail->next = tmp;
            tail = tail->next;
        }
    }


    void Display_urls(){

    	url* tmp = head;
    	cout << " URL LIST ::"<<endl;

    	while( tmp != NULL){
    		cout << tmp->_url<< "\r\n" << endl;
    		tmp = tmp ->next;
    	}


    }


/***

    void deleteList(Node** head_ref)
	{
 
    /* deref head_ref to get the real head 
    Node* current = *head_ref;
    Node* next = NULL;
 
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
 
    /* deref head_ref to affect the real head back
        in the caller. 
    *head_ref = NULL;
}
***/

};













int main( ){

	char buf[ BUFFSIZE ];
	string s;
	int read_b, total ;
	char* file = "text.txt";
	char* filename = (char*)malloc(sizeof(file) + 1);
	strcpy(filename,file);
	cout<<filename<<endl;

	int filedes;
	char str[BUFFSIZE];
	

	url_list urls;

	if ( (filedes=open(filename, O_RDWR, 0666)) == -1){
		perror("Opening given file  ");
	}else{
		printf ("Managed to open [ %s ] successfully. \n", filename);
	}

	//read


	while ( (read_b= read(filedes, buf , sizeof(buf))) > 0 ){
		strcpy(str,buf);
		cout<<str<< " || "<< endl;
		total = total + read_b ;
		s.append(str, read_b);
		printf ("Total char in anotherfile %ld \n",total );
		memset(buf,0,sizeof(buf));

	}

	cout<<" String "<< s <<endl;


regIter::regex_type rx(
  "https?:\\/\\/(www\\.)?"\
  "[-a-zA-Z0-9@:%._\\+~#=]{1,256}"\
  "\\.[a-zA-Z0-9()]{1,6}\\b([-a-z"\
  "A-Z0-9()@:%_\\+.~#?&//=]*)");

  // stl iterator for looping through 
  // all the links found in the string 
  regIter n(s.begin(), s.end(), rx), e;

  // link counter 
  int lnkCounter = 0;

  while (n != e)
  {

    cout << "Link# " << ++lnkCounter;

    cout << "====\r\n";

    urls.add_url(n->str());

    cout << n->str() << "\r\n" << endl;

    n++;

  }


    urls.Display_urls();

    //write to file 


    //clear list 

    //sig to self




	//Clear filename.
	free(filename);
	filename = NULL;




	return 0;
}