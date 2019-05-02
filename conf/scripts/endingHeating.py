from config_mysql import connection
import sys
import datetime
import time


id = None
room = sys.argv[1]
external_temperature = None
internal_temperature = None
starting_time= None
ending_time = time.strftime("%H:%M:%S")
goal_degrees = None


mydb = connection()

cursor = mydb.cursor()

cursor.execute("SELECT id, room, external_temperature, internal_temperature, starting_time, goal_degrees FROM heatinghome WHERE room = %s AND ending_time IS NULL" ,(room,))

# Fetch a single row using fetchone() method.
results = cursor.fetchall()


for row in results:
	id = row[0]
	room = row[1]
	external_temperature = row[2]
	internal_temperature = row[3]
	starting_time = row[4]
	goal_degrees = row[5] 

if id!=None:
	cursor.execute("UPDATE heatinghome SET room=%s, external_temperature=%s, internal_temperature=%s, starting_time=%s, ending_time=%s, goal_degrees=%s WHERE id=%s", 
				   (room,external_temperature,internal_temperature,starting_time,ending_time,goal_degrees,id))
	


mydb.commit()
cursor.close()
mydb.close()


