# kmeanscode
C code for mainly Hartigan's view of the kmeans algorithm 

# Intro
Despite its enigmatic, though concise, name, the k-means algorithm is quite simplistic in nature. We have a large number of data points and we want to group them into a certain number - the "k" - of groups. That's about it really. The "k" in the name refers to the variable number of groups in which it is your desire to group them.

## Trivial
In fact there is one value of "k" in which we don't have to - technically - do anything: k=1. In other words we have only one group, so, by default, all data goes into this group. You could call this the "do-nothing" option, which often is invalid, but not in this case. Nevertheless it clearly is a facetious case, and in reality -even for this case - we would be asked for the centroid, i.e. the one value which represents the rest, so some wort of work is required.

