import csv
import math
print("Hello, world!")
filename = input("enter the filename (withour extension) ")
with open(filename+".csv", newline='') as csvfile:
     with open(filename+"_out.csv", 'a') as out:
         out.write("time, ax, ay, az, gx, gy, gz\n")
         spamreader = csv.DictReader(csvfile)
         pre = 0.0
         current = 0.0
         lc = 0;
         lcLast = 0;
         freeFallCounter = 0;
         flagfall = 0;
         flagflit = 0;
         ax = 0;
         ay = 0;
         az = 0;
         gx = 0;
         gy = 0;
         gz = 0;
         starttime = 0;
         for row in spamreader:
             gx = float(row['gx'])
             gy = float(row['gy'])             
             gz = float(row['gz'])             
             ax = float(row['ax'])
             ay = float(row['ay'])
             az = float(row['az'])
             if lc == 0:
                starttime = row['time']
                print("starttime at "+starttime)
             roll = 180 * math.atan(ax / math.sqrt(ay**2 + az**2)) / math.pi
             pitch = 180 * math.atan(ay / math.sqrt(ax**2 + az**2)) / math.pi
             yaw = 180 * math.atan(az / math.sqrt(ax**2 + az**2)) / math.pi                  
             pre = current
             current = math.sqrt(math.pow(ax, 2) + math.pow(ay, 2) + math.pow(az, 2));
             current = 0.7 * current + 0.3 * pre;
             if current < 0.65:
                 freeFallCounter +=1
                 lcLast = lc
             elif lc  - lcLast > 5:
                 freeFallCounter = 0
             if freeFallCounter > 10:
                 print("fall detected at time %f",(float(row['time']) - float(starttime))/1000000000.0)                 
             if (abs(pitch)> 60 or abs(roll)>60):
                 print("flit detected at time %f",(float(row['time']) - float(starttime))/1000000000.0)
                 
             
             temp = row['time']+ ", " + str(ax) + ", " + str(ay) + ", " + str(az) + ", " + str(gx) + ", " + str(gy) + ", " + str(gz) + "\n"
             out.write(temp)
             lc+=1
         out.close();
     csvfile.close();
     

