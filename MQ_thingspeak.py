import serial # import Serial Library
import sys
import urllib
from time import sleep
import time
import numpy as np
#from itertools import izip_longest

myAPI= 'FUR71G0CGY220AU2'
baseURL = 'https://api.thingspeak.com/update?api_key=%s' % myAPI

arduinoData = serial.Serial('/dev/ttyACM0', 9600) #Creating our serial object named arduinoData

while True: # While loop that loops forever
    if (arduinoData.inWaiting()==0): #Wait here until there is data
        continue 
    arduinoString1 = arduinoData.readline().decode('utf-8') #read the line of text from the serial port
    
    arduinoString2 = arduinoString1.replace("\r\n", "")#Split it into an array called dataArray
        
    #arduinoString = eval('['+ arduinoString2 +']')
    
    arduinoString = arduinoString2.split(',')
    
    def isfloat(value):
        try:
            float(value)
            return True
        except ValueError:
            return False
        
    if isinstance(arduinoString, list) == True and\
       len(arduinoString) == 8 and\
       float(arduinoString[7]) > 0 and\
       isfloat(arduinoString[0]) == True and\
       isfloat(arduinoString[1]) == True and\
       isfloat(arduinoString[2]) == True and\
       isfloat(arduinoString[3]) == True and\
       isfloat(arduinoString[4]) == True and\
       isfloat(arduinoString[5]) == True and\
       isfloat(arduinoString[6]) == True and\
       isfloat(arduinoString[7]) == True:
        dataArray = arduinoString  
    else:
         dataArray = ['nan']*8
        
    #arduinoString = eval('['+ arduinoString2 +']')

    MQ3 =    float(dataArray[0])
    MQ4 =    float(dataArray[1])
    MQ5 =    float(dataArray[2])
    MQ7 =    float(dataArray[3])
    MQ9 =    float(dataArray[4])
    MQ135 =    float(dataArray[5])
    H =    float(dataArray[6])
    T =    float(dataArray[7])
    
    intentos = 0
    while intentos < 10:
        try:
            conn = urllib.urlopen(baseURL + '&field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field6=%s&field7=%s&field8=%s' % (MQ3, MQ4, MQ5, MQ7, MQ9, MQ135, H, T))
            print(MQ3, MQ4, MQ5, MQ7, MQ9, MQ135, H, T)
            conn.close()
            sleep(8)
            break
        except IOError:
            intentos += 1
            print("IOError")



