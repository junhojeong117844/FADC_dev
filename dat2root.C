#include <cstdio>
//#define PC_DRAM_SIZE    10
#define PC_DRAM_SIZE    1
#define DATA_ARRAY_SIZE PC_DRAM_SIZE*1024*1024
#define CHUNK_SIZE      PC_DRAM_SIZE*1024

#define pedStart 0
#define pedEnd 100
//#define pedStart 15750
//#define pedStart 7750
//#define pedEnd 16250

void dat2root(TString filename)
{

  TString name = filename;
  filename.ReplaceAll(".dat",".root");
  std::cout<< name << std::endl;
  std::cout<< filename << std::endl;

  TH1F* his[4];
  for( int i = 0; i< 4; i++)
  {
    his[i] = new TH1F( Form("his_%d",i), Form("his_%d", i ),1024,0,4096);
  }

  //char filename[256];
  //sprintf( filename, "nkfadc500_source_2k_coin_sup.dat");
  //sprintf( filename, "nkfadc500_source_20k_or_sup.dat");
  FILE* fp = fopen( name.Data(), "rb");
  int flag = 1; 
  int read_size = 0;
  char data[DATA_ARRAY_SIZE];
  int adc;

#if 1
  /// TFILE ///
  TFile* tf = new TFile(filename.Data(),"recreate");
  TTree* wftree = new TTree("wftree","waveTree");
  int eventnumber;
  int mid;
  int channel;
  int trignum;
  int trigpat;
  float wf[4][512-16];
  int  wftime[512-16];
  //float wf[16384-16];
  //int  wftime[16384-16];
  float wfsum[4] = {0};
  float wfped[4] = {0};
  float wfpeak[4] = {0};
  unsigned long ttime;
  unsigned long trigtimel;
  unsigned long trigtimeh;
  unsigned long strttimel;
  unsigned long strttimeh;

  wftree->Branch("eventnumber", &eventnumber, "eventnumber/I");
  wftree->Branch("mid"      , &mid      , "mid/I");
  wftree->Branch("channel"  , &channel  , "channel/I");
  wftree->Branch("trignum"  , &trignum  , "trignum/I");
  wftree->Branch("trigpat"  , &trigpat  , "trigpat/I");
  wftree->Branch("ttime"    , &ttime    , "ttime/l"); 
  wftree->Branch("trigtimel", &trigtimel, "trigtimel/l");
  wftree->Branch("trigtimeh", &trigtimeh, "trigtimeh/l");
  wftree->Branch("strttimel", &strttimel, "strttimel/l");
  wftree->Branch("strttimeh", &strttimeh, "strttimeh/l");
  wftree->Branch("wf"       , wf        , "wf[4][496]/F");
  wftree->Branch("wftime"   , wftime    , "wftime[496]/I");
  wftree->Branch("wfsum"    , &wfsum    , "wfsum/F");
  wftree->Branch("wfped"    , &wfped    , "wfped/F");
  wftree->Branch("ch1_wfpeak"   , &wfpeak[0]   , "ch1_wfpeak/F");
  wftree->Branch("ch2_wfpeak"   , &wfpeak[1]   , "ch2_wfpeak/F");
  wftree->Branch("ch3_wfpeak"   , &wfpeak[2]   , "ch3_wfpeak/F");
  wftree->Branch("ch4_wfpeak"   , &wfpeak[3]   , "ch4_wfpeak/F");

  for( int i = 0; i< 512-16; i++)
  //for( int i = 0; i< 16384-16; i++)
  {
    wftime[i] = i*2;
  }

  unsigned int itmp;
  unsigned long ltmp;
  eventnumber = 0;
  while( 1 )
  {
    read_size = fread( data, 1, CHUNK_SIZE* 1024, fp );    
    //cout << " read size : " << read_size << endl;
    //cout << " chunk size : " << CHUNK_SIZE*1024 << endl;
    //getchar();
    if( read_size == 0 ){ break; }
    int k = 0;
    //while( k < CHUNK_SIZE*1024 )
    while( k < read_size )
    {
      if( eventnumber%1000 == 0 ){ std::cout<<eventnumber<< std::endl; }
      /// data_length /// 
      int data_length = data[k] & 0xFF;
      itmp = data[k+1*4] & 0xFF;
      data_length = data_length + (unsigned int)(itmp << 8);
      itmp = data[k+2*4] & 0xFF;
      data_length = data_length + (unsigned int)(itmp << 16);
      itmp = data[k+3*4] & 0xFF;
      data_length = data_length + (unsigned int)(itmp << 24);

      /// module id /// 
      mid = data[k+15*4] & 0xFF;

      /// channel id /// 
      channel = data[k+16*4] & 0xFF;

      /// trignum ///
      trignum = data[k+17*4] & 0xFF;
      itmp = data[k+18*4] & 0xFF;
      trignum = trignum + (unsigned int)(itmp << 8);
      itmp = data[k+19*4] & 0xFF;
      trignum = trignum + (unsigned int)(itmp << 16);
      itmp = data[k+20*4] & 0xFF;
      trignum = trignum + (unsigned int)(itmp << 24);

      /// trigpat ///
      trigpat = data[k+21*4] & 0xFF;
      itmp = data[k+22*4] & 0xFF;
      trigpat = trigpat + (unsigned int)(itmp << 8);
      itmp = data[k+23*4] & 0xFF;
      trigpat = trigpat + (unsigned int)(itmp << 16);
      itmp = data[k+24*4] & 0xFF;
      trigpat = trigpat + (unsigned int)(itmp << 24);
      
      /// trigger fine time ///
      trigtimel = data[k+11*4] & 0xFF;

      /// trigger coarse time ///
      ltmp = data[k+12*4] & 0xFF;
      trigtimeh = ltmp;
      ltmp = data[k+13*4] & 0xFF;
      ltmp = ltmp << 8;
      trigtimeh = trigtimeh + ltmp;
      ltmp = data[k+14*4] & 0xFF;
      ltmp = ltmp << 16;
      trigtimeh = trigtimeh + ltmp;

      strttimel = data[k+25*4] & 0xFF;
	  
      // get local starting coarse time
      ltmp = data[k+26*4] & 0xFF;
      strttimeh = ltmp;
	  ltmp = data[k+27*4] & 0xFF;
	  ltmp = ltmp << 8;
	  strttimeh = strttimeh + ltmp;
	  ltmp = data[k+28*4] & 0xFF;
	  ltmp = ltmp << 16;
	  strttimeh = strttimeh + ltmp;

	  int number_of_points = (data_length -32 )/2;
	  //std::cout<< number_of_points << std::endl;
	  //cout << number_of_points << endl;

	  for (int ch = 0; ch < 4; ch++)
	  {
		  wfsum[ch] = 0;
		  wfped[ch] = 0;
		  wfpeak[ch] = 0;

		  for( int j = 0; j< number_of_points; j++)
		  {
			  adc = data[k+128+j*8 + ch] & 0xFF; 
			  itmp = data[k+128+j*8+4 + ch] & 0xFF;
			  adc = adc + (unsigned int)(itmp << 8);
			  wf[ch][j]= 4096 - adc;

			  if( j > pedStart  && j <= pedEnd )
			  {
				  wfped[ch] += wf[ch][j];
			  }
			  {
				  wfsum[ch] +=  wf[ch][j];
			  }
			  if( wfpeak[ch] < wf[ch][j] )
			  {
				  wfpeak[ch] = wf[ch][j];
			  }
		  }
		  wfped[ch] = wfped[ch]/ (pedEnd - pedStart);
		  wfpeak[ch] -= wfped[ch];
	  }
	  wftree->Fill();
	  eventnumber++;

	  //if (eventnumber >= 50) break;

	  k = k+data_length*4;
	}
	//if (eventnumber >= 50) break;
  }

  wftree->Write();
  tf->Close();

  fclose( fp );
#endif
}
