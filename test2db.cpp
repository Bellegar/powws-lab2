#include <postgresql/libpq-fe.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
using namespace std;
char* askdb(char* word, int tablenum);
int main(int argc, char** argv)
{
    int pid,status,len;
    //int chan[2];
    //char buf[300];
    if (!(pid = fork()))
    	{
    		//strcpy(files.sa_data, "socket.soc");
    		printf("Worker2\n");
    		//int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
    		//connect(sockidW2, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
    		//recv(sockidW2, (char*) buf, BUFSIZE,0);
                cout << askdb("doll",2);
    		//send(sockidW2, askdb(buf,2), BUFSIZE,0);

    	}
    	else
    	{
    		if (!(pid = fork()))
    		{
    			//strcpy(files.sa_data, "socket.soc");
        		printf("Worker3\n");
        		//int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
        		//connect(sockidW3, &files,(strlen(files.sa_data) +	sizeof(files.sa_family)));
        		//recv(sockidW3, (char*) buf, BUFSIZE,0);
                cout << askdb("doll",3);
        		//send(sockidW3, askdb(buf,3), BUFSIZE,0);
    		}
    		else
    		{
    			//strcpy(files.sa_data, "socket.soc");
    	    	printf("Worker1\n");
    	    	//int sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
    	    	//connect(sockidW1, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
    	    	//recv(sockidW1, (char*) buf, BUFSIZE,0);
                cout << askdb("doll",1);
        		//send(sockidW1, askdb(buf,1), BUFSIZE,0);
    		}
    	}
}

char* askdb(char* word, int tablenum)
{
		PGconn *conn;
	    PGresult *res;
		char out[300] ="";
		int col, row;
    	conn = PQconnectdb("dbname=bellegar host=localhost user=bellegar password=bugboom");
    	if (conn == NULL)
    	{
    		strcpy(out, "no connection to bellegar DB\n");
    		return out;
    	}
    	//std::cout << PQdb(conn);
    	string namecol;
    	switch (tablenum)
    	{
    	case 1:
    		namecol = "name";
    		break;
    	case 2:
    		namecol = "type";
    		break;
    	case 3:
    		namecol = "book";
    		break;
    	}
        //char* numch;
        //sprintf(numch,"%d",tablenum);
    	string comm = "select * from table";
        comm += char(tablenum + (int)'0');
        comm = comm + " where " + string(namecol) + " like '%" + string(word) + "%';";
        //cout << tablenum<< ' '<< comm << "\n";
    	res = PQexec(conn, comm.c_str());
    	row = PQntuples(res);
    	col = PQnfields(res);
        //cout << row<<' ' << col <<"\n";
   
    if (row !=0)
{
 cout<<"table"<<tablenum<<":\n";
	for (int icol = 0; icol < col; icol++)
	{
			strcat(out,PQfname(res,icol));
			strcat(out,"\t");
	}
	strcat(out,"\n");
}
	for (int irow=0; irow<row; irow++)
	{
		for (int icol=0; icol < col; icol++)
		{
			strcat(out, PQgetvalue(res, irow, icol));
			strcat(out,"\t");
		}
	strcat(out,"\n");
	}
cout << out;
}

