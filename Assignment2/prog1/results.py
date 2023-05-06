import json
import statistics
#open results.txt
results = open("results.txt", "r")
#read the file
lines = results.readlines()


res = {}

lines = [line for line in lines if line != '\n' and not line.startswith('#')]


for idx,process in enumerate([1,2,4,8]):
    
    number = idx*10
    line = lines[number:number+10]
    line = [float(l.strip()) for l in line]
    #average the 10 lines
    average = statistics.mean(line)
    stddev = statistics.stdev(line)
    
    
    res[process] = [average, stddev]

#put res in results_final.json
with open("ex1results_final8K.json", "w") as f:
    json.dump(res, f, indent=4)
    
