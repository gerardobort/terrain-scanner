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


find ./positive -iname "*.png" > positives.txt
find ./negative -iname "*.png" > negatives.txt
rm -Rf samples samples.vec classifier
mkdir classifier
cd negative
find . -iname "*.png" -exec ../conv.sh {} 50x50 \;
cd positive
find . -iname "*.png" -exec ../conv.sh {} 50x50 \;
perl createsamples.pl positives.txt negatives.txt samples 1500  "/usr/local/Cellar/opencv/2.4.12/bin/opencv_createsamples -bgcolor 0 -bgthresh 0 -maxxangle 1.1 -maxyangle 1.1 maxzangle 0.5 -maxidev 40 -w 50 -h 50"
# NEW CV XML versions
/usr/local/Cellar/opencv/2.4.12/bin/opencv_traincascade -data classifier -vec samples.vec -bg negatives.txt -numStages 20 -minHitRate 0.999 -maxFalseAlarmRate 0.5 -numPos 1000  -numNeg 600 -w 50 -h 50 -mode ALL -precalcValBufSize 1024
# OLD CV XML versions (OF)
/usr/local/Cellar/opencv/2.4.12/bin/opencv_haartraining -data classifier -vec samples.vec -bg negatives.txt -nstages 20 -minhitrate 0.999 -maxfalsealarmrate 0.5 -npos 400 -nneg 300 -w 50 -h 50 -mode ALL
