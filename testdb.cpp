#include <postgresql/libpq-fe.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
using namespace std;
int main(int argc, char** argv)
{
    PGconn *conn;
    PGresult *res;
int col,row;
    try {

    	    	conn = PQconnectdb("dbname=bellegar host=localhost user=bellegar password=bugboom");
    	    	}
    	    	catch(...){ std::cout<< "no connection\n";}
    	    	//std::cout << PQdb(conn);
    	    	char* comm = "select book, pgcount from table3 where book='";
                char* word = "doll";
    	    	strcat(comm,word);//buf = "doll";////
    	    	strcat(comm,"';");
        		res = PQexec(conn, comm);
        		row = PQntuples(res);
        		col = PQnfields(res);
        		char* out ="";
        		for (int icol = 0; icol < col; icol++)
        		{
        			strcat(out,PQfname(res,icol));
        			strcat(out," ");
        		}
        		strcat(out,"\n");
        		for (int irow=0; irow<row; irow++)
        		{
        			for (int icol=0; icol < col; icol++)
        			{
        				strcat(out, PQgetvalue(res, irow, icol));
        				strcat(out," ");
        			}
        			strcat(out,"\n");
        		}
            cout << out;
            cin >> row;
        return 0;
}
