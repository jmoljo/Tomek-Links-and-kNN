# Tomek-Links-and-kNN
code for kNN

Contains the code for a k nearest neighbor classifier. The test data a square inside of a square. There are random points 
distributed throughout the squares. Points in the inner square should be one classifier and points in the outer square should
be another. The point are initialized and then a percentage of them are randomly flipped. Then we go through each point and if
it's nearest neighbor has the opposite classification then the pair is deleted. This is called a tomek link and helps to increase
accuracy of the classifier. The program outputs the accuracy of the classifier without tomek links and then with tomek links for 
various levels of noise. Starting with no noise and then increasing by 10 percentage points each time to 100% noise. The data file
here is an average of 100 trials
