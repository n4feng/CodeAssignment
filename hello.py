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
	     flagacc = 0;
	     flaggr = 0;
	     ax = 0;
	     ay = 0;
	     az = 0;
	     gx = 0;
	     gy = 0;
	     gz = 0;
	     for row in spamreader:
	         if int(row['mark']) == 1:
	             ax = float(row['x'])
	             ay = float(row['y'])
	             az = float(row['z'])
	             flagacc = 1;
	         else:
	             gx = float(row['x'])
	             gy = float(row['y'])
	             gz = float(row['z'])
	             flaggr = 1;
	         if flagacc == 1 and flaggr == 1:
	         	 flagacc = 0
	         	 flaggr = 0
	         	 pre = current
	         	 current = math.sqrt(math.pow(ax, 2) + math.pow(ay, 2) + math.pow(az, 2));
	         	 current = 0.7 * current + 0.3 * pre;
	         	 if current < 0.65:
	         	 	freeFallCounter +=1
	         	 	lcLast = lc
	         	 elif lc  - lcLast > 5:
	         	    freeFallCounter = 0
	         	 if freeFallCounter > 10:
	         	    print("fall detected")
	         	 print("current acc is: %f",current)
	         	 temp = row['time']+ ", " + str(ax) + ", " + str(ay) + ", " + str(az) + ", " + str(gx) + ", " + str(gy) + ", " + str(gz) + "\n"
	         	 out.write(temp)
	         lc+=1
	     out.close();
     csvfile.close();
     

