
Name : Samiksha Agrawal
Roll no : 204101069
Project name: vowelRecognition
 
Vowel files are named as:
original file for training = 204101069_vowelname
file after applying dc shift and normalization = 204101069_vowelname_modified
referrence file off vowel = 204101069_modified_referrence
 original file for testing = 204101069_vowelname_test

IMPORTANT NOTE :
1)Training and testing files contains 10 utterances of same vowel per file.
2)Running time of whole programme is more than 50 seconds (including testing and training). 

SO, PLEASE WAIT AFTER HITTING THE RUN BUTTON (OUTPUT WILL BE DISPLAYED AFTER   50 seconds).

3) Overall accuracy is 96%  (48/50 vowels are recognised correctly {a-100%, e-90%, i-100%, o-100%, u-90%} ).

Programme flow is as follows:

1) In main after applying dc_shift and normalization, modified file is passed in training function(training function is called for each vowel with the help of a loop.)
2) training funtion will further call following funtions:
	i) markFrameHavingMaxSte : 
		In this function, we are marking the starting and end points of each vowel and between these markers, we are storing frame no. having max ste in a vector.
	ii)storing_5_frames_around_max_ste_and_calculating_ci:
		In this function, vector having max ste frame no for each vowel , is passed and we are storing 5 frames around each of those max ste frames and calculating ci for
		each of  these 5 frames in ci_of_frames array.
		ci_of_frames array contains 50 rows and 12 columns(50 rows for 5 frames of each of 10 utterances(5*10) and 12 columns for 12 ci's.)
	iii)creating_referrence_file
		this function is called only if my file is training file and will not be called for testing files.
		In this function, ci_of_frames array is passed, with the help of which referrence files for each vowel(5 vowels) is created.
		referrence file contains 5 rows and 12 columns as asked.
3)In main, after training function testing funtion is called for each vowel file.
4)testing funtion will further call following functions:
	i)training:
		 testing file will be trained.
	ii)store_referrence_file_in_a_vector:
		this function will store the referrence file created for that vowel in a vector.
	iii)tokuhara:
		this function will calculate the tokuhara distance of each vowel utterance from the referrence file created for that vowel
	output will be the vowel having minimum tokuhara distance.
	