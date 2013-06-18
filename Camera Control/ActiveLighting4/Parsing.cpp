#include <stdio.h>   /* required for file operations */
#include <conio.h>  /* for clrscr */
#include <stdlib.h>
#include <string.h>
#include <dos.h>  /* for delay */
#include "GUI.h"
#include "Parse.h"
#include "Utils.h"
#include "DSLRRemoteLib.h"

           /* declare the file pointer */



struct Line {  
	char line[100];   
	int index; 
} newLine = { "", 0};



bool getNextWord (struct Line &L, char word[])
{   
	  int i =0;

while ((L.line [L.index] == ' ' )|| (L.line [L.index] == '\t' ) ) //find the beginning of the next word
{   
	if (L.index==100)
		return false;
	L.index ++;
}

while ((L.line [L.index] != ' ')&&(L.line [L.index] != '\n') &&(L.line [L.index] != '\t') ) //copy the word until the next space is encountered
{	

	if (L.index==100)
		return false;
	word[i] = L.line[L.index];
	L.index++;
	i++;
}

word[i] = '\0';
return true;
}


void parse ()

{
	
	
	struct Line L;
	int temp, temp2;
	char all[100]= {"   "};

	L.index = 0;
	
	fr = fopen ("info.txt", "rt");  /* open the file for reading */
	
	
	while(fgets(L.line, 100, fr) != NULL)
	{
		L.index = 0;

		
		getNextWord(L, all);
		//printf ("%s ",all);


		if (strcmp(all, "exposure") ==0){
			int i = 0;
			///getNextWord(L, all);
			//temp = atoi(all);
			getNextWord(L, all);
			//temp2 = atoi(all);

			while ((strcmp (ext[i], all) != 0 ) && (i<36) )
				i++;
			zzz.push_back(ex[i]);
			exposures.push_back(i);
			char ovo [30];
		}

		if (strcmp(all, "AMBexposure") ==0){
			int i = 0;
		//	getNextWord(L, all);
		//	temp = atoi(all);
			getNextWord(L, all);
			//temp2 = atoi(all);

			while ((strcmp (ext[i], all) != 0 ) && (i<36) )
				i++;
			zzzAmb.push_back(ex[i]);
			exposuresAmb.push_back(i);
		}
		
		if (strcmp(all, "aperture") ==0){
			int i =0;
			getNextWord(L, all);
			sprintf (aperture, "%s", all);
			//  DisplayErrorMessageBox(all);
			while (strcmp (ap[i], all) != 0)
				i++;
			SetShutterAperture(-1, i-1);
			
		}
	
		if (strcmp(all, "projectors") ==0){
			getNextWord(L, all);
			
			temp = atoi(all);
		   
			P = temp;
		}

		if (strcmp(all, "lights") ==0){
			int i =0;
			l++;
			getNextWord(L, all);
			temp = atoi(all);
			//if (temp > L)
			//	L = temp;  //counting how many lighting conditions are to be taken
			getNextWord(L, all);
			for (i=0; i<8; i++)
			{
				allLights[temp-1][i] = all[i] - 48;
				//printf ("%d", lights[temp][i]);
			}
			
		}

		if (strcmp(all, "sceneName") ==0) {
			getNextWord(L, all);
			sprintf (scname, "%s", all);
		}

		if (strcmp(all, "directory") ==0) {
			getNextWord(L, all);
			sprintf (dir, "%s", all);
		}









		//printf ("%d %d ",temp, temp2);

	}

	//printf( "%s\\%s\\orig\\", dir, scname);
	//printf ("Exposure: %d, aperture: %s, Projectors: %d, Scene Name: %s, Directory: %s", exposures[0], aperture, P, scname, dir);
	//printf (path);

	//}
	fclose(fr);  /* close the file prior to exiting the routine */
} 
//void main(){
//
//	parse ();
//
//}

