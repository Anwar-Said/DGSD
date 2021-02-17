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
	

    g = nx.erdos_renyi_graph(10, 0.7) # create a random graph with 10 nodes
    descriptor = dgsd.get_descriptor(g) # compute the descriptor

That's it! 

For more advanced usage, check out `online documentation <http://dgsd.readthedocs.org/>`_.


Requirements
------------
* numpy
* networkx
*multiprocessing



Installation
------------
#. cd dgsd
#. pip install -r requirements.txt 
#. python setup.py install

Or simply ``pip install dgsd``

Citing
------
