// Copyright (C) 2024 Moritz Scheer

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int initialize_database()
{

	return 0;
}

int get()
{

}

int add()
{

}

int update()
{

}

int delete(int id)
{
	char str[20];
	snprintf(str, sizeof(str), "%d", id);

	char *sql = "DELETE FROM address WHERE address_ID = ";
}