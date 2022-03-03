# software-project

The assignments and final project we submitted for the software project class in Tel Aviv University
* First assignment - PCA
* Second assignment - Sparse Matrix
* Final Project - Community Structure in Networks

## Pricipal Componant Analysis

Principal Component Analysis (PCA) is a statistical procedure for dimensionality reduction,
enabling us to identify correlations and patterns in a data set so that it can be transformed
into a data set of significantly lower dimension with minimal loss of important information.

In this assignment, we implemented two C programs operating on matrices. The
first program receives an input matrix and outputs its covariance matrix. The second
program receives a covariance matrix and outputs an estimate of its eigenvector with the
highest eigenvalue. Using these two, we can easily compute the first principal component,
i.e., a single-dimension vector such that projecting the data onto it maximizes the variance
of the projected points.

## Sparse Matrix

A Sparse Matrix is a matrix in which most of the elements are zero, and often appears in
scientific or engineering applications. It can be very beneficial to take advantage of the
sparse structure of a matrix when storing and manipulating sparse matrices.

In this assignment, we implemented a C module with two implementations for
sparse matrices (linked lists and arrays), and used it to reimplement our eigenvector
program from the previous assignment. 
The program receives an input matrix and an implementation choice and outputs an estimate of its eigenvector
with the highest eigenvalue, by storing and manipulating the input matrix as a sparse matrix
using the chosen implementation.

## Community Structure in Networks

In complex networks, a network is said to have community structure if the nodes of the
network can be grouped into groups of nodes with dense connections internally, and sparser
connections between groups.

In this project we implemented an algorithm for detecting community structures (or
clusters) in a network.
