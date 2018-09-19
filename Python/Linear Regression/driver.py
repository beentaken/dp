import numpy as np
import lin_reg as algo
import matplotlib.pyplot as plt
import math
import csv
import time

def write_to_csv(X, outfile):
    with open(outfile, 'wb') as csvfile:
      writer = csv.writer(csvfile)
      writer.writerow(["CRIM", "ZN", "INDUS", "CHAS", "NOX", "RM", "AGE", "DIS", "RAD", "TAX", "PTRATIO", "B:1000", "LSTAT", "MEDV"])
      for i in range(len(X)):
        writer.writerow(X[i])

def testNorm(X_norm):
    xMerged = np.copy(X_norm[0])
    #merge datasets
    for i in range(len(X_norm)-1):
        xMerged = np.concatenate((xMerged,X_norm[i+1]))
    print np.mean(xMerged,axis=0)
    print np.sum(xMerged,axis=0)

def runGD(trainSet, testSet):
    startTime = time.time()
    # run GD for a maximum of 1000 times
    weights = algo.gradientDescent(trainSet, np.zeros((14, 1)), 0.5, 2000)
    runTime = time.time() - startTime
    print(runTime)
    # use testset and predict output using trained weights(model) while computing rmse
    N = len(testSet)
    predictedVals = [(np.dot(trainSet[i, 0:14], weights)) for i in range(N)]
    actualVals = [(trainSet[i, 14]) for i in range(N)]
    #compute rmse
    rmse = algo.rmse(predictedVals, actualVals) 
    return predictedVals, actualVals, rmse

def splitCV(X_norm, K):
    #shuffle and split the n-D array into a list of K n-D arrays
    np.random.shuffle(X_norm)
    return np.array_split(X_norm, K)

def k_fold_CV(folds):
    rmse_list = []
    N = len(folds)
    for i in range(N):
        #trainset set has all rows except the ith fold
        copy = np.delete(folds, i, 0)
        trainSet = np.copy(copy[0])
        for j in range(len(copy)-1):
            trainSet = np.concatenate((trainSet, copy[j+1]))
        #the ith element is the test set
        testSet = folds[i]
        #save training and test data
        np.savetxt("data/" + str(N) + "_" + str(i) + "_trainSet.txt", trainSet, fmt="%.5f")
        np.savetxt("data/" + str(N) + "_" + str(i) + "_testSet.txt", testSet, fmt="%.5f")
        predictedVals, actualVals, rmse = runGD(trainSet, testSet)
        rmse_list.append(rmse)
        #output into excel
        with open("output/rs_fold_" + str(N) + "_" + str(i) + ".csv", "wb") as outFile:
            writer = csv.writer(outFile)
            writer.writerow(["predictedY", "actualY"])
            for i in range(len(predictedVals)):
                writer.writerow([predictedVals[i][0], actualVals[i]])
    return rmse_list

def PlotGD(X, Theta, Alpha, Num_iters):
    count = 0
    errList = []
    while count != Num_iters:
        #use gradient descent to get new theta every iteration
        yDiffs = [(np.dot(X[i, 0:14], Theta)-X[i][-1]) for i in range(len(X))]
        newWeights = [algo.computeParam(Theta[i], i, Alpha, X, yDiffs) for i in range(len(Theta))]
        error = algo.errCompute(X, newWeights)
        errList.append(error)
        Theta = newWeights
        count += 1
    iterations = range(count)
    plt.plot(iterations, errList)
    plt.title("Iterations vs E(B) for Learning Rate:" + str(Alpha))
    plt.ylabel("Error function")
    plt.xlabel("Iteration #")
    plt.savefig("plots/Alpha_" + str(Alpha) + ".png")
    plt.clf()
    return errList[-1]
    
Alpha_list = [0.75, 0.5, 0.25, 0.1, 0.05, 0.01]
def PlotLearningRates(filename):
    X = algo.dataLoad(filename)
    X_norm = algo.dataNorm(X)
    folds = splitCV(X_norm, 10)
    copy = np.delete(folds, 0, 0)
    trainSet = np.copy(copy[0])
    for j in range(len(copy)-1):
        trainSet = np.concatenate((trainSet, copy[j+1]))
    for a in Alpha_list:
        lastErr = PlotGD(trainSet, np.zeros((14, 1)), a, 500)
        print(lastErr)

feature_dict = {"CRIME RATE(PER CAPITA)":1, "AGE OF BUILDING(YEARS)":7, "DISTANCE TO TOWN":8, "TAX RATE(PER $10000) ":10, "PROPORTION OF BLACKS":12}
def PlotXvsYScatter(filename):
    X = algo.dataLoad(filename)
    y_out = X[:, -1]
    for key in feature_dict:
        index = feature_dict[key]
        x_in = X[:, index]
        plt.scatter(x_in, y_out)
        plt.title(str(key) + " Against MEDIAN VALUE")
        plt.xlabel(str(key))
        plt.ylabel("MEDIAN VALUE (per $1000s)")
        plt.savefig(str(key) + " against output.png")
        plt.clf()
    


CV_list = [5, 10, 15]
def driver(filename):
    X = algo.dataLoad(filename)
    X_norm = algo.dataNorm(X)
    count = 1
    with open("rmse.csv", "wb") as resultFile:
        writer = csv.writer(resultFile)
        writer.writerow(["Fold_number", "rmse"])
        for val in CV_list:
            folds = splitCV(X_norm, val)
            rmse_list = k_fold_CV(folds)
            for rmse in rmse_list:
                writer.writerow([count, rmse])
                count += 1


#PlotLearningRates("housing.data")
driver("housing.data")
#PlotXvsYScatter("housing.data")