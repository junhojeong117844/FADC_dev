void save(const char* filename = "default") {

	gROOT->SetBatch(kTRUE);

    TFile *f = TFile::Open(filename);
    TTree *tree = (TTree*)f->Get("wftree");

	TString outdir = filename;
	outdir.ReplaceAll(".root", "");
	gSystem->mkdir(outdir, kTRUE);
	
    Float_t wf[4][496];
    Int_t wftime[496];

    tree->SetBranchAddress("wf", wf);
    tree->SetBranchAddress("wftime", wftime);

    int nEvents = tree->GetEntries();
    gStyle->SetOptStat(0);

    gSystem->mkdir("CsI_w_PMT_5000", kTRUE);

    for (int eventID = 0; eventID < nEvents; ++eventID) {
        tree->GetEntry(eventID);

        int nBins = 496;
        float xMin = wftime[0];
        float xMax = wftime[495] + 1;

        TH1F* h[4];

        TCanvas *c1 = new TCanvas("c1", "", 800, 800);
        c1->Divide(2, 2);

        for (int ch = 0; ch < 4; ++ch) {
            h[ch] = new TH1F(Form("h%d", ch), "", nBins, xMin, xMax);

            for (int i = 0; i < 496; ++i) {
                int bin = h[ch]->FindBin(wftime[i]);
                h[ch]->SetBinContent(bin, wf[ch][i]);
            }

            c1->cd(ch + 1);
            h[ch]->SetTitle(Form("Channel%d", ch+1));
            h[ch]->Draw();
        }

		c1->SaveAs(Form("%s/event_%04d.png", outdir.Data(), eventID));
		for (int ch = 0; ch < 4; ++ch) {
			delete h[ch];
		}

		delete c1;
	}
}
