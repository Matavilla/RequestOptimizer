import random
import statistics
import os
import sys
import subprocess
import argparse

NumTests = 1
ErrorCount = 0
FirstAns = []
SecondAns = []

def createParser():
    parser = argparse.ArgumentParser(description="generate tests for topology")
    parser.add_argument('--dirName', type=str)
    parser.add_argument('--fileName', type=str)
    parser.add_argument('--typeClass', type=int)
    parser.add_argument('--numWork', type=int, default=10)
    parser.add_argument('--numTests', type=int, default=1)
    return parser

def checkAns(out):
    global MaxLen
    print(out)
    s = out.split('\n')
    l = s[0].split(' ')
    m = s[1].split(' ')
    tmp1 = float(l[1])
    tmp2 = float(l[3])
    tmp3 = float(m[3])
    return (tmp1, tmp2, tmp3)

def printAns():
    print("Errors: ", ErrorCount)
    print("First iter:")
    print(statistics.mean(FirstAns), "+-", statistics.stdev(FirstAns))
    print("Second iter:")
    print(statistics.mean(SecondAns), "+-", statistics.stdev(SecondAns))
    
def generateOneTest(fileName, data):
    text = ""
    f = open(f"{fileName}.xml", 'r')
    textArr = f.readlines()
    text = "".join(textArr)

    text += "<Tmax>"
    text += str(data.Tmax)
    text += "</Tmax>" + os.linesep

    for it in data.Works:
        text += "<Work>"
        text += "<N>" + str(it[0]) + "</N>"
        text += "<vCPU>" + str(it[1]) + "</vCPU>"
        text += "<vRAM>" + str(it[2]) + "</vRAM>"
        text += "<vHDD>" + str(it[3]) + "</vHDD>"
        text += "<vIO>" + str(it[4]) + "</vIO>"
        text += "<TBase>" + str(it[5]) + "</TBase>"
        text += "</Work>" + os.linesep
    text += "</Task>"
    return text

class Data:
    Works = []
    Tmax = 0

def generateData(namespace):
    data = Data()
    data.Works.clear()
    time = 0
    for i in range(NumWork):
        t = 0
        Work = [i]
        type = namespace.typeClass
        if type == 4:
            type = random.randint(0, 4)

        if type == 3:
            Work.append(random.randint(16, 32))
            Work.append(random.randint(32, 128))
            Work.append(16)
            Work.append(2)
            Work.append(400)
            time += 400
        elif type == 2:
            Work.append(random.randint(8, 16))
            Work.append(random.randint(16, 32))
            Work.append(16)
            Work.append(2)
            Work.append(200)
            time += 200
        else:
            Work.append(random.randint(1, 8))
            Work.append(random.randint(1, 16))
            Work.append(16)
            Work.append(2)
            Work.append(100)
            time += 100
        data.Works.append(Work)
    #data.Tmax = random.randint(time // 2, time)
    data.Tmax = random.randint(4000, 4500)
    return data

parser = createParser()
namespace = parser.parse_args(sys.argv[1:])
NumWork = namespace.numWork
NumTests = namespace.numTests

if not os.path.exists(namespace.dirName):
    os.makedirs(namespace.dirName)

os.system(f"rm -rf {namespace.dirName}/*")

for i in range(1, NumTests + 1):
    fileName = f"{namespace.dirName}/test_{i}.xml"
    
    data = generateData(namespace)
    
    f = open(fileName, 'w')
    f.write(generateOneTest(namespace.fileName, data))
    f.close()
    try:
        process = subprocess.run(f"./RequestOptimizer --dataPath {fileName}", shell=True, stdout=subprocess.PIPE)
    except:
        ErrorCount += 1
    else:
        baseCost, firstCost, secondCost = checkAns(process.stdout.decode('utf-8'))
        FirstAns.append(1 - firstCost / baseCost)
        SecondAns.append(1 - secondCost / baseCost)
printAns()
