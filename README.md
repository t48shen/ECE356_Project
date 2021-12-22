# ECE356_Project

Steps to run our project.

1. Run ERDtoRS.sql in marmoset.

2. In projectclient.cpp:
	- Change line 2355 to your UserID.
	- Change line 2356 to the Marmoset server that you ran ERDtoRS.sql in.
	- Change line 2357 to the database name that you ran ERDtoRS.sql in.

3. Within your terminal run the following command to compile:

	g++ -o projectclient projectclient.cpp `mysql_config --cflags --libs`

4. Run the created executable file with the following command:

	./projectclient

5. Enter your password to marmoset 
   (Ensure this is the password to the marmoset server in step 1 and 2)
