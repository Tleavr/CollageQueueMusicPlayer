#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#include <wx/wx.h>
#include <vector>
#include <string>
#include "audio.h"

class MusicPlayerFrame : public wxFrame {
public:
    MusicPlayerFrame(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(700, 480)) {

        InitGameAudio();

        // قائمة الأغاني التجريبية (تأكد من وجودها داخل مجلد assets)
        // قائمة الأغاني الحقيقية الموجودة في مجلد assets
        playlist = {
            "assets/Gustavo_Santaolalla.mp3",
            "assets/LastNight.mp3",
            "assets/Memory_Reboot.mp3",
            "assets/Royalty.mp3",
            "assets/Thunder.mp3",
            "assets/XchoRemix.mp3"
        };
        currentTrackIndex = 0;
       

        if (!playlist.empty()) {
            LoadSong(playlist[currentTrackIndex]);
        }

        // Timer لتحديث تدفق الصوت
        audioTimer = new wxTimer(this, wxID_ANY);
        audioTimer->Start(50);
        Bind(wxEVT_TIMER, &MusicPlayerFrame::OnAudioTick, this, audioTimer->GetId());

        // تصميم الواجهة والأزرار
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticText* titleText = new wxStaticText(this, wxID_ANY, "Music Playlist Manager", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
        titleText->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

        statusText = new wxStaticText(this, wxID_ANY, "Playing: song1.mp3", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
        statusText->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));

        mainSizer->AddSpacer(40);
        mainSizer->Add(titleText, 0, wxALIGN_CENTER | wxALL, 10);
        mainSizer->Add(statusText, 0, wxALIGN_CENTER | wxALL, 5);
        mainSizer->AddSpacer(30);

        // أزرار التحكم (السابق، تشغيل/إيقاف، التالي)
        wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton* btnPrev = new wxButton(this, wxID_ANY, "<< Previous", wxDefaultPosition, wxSize(110, 40));
        wxButton* btnPlay = new wxButton(this, wxID_ANY, "Play / Pause", wxDefaultPosition, wxSize(130, 40));
        wxButton* btnNext = new wxButton(this, wxID_ANY, "Next >>", wxDefaultPosition, wxSize(110, 40));

        btnSizer->Add(btnPrev, 0, wxALL, 5);
        btnSizer->Add(btnPlay, 0, wxALL, 5);
        btnSizer->Add(btnNext, 0, wxALL, 5);

        mainSizer->Add(btnSizer, 0, wxALIGN_CENTER | wxALL, 10);

        SetSizer(mainSizer);
        Centre();

        // ربط الأحداث بالأزرار
        btnPrev->Bind(wxEVT_BUTTON, &MusicPlayerFrame::OnPrevTrack, this);
        btnPlay->Bind(wxEVT_BUTTON, &MusicPlayerFrame::OnPlayPause, this);
        btnNext->Bind(wxEVT_BUTTON, &MusicPlayerFrame::OnNextTrack, this);
    }

    ~MusicPlayerFrame() {
        audioTimer->Stop();
        delete audioTimer;
        StopAndCleanupAudio();
    }

private:
    wxTimer* audioTimer;
    wxStaticText* statusText;
    std::vector<std::string> playlist;
    size_t currentTrackIndex;

    void OnAudioTick(wxTimerEvent& event) {
        UpdateGameAudio();
    }

    void OnPlayPause(wxCommandEvent& event) {
        TogglePlayPause();
    }

    void OnNextTrack(wxCommandEvent& event) {
        if (playlist.empty()) return;
        currentTrackIndex = (currentTrackIndex + 1) % playlist.size();
        LoadSong(playlist[currentTrackIndex]);
        statusText->SetLabel("Playing: " + wxString(playlist[currentTrackIndex]));
    }

    void OnPrevTrack(wxCommandEvent& event) {
        if (playlist.empty()) return;
        currentTrackIndex = (currentTrackIndex == 0) ? playlist.size() - 1 : currentTrackIndex - 1;
        LoadSong(playlist[currentTrackIndex]);
        statusText->SetLabel("Playing: " + wxString(playlist[currentTrackIndex]));
    }
};

class MyApp : public wxApp {
public:
    bool OnInit() override {
        MusicPlayerFrame* frame = new MusicPlayerFrame("Music Playlist Manager - Test");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);