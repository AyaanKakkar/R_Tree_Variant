createDataset :
	g++ -Ofast -o createDataset rectangle.cpp createDataset.cpp generate.cpp -I .
buildTree:
	g++ -Ofast -o buildTree load.cpp leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp buildTree.cpp -I .
experiments:
	g++ -o experiments saveGraph.cpp leafNode.cpp rectangle.cpp rtree.cpp treeNode.cpp experiments.cpp -I .