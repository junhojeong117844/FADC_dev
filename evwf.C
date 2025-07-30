void evwf(int eventID = 0, const char* filename = "default") {
    TFile *f = TFile::Open(filename);
    TTree *tree = (TTree*)f->Get("wftree");

    Float_t wf[4][496];
    Int_t wftime[496];

    tree->SetBranchAddress("wf", wf);
    tree->SetBranchAddress("wftime", wftime);
    tree->GetEntry(eventID);

    int nBins = 496;
    float xMin = wftime[0];
    float xMax = wftime[495] + 1;

    TH1F* h[4];
    float peak[4];

    TCanvas *c1 = new TCanvas("c1", Form("Event %d - All Channels", eventID), 800, 800);
    c1->Divide(2, 2);

    for (int ch = 0; ch < 4; ++ch) {
        h[ch] = new TH1F(Form("h%d", ch), "", nBins, xMin, xMax);
        peak[ch] = wf[ch][0];

        for (int i = 0; i < 496; ++i) {
            int bin = h[ch]->FindBin(wftime[i]);
            h[ch]->SetBinContent(bin, wf[ch][i]);
            if (wf[ch][i] > peak[ch]) peak[ch] = wf[ch][i];
        }

        c1->cd(ch + 1); 
        h[ch]->SetTitle(Form("Channel%d", ch+1));
        h[ch]->Draw();
    }

    gStyle->SetOptStat(0);
}
