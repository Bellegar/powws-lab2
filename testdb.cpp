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
    	    	string comm = "select * from table2 where type like '%";
                char* word = "doll";
comm += string(word) + "%';";
    	    	//strcat(comm,word);//buf = "doll";////
    	    	//strcat(comm,"%';");
        		res = PQexec(conn, comm.c_str());
        		row = PQntuples(res);
        		col = PQnfields(res);
        		char out[300] ="";
        		for (int icol = 0; icol < col; icol++)
        		{
        			strcat(out,PQfname(res,icol));
        			strcat(out,"\t");
        		}
        		strcat(out,"\n");
        		for (int irow=0; irow <  row; irow++)
        		{
        			for (int icol=0; icol < col; icol++)
        			{
        				strcat(out, PQgetvalue(res, irow, icol));
        				strcat(out,"\t");
        			}
        			strcat(out,"\n");
        		}
            cout << out;
            getchar();
            //cin >> row;
        return 0;
}
