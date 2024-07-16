import sys
import pickle

if len(sys.argv) < 2:
    print("Usage: python view_grapth.py path/to/graph.pickle")
    sys.exit(-1)

figx = pickle.load(open(sys.argv[1], "rb"))

figx.show()
input("enter something to quit: ")
