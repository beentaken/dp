import numpy as np
import math
import random
from scipy.special import expit as sigmoid

def loadData(filename):
  temp = np.loadtxt(filename)
  dummies = np.ones((4,))
  X = np.column_stack((dummies, temp))
  return X

def paraIni():
  weights = [[random.uniform(-1, 1) for i in range(3)] for i in range(3)]
  #weights for hidden layer (2 neurons)
  wh = np.array([weights[0], weights[1]])
  #weights for output layer (1 neurons)
  w0 = np.array([weights[2]])
  return[wh, w0]

def feedforward(X, W):
  #separate data into input and output
  features = X[:, :-1]
  #hidden layer params and output layer params
  Wh, Wo = W[0], W[1]
  #tanh is activation function for hidden layer
  oh = np.tanh(np.matmul(Wh, features.transpose()))
  #append row of ones for input for output layer
  ino = np.concatenate((np.ones((1,4)), oh))
  #sigmoid is activation function for output layer
  oo = sigmoid(np.matmul(Wo, ino))
  return [oh, ino, oo]

def errCompute(Y, Yhat):
  Y, Yhat = Y.flatten(), Yhat.flatten()
  N = len(Y)
  errors = [math.pow(Y[i] - Yhat[i], 2) for i in range(N)]
  return (1/(2.0 * N)) * sum(errors)

def backpropagate(X, W, intermRslt, alpha):
  #named variables according to slides
  oh, ino, oo = intermRslt[0], intermRslt[1], intermRslt[2]
  Wh, Wo = W[0], W[1][0]
  Yvec = X[:, -1]
  # compute delta for output layer
  one_minus_oo = np.subtract(np.ones((4,)), oo)
  temp = np.multiply(np.subtract(Yvec.transpose(), oo), oo)
  delta_o = np.multiply(temp, one_minus_oo)
  #compute new weights for output layer
  temp = np.matmul(delta_o, ino.transpose()) * (alpha/4.0)
  Wo_new = np.add(Wo, temp)
  #reshape for transpose and so matrix multiplication works (using old output layer weights)
  lhs = Wo[1:].reshape(-1, 1)
  rhs = delta_o.reshape(1, -1)
  #compute delta for hidden layer
  oh_sq = np.multiply(oh, oh)
  temp = np.subtract(np.ones((4,)), oh_sq)
  delta_h = np.multiply(np.matmul(lhs, rhs), temp)
  #compute weights for hidden layer 
  temp = np.matmul(delta_h, X[:, :-1]) * (alpha/4.0)
  Wh_new = np.add(Wh, temp) 
  #return the new weights
  return[Wh_new, Wo_new]