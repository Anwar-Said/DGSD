===============================
DGSD
===============================

DGSD is a statistical graph descriptor that computes a feature vector from graph which can be used for many downstream machine learning tasks. 

Quick start
-----------

.. code-block:: python

    import dgsd
    import multiprocessing
    import networkx as nx
	

    g = nx.erdos_renyi_graph(100, 0.2) # create a random graph with 100 nodes
    descriptor = dgsd.DGSD().get_descriptor(g,bins = 50, workers = 1) # compute the descriptor with given number of worker machines. Number of workers can be increased for fast and parallel processing

That's it! 



Requirements
------------
* numpy
* networkx
* multiprocessing



Installation
------------

Or simply ``pip install DGSD==1.0.4``

Citing
------
