import numpy as np
import knn as algo
import csv
import time
from a2 import loadData

TT_list = [0.7, 0.6, 0.5]
CV_list = [5, 10, 15]
K_list = [1, 5, 10, 15, 20]
typeList = ["normal", "manhattan", "minkow"]

def TestKNN(train, test, algoType, K):
    accuracy = 0.0
    startTime = time.time()
    if algoType == "normal":
        accuracy = algo.KNN(train, test, K)
    elif algoType == "manhattan":
        accuracy = algo.KNNManhattan(train, test, K)
    elif algoType == "minkow":
        accuracy = algo.KNNMinkow(train, test, K)
    runTime = time.time() - startTime
    return accuracy, runTime

def k_fold_CV(folds, K, algoType):
    N = len(folds)
    total_acc = 0.0
    total_time = 0.0
    for i in range(N):
        #trainset set has all rows except the ith element
        copy = np.delete(folds, i, 0)
        trainSet = np.copy(copy[0])
        for j in range(len(copy)-1):
            trainSet = np.concatenate((trainSet, copy[j+1]))
        #the ith element is the test set
        testSet = folds[i]
        accuracy, runtime = TestKNN(trainSet, testSet, algoType, K)
        total_acc += accuracy
        total_time += runtime
    avg_acc = total_acc/float(N)
    avg_time = total_time/float(N)
    return avg_acc, avg_time

def runKFold(K, algoType, X_norm):
    accList = []
    timeList = []
    for val in CV_list:
        folds = algo.splitCV(X_norm, val)
        acc, time = k_fold_CV(folds, K, algoType)
        accList.append(acc)
        timeList.append(time)
    return accList, timeList

def runTT(K, algoType, X_norm):
    print "In function"
    accList = []
    timeList = []
    for val in TT_list:
        train, test = algo.splitTT(X_norm, val)
        acc, time = TestKNN(train, test, algoType, K)
        accList.append(acc)
        timeList.append(time)
    return accList, timeList

def driver(filename):
    #data load
    X = loadData(filename)
    #normalization
    X_norm = algo.dataNorm(X)
    for algoType in typeList:
        accs_list = []
        times_list = []
        for K in K_list:
            TT_accs, TT_times = runTT(K, algoType, X_norm)
            CV_accs, CV_times = runKFold(K, algoType, X_norm)
            accs = TT_accs + CV_accs
            times = TT_times + CV_times
            accs_list.append(accs)
            times_list.append(times)
        with open(algoType + "_accuracy.csv", 'wb') as accFile:
            accWriter = csv.writer(accFile)
            accWriter.writerow(["Accuracy", "Train and Test", "Cross Validation"])
            accWriter.writerow(["0.7-0.3", "0.6-0.4", "0.5-0.5", "5-fold", "10-fold", "15-fold"])
            for i in range(len(accs_list)):
                accs_list[i].insert(0, K_list[i])
                accWriter.writerow(accs_list[i])
        with open(algoType + "_runtime.csv", 'wb') as timeFile:
            timeWriter = csv.writer(timeFile)
            timeWriter.writerow(["Runtime", "Train and Test", "Cross Validation"])
            timeWriter.writerow(["0.7-0.3", "0.6-0.4", "0.5-0.5", "5-fold", "10-fold", "15-fold"])
            for i in range(len(times_list)):
                times_list[i].insert(0, K_list[i])
                timeWriter.writerow(times_list[i])   
            
driver("abalone.data")



        
