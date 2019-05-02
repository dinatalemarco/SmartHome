from config_mysql import connection
import sys
import datetime
import time

room = sys.argv[1]
month = datetime.date.today().strftime("%B")
dayofweek = datetime.date.today().strftime("%A")
dayoutside = time.strftime("%Y-%m-%d")
timeoutside = time.strftime("%H:%M:%S")


mydb = connection()

mycursor = mydb.cursor()

sql = "INSERT INTO presence_at_home (room, month, dayofweek, dayoutside, timeoutside) VALUES (%s, %s, %s, %s, %s)"
val = (room, month, dayofweek, dayoutside, timeoutside)
mycursor.execute(sql, val)

mydb.commit()



