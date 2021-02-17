import multiprocessing as mp
import time
import networkx as nx
import numpy as np
from itertools import repeat
import csv
import math
import sys

class DGSD_class:
    def __init__(self):
        self.graph = None
    def get_descriptor(self, graph, bins, workers,p):
        self.graph = nx.convert_node_labels_to_integers(graph)
        nodes = list(self.graph.nodes())
        if workers<len(nodes):
            batches = np.array_split(nodes, workers)
        else:
            workers = len(nodes)
            batches = np.array_split(nodes, workers)
        emb = p.starmap(self.Generate_Embeddings, zip(batches, repeat(bins)))
        embeddings = np.sum(np.array(emb),axis = 0)
        return embeddings

    def Generate_Embeddings(self, batch, nbins):
        total_nodes = self.get_nodes()
        d = []
        for v in batch:
            N_v = self.request_neighbors(v)
            d_v = len(N_v)
            for u in range(total_nodes):
                if u == v:
                    d.append(0)
                    continue
                N_u = self.request_neighbors(u)
                delta = 0
                if u in N_v:
                    delta = 1
                else:
                    delta = 0
                d_u = len(N_u)
                common = len(list(set(N_u) & set(N_v)))
                dist = 0
                if (((d_u + d_v) + common + delta))>0:
                    dist = (d_u + d_v) / ((d_u + d_v) + common + delta)
                delta = 0
                d.append(dist)
        hist, bin_edges = np.histogram(d, range=(0, 1), bins=nbins)
        return hist

    def request_neighbors(self, node):
        N_i = list(self.graph.neighbors(node))
        return N_i

    def get_nodes(self):
        return self.graph.number_of_nodes()
    
