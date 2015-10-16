# Project Setup (OSX)
1. Download OpenFrameworks 0.8.4
2. run setup the environment variable ```$OF_ROOT``` pointing to the downloaded/extracted folder.
3. ```make && make run```

# Training steps (OSX)
1. ```brew tap homebrew/science```
2. ```brew update```
3. ```brew install --with-tbb opencv``` or simply ```brew install opencv && brew reinstall $(brew deps opencv)```  @see https://github.com/Homebrew/homebrew-science/issues/951
4. In case of errors, ```sudo chown -R $USER /usr/local``` and then ```brew link eigen``` and ```brew link opencv```and ```brew install opencv```
5. go to ```bin/data/haar/cows``` (and follow the next steps, based on http://coding-robin.de/2013/07/22/train-your-own-opencv-haar-classifier.html)
6. ```find ./positive -iname "*.png" > positives.txt```
7. ```find ./negative -iname "*.png" > negatives.txt```
8. escape spaces in .txt files ```:%s/ /\\ /g``` (from vim)
10. https://github.com/wulfebw/mergevec (python solution)
