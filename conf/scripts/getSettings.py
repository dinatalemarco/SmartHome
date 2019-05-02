from config_mysql import connection
import sys
import datetime
import time

input_room = sys.argv[1]
input_sensor = sys.argv[2]
value = sys.argv[3]


min_temperature = None
ideal_temperature = None


try:

	mydb = connection()


	cursor = mydb.cursor()

	cursor.execute("SELECT min_value, ideal_value FROM settings WHERE room = %s AND sensor = %s" ,(input_room,input_sensor,))

	# Fetch a single row using fetchone() method.
	results = cursor.fetchall()

	for row in results:
		if value=="min":
			print(row[0])

		if value=='ideal':
			print(row[1])
			
	if results == None:
		print(0)


	mydb.commit()
	cursor.close()
	mydb.close()

except:
	print(0)
