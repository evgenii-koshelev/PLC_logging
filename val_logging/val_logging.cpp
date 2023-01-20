#define _CRT_SECURE_NO_WARNINGS		
//
//#include "stdafx.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include "snap7.h"  
#include "snap7.cpp"  
#include "s7.h"  
#include "s7.cpp"  



#include "mysql_connection.h"
#include <stdlib.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>




//for demonstration only. never save your password in the code!
const string server = "188.65.208.80/186267_somsom";
const string username = "";
const string password = "";



using namespace std;





TS7Client* Client;                                                          // Creating PLC client
byte output_ON[1] = { 1 };                                                  // Data for PLC output to ON
byte output_OFF[1] = { 0 };                                                 // Data for PLC output to OFF
byte output_bit_value[1] = { 0 };
int Rack = 0;                                                               // PLC Rack number
int Slot = 1;                                                               // PLC Slot number
int DB_NUMBER = 251;                                                          // PLC Data Block number
int START_ADDRESS = 0;                                                      // Start Address for PLC output
int PLC_dataSize = 1;                                                       // Data size for sending data to PLC
//char Address = "192.168.0.101";                                          // PLC IP address

byte DB_data[256];																//Storage for 

float Temp;



void plc_connect()                                                          // Connecting to PLC function
{
	Client = new TS7Client();

	cout << "connecting" << endl;
	
	try
	{
		int ConnectingStatus;
		ConnectingStatus = Client->ConnectTo("192.168.0.1", Rack, Slot); // var
		
		cout << "status is  " << ConnectingStatus << endl;

		if (ConnectingStatus == 0)
		{
			cout << "Cusseccfully!!!" << std::endl;
		}
		else if (ConnectingStatus == 10065) {
			cout << "huerga kakayato!!!" << std::endl;
			
		}
	}
	

	catch (const std::exception& err)
	{
		cout << "Error!!!" << endl;
	}


}



void readData_real_from_BD()
{			
	cout << "Reading real " << endl;

	Client->ReadArea(S7AreaDB, DB_NUMBER, 4, 1, S7WLReal, &DB_data); //не нужно ничего делить/умножать/складывать, сразу указывать адрес. например 2.0
	
	Temp = S7_GetRealAt(DB_data, 0);

	cout << Temp << " is temperature of chiller" << endl;

	
}



void add_to_file() {	
	std::ofstream file;
	string line = "Temperature of heater is ";

	cout << "adding to file...." << endl;


	// current date/time based on current system
	time_t now = time(0);
	// convert now to string form
	char* dt = ctime(&now);

	cout << "The local date and time is: " << dt << endl;


	
	file.open("test.txt", std::ios::out | std::ios::app);
	file << line <<  Temp << "  " << dt << std::endl;

	cout << "done!" << endl;

			
}



void plc_disconnect()                                                       // Disconnecting to PLC function
{
	Client->Disconnect();
	cout << "disconnecteeed!" << endl;
	delete Client;

}






void DBconnect()
{
	cout << "Connecting to SQL database...." << endl;

	


	cout << "Let's have MySQL count from 10 to 1..." << endl;
	try {
		sql::Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;
		sql::ResultSet* res;
		sql::PreparedStatement* pstmt;
		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect(server, username, password);

		/* Connect to the MySQL test database */
		//con->setSchema("186267_somsom");
		//stmt = con->createStatement();
		//stmt->execute("186267_somsom");
		//delete stmt;

		
		/* Select in ascending order */
		pstmt = con->prepareStatement("SELECT valve_mode FROM `som`");
		res = pstmt->executeQuery();
		
		/* Fetch in reverse = descending order! */
		res->afterLast();
		

		/* Fetch in reverse = descending order! */
		res->previous();
		cout << res->getInt(1) <<" valve mode is " << endl;
		



		//cout << res->getInt(1) << endl;
		while (res->previous())
			cout << res->getInt(1)<< endl;
		delete res;
		delete pstmt;
		delete con;

	
}
catch (sql::SQLException& e) {
	cout << "# ERR: SQLException in " << __FILE__;
	cout << "(" << __FUNCTION__ << ") on line "		<< __LINE__ << endl;
	cout << "# ERR: " << e.what();
	cout << " (MySQL error code: " << e.getErrorCode();
	cout << ", SQLState: " << e.getSQLState() << 	" )" << endl;
}
cout << endl;






























	
	/*

	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;

	
	
	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);

	} 
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		//system("pause");
		exit(1);
	}
	
	cout << "Connected to database" << endl;
	
	//please create database "quickstartdb" ahead of time
	con->setSchema("186267_somsom");

	//stmt = con->createStatement();



	


	/*
	stmt = con->createStatement();
	stmt->execute("DROP TABLE IF EXISTS inventory");
	cout << "Finished dropping table (if existed)" << endl;
	stmt->execute("CREATE TABLE inventory (id serial PRIMARY KEY, name VARCHAR(50), quantity INTEGER);");
	cout << "Finished creating table" << endl;
	delete stmt;

	pstmt = con->prepareStatement("INSERT INTO inventory(name, quantity) VALUES(?,?)");
	pstmt->setString(1, "banana");
	pstmt->setInt(2, 150);
	pstmt->execute();
	cout << "One row inserted." << endl;

	pstmt->setString(1, "orange");
	pstmt->setInt(2, 154);
	pstmt->execute();
	cout << "One row inserted." << endl;

	pstmt->setString(1, "apple");
	pstmt->setInt(2, 100);
	pstmt->execute();
	cout << "One row inserted." << endl;

	delete pstmt;
	delete con;

	*/



}



void DBadd()
{

	cout << "Added to database" << endl;


}


// /*

void BDwrite()
{
	while (1)
	{
		DBconnect();
		DBadd();
		
		this_thread::sleep_for(chrono::seconds(1));		// waiting 
	}
	


}


// */





int main()
{
	

	std::thread thr(BDwrite);								// consider opp to run thread after successfull plc data read
	
	thr.detach();											// this case no need to wait finish of this th. Use join or smtnk else in othes case



	
	



	while (1)
	{
		/*
		plc_connect();										// conneting to plc
		readData_real_from_BD();							// reading temperature		
		plc_disconnect();									// disconnecting 
		add_to_file();										// adding to the file data and time
		*/
		cout << "next Temp check in 60 sec" << endl;



		this_thread::sleep_for(chrono::seconds(2));		// waiting 






		
		// system("PAUSE");									//waiting intill key pressed

		//return 0;
	}


}


