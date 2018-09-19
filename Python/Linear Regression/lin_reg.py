import numpy as np
import math

def dataLoad(filename):
    X=[]
    count = 0
    text_file = open(filename, "r")
    lines = text_file.readlines()
    for line in lines:
        X.append([])
        words = line.split()
        # convert values into float  
        for word in words:
            X[count].append(float(word))
        count += 1
    return np.asarray(X)

def dataNorm(X):
    num_col = X.shape[-1]
    last_column = X[:, num_col-1]
    X_norm = (X - X.min(axis = 0)) / (X.ptp(axis = 0))
    X_norm[:, num_col-1] = last_column
    #append column of ones as first element
    temp = np.ones(X_norm.shape[0])
    result = np.column_stack((temp, X_norm))
    return result

def errCompute(X_norm, Theta):
    sumErrSq = 0
    N = len(X_norm)
    for i in range(N):
        #y_out = predict(X_norm[i], Theta)
        yPrime = np.dot(X_norm[i, 0:14], Theta)
        err = X_norm[i][-1] - yPrime
        sumErrSq += (err * err)
    return sumErrSq / (2 * N)

def computeParam(currParam, j, alpha, X_norm, yDiffs):
    result = 0
    N = len(X_norm)
    for i in range(N):
        #(y-yprime) * x_ij
        result += yDiffs[i] * X_norm[i][j]
    result *= (alpha/float(N))
    return currParam - result
    

def gradientDescent(X, Theta, Alpha, Num_iters):
    converge = False
    prevError = 0.0
    tolerance = 0.000001  
    while Num_iters != 0 and converge == False:
        #use gradient descent to get new theta every iteration
        yDiffs = [(np.dot(X[i, 0:14], Theta)-X[i][-1]) for i in range(len(X))]
        newWeights = [computeParam(Theta[i], i, Alpha, X, yDiffs) for i in range(len(Theta))]
        #check for convergence
        error = errCompute(X, newWeights)
        converge = abs(prevError - error) <= tolerance
        #replace theta and prevError
        Theta = newWeights
        prevError = error
        Num_iters -= 1
    return Theta

def rmse(testY, stdY):
    N = len(testY)
    sumErr = 0.0
    for i in range(N):
        sumErr += (stdY[i] - testY[i]) ** 2
    return math.sqrt(sumErr/float(N))
