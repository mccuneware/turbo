#ifndef TURBO_SEARCHBOX_H
#define TURBO_SEARCHBOX_H

#define Uses_TGroup
#define Uses_TPalette
#define Uses_TInputLine
#define Uses_TValidator
#include <tvision/tv.h>
#include <string_view>

struct EditorWindow;

struct SearchBox : public TGroup {

    SearchBox(const TRect &bounds, EditorWindow &win);

    TInputLine *findBox;
    bool visible;

    void handleEvent(TEvent &ev) override;
    void draw() override;
    void open();
    void close();

};

void insertSearchBox(EditorWindow &win);

struct Searcher : public TValidator {

    turbo::Editor &editor;
    bool onDemand {false}, typing {false};
    sptr_t result, resultEnd;
    enum {forward=0, backwards=1} direction;

    Searcher(turbo::Editor &aEditor) :
        editor(aEditor)
    {
    }

    Boolean isValid(const char *) override;
    Boolean isValidInput(char *, Boolean) override;

    void targetFromCurrent();
    void targetNext();
    void targetPrev();
    void searchText(std::string_view, bool wrap);

};

struct SearchInputLine : public TInputLine {

    Searcher *searcher;

    template<typename ...Args>
    SearchInputLine(Searcher *searcher, Args&& ...args) :
        TInputLine(args..., searcher),
        searcher(searcher)
    {
        eventMask |= evBroadcast; // For cmDefault events.
    }

    void setState(ushort aState, Boolean enable) override;
    void handleEvent(TEvent &ev) override;
    void doSearch();

};

#endif
