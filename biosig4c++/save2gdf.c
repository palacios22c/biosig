/*

    $Id: save2gdf.c,v 1.56 2009/04/15 20:32:03 schloegl Exp $
    Copyright (C) 2000,2005,2007,2008 Alois Schloegl <a.schloegl@ieee.org>
    Copyright (C) 2007 Elias Apostolopoulos
    This file is part of the "BioSig for C/C++" repository 
    (biosig4c++) at http://biosig.sf.net/ 
 

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 
    
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "biosig-dev.h"

#ifndef INF
#define INF (1.0/0.0)
#endif 

#ifdef __cplusplus
extern "C" {
#endif 
int savelink(const char* filename);
#ifdef __cplusplus
}
#endif 

#ifdef WITH_PDP 
void sopen_pdp_read(HDRTYPE *hdr);
#endif


int main(int argc, char **argv){
    
    HDRTYPE 	*hdr,*rrFile=NULL; 
    size_t 	count, k1, ne=0;
    uint16_t 	numopt = 0;
    char 	*source, *dest, tmp[1024]; 
    enum FileFormat SOURCE_TYPE, TARGET_TYPE=GDF; 		// type of file format
    int		COMPRESSION_LEVEL=0;
    int		status, k; 
    int		TARGETSEGMENT=1; 	// select segment in multi-segment file format EEG1100 (Nihon Kohden)
    int 	VERBOSE	= 1; 
	
    if (argc<2)
    	;
    else 
    {
    	for (k=1; k<argc && argv[k][0]=='-'; k++)
    	if (!strcmp(argv[k],"-v") || !strcmp(argv[k],"--version") ) {
		fprintf(stdout,"save2gdf (BioSig4C++) v%04.2f\n", BIOSIG_VERSION);
		fprintf(stdout,"Copyright (C) 2006,2007,2008,2009 by Alois Schloegl and others\n");
		fprintf(stdout,"This file is part of BioSig http://biosig.sf.net - the free and\n");
		fprintf(stdout,"open source software library for biomedical signal processing.\n\n");
		fprintf(stdout,"BioSig is free software; you can redistribute it and/or modify\n");
		fprintf(stdout,"it under the terms of the GNU General Public License as published by\n");
		fprintf(stdout,"the Free Software Foundation; either version 3 of the License, or\n");
		fprintf(stdout,"(at your option) any later version.\n\n");
	}	
    	else if (!strcmp(argv[k],"-h") || !strcmp(argv[k],"--help") ) {
		fprintf(stdout,"\nusage: save2gdf [OPTIONS] SOURCE DEST\n");
		fprintf(stdout,"  SOURCE is the source file \n");
		fprintf(stdout,"      SOURCE can be also network file bscs://<hostname>/ID e.g. bscs://129.27.3.99/9aebcc5b4eef1024 \n");
		fprintf(stdout,"  DEST is the destination file \n");
		fprintf(stdout,"      DEST can be also network server bscs://<hostname>\n");
		fprintf(stdout,"      The corresponding ID number is reported and a bscs-link file is stored in /tmp/<SOURCE>.bscs\n");
		fprintf(stdout,"\n  Supported OPTIONS are:\n");
		fprintf(stdout,"   -v, --version\n\tprints version information\n");
		fprintf(stdout,"   -h, --help   \n\tprints this information\n");
		fprintf(stdout,"   -r, --ref=MM  \n\trereference data with matrix file MM. \n\tMM must be a 'MatrixMarket matrix coordinate real general' file.\n");
		fprintf(stdout,"   -f=FMT  \n\tconverts data into format FMT\n");
		fprintf(stdout,"\tFMT must represent a valid target file format\n"); 
		fprintf(stdout,"\tCurrently are supported: HL7aECG, SCP_ECG (EN1064), GDF, EDF, BDF, CFWB, BIN, ASCII\n"); 
		fprintf(stdout,"   -z=#, compression level \n");
		fprintf(stdout,"\t#=0 no compression; #=9 best compression\n");
		fprintf(stdout,"   -s=#\tselect target segment # (in the multisegment file format EEG1100)\n");
		fprintf(stdout,"   -VERBOSE=#, verbosity level #\n\t0=silent, 9=debugging");
		fprintf(stdout,"\n\n");
		return(0);
	}	
    	else if (!strncmp(argv[k],"-z",3))  	{
#ifdef ZLIB_H
		COMPRESSION_LEVEL = 1;  
		if (strlen(argv[k])>3) {
	    		COMPRESSION_LEVEL = argv[k][3]-48;
	    		if (COMPRESSION_LEVEL<0 || COMPRESSION_LEVEL>9)
				fprintf(stderr,"Error %s: Invalid Compression Level %s\n",argv[0],argv[k]); 
    		}   
#else
	     	fprintf(stderr,"Warning: option -z (compression) not supported. zlib not linked.\n");
#endif 
	}
    	else if (!strncmp(argv[k],"-VERBOSE",2))  	{
	    	VERBOSE = argv[k][strlen(argv[k])-1]-48;
#ifndef VERBOSE_LEVEL
	// then VERBOSE_LEVEL is not a constant but a variable
	VERBOSE_LEVEL = VERBOSE; 
#endif
	}
    	else if (!strncmp(argv[k],"-f=",3))  	{
    		if (0) {}
    		else if (!strncmp(argv[k],"-f=ASCII",8))
			TARGET_TYPE=ASCII;
    		else if (!strcmp(argv[k],"-f=BDF"))
			TARGET_TYPE=BDF;
    		else if (!strncmp(argv[k],"-f=BIN",6))
			TARGET_TYPE=BIN;
    		else if (!strncmp(argv[k],"-f=BVA",6))
			TARGET_TYPE=BrainVision;
    		else if (!strncmp(argv[k],"-f=CFWB",7))
			TARGET_TYPE=CFWB;
    		else if (!strcmp(argv[k],"-f=EDF"))
			TARGET_TYPE=EDF;
    	 	else if (!strcmp(argv[k],"-f=GDF"))
			TARGET_TYPE=GDF;
    		else if (!strcmp(argv[k],"-f=GDF1"))
			TARGET_TYPE=GDF1;
    		else if (!strncmp(argv[k],"-f=HL7",6))
			TARGET_TYPE=HL7aECG;
    		else if (!strncmp(argv[k],"-f=MFER",7))
			TARGET_TYPE=MFER;
    		else if (!strncmp(argv[k],"-f=SCP",6))
			TARGET_TYPE=SCP_ECG;
//    		else if (!strncmp(argv[k],"-f=TMSi",7))
//			TARGET_TYPE=TMSiLOG;
		else {
			fprintf(stderr,"format %s not supported.\n",argv[k]);
			return(-1);
		}	
	}

    	else if (!strncmp(argv[k],"-r=",3) || !strncmp(argv[k],"--ref=",6) )	{
    	        // re-referencing matrix 
#ifdef WITH_CHOLMOD
    	        rrFile = sopen(strchr(argv[k],'=')+1,"r",NULL); 
    	        
		if (VERBOSE_LEVEL>8) fprintf(stdout,"[rrFile] %Li %Li \n", rrFile->Calib->nrow, rrFile->Calib->ncol); 
#else
                fprintf(stdout,"error: option %s not supported (compile with -D=WITH_CHOLMOD)\n",argv[k]); 
#endif
	}

    	else if (!strncmp(argv[k],"-s=",3))  	{
    		TARGETSEGMENT = atoi(argv[k]+3);
	}

	numopt = k-1;	
		
    }


	source = NULL;
	dest = NULL;
    	switch (argc - numopt) {
    	case 1:
		fprintf(stderr,"save2gdf: missing file argument\n");
		fprintf(stdout,"usage: save2gdf [options] SOURCE DEST\n");
		fprintf(stdout," for more details see also save2gdf --help \n");
		exit(-1);
    	case 3:
    		dest   = argv[numopt+2]; 
    	case 2:
	    	source = argv[numopt+1]; 
    	}	

	if (VERBOSE_LEVEL<0) VERBOSE=1; // default 
	if (VERBOSE_LEVEL>8) fprintf(stdout,"[111] SAVE2GDF started\n");

	tzset();
	hdr = constructHDR(0,0);
	// hdr->FLAG.OVERFLOWDETECTION = FlagOverflowDetection; 
	hdr->FLAG.UCAL = ((TARGET_TYPE==BIN) || (TARGET_TYPE==ASCII));
	// hdr->FLAG.ROW_BASED_CHANNELS = 0; 
	hdr->FLAG.TARGETSEGMENT = TARGETSEGMENT;


	hdr->FileName = source;
	hdr = sopen(source, "r", hdr);
#ifdef WITH_PDP 
	if (B4C_ERRNUM) {
		B4C_ERRNUM = 0;  
		sopen_pdp_read(hdr);
	}	
#endif

	if (VERBOSE_LEVEL>8) fprintf(stdout,"[112] SOPEN-R finished\n");

	if ((status=serror())) {
		destructHDR(hdr);
		destructHDR(rrFile);
		exit(status); 
	} 
	
	if (VERBOSE_LEVEL>8) fprintf(stdout,"[113] SOPEN-R finished\n");

	hdr2ascii(hdr,stdout,VERBOSE);
//	hdr2ascii(hdr,stdout,3);

	// all channels are converted - channel selection currently not supported
    	for (k=0; k<hdr->NS; k++) {
    		if (!hdr->CHANNEL[k].OnOff && hdr->CHANNEL[k].SPR) {
			if ((hdr->SPR/hdr->CHANNEL[k].SPR)*hdr->CHANNEL[k].SPR != hdr->SPR)
				 fprintf(stdout,"Warning: channel %i might be decimated!\n",k+1);
    		};
    		// hdr->CHANNEL[k].OnOff = 1;	// convert all channels
    	}	

	hdr->FLAG.OVERFLOWDETECTION = 0;
	hdr->FLAG.UCAL = (rrFile==NULL);
	hdr->FLAG.ROW_BASED_CHANNELS = (rrFile!=NULL);
	
	if (VERBOSE_LEVEL>8) fprintf(stdout,"[121]\n");

	if (dest!=NULL)
		count = sread(NULL, 0, hdr->NRec, hdr);

	biosig_data_type* data = hdr->data.block;
	if ((VERBOSE_LEVEL>8) && (hdr->data.size[0]*hdr->data.size[1]>500))
		fprintf(stdout,"[122] UCAL=%i %e %e %e \n",hdr->FLAG.UCAL,data[100],data[110],data[500+hdr->SPR]);
	
	if ((status=serror())) {
		destructHDR(hdr);
		destructHDR(rrFile);
		exit(status);
	};

	if (VERBOSE_LEVEL>8) 
		fprintf(stdout,"\n[129] SREAD on %s successful [%i,%i].\n",hdr->FileName,hdr->data.size[0],hdr->data.size[1]);

//	fprintf(stdout,"\n %f,%f.\n",hdr->FileName,hdr->data.block[3*hdr->SPR],hdr->data.block[4*hdr->SPR]);
	if (VERBOSE_LEVEL>8) 
		fprintf(stdout,"\n[130] File  %s =%i/%i\n",hdr->FileName,hdr->FILE.OPEN,hdr->FILE.Des);

	if (dest==NULL) {
		if (ne)	/* used for testig SFLUSH_GDF_EVENT_TABLE */
		{	
			if (hdr->EVENT.N > ne)
				hdr->EVENT.N -= ne;
			else 
				hdr->EVENT.N  = 0;
					
			// fprintf(stdout,"Status-SFLUSH %i\n",sflush_gdf_event_table(hdr));
		}	
		
		if (VERBOSE_LEVEL>8) fprintf(stdout,"[131] going for SCLOSE\n");
		sclose(hdr);
		if (VERBOSE_LEVEL>8) fprintf(stdout,"[137] SCLOSE finished\n");
		destructHDR(hdr);
		destructHDR(rrFile);
		exit(serror());
	}

	if (hdr->FILE.OPEN){
		sclose(hdr); 
		free(hdr->AS.Header);
		hdr->AS.Header = NULL;
		if (VERBOSE_LEVEL>8) fprintf(stdout,"[138] file closed\n");
	}
	if (VERBOSE_LEVEL>8) 
		fprintf(stdout,"\n[139] File %s closed sd=%i/%i\n",hdr->FileName,hdr->FILE.OPEN,hdr->FILE.Des);

	SOURCE_TYPE = hdr->TYPE;
	hdr->TYPE = TARGET_TYPE;
	if ((hdr->TYPE==GDF) && (hdr->VERSION<2)) hdr->VERSION = 2.0;

	hdr->FILE.COMPRESSION = COMPRESSION_LEVEL;

   /********************************* 
   	re-referencing 
   *********************************/
#ifdef WITH_CHOLMOD
        if (rrFile) {
        	if (VERBOSE_LEVEL>8) fprintf(stdout,"\nrereferencing\n");
                /* TODO: 
                        - check physdimcode
                        - if FLAG.UCAL: check scaling
                        - rereferencing
                        - order channel information
                        - set GDFTYP = 17 (double)
                */
                uint16_t i,j;
                char flag = 0;
                for (i=1; i<hdr->NS; i++)
			if (hdr->CHANNEL[i].OnOff > hdr->CHANNEL[i-1].OnOff) {
			/* a more sophisticated check would test whether any of these channels is actually used
			*/
				fprintf(stderr,"Warning: possible channel mix-up. \n");
				break;
			}

        	if (VERBOSE_LEVEL>8) fprintf(stdout,"\n403\n");

                cholmod_dense *Cd;
                cholmod_common c;
                cholmod_start(&c); // start CHOLMOD 
                //c.print = 5;
                //cholmod_print_sparse(rrFile->Calib,"Calib(sparse)",&c);
                Cd = cholmod_sparse_to_dense(rrFile->Calib, &c); /* sparse_to_dense */
                //cholmod_print_dense(Cd,"Calib(dense)",&c);
                cholmod_finish(&c); /* finish CHOLMOD */

		int newNS = Cd->ncol;
        	if (VERBOSE_LEVEL>8) fprintf(stdout,"\n403\n");

		CHANNEL_TYPE *NEWCHANNEL = (CHANNEL_TYPE*) malloc(newNS*sizeof(CHANNEL_TYPE));
		for (i=0; i<newNS; i++) {
			flag = 0;
			int mix = -1, oix = -1, pix = -1;
			double m  = 0.0;
			double *v = Cd->x + i*Cd->nrow;
			for (j=0; j<Cd->nrow; j++) {

               	if (VERBOSE_LEVEL>8) fprintf(stdout,"\n404 %i %i %f\n",i,j,v);

				if (v[j]>m) {
					m=v[j];
					mix=j;
				}
				if (v[j]==1.0) {
					if (oix<0) 
						oix = j;
					else
						fprintf(stderr,"Warning: ambiguous channel information (in new #%i, more than one scaling factor of 1.0 is used.) \n",i,j);
				}
				if (v[j]) {
					if (pix == -1)
						pix = hdr->CHANNEL[j].PhysDimCode;
					else if (pix != hdr->CHANNEL[j].PhysDimCode)
						flag = 1;
				}
			}

        	if (VERBOSE_LEVEL>8) fprintf(stdout,"\n406 %i %i\n",mix,oix);

			if (mix>-1) j=mix;
			else if (oix>-1) j=oix; 
			else j = -1;

			if (!flag && (j<hdr->NS)) {

        	if (VERBOSE_LEVEL>8) fprintf(stdout,"\n407 %i %i\n",i,j);

				memcpy(NEWCHANNEL+i, hdr->CHANNEL+j, sizeof(CHANNEL_TYPE));
				NEWCHANNEL[i].GDFTYP = 17; // double
                        }
			else {
				fprintf(stderr,"Error: check for channel information failed) (%i %i)\n",i,j);
				free(NEWCHANNEL);
				destructHDR(hdr);
				destructHDR(rrFile);
				exit(-1);
			}
                }


        	if (VERBOSE_LEVEL>7) fprintf(stdout,"\n407 %i %i\n",i,j);
        	
		
        	{
        		cholmod_dense X,Y;
			X.nrow = hdr->data.size[0];
			X.ncol = hdr->data.size[1];
			X.d    = hdr->data.size[0];
			X.nzmax= hdr->data.size[1]*hdr->data.size[0];
			X.x    = hdr->data.block;
                        X.xtype = CHOLMOD_REAL;
                        X.dtype = CHOLMOD_DOUBLE;

			Y.nrow = rrFile->Calib->ncol;
			Y.ncol = hdr->data.size[1];
			Y.d    = Y.nrow;
			Y.nzmax= Y.nrow * Y.ncol;
			Y.x    = malloc(Y.nzmax*sizeof(double)); 
                        Y.xtype = CHOLMOD_REAL;
                        Y.dtype = CHOLMOD_DOUBLE;
			cholmod_common C; 
			double alpha[]={1,0},beta[]={0,0};

                        cholmod_common c ;
                        cholmod_start (&c) ; // start CHOLMOD 
/*
                        c.print = 4; 
                        cholmod_print_sparse(rrFile->Calib.Cs,"Calib(sparse)",&c);
                        cholmod_print_dense(rrFile->Calib.Cd,"Calib(dense)",&c);
                        cholmod_print_dense(&X,"X",&c);
                        cholmod_print_dense(&Y,"Y",&c);
*/

			cholmod_sdmult(rrFile->Calib,1,alpha,beta,&X,&Y,&c);
                        cholmod_finish (&c) ; /* finish CHOLMOD */

			if (VERBOSE_LEVEL>8) fprintf(stdout,"%f -> %f\n",*(double*)X.x,*(double*)Y.x);
			free(X.x);
			hdr->data.block = Y.x;
                        data = hdr->data.block;
			hdr->data.size[1] = Y.ncol;
        		hdr->NS = newNS; 
				
                        free(hdr->CHANNEL);
                        hdr->CHANNEL = NEWCHANNEL;
                }
        }

    else 
#endif         // WITH_CHOLMOD 
   /********************************* 
   	Write data 
   *********************************/
   {	double PhysMaxValue0 = -INF; //hdr->data.block[0];
	double PhysMinValue0 = +INF; //hdr->data.block[0];
	double val; 
	size_t N = hdr->NRec*hdr->SPR;
	int k2=0;
    	for (k=0; k<hdr->NS; k++)
    	if (hdr->CHANNEL[k].OnOff && hdr->CHANNEL[k].SPR) 
    	{
		double MaxValue = hdr->data.block[k2*N];
		double MinValue = hdr->data.block[k2*N];
		
		/* Maximum and Minimum for channel k */ 
		for (k1=1; k1<N; k1++) {

			if (MaxValue < hdr->data.block[k2*N+k1])
		 		MaxValue = hdr->data.block[k2*N+k1];
	 		if (MinValue > hdr->data.block[k2*N+k1])
	 			MinValue = hdr->data.block[k2*N+k1];
		}

		if (!hdr->FLAG.UCAL) {
			MaxValue = (MaxValue - hdr->CHANNEL[k].Off)/hdr->CHANNEL[k].Cal;
			MinValue = (MinValue - hdr->CHANNEL[k].Off)/hdr->CHANNEL[k].Cal;
		}
		val = MaxValue * hdr->CHANNEL[k].Cal + hdr->CHANNEL[k].Off;		
		if (PhysMaxValue0 < val)
			PhysMaxValue0 = val;
		val = MinValue * hdr->CHANNEL[k].Cal + hdr->CHANNEL[k].Off;		
 		if (PhysMinValue0 > val)
 			PhysMinValue0 = val;

		if ((SOURCE_TYPE==alpha) && (hdr->CHANNEL[k].GDFTYP==(255+12)) && (TARGET_TYPE==GDF)) 
			// 12 bit into 16 bit 
;//			hdr->CHANNEL[k].GDFTYP = 3;
		else if ((SOURCE_TYPE==ETG4000) && (TARGET_TYPE==GDF)) {
			hdr->CHANNEL[k].GDFTYP  = 16;
			hdr->CHANNEL[k].PhysMax = MaxValue * hdr->CHANNEL[k].Cal + hdr->CHANNEL[k].Off;
			hdr->CHANNEL[k].PhysMin = MinValue * hdr->CHANNEL[k].Cal + hdr->CHANNEL[k].Off;
			hdr->CHANNEL[k].DigMax  = MaxValue;
			hdr->CHANNEL[k].DigMin  = MinValue;
		}
		else if ((SOURCE_TYPE==GDF) && (TARGET_TYPE==GDF)) 
			;
		else if ((hdr->CHANNEL[k].GDFTYP<10 ) && (TARGET_TYPE==GDF || TARGET_TYPE==CFWB)) {
			/* heuristic to determine optimal data type */
			if ((MaxValue <= 127) && (MinValue >= -128))
		    		hdr->CHANNEL[k].GDFTYP = 1;
			else if ((MaxValue <= 255.0) && (MinValue >= 0.0))
			    	hdr->CHANNEL[k].GDFTYP = 2;
			else if ((MaxValue <= ldexp(1.0,15)-1.0) && (MinValue >= ldexp(-1.0,15)))
		    		hdr->CHANNEL[k].GDFTYP = 3;
			else if ((MaxValue <= ldexp(1.0,16)-1.0) && (MinValue >= 0.0))
			    	hdr->CHANNEL[k].GDFTYP = 4;
			else if ((MaxValue <= ldexp(1.0,31)-1.0) && (MinValue >= ldexp(-1.0,31)))
		    		hdr->CHANNEL[k].GDFTYP = 5;
			else if ((MaxValue <= ldexp(1.0,32)-1.0) && (MinValue >= 0.0))
		    		hdr->CHANNEL[k].GDFTYP = 6;
		}    		
		
		if (VERBOSE_LEVEL>8) fprintf(stdout,"#%3d %d [%f %f][%f %f]\n",k,hdr->CHANNEL[k].GDFTYP,MinValue,MaxValue,PhysMinValue0,PhysMaxValue0);
		k2++;
	}
	if (0) //(hdr->TYPE==SCP_ECG && !hdr->FLAG.UCAL) 
	    	for (k=0; k<hdr->NS; k++) {
	    		hdr->CHANNEL[k].GDFTYP = 3;
	    		hdr->CHANNEL[k].PhysMax = (PhysMaxValue0 > -PhysMinValue0 ? PhysMaxValue0 : -PhysMinValue0);
	    		hdr->CHANNEL[k].PhysMin = -hdr->CHANNEL[k].PhysMax;
	    		hdr->CHANNEL[k].Cal = ceil(hdr->CHANNEL[k].PhysMax/(ldexp(1.0,15)-1));
	    		hdr->CHANNEL[k].Off = 0.0;
	    		hdr->CHANNEL[k].DigMax = hdr->CHANNEL[k].PhysMax/hdr->CHANNEL[k].Cal;
	    		hdr->CHANNEL[k].DigMin = -hdr->CHANNEL[k].DigMax;
		}

    }
//	if (VERBOSE_LEVEL>8) fprintf(stdout,"[201]\n");

	/* write file */
	strcpy(tmp,dest);
	if (hdr->FILE.COMPRESSION)  // add .gz extension to filename  
		strcat(tmp,".gz");

	if (VERBOSE_LEVEL>8) 
		fprintf(stdout,"[211] z=%i sd=%i\n",hdr->FILE.COMPRESSION,hdr->FILE.Des);

	hdr->FLAG.ANONYMOUS = 1; 	// no personal names are processed 

	hdr = sopen(tmp, "wb", hdr);
	if ((status=serror())) {
		destructHDR(hdr);
		destructHDR(rrFile);
		exit(status); 
	}	
#ifndef WITHOUT_NETWORK
	if (hdr->FILE.Des>0) 
		savelink(source);
#endif 
	if (VERBOSE_LEVEL>8)
		fprintf(stdout,"\n[221] File %s opened. %i %i %Li Des=%i\n",hdr->FileName,hdr->AS.bpb,hdr->NS,hdr->NRec,hdr->FILE.Des);

	swrite(data, hdr->NRec, hdr);
	if (VERBOSE_LEVEL>8) fprintf(stdout,"[231] SWRITE finishes\n");
	if ((status=serror())) { 
		destructHDR(hdr);
		destructHDR(rrFile);
		exit(status); 
    	}	

	if (VERBOSE_LEVEL>8) fprintf(stdout,"[236] SCLOSE finished\n");

	sclose(hdr);
	if (VERBOSE_LEVEL>8) fprintf(stdout,"[241] SCLOSE finished\n");
	destructHDR(hdr);
	destructHDR(rrFile);
	exit(serror()); 
}
