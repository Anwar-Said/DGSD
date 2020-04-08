from multiprocessing import Pool
import multiprocessing as mp
import time
import networkx as nx
import numpy as np
from grakel import datasets
from sklearn.utils import shuffle
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import cross_val_score
import sys
import csv
from itertools import repeat

class DGSD:
    def __init__(self):
        self.graph = None

    def main(self, data, bins, workers,p):
        start = time.time()
        feature_matrix = []
        for i, graph in enumerate(data):
            self.graph = nx.convert_node_labels_to_integers(graph)
            nodes = list(self.graph.nodes())
            if workers<len(nodes):
                batches = np.array_split(nodes, workers)
            else:
                workers = len(nodes)
                batches = np.array_split(nodes, workers)
            end = time.time()
            print("time in main:",end-start)
            start = time.time()
            print(batches)
            # emb = self.compute_scores_k([self.graph])
            # emb = self.Generate_Embeddings(batches[0], bins)
            end = time.time()
            print("time in embedding:", end-start)
            emb = p.starmap(self.Generate_Embeddings, zip(batches, repeat(bins)))
            embeddings = np.sum(np.array(emb),axis = 0)
            feature_matrix.append(embeddings)
        return feature_matrix

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
                #print("common:", common, delta)
                delta = 0
                d.append(dist)
        hist, bin_edges = np.histogram(d, range=(0, 1), bins=nbins)
        return hist
    def return_dataset(self, file_name):
        dd = datasets.fetch_dataset(file_name, verbose=True)
        graph_list = []
        for gg in dd.data:
            v = set([i[0] for i in gg[0]]).union(set([i[1] for i in gg[0]]))
            g_ = nx.Graph()
            g_.add_nodes_from(v)
            g_.add_edges_from([(i[0], i[1]) for i in gg[0]])
            graph_list.append(g_)
        y = dd.target
        return graph_list, y

    def request_neighbors(self, node):
        N_i = list(self.graph.neighbors(node))
        return N_i

    def get_nodes(self):
        return self.graph.number_of_nodes()

    def apply_RF(self, feature_matrix, labels):
        feature_matrix, labels = shuffle(feature_matrix, labels)
        model = RandomForestClassifier(n_estimators=500)
        res = cross_val_score(model, feature_matrix, labels, cv=10, scoring='accuracy')
        return np.mean(res)


if __name__ == '__main__':
    dataset =  ["MUTAG"]
    workers = mp.cpu_count()-2
    dgsd = DGSD()
    p = Pool(workers)
    for d in dataset:
        data, y = dgsd.return_dataset(d)
        print("computing representations---")
        start = time.time()
        feature_matrix = dgsd.main(data, 20, workers,p)
        end = time.time()
        print("time for workers:", end - start)
        print("done: applying classifier")
        acc = dgsd.apply_RF(feature_matrix, y)
        print("10-fold cross validation accuracy: ", np.mean(acc))

    p.terminate()