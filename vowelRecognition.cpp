// aiforgivensample.cpp : Defines the entry point for the console application.
//
// BEFORE RUNNING THE CODE PLEASE READ README FILE.
#include "stdafx.h"
#include "stdio.h"
#include "vector"
#include "fstream"
#include "math.h"
#define ld long double
using namespace std;
#include "iostream"
#include "string.h"
#include "string"


// this function is to calculate ri for a frame(weights array contains weights of hamming window)
//hamming window is applied on ri's by multiplying weights
void ri_calculation(ld *R, vector<ld> &vec, ld  *weights)
{

	for(int i=0; i<=12; i++)
	{
		R[i]=0;
		for(int m=0;m<=319-i;m++)
			R[i]=R[i]+((vec[m]*weights[m])*((vec[m+i])*weights[m+i]));
	}
}

//this function is to calculate ai for a frame
void ai_calculation(ld *R, ld a[13][13], ld *k, ld *e)
{
	e[0]=R[0];
    if(R[0]==0){
        cout<<" Energy cannot be zero : ";
    }
    else
    {       
            e[1]=((R[0]*R[0])-(R[1]*R[1]))/R[0];
            k[1]=R[1]/R[0];
            a[1][1]=k[1];
            for(int i=2;i<13;i++)
            {
                ld sum1=0;
                for(int j=1;j<=i-1;j++)
                    sum1=sum1+(a[j][i-1]*R[i-j]);
                k[i]=(R[i]-sum1)/e[i-1];
                a[i][i]=k[i];
                e[i]=(1-(k[i]*k[i]))*e[i-1];  
                for(int j=1;j<=i-1;j++)
                    a[j][i]=a[j][i-1]-(k[i]*a[i-j][i-1]);
            }
			
            /*for(int i=1;i<=12;i++)
			{
				a[i][12]*=-1;
            }*/
    }
}

//this function is to calculate ci for a frame and storing it in ci_of_frames.
//IN ci_calculation parameter x is representing a frame. 
void ci_calculation(ld a[13][13], ld *c, ld ci_of_frames[100][13], int &x)
{
			for(int i=1;i<=12;i++)
            {
				ld sum1=0;
				for(int j=1;j<=i-1;j++)
					sum1=sum1+(j*c[j]*a[i-j][12])/i;
				c[i]=(sum1+a[i][12]);
            }
			ci_of_frames[x][0]=0;
			for(int i=1;i<=12;i++)
			{
				ci_of_frames[x][i]=c[i];
			}
}

//this function is just calculating dc_shift and normalization
void dc_shift_and_normalization_calculation(ld &dc_shift, ld &normalization_factor, const char *fname)
{
	ifstream file1;
	ld count=0,max_amplitude=0,min_amplitude=0;
	ld sum=0,w;
	int flag=0;
	file1.open(fname);
	while(file1>>w)
	{
		count++;
		sum+=w;
		if(w>max_amplitude)
			max_amplitude=w;
		if(w<min_amplitude)
			min_amplitude=w;
		//dc_shift is calculated for starting 3 frames(because they definitely conatain silence)
		if(count>=(320*3)&&flag==0)
		{
			dc_shift=sum/960;
			flag=1;
			count=0;
		}
	}
	if(abs(min_amplitude)>abs(max_amplitude))
		max_amplitude=abs(min_amplitude);
	//normalizing each amplitude in range of (-10,000 to 10,000)
	normalization_factor=10000/max_amplitude;
	file1.close();
}

void hamming_window(ld *weights)
{
	for(int i=0;i<320;i++)
		weights[i]=(0.54-(0.46*cos((2*3.14*i)/319)));
}

void raised_sine_window(ld *rsw)
{
	for(int i=1;i<=12;i++)
		rsw[i]=1+(6*sin((3.14*i)/12));
}

/* This funtion is applying dc-shift and normalization to amplitudes of my original file
and returning a new file with new amplitude values*/
string applying_dc_shift_and_normalization(ld &dc_shift, ld &normalization_factor, string fname)
{
	ld w;
	string filename= fname;
	filename+=".txt";
	fname+="_modified";
	string fn=fname;
	fname+=".txt";
	ifstream file1;
	file1.open(filename);
	ofstream modified_file;
	modified_file.open(fname);
	while(file1>>w)
	{
		modified_file<<(w-dc_shift)*normalization_factor<<endl;
	}
	file1.close();
	modified_file.close();
	return fn;
}

//IN this function we are marking start and end of vowel utterance and frame no which is having maxmium STE
//and storing that frame number in a vector
void markFrameHavingMaxSteForEachUtterance(string filename, vector<long int> &v)
{
	ifstream file1;
	int flag=0;
	int i=0;
	ld sum=0,w,ste,max_ste=0;
	long int count=0,frame;
	filename+=".txt";
	file1.open(filename);
	while(file1>>w)
	{
		count++;
		sum+=(w*w);
		if(count==320)
		{
			i++;
			ste=sum/320;
			if(ste>max_ste)
			{
				max_ste=ste;
				frame=i;
			}
			//ste threshold is taken to be 15000
			if(ste>=15000 &&  flag==0)
			{
				flag=1;
			}
			if(ste<15000 && flag==1)
			{
				flag=0;
				v.push_back(frame);
				max_ste=0;
			}
			count=0;
			sum=0.0;
		}
	}
	file1.close();
}

//storing 5 most stable framed around max ste frame for each utterance and calculating ci on each of these frames for each utterance.
void storing_5_frames_around_max_ste_and_calculating_ci(string filename,vector<long int> &v, ld ci_of_frames[100][13], ld* R, ld a[13][13], ld* k, ld *e, ld* c, ld *weights)
{
	ifstream file1;
	long int count=0, count_frame=0;
	int frame_no=0,i=0, count2=0;
	vector<ld> signal,vec;
	ld w;
	filename+=".txt";
	file1.open(filename);
	while(file1>>w)
	{
		if(i>=10)
			break;
			count++;
			if(count==320)
			{
				count_frame++;
				count=0;
			}
			if(count_frame==(v[i]-3))
			{
				int count1=0;
				//In vec we are storing amplitudes of 5 frames around max ste
				while(count1<=(5*320)-1)
				{
					vec.push_back(w);
					count1++;
					file1>>w;
				}
				count_frame+=5;
				long unsigned int j=1,m=0;
				long unsigned int x=0;
				while(x<vec.size())
				{
					//out of those 5 frames, we are storing each frame in signal and calculating ri,ci and ai.
					while(x<=(320*j)-1)
					{
						signal.push_back(vec[x]);
						x++;
					}
					ri_calculation(R, signal, weights);
					ai_calculation(R,a,k,e);
					ci_calculation(a,c, ci_of_frames,frame_no);
					frame_no++;
					signal.clear();
					j++;
				}
				i++;
				vec.clear();
			}
	}
	file1.close();
}

//creatig referrence file for each vowel containing 5 rows and 12 columns.
void create_referrence_file(ld ci_of_frames[100][13], string filename)
{
	
	ofstream file1;
	filename+="_referrence.txt";
	file1.open(filename);
	int count=0; ld avg_ci;
	while(count!=5)
	{
		for(int i=1; i<=12; i++)
		{
			ld sum=ci_of_frames[count][i];
			for(int j=(count+5);j<50;j+=5)
				sum+=ci_of_frames[j][i];
			//we are dividing by 10, because there are 10 utterances in a file
			avg_ci = sum/10;
			file1<<avg_ci<<" ";
		}
		count++;
		file1<<endl;
	}
}

//storing referrence file in a vector
vector<vector<ld>> store_referrence_file_in_a_vector(string filename)
{
	vector<vector<ld>> final_ci;
	ld w;
	ifstream file1;
	filename+=".txt";
	file1.open(filename);
	for(int i=0;i<5;i++)
	{
		final_ci.push_back(vector<ld>());
		for(long unsigned int j=0;j<12;j++)
		{
			file1>>w;
			final_ci[i].push_back(w);
		}
	}
	return final_ci;
}

//calculating tokuhara distance of each utterance in testing file  from referrence file for each vowel
void tokuhara(vector<vector<ld>> &ci_train, vector<vector<ld>> &ci_test, ld &final_distance)
{
	ld difference,tokuhara_distance;
	ld weight_T[]={1.0,3.0,7.0,13.0,19.0,22.0,25.0,33.0,42.0,50.0,56.0,61.0};
	for(long unsigned int i=0; i<ci_train.size();i++)
	{
		tokuhara_distance=0;
		for(int j=0;j<12;j++)
		{
			difference=ci_train[i][j]-ci_test[i][j];
			tokuhara_distance+=(difference*difference*weight_T[j]);
		}
		final_distance+=tokuhara_distance;
	}
}

//function for training 
void training(string fname, ld ci_of_frames[100][13])
{
	ld R[13]={0}, a[13][13], e[13]={0}, k[13]={0}, c[13]={0};
	vector<long int> v;
	ld weights[320], rsw[13];
	vector<ld> vec;
	hamming_window(weights);
	raised_sine_window(rsw);
	markFrameHavingMaxSteForEachUtterance(fname,v);
	storing_5_frames_around_max_ste_and_calculating_ci(fname,v,ci_of_frames, R,a,k,e,c,weights);
	string s1,s2,s3,s4,s5;
	s1="204101069_a_test_modified";
	s2="204101069_e_test_modified";
	s3="204101069_i_test_modified";
	s4="204101069_o_test_modified";
	s5="204101069_u_test_modified";
	if(fname!=s1 && fname!=s2 && fname!=s3 && fname!=s4 && fname!=s5)
		create_referrence_file(ci_of_frames, fname);
}

//function for testing
void testing(string filename, ld ci_of_frames[100][13])
{
	vector<vector<ld>> ci_training;
	vector<vector<ld>> ci_testing;
	training(filename, ci_of_frames);
	for(int i=0;i<50;i+=5)
	{
		for(int j=0; j<5; j++)
		{
			ci_testing.push_back(vector<ld>());
			//ci_testing contains 5 frames corresponding to each utterance and 12 ci values for each frame
			for(int m=1; m<=12; m++)
			{	
				ci_testing[j].push_back(ci_of_frames[i+j][m]);
			}
		}
		char vowel[]={'a','e', 'i', 'o', 'u'};
		char output;
		ld min=9999999.000,final_distance=0.0; 
		for(int x=0; x<5; x++)
		{
			string s;
			s="204101069_";
			s+=vowel[x];
			s+="_modified_referrence";
			ci_training= store_referrence_file_in_a_vector(s);
			tokuhara(ci_training,ci_testing, final_distance);
			ci_training.clear();
			if(final_distance<min)
			{
				min=final_distance;
				output=vowel[x];
			}
			final_distance=0.0;
		}
		cout<<"Tokuhara_Distance: "<<min<<endl;
		cout<<"Vowel Spoken: "<<output<<endl;
		ci_testing.clear();
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	ld dc_shift, normalization_factor;
	ld ci_of_frames[100][13];
	char vowel[5]={'a', 'e', 'i', 'o', 'u'};
	for(int i=0;i<5;i++)
	{
		string s;
		s="204101069_";
		s+=vowel[i];
		string s1=s;
		s+=".txt";
		const char *filename_original= s.c_str();
		dc_shift_and_normalization_calculation(dc_shift, normalization_factor, filename_original);
		string filename = applying_dc_shift_and_normalization(dc_shift, normalization_factor, s1);
		cout<<"Training of "<<vowel[i]<<" started"<<endl;
		training(filename, ci_of_frames);
	}
	cout<<"Training finished"<<endl;
	cout<<endl;
	for(int i=0;i<5;i++)
	{
		string s;
		s="204101069_";
		s+=vowel[i];
		string s1=s;
		s+="_test.txt";
		s1+="_test";
		const char *filename_original= s.c_str();
		dc_shift_and_normalization_calculation(dc_shift, normalization_factor, filename_original);
		string filename = applying_dc_shift_and_normalization(dc_shift, normalization_factor, s1);
		cout<<"testing of "<<vowel[i]<<" started"<<endl;
		testing(filename, ci_of_frames);
		cout<<endl;
		cout<<"-----------------------------------"<<endl;
		cout<<endl;
	}
	return 0;
}



