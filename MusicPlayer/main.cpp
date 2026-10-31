#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include "song.h"
#include "audio.h"
#include "playlist.h"

class MusicPlayerFrame : public wxFrame {
private:
    Playlist playlist;
    wxTimer* updateTimer;

    wxButton* btnClear;
    wxButton* btnAdd;
    wxListCtrl* songListCtrl;
    wxStaticText* lblTotalSongs;
    wxStaticText* lblTotalDuration;
    wxStaticText* lblCurrentSong;
    wxButton* btnPrev;
    wxButton* btnPlay;
    wxButton* btnNext;
    wxStaticText* lblCurrentDuration;

    bool isPlaying = false;

    void OnAddSong(wxCommandEvent& event) {
        wxFileDialog openFileDialog(this, _("Choose Audio File"), "", "",
            "Audio files (*.wav;*.ogg;*.mp3)|*.wav;*.ogg;*.mp3", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (openFileDialog.ShowModal() == wxID_CANCEL) return;

        std::string filePath = std::string(openFileDialog.GetPath().mb_str());
        std::string fileName = std::string(openFileDialog.GetFilename().mb_str());

        wxTextEntryDialog titleDialog(this, "Enter Song Name:", "Song Title", fileName);
        if (titleDialog.ShowModal() != wxID_OK) return;
        std::string title = std::string(titleDialog.GetValue().mb_str());

        wxTextEntryDialog artistDialog(this, "Enter Artist Name:", "Artist", "Unknown Artist");
        if (artistDialog.ShowModal() != wxID_OK) return;
        std::string artist = std::string(artistDialog.GetValue().mb_str());

        Song newSong(title, artist, filePath, 180);
        playlist.addSong(newSong);
        RefreshUI();
    }

    void OnClearPlaylist(wxCommandEvent& event) {
        playlist.clearPlaylist();
        StopAndCleanupAudio();
        InitGameAudio();
        isPlaying = false;
        RefreshUI();
        if (lblCurrentSong) lblCurrentSong->SetLabel("No Song Playing");
        if (btnPlay) btnPlay->SetLabel(">>");
    }

    void OnNextSong(wxCommandEvent& event) {
        playlist.nextSong();
        if (!playlist.isEmpty()) {
            Song current = playlist.getCurrentSong();
            LoadSong(current.filePath);
            isPlaying = true;
            if (btnPlay) btnPlay->SetLabel("||");
        }
        else {
            StopAndCleanupAudio();
            InitGameAudio();
            isPlaying = false;
            if (btnPlay) btnPlay->SetLabel(">>");
        }
        RefreshUI();
    }

    void OnPlayToggle(wxCommandEvent& event) {
        if (playlist.isEmpty()) return;

        if (!isPlaying) {
            Song current = playlist.getCurrentSong();
            LoadSong(current.filePath);
            isPlaying = true;
            btnPlay->SetLabel("||");
        }
        else {
            TogglePlayPause();
            isPlaying = !isPlaying;
            btnPlay->SetLabel(isPlaying ? "||" : ">>");
        }
    }

    void OnTimer(wxTimerEvent& event) {
        UpdateGameAudio();
    }

    void RefreshUI() {
        songListCtrl->DeleteAllItems();
        std::vector<Song> songs = playlist.getAllSongs();

        int index = 0;
        for (const auto& song : songs) {
            songListCtrl->InsertItem(index, wxString::FromUTF8(song.title));
            songListCtrl->SetItem(index, 1, wxString::FromUTF8(song.artist));
            songListCtrl->SetItem(index, 2, wxString::FromUTF8(song.getFormattedDuration()));
            index++;
        }

        lblTotalSongs->SetLabel(wxString::Format("Total songs : %zu", playlist.getSongCount()));
        lblTotalDuration->SetLabel(wxString::Format("Total Duration : %s", playlist.getFormattedTotalDuration()));

        if (!playlist.isEmpty()) {
            Song curr = playlist.getCurrentSong();
            lblCurrentSong->SetLabel(wxString::FromUTF8(curr.title));
        }
        else {
            lblCurrentSong->SetLabel("No Song Playing");
        }
    }

public:
    MusicPlayerFrame() : wxFrame(nullptr, wxID_ANY, "Music Player - Queue System", wxDefaultPosition, wxSize(750, 520)) {
        InitGameAudio();

        SetBackgroundColour(wxColour(25, 25, 25));

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // الشريط العلوي
        wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
        btnClear = new wxButton(this, wxID_ANY, "Clear Playlist X");
        btnAdd = new wxButton(this, wxID_ANY, "Add song +");

        topSizer->Add(btnClear, 0, wxALL, 10);
        topSizer->AddStretchSpacer();
        topSizer->Add(btnAdd, 0, wxALL, 10);
        mainSizer->Add(topSizer, 0, wxEXPAND);

        // جدول الأغاني (تمت إزالة wxLC_VIRTUAL هنا لتجنب أي مشاكل)
        songListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 220), wxLC_REPORT | wxLC_SINGLE_SEL);
        songListCtrl->InsertColumn(0, "Song Name", wxLIST_FORMAT_LEFT, 250);
        songListCtrl->InsertColumn(1, "Artiest", wxLIST_FORMAT_LEFT, 230);
        songListCtrl->InsertColumn(2, "Durration", wxLIST_FORMAT_CENTER, 180);

        mainSizer->Add(songListCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT, 15);

        // الإحصائيات
        wxBoxSizer* statsSizer = new wxBoxSizer(wxHORIZONTAL);
        lblTotalSongs = new wxStaticText(this, wxID_ANY, "Total songs : 0");
        lblTotalDuration = new wxStaticText(this, wxID_ANY, "Total Duration : 00:00");

        lblTotalSongs->SetForegroundColour(*wxWHITE);
        lblTotalDuration->SetForegroundColour(*wxWHITE);

        statsSizer->Add(lblTotalSongs, 0, wxALL, 10);
        statsSizer->AddStretchSpacer();
        statsSizer->Add(lblTotalDuration, 0, wxALL, 10);
        mainSizer->Add(statsSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);

        // شريط التحكم السفلي
        wxStaticBoxSizer* controlBoxSizer = new wxStaticBoxSizer(wxVERTICAL, this, "");
        wxBoxSizer* controlLayout = new wxBoxSizer(wxHORIZONTAL);

        lblCurrentSong = new wxStaticText(this, wxID_ANY, "Song Name");
        lblCurrentSong->SetForegroundColour(*wxWHITE);

        btnPrev = new wxButton(this, wxID_ANY, "<<", wxDefaultPosition, wxSize(40, 40));
        btnPlay = new wxButton(this, wxID_ANY, "|>", wxDefaultPosition, wxSize(40, 40));
        btnNext = new wxButton(this, wxID_ANY, ">>", wxDefaultPosition, wxSize(40, 40));
        lblCurrentDuration = new wxStaticText(this, wxID_ANY, "Durration\n 00:00");
        lblCurrentDuration->SetForegroundColour(*wxWHITE);

        controlLayout->Add(lblCurrentSong, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        controlLayout->AddStretchSpacer();
        controlLayout->Add(btnPrev, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
        controlLayout->Add(btnPlay, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
        controlLayout->Add(btnNext, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
        controlLayout->AddStretchSpacer();
        controlLayout->Add(lblCurrentDuration, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

        controlBoxSizer->Add(controlLayout, 1, wxEXPAND);
        mainSizer->Add(controlBoxSizer, 0, wxEXPAND | wxALL, 15);

        SetSizer(mainSizer);

        // ربط الأحداث
        btnAdd->Bind(wxEVT_BUTTON, &MusicPlayerFrame::OnAddSong, this);
        btnClear->Bind(wxEVT_BUTTON, &MusicPlayerFrame::OnClearPlaylist, this);
        btnNext->Bind(wxEVT_BUTTON, &MusicPlayerFrame::OnNextSong, this);
        btnPlay->Bind(wxEVT_BUTTON, &MusicPlayerFrame::OnPlayToggle, this);

        updateTimer = new wxTimer(this, wxID_ANY);
        Bind(wxEVT_TIMER, &MusicPlayerFrame::OnTimer, this);
        updateTimer->Start(16);
    }

    ~MusicPlayerFrame() {
        updateTimer->Stop();
        delete updateTimer;
        StopAndCleanupAudio();
    }
};

class MusicPlayerApp : public wxApp {
public:
    bool OnInit() override {
        MusicPlayerFrame* frame = new MusicPlayerFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MusicPlayerApp);