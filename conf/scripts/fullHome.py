from config_mysql import connection
import sys
import datetime
import time


id = None
month = None
dayofweek = None
dayoutside= None
timeoutside = None

room = sys.argv[1]
dayinside = time.strftime("%Y-%m-%d")
timeinside = time.strftime("%H:%M:%S")


# Connetto lo script al database
mydb = connection()

cursor = mydb.cursor()

# Verifico se la stanza ha già uno stato aperto
# Quando un utente esce da una stanza memorizzo il tempo in cui l'utente è fuori
# Recupero l'ultimo stato aperto per quella stanza
cursor.execute("SELECT id, room, month, dayofweek, dayoutside, timeoutside FROM presence_at_home WHERE room = %s AND dayinside IS NULL AND timeinside IS NULL" ,(room,))

# Fetch a single row using fetchone() method.
results = cursor.fetchall()


for row in results:
	id = row[0]
	room = row[1]
	month = row[2]
	dayofweek = row[3]
	dayoutside = row[4]
	timeoutside = row[5] 

# Esiste uno stato aperto
if id!=None:
	# Estraggo il tempo di uscita dalla stanza e verifico il tempo totole in cui l'utente non è presente
	timeinside = datetime.datetime.strptime(str(timeinside), "%H:%M:%S")
	timeoutside = datetime.datetime.strptime(str(timeoutside), "%H:%M:%S")
	diffTime = timeinside - timeoutside

	# Se il tempo totale in cui l'utente non è presente è maggiore di 1 ora allora ritengo il dato utile
	# altrimenti cancello lo stato
	if diffTime.seconds > 36000:
		cursor.execute("UPDATE presence_at_home SET room=%s, month=%s, dayofweek=%s, dayoutside=%s, dayinside=%s, timeoutside=%s, timeinside=%s WHERE id=%s", 
				      (room,month,dayofweek,dayoutside,dayinside,timeoutside,timeinside,id))
	else:
		cursor.execute("DELETE FROM presence_at_home WHERE id = %s",(id,))
	


mydb.commit()
cursor.close()
mydb.close()


