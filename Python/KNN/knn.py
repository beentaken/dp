import math
import numpy as np
from operator import itemgetter

#normalizes a given dataset X (where X is a n-D array. The last column is not normalized)
def dataNorm(X):
    num_col = X.shape[-1]
    last_column = X[:, num_col-1]
    X_norm = (X - X.min(axis = 0)) / (X.ptp(axis = 0))
    X_norm[:, num_col-1] = last_column
    return X_norm

#split normalized dataset into training and test set, PercentTrain is the % of data for training
def splitTT(X_norm, PercentTrain):
    #shuffle the rows
    np.random.shuffle(X_norm)
    #find where to split
    sI = int(PercentTrain * len(X_norm))
    #training and test split (train set: everything until splitindex, test set: from split index to end)
    return X_norm[:sI], X_norm[sI:]

def splitCV(X_norm, K):
    #shuffle and split the n-D array into a list of K n-D arrays
    np.random.shuffle(X_norm)
    return np.array_split(X_norm, K)

#generalizes euclidean/manhattan and minkowski into a single function
def minkow_dist(A, B, length, order):
    if(order == 2):
        return eucl_dist(A, B, length)
    else:
        distance = 0.0
        num_attribs = length - 1  #dont include rings
        for i in range(num_attribs):
            distance += pow(abs(A[i] - B[i]), order)
        return (distance ** 1.0/order)

#finds the euclidean distance between 2 equal length tuples A and B given the number of elements in each tuple
def eucl_dist(A, B, length):
    distance = 0.0
    num_attribs = length - 1 #dont include rings
    for i in range(num_attribs):
        distance += pow((A[i] - B[i]), 2)
    return math.sqrt(distance)  

def getKNN(testData, dataset, num_cols, K, order):
    distance_list = []
    #pair distance and data in a list and sort based on smallest distance, only add last col(rings)
    for i in range(len(dataset)):
        dist = minkow_dist(testData, dataset[i], num_cols, order)
        distance_list.append((dist, dataset[i][-1]))
    distance_list = sorted(distance_list, key=itemgetter(0))
    #get the first K neighbours after sorting
    neighbours = []
    for i in range(K):
        neighbours.append(distance_list[i][1])
    return neighbours

def predict(neighbours, K):
    #compute the count for each unique label in neighbours
    instances, counts = np.unique(neighbours, return_counts = True)
    countlist = []
    for i in range(len(counts)):
        countlist.append((counts[i], instances[i]))
    #reverse so highest count will be first value in dict
    countlist = sorted(countlist, key = itemgetter(0), reverse = True)
    #take the first value as the result
    return countlist[0][1]

def KNN(X_train, X_test, K):
    N = len(X_test)
    num_col = X_test.shape[-1]
    correct = 0.0
    for i in range(N):
        neighbours = getKNN(X_test[i], X_train, num_col, K, 2)
        result = predict(neighbours, K)
        # if number of rings are correct
        if result == X_test[i][-1]:
             correct += 1.0
    #return percentage of correct as accuracy
    return (correct / float(N)) * 100.0

def KNNManhattan(X_train, X_test, K):
    N = len(X_test)
    num_col = X_test.shape[-1]
    correct = 0.0
    for i in range(N):
        #order is 1 for manhattan dist
        neighbours = getKNN(X_test[i], X_train, num_col, K, 1)
        result = predict(neighbours, K)
        # if number of rings are correct
        if result == X_test[i][-1]:
             correct += 1.0
    #return percentage of correct as accuracy
    return (correct / float(N)) * 100.0

def KNNMinkow(X_train, X_test, K):
    N = len(X_test)
    num_col = X_test.shape[-1]
    correct = 0.0
    for i in range(N):
        #order is 3 for minkowski dist
        neighbours = getKNN(X_test[i], X_train, num_col, K, 3)
        result = predict(neighbours, K)
        # if number of rings are correct
        if result == X_test[i][-1]:
             correct += 1.0
    #return percentage of correct as accuracy
    return (correct / float(N)) * 100.0


