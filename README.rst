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

simply ``pip install DGSD==1.0.4``

Citing -
------

@article{said2021dgsd,
  title={DGSD: Distributed graph representation via graph statistical properties},
  author={Said, Anwar and Hassan, Saeed-Ul and Tuarob, Suppawong and Nawaz, Raheel and Shabbir, Mudassir},
  journal={Future Generation Computer Systems},
  year={2021},
  publisher={Elsevier}
}
