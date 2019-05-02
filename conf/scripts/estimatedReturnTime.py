from datetime import timedelta
from config_mysql import connection
import sys
import datetime
import time



def averageTimeHeatingRoom(input_room,input_externalT,input_internalT,input_goal):

	lbe_Temp = int(input_externalT)-1
	upe_Temp = int(input_externalT)+1
	lbi_Temp = int(input_internalT)-1
	upi_Temp = int(input_internalT)+1


	# Connetto lo script al database
	mydb = connection()

	cursor = mydb.cursor()

	# Creo la query che avrà il compito di calcolare i tempi che in passato sono stati necessari per scaldare una 
	# determinata stanza in circostanze simili a quelle che si stanno verificando.
	# la query restituirà il tempo medio che viene impiegato dall'impianto di riscaldamento a portare la stanza
	# alla temperatura richiesta dall'utente
	sql = "SELECT TIMEDIFF(ending_time,starting_time) FROM heatinghome WHERE room = %s AND external_temperature >= %s AND external_temperature <= %s AND internal_temperature >= %s AND internal_temperature <= %s AND goal_degrees = %s"

	cursor.execute(sql ,(input_room,lbe_Temp,upe_Temp,lbi_Temp,upi_Temp,input_goal,))

	# Fetch a single row using fetchone() method.
	times = cursor.fetchall()

	average = 0

	if(len(times) > 0):
		for time in times:
			average = average + time[0].seconds

		# Calcolo la media dei tempi impiegati
		average = (average/len(times))
		# Mantengo solo un numero dopo la virgola
		average = round(average,2)

	mydb.commit()
	cursor.close()
	mydb.close()

	return average


def averageTimeHeating(input_room,input_externalT,input_internalT,input_goal):

	average = 0

	if input_room != "Home":
		average = averageTimeHeatingRoom(input_room,input_externalT,input_internalT,input_goal)
	else:
		# Connetto lo script al database
		mydb = connection()

		cursor = mydb.cursor()

		sql = "SELECT room FROM heatinghome GROUP BY room"

		cursor.execute(sql)

		rooms = cursor.fetchall()

		for room in rooms:
			average = average + averageTimeHeating(room[0],input_externalT,input_internalT,input_goal)

		average = average/len(rooms)

	return average



def estimatedReturnTime():

	room = "Home"
	average = 0
	timeoutside = None
	timeinside = None
	dayoutside = time.strftime("%Y-%m-%d")
	dayofweek = datetime.date.today().strftime("%A")

	# Connetto lo script al database
	mydb = connection()

	# Recupero l'ora in cui è uscito l'utente di casa. Quindi recupero l'ultima informazione sulla casa in cui 
	# ho un orario di uscita ma non ho un orario di ritorno
	cursor = mydb.cursor()
	cursor.execute("SELECT timeoutside FROM presence_at_home WHERE room = %s AND dayoutside = %s AND dayinside IS NULL AND timeinside IS NULL" ,(room,dayoutside))

	# Recupero l'orario in cui la casa non percepisce più la presenza al suo interno
	exit_time = cursor.fetchone()


	if(exit_time != None):
		# Data l'ora in cui la casa non percepisce più la presenza stimo una fascia oraria di 20 minuti in difetto e 20 minuti
		# in eccesso
		lb_Temp = datetime.timedelta(seconds= exit_time[0].seconds-1200)
		upb_Temp = datetime.timedelta(seconds= exit_time[0].seconds+1200)

		# Prendo tutti i tempi in cui è tornto l'utente quando è uscito nell'intervallo di tempo selezionato
		cursor.execute("SELECT timeinside FROM presence_at_home WHERE room = %s AND dayofweek = %s AND timeoutside >= %s AND timeoutside <= %s AND timeinside IS NOT NULL " ,(room,dayofweek,lb_Temp,upb_Temp,))

		# Recupero l'orario in cui la casa non percepisce più la presenza al suo interno
		listTimes = cursor.fetchall()


		if len(listTimes) > 0:
			for x in listTimes:
				average = average + x[0].seconds

			# Calcolo la media dei tempi impiegati
			average = average/len(listTimes)
			# Mantengo solo un numero dopo la virgola
			average = round(average,2)

		return average

	# Non ho informazioni sul tempo di uscita dell'utente. Non posso stimare il suo rientro
	return 99999999999;






if __name__== "__main__":
	input_room = sys.argv[1]
	input_externalT = sys.argv[2]
	input_internalT = sys.argv[3]
	input_goal = sys.argv[4]

	x = time.strptime(time.strftime("%H:%M:%S").split(',')[0],'%H:%M:%S')
	CuttentTime = datetime.timedelta(hours=x.tm_hour,minutes=x.tm_min,seconds=x.tm_sec).total_seconds()	

	TimeHeating = averageTimeHeating(input_room,input_externalT,input_internalT,input_goal)
	ReturnTime = estimatedReturnTime()

	#print("Return time "+str(datetime.timedelta(seconds=ReturnTime)))
	#print("Heating Time "+str(datetime.timedelta(seconds=TimeHeating)))
	#print("Diference Time "+str(datetime.timedelta(seconds=ReturnTime-TimeHeating-300)))
	#print("Current time "+str(datetime.timedelta(seconds=CuttentTime)))

	if ReturnTime - TimeHeating -300 <= CuttentTime:
		print("ON")
	else:
		print("OFF")





