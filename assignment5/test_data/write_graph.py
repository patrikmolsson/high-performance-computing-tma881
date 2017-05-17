from random import randint

def write_graph(g, w, file_name):
  with open(file_name, 'w') as f:
    for v in g.vertices():
      for e in g[v]:
        f.write("{} {} {}\n".format(v, e, randint(1,w)))

def write_graphdn(d, n, w, file_name):
  with open(file_name, 'w') as f:
    for v in range(n):
      edges = set([])
      while len(edges) != d:
        edges.add(randint(0,n-1))
      for e in edges:
        f.write("{} {} {}\n".format(v, e, randint(1,w)))
