import json
import statistics
#open results.txt
results = open("results.txt", "r")
#read the file
lines = results.readlines()


res = {}

lines = [line for line in lines if line != '\n' and not line.startswith('#')]


for idx,file in enumerate(["dataSet2/datSeq256K.bin" ,"dataSet2/datSeq1M.bin" ,"dataSet2/datSeq16M.bin" ]) :
    res[file] = {}
    
    for idx2,process in enumerate([1,2,4,8]):
        number = idx*40 + idx2*10
        line = lines[number:number+10]
        line = [float(l.strip()) for l in line]
        #average the 10 lines
        average = statistics.mean(line)
        stddev = statistics.stdev(line)
        
        
        res[file][process] = [average, stddev]

#put res in results_final.json
with open("ex2results_final.json", "w") as f:
    json.dump(res, f, indent=4)
    
