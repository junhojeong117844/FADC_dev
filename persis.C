void persis(const char* filename = "default") {
    TFile *f = TFile::Open(filename);
    TTree *tree = (TTree*)f->Get("wftree");

    Float_t wf[4][496];
    Int_t wftime[496];

    tree->SetBranchAddress("wf", wf);
    tree->SetBranchAddress("wftime", wftime);

    int nEvents = tree->GetEntries();
    int nBinsX = 496*0.3;
    float xMin = 350;
    float xMax = 650;
    int nBinsY = 240;
    float yMin = 300;
    float yMax = 1500;

    TH2F* h[4];
    for (int ch=0; ch<4; ++ch) {
        h[ch] = new TH2F(Form("h%d", ch), Form("Channel %d", ch+1),
                         nBinsX, xMin, xMax,
                         nBinsY, yMin, yMax);
    }

    for (int eventID=0; eventID<nEvents; ++eventID) {
        tree->GetEntry(eventID);

        for (int ch=0; ch<4; ++ch) {
            for (int i=0; i<496; ++i) {
                h[ch]->Fill(wftime[i], wf[ch][i]);
            }
        }
    }

	for (int ch=0; ch<4; ++ch) {
		std::vector<double> vals;
		for (int i=1; i<=h[ch]->GetNbinsX(); i++) {
			for (int j=1; j<=h[ch]->GetNbinsY(); j++) {
				double v = h[ch]->GetBinContent(i,j);
				if (v>0) vals.push_back(v);
			}
		}
		std::sort(vals.begin(), vals.end());
		int idx = (int)(0.80 * vals.size()); // % cut off
		double cutoff = vals[idx];
		h[ch]->SetMaximum(cutoff);
	}

	TCanvas *c1 = new TCanvas("c1", "Persistence mode", 800, 800);
	c1->Divide(2, 2);

	gStyle->SetOptStat(0);

	for (int ch=0; ch<4; ++ch) {
		c1->cd(ch+1);
		h[ch]->Draw("COL");
		h[ch]->GetXaxis()->SetTitle("ns");
		//h[ch]->GetYaxis()->SetTitle("ADC ch");
	}
}
