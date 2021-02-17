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
    descriptor = dgsd.DGSD().get_descriptor(g) # compute the descriptor

That's it! 



Requirements
------------
* numpy
* networkx
* multiprocessing



Installation
------------
#. cd dgsd
#. pip install -r requirements.txt 
#. python setup.py install

Or simply ``pip install dgsd``

Citing
------
