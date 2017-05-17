

import networkx as nx
#import numpy as np
import matplotlib.pyplot as plt
#import pylab
#import csv
import sys
import os

cwd =os.getcwd()

print('Number of arguments:', len(sys.argv), 'arguments.')
print('Argument List:', str(sys.argv) )
filename = "/home/jakob/high-performance-computing-tma881/assignment5" + "/test_data/test_graph"
G = nx.read_edgelist(filename, delimiter=' ', data=[("weight", int)]) 
G.edges(data=True)


edge_labels = dict( ((u, v), d["weight"]) for u, v, d in G.edges(data=True) )
#pos = nx.random_layout(G)
#pos = nx.fruchterman_reingold_layout(G, k=1.0)
pos = nx.circular_layout(G)
nx.draw(G, pos)
nx.draw_networkx_nodes(G,pos,node_size=1000)

#nx.draw_networkx_nodes(G,pos,node_size=700)

nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels)
nx.draw_networkx_labels(G,pos,font_size=20,font_family='sans-serif')
plt.show()
#%%
