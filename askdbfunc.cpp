//***Function of searching in db Bellegar data fields with incoming word with searching in tabl1,2,3
#ifndef ASKDB
#define ASKDB
#include <postgresql/libpq-fe.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
using namespace std;
char* askdb(char* word, int tablenum)
{
	PGconn *conn;
	PGresult *res;
	char out[300] = "";
	int col, row;
	conn = PQconnectdb(
			"dbname=bellegar host=localhost user=bellegar password=bugboom");
	if (conn == NULL)
	{
		strcpy(out, "no connection to bellegar DB\n");
		return out;
	}
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
	string comm = "select * from table";
	comm += char(tablenum + (int) '0');
	comm = comm + " where " + string(namecol) + " like '%" + string(word)
			+ "%';";
	res = PQexec(conn, comm.c_str());
	row = PQntuples(res);
	col = PQnfields(res);
	if (row != 0)
	{
		for (int icol = 0; icol < col; icol++)
		{
			strcat(out, PQfname(res, icol));
			strcat(out, "\t");
		}
		strcat(out, "\n");
	}
	for (int irow = 0; irow < row; irow++)
	{
		for (int icol = 0; icol < col; icol++)
		{
			strcat(out, PQgetvalue(res, irow, icol));
			strcat(out, "\t");
		}
		strcat(out, "\n");
	}
	return out;
}

void askdb(char* word, int tablenum, char* output)
{
	PGconn *conn;
	PGresult *res;
	char out[300] = "";
	int col, row;
	conn = PQconnectdb(
			"dbname=bellegar host=localhost user=bellegar password=bugboom");
	if (conn == NULL)
	{
		strcpy(out, "no connection to bellegar DB\n");
		strcpy(output, out);
		return;
	}
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
	string comm = "select * from table";
	comm += char(tablenum + (int) '0');
	comm = comm + " where " + string(namecol) + " like '%" + string(word)
			+ "%';";
	res = PQexec(conn, comm.c_str());
	row = PQntuples(res);
	col = PQnfields(res);
	if (row != 0)
	{
		for (int icol = 0; icol < col; icol++)
		{
			strcat(out, PQfname(res, icol));
			strcat(out, "\t");
		}
		strcat(out, "\n");
	}
	for (int irow = 0; irow < row; irow++)
	{
		for (int icol = 0; icol < col; icol++)
		{
			strcat(out, PQgetvalue(res, irow, icol));
			strcat(out, "\t");
		}
		strcat(out, "\n");
	}
	strcpy(output, out);
	return;
}
#endif
