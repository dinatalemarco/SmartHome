from config_mysql import connection
import sys
import datetime
import time

input_room = sys.argv[1]
input_sensor = sys.argv[2]
input_min = sys.argv[3]
input_ideal = sys.argv[4]

id = None
room = None
sensor = None
min_value = None
ideal_value = None


if input_min != 'UNDEF' and input_ideal != 'UNDEF' and input_min != 'NULL' and input_ideal != 'NULL':

	mydb = connection()


	cursor = mydb.cursor()

	cursor.execute("SELECT id, room, sensor FROM settings WHERE room = %s AND sensor = %s" ,(input_room,input_sensor,))

	# Fetch a single row using fetchone() method.
	results = cursor.fetchall()


	for row in results:
		id = row[0]
		room = row[1]
		sensor = row[2]


	if id == None:
		mycursor = mydb.cursor()

		sql = "INSERT INTO settings (room, sensor, min_value, ideal_value) VALUES (%s, %s, %s, %s)"
		val = (input_room,input_sensor, input_min, input_ideal)
		mycursor.execute(sql, val)

	else:		
		sql = "UPDATE settings SET room=%s, sensor=%s, min_value=%s, ideal_value=%s WHERE id=%s"
		val = (room,sensor,input_min,input_ideal,id)
		cursor.execute(sql, val)


	mydb.commit()
	cursor.close()
	mydb.close()

