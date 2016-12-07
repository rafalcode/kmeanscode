# kmeanscode
C code for mainly Hartigan's view of the kmeans algorithm 

# Intro
Despite its enigmatic, though concise, name, the k-means algorithm is quite simplistic in nature. We have a large number of data points and we want to group them into a certain number - the "k" - of groups. That's about it really. The "k" in the name refers to the variable number of groups in which it is your desire to group them.

## Trivial
In fact there is one value of "k" in which we don't have to - technically - do anything: k=1. In other words we have only one group, so, by default, all data goes into this group. You could call this the "do-nothing" option, which often is invalid, but not in this case. Nevertheless it clearly is a facetious case, and in reality -even for this case - we would be asked for the centroid, i.e. the one value which represents the rest, so some wort of work is required.


# First prototype: hkmeans
We shall provide our program with two arguments. First a data file of two letter foodtype names, followed by three integers which are
the three features of the foodstuff. These values are hard coded for now, though our datafile can have any number of cases / data points ( in this hardcoded case we may refer to them as food points.

## test file
Our first simple one is fs.dat, there are only 8 cases, this is the number of rows, which we represent by m. n is the number of dimensions. k of course is the number of clusters.

## data structure
v\_t is simply for holding all the above 3 parameters together in one group. m is the number of points (aka cases aka foodpoints) and k is obvious, the number of clusters. n is the dimensionality of the datai.e how many paramters

# Returning after a long while
ALway disappointing when iyou come back and it doesn't work. The last-known good. It doesn't have to be the whole problems solved
(let's face it, it never is) but at the very least, it should have a working copy that represents progress so far.

hkmeans was available as a binary,even if hkmeans.c didn't compile properly. However, this binary threa out a segmentation fault, althouugh it does give some output

## actually only a few simple corrections were required.
The clusters variable cts, was named to b for some reason, and only partially. Getting rid of that solved the main noncomilation issue. There was also a trivial warning got rid of too.
