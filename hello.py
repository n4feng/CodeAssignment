import csv
import math
print("Hello, world!")

with open('test.csv', newline='') as csvfile:
     spamreader = csv.DictReader(csvfile)
     pre = 0.0
     current = 0.0
     lc = 0;
     lcLast = 0;
     freeFallCounter = 0;
     for row in spamreader:
         print(row['ax'], row['ay'], row['az'])
         ax = float(row['ax'])
         ay = float(row['ay'])
         az = float(row['az'])
         gx = float(row['gx'])
         gy = float(row['gy'])
         gz = float(row['gz'])

         pre = current;
         current = sqrt(pow(ax, 2) + pow(ay, 2) + pow(az, 2));
         if lc == 0:
         	pre = current;
         current = 0.7 * current + 0.3 * pre;
         if current < 0.65:
         	freeFallCounter++
         	lcLast = lc;
         else if lc  - lcLast > 5:
         	freeFallCounter = 0

         if freeFallCounter > 10:
         	print("fall detected")
         	freeFallCounter = 0
         lc++
         print(az)

