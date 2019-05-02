from config_mysql import connection
import sys
import datetime
import time

room = sys.argv[1]
external_temperature = sys.argv[2]
internal_temperature = sys.argv[3]
starting_time = time.strftime("%H:%M:%S")
goal_degrees = sys.argv[4]


mydb = connection()

mycursor = mydb.cursor()

sql = "INSERT INTO heatinghome (room, external_temperature, internal_temperature, starting_time, goal_degrees) VALUES (%s, %s, %s, %s, %s)"
val = (room, external_temperature, internal_temperature, starting_time, goal_degrees)
mycursor.execute(sql, val)

mydb.commit()



