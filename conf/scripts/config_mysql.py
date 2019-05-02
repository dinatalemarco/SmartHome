import mysql.connector


def connection():

	hostName="localhost"
	portNumber="8889"
	userName="root"
	userPasswd="root"
	databaseName="smarthome"

	return mysql.connector.connect(host=hostName,
								   port=portNumber,
								   user=userName,
								   passwd=userPasswd,
								   database=databaseName)