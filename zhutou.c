#if 0
# //gcc="ftlibtool --mode=link gcc"
gcc=gcc
src=zhutou.c
out=zhutou.e
ft2="../ft2"
ft2include="$ft2/include"
inc="-I$ft2include -I$ft2"
libs="-lz -lpng16 -lX11 -lm"
ft2objs="$ft2/common.o $ft2/strbuf.o $ft2/output.o $ft2/mlgetopt.o"
ft2objs+=" $ft2/ftcommon.o $ft2/libfreetype.so $ft2/graph.a"
objs="$ft2objs"
$gcc -o $out $inc $src $objs $libs
exit 0
#else
#include <ftcommon.h>
#include <common.h>
#include <mlgetopt.h>

// comment out once debugged
#include <freetype/internal/ftdebug.h>

#include FT_MODULE_H
#include <freetype/internal/ftobjs.h>
#include <freetype/internal/ftdrv.h>

//#include FT_STROKER_H
//#include FT_SYNTHESIS_H
//#include FT_LCD_FILTER_H
#include FT_DRIVER_H
#include FT_COLOR_H
#include FT_BITMAP_H

//#ifdef _WIN32 //  double-check WIN32 AND _WIN32
#ifdef _WIN32
#define snprintf _snprintf
#endif

//static int LCDModes[] = {
//  LCD_MODE_MONO, LCD_MODE_AA, LCD_MODE_LIGHT, LCD_MODE_RGB,
//  LCD_MODE_BGR,  LCD_MODE_VRGB, LCD_MODE_VBGR			};
//#define LCDModesLEN   ((int)(sizeof(LCDModes) / sizeof(int)))

//enum { RENDER_MODE_ALL = 0, RENDER_MODE_FANCY, RENDER_MODE_STROKE,
//       RENDER_MODE_TEXT, RENDER_MODE_WATERFALL, N_RENDER_MODES	};

#define VDT  void
#define UCT  unsigned char
#define CH   char // not const .....
#define CST  const char *
#define INT  int
#define UNT  unsigned int
#define FPT  double
#define XPT  FT_Fixed
#define YNT  FT_Bool
#define ERT  FT_Error
#define FAT  FT_Face
#define GST  FT_GlyphSlot
#define GLT  FT_Glyph
#define UIT  FT_UInt
#define SVDT static VDT
#define SCST static CST
#define SERT static ERT
#define SINT static INT
// UNT -> UIT ?

#define IF  if
#define EF  else if
#define EL  else
#define DO  do
#define WI  while
#define BK  break
#define CT  continue
#define RT  return
#define INC ++
#define DEC --
#define LT  <
#define GT  >
#define LQ  <=
#define GQ  >=
#define EQ  ==
#define NQ  !=
#define CS char * // not constant ....
#define NUL  '\0'
#define AND &&
#define OR  ||
#define MSG printf

// it is expected that DOSTART starts within a condition
// DOUNTIL will end the single-iteration loop and the condition
// this allows break; for early exit with proper { cleanup(); }
#define DOSTART      do {
#define AFTERDO      } while(0); }
#define ELSEDO       else { DOSTART
// NOTE THAT AFTERDO CLOSES THE CONDITION BUT THERE IS NO DIFFERENCE
// BETWEEN THE END OF THE CONDITION BLOCK AND AFTER THE CONDITION BLOCK
// THE BRACES OF AFTERDO ARE OPTIONAL
//FILE *file = fopen(filename, "r");
//if (!file) {
//  // return error message and code
//} ELSEDO
//  // file is open
//  // do things
//  AFTERDO { fclose(file); }
//}


#include "utf8.h"

#define FONT0PATH "./Ukai.ttf"
#define FONT1PATH "./Itim.otf"
#define FONT2PATH "./OpenSansEmoji.ttf"
#define FONT3PATH "./EmojiOneColor.otf"

#define ZHU        "\xE7\x8C\xAA" // ???
#define TOU        "\xE5\xA4\xB4" // ???

#define SPACE      " "
#define LINEBREAK  "\n"
//#define LINEBREAK  "\x0a"

#define MA    "\xe0\xb8\xa1\xe0\xb8\xb2" //
#define LAEW  "\xe0\xb9\x81\xe0\xb8\xa5\xe0\xb9\x89\xe0\xb8\xa7" // 
#define KHA   "\xe0\xb8\x84\xe0\xb9\x88\xe0\xb8\xb0" // 
#define MALAEWKHA_UTF  MA LAEW KHA
#define MALAEWKHA_UNI  "???????????????????????????"
#define MALAEWKHA_ASC  "`8!`82`9`8%`9	`8'`8`9`80"
#define MALAEWKHA_STR  "ma laew kha"
#define MALAEWKHA_ENG  "here it is"

//#define INITIALTEXT \
//  "\xE4\xBD\xA0\xE8\xBF\x98\xE7\x88\xB1\xE6\x88\x91\xE5\x90\x97\xEF\xBC\x9F"

//SCST Text = ZHU TOU ZHU TOU ZHU TOU;
//#define INITIALTEXT    ZHU TOU ZHU TOU LINEBREAK MALAEWKHA_UTF
#define CONNECTION    "\xf0\x9f\x93\xb6"
#define MICROPHONE    "\xf0\x9f\x8e\xa4"
#define SPEAKER       "\xf0\x9f\x94\x89"
//static CST j ZHUTOU = ZHU TOU;
#define NOTRANSLATION "NO TRANSLATION"
#define NOTRAN        NOTRANSLATION
#define ZHUTOU        ZHU TOU
//static const char const *ZHUTOU = ZHU TOU;

//#define INITIALTEXT   CONNECTION SPACE MICROPHONE SPACE SPEAKER
//#define TEXTSEQUENCE        \
//SETFONT1 SETFONT2           \
//FONT1 INITIALTEXT LINEBREAK \
//FONT2 INITIALTEXT

#define WORDMATCH(str, match)     (strcmp(str, match) == 0)

typedef struct _Translation {
  CST sequence;
  struct _Translation *next;
} Translation;

typedef struct _Translations {
  Translation first;
  INT selectionindex;
  INT totalitems;
} Translations;

Translations onetranslation(CST sequence) {
  Translation t = { sequence: sequence, next: NULL };
  Translations ts = { first: t, selectionindex: 0, totalitems: 1 };
  return ts;
}

Translations twotranslations(CST seq1, CST seq2) {
  Translation t1 = { sequence: seq1, next: NULL };
  Translation *t2 = malloc(sizeof(Translation));
  t2->sequence = seq2;
  t2->next = NULL;
  t1.next = t2;
  Translations ts = { first: t1, selectionindex: 0, totalitems: 2 };
  return ts;
}

VDT freetranslations(Translations menu) {
  Translation *t = &menu.first;
  WI (t->next) {
    Translation *nextt = t->next;
    IF (t != &menu.first) {
      free(t);
    } // first item is static
    t = nextt;
  }
}

//Translations notranslations() {
//  return translationmenu(ZHUTOU);
//}
const Translation  ZhuTou         = { sequence: ZHUTOU, next: NULL };
const Translation  NoTranslation  = { sequence: NOTRAN, next: NULL };
const Translations NoTranslations = { first: NoTranslation, selectionindex: 0, totalitems: 1 };

Translations translatemnemonic(CS word) {
  IF WORDMATCH(word, "CONNECTION" ) { RT onetranslation(CONNECTION);    }
  EF WORDMATCH(word, "MICROPHONE" ) { RT onetranslation(MICROPHONE);    }
  EF WORDMATCH(word, "SPEAKER"    ) { RT onetranslation(SPEAKER   );    }
  EF WORDMATCH(word, "MA LAEW KHA") { RT onetranslation(MALAEWKHA_UTF); }
  EF WORDMATCH(word, MALAEWKHA_UTF) { RT onetranslation(MALAEWKHA_ENG); }
  EF WORDMATCH(word, ZHU TOU      ) { RT twotranslations("ZHU TOU", "PIG HEAD"); }
  EF WORDMATCH(word, "ZHU TOU"    ) { RT onetranslation("$PIGHEAD"); }
//  EF WORDMATCH(word, "ZHU TOU"    ) { RT onetranslation("PIG HEAD"); }
//  EF WORDMATCH(word, "ZHU-TOU"    ) { RT ZhuTou; } // invalid!NoTranslations!
//  EF WORDMATCH(word, "ZHU TOU"    ) { RT ZhuTou; } // invalid!NoTranslations!
  EL { RT NoTranslations; } // notranslations(); }
} // for zhutou, draw a zhutou instead

#define MENUHASNOTRANSLATIONS(menu) \
  (menu.first.sequence  EQ NOTRAN)
#define MENUPHASNOTRANSLATIONS(menu) \
  (menu->first.sequence EQ NOTRAN)

static struct status_ {
  INT update;
  INT fails;
} status = {
  1   , // update
  0//   , // fails
};

#define TEXTSTRINGSIZE       1024
// TODO: THIS WILL DEPEND ON DOCUMENT SIZE

#define DEFAULTFONTWIDTH     120
#define DEFAULTFONTHEIGHT    120
#define DEFAULTFONTSIZE      fontwidth: DEFAULTFONTWIDTH, \
                            fontheight: DEFAULTFONTHEIGHT
#define DEFAULTTEXTMARGINX    10
#define DEFAULTTEXTMARGINY    10
#define DEFAULTTEXTMARGIN   DEFAULTTEXTMARGINX, \
                            DEFAULTTEXTMARGINY

#define EMOJIFONTWIDTH       150
#define EMOJIFONTHEIGHT      150
#define EMOJIFONTSIZE        fontwidth: EMOJIFONTWIDTH, \
                            fontheight: EMOJIFONTHEIGHT
#define LANGUAGEFONTWIDTH     80
#define LANGUAGEFONTHEIGHT    80
#define LANGUAGEFONTSIZE     fontwidth: LANGUAGEFONTWIDTH, \
                            fontheight: LANGUAGEFONTHEIGHT

#define NOCOLOUR         'N'
#define BLACKCOLOUR      'K'
#define REDCOLOUR        'R'
#define YELLOWCOLOUR     'Y'
#define GREENCOLOUR      'G'
#define CYANCOLOUR       'C'
#define DARKCYANCOLOUR   'c'
#define BLUECOLOUR       'B'
#define MAGENTACOLOUR    'M'
#define FC3COLOUR        'F'
#define WHITECOLOUR      'W'

typedef struct _Colour {
  INT r, g, b, a;
} Colour;

Colour colourcode(CH code) {
  IF (code == NOCOLOUR      ) { RT (Colour){  127,  127,  127, 127 }; }
  EF (code == BLACKCOLOUR   ) { RT (Colour){    0,    0,    0, 255 }; }
  EF (code == REDCOLOUR     ) { RT (Colour){  255,    0,    0, 255 }; }
  EF (code == YELLOWCOLOUR  ) { RT (Colour){  255,  255,    0, 255 }; }
  EF (code == GREENCOLOUR   ) { RT (Colour){    0,  255,    0, 255 }; }
  EF (code == CYANCOLOUR    ) { RT (Colour){    0,  255,  255, 255 }; }
  EF (code == DARKCYANCOLOUR) { RT (Colour){    0,  127,  127, 255 }; }
  EF (code == BLUECOLOUR    ) { RT (Colour){    0,    0,  255, 255 }; }
  EF (code == MAGENTACOLOUR ) { RT (Colour){  255,    0,  255, 255 }; }
  EF (code == FC3COLOUR     ) { RT (Colour){ 0xFF, 0xCC, 0x33, 255 }; }
  EF (code == WHITECOLOUR   ) { RT (Colour){  255,  255,  255, 255 }; }
  EL { RT (Colour){ 255, 0, 255, 127 }; } // 50% opacity magenta
}

#define EMPTYCOLOUR      CYANCOLOUR

#define CIRCLE           'C'
#define RECTANGLE        'R'
#define ROUNDEDRECTANGLE 'O'

#define OPACITYNONE      'N'
#define OPACITYVERYLOW   'V'
#define OPACITYLOW       'L'
#define OPACITYMEDIUM    'M'
#define OPACITYHIGH      'H'
#define OPACITYFULL      'F'

// array hold ....

typedef struct _Hotkey {
  CS keysequence; // "()" "(((EMOJI)))"
  CS actioncommand;
  CS actiontable; // command ...... or table .... or tablecache ....
  // how to map to a hotkey action table .... might as well process as string
  // and then do what
// table comes from instance of use of hotkey ....
} Hotkey;

typedef struct _Region {
//  Input inputs;
//  Output outputs;
  Hotkey hotkey;
  INT index;
  CS command; // actiontable is different -- run command with emoji support, or, return an emoji
//  IN topleftx;
//  IN toplefty;
  struct _Region *next;
} Region;

// todo: animations, and sound effects
// [ ] to run a command
// { } to create a program
// ( ) to open a map -- (pinyin zhutou)  "( ^P ---"

// so i guess this is to be the first self-editing program
// need Highlight to know font of current range
// many fonts, one size, at the moment ....
// instead, maintain a list of Fontmodes ....
//  language font mode, emoji font mode
//F1{ukai.ttf:64x64} F2{emoji.ttf:128x128}
//F3{F1:red} F4{F1:green} F5{F1:blue}

typedef struct _Font {
  CS filename;
  INT width;
  INT height;
  FT_Face face;
  struct _Font *next;
} Font;

typedef struct _Textbox {
//  FT_Face *fontlist;
//  INT numfonts;
  FT_Library *ftlib;
  Font font;
  INT fontscalenumer;
  INT fontscaledenom;
  INT packchars;

//  INT fontindex;
//  INT fontwidth;
//  INT fontheight;
  INT marginx;
  INT marginy;
  CS textstring;
  INT textstringsize;
//  Region region;
  // hotkeys ... for .... money
} Textbox;

VDT textboxaddfont(Textbox *textbox, CS fontpath, INT fontwidth, INT fontheight) {
  FT_Library *tftlib = textbox->ftlib;
  Font *newfont = (Font *)malloc(sizeof(Font));
  IF (!newfont) { MSG("ADDFONT MEMORYERROR\n"); RT; }
  newfont->filename = fontpath;
  newfont->width    = fontwidth;
  newfont->height   = fontheight;
  printf("ADDING FONT %s\n", newfont->filename);
  error = FT_New_Face((*tftlib), newfont->filename, 0, &(newfont->face));
  if (error) { MSG("ADDFONTNEWFACEFAIL\n"); RT; }
  error = FT_Set_Pixel_Sizes(newfont->face, newfont->width, newfont->height);
  if (error) { MSG("ADDFONTSETSIZEFAIL\n"); RT; }
  newfont->next = NULL; // one more font loaded
  Font *lastfont = &(textbox->font);
  WI (lastfont->next) { lastfont = lastfont->next; }
  lastfont->next = newfont;
}


Textbox newtextbox(FT_Library *ftlibp, INT leftmargin, INT topmargin,
                   CS fontpath, INT fontwidth, INT fontheight        ) {
  Textbox t;
  t.textstring = NULL; // in case of error return
  t.ftlib = ftlibp;
  t.font.filename = fontpath;
  t.font.width = fontwidth;
  t.font.height = fontheight;
  printf("LOADING FONT %s\n", t.font.filename);
  error = FT_New_Face((*t.ftlib), t.font.filename, 0, &t.font.face);
  if (error) { printf("FONTNEWFACEFAIL\n"); RT t; }
  error = FT_Set_Pixel_Sizes(t.font.face, t.font.width, t.font.height);
  if (error) { printf("FONTSETSIZEFAIL\n"); RT t; }
  t.font.next = NULL; // one font loaded
  t.fontscalenumer = 1;
  t.fontscaledenom = 1;
  t.packchars = 0;
  t.marginx = leftmargin;
  t.marginy = topmargin;
  t.textstringsize = TEXTSTRINGSIZE;
  t.textstring = (CS)malloc(t.textstringsize + 1);
  RT t;
}

Textbox minitextbox(Textbox *fontsource, INT numerscale, INT denomscale,
                    INT leftmargin, INT topmargin                        ) {
  Textbox mt;
  mt.textstring = NULL;
  mt.textstringsize = 0;
  mt.ftlib = fontsource->ftlib;
  mt.font = fontsource->font;
  mt.fontscalenumer = numerscale;
  mt.fontscaledenom = denomscale;
  mt.marginx = leftmargin;
  mt.marginy = topmargin;
  RT mt;
}

VDT freeminitextbox(Textbox *textbox) {
  textbox->ftlib = NULL;
} // minitextbox does not own its fonts

VDT freetextbox(Textbox *textbox) {
//    INT donefontix = -1;
//    WI (INC donefontix < numfonts) {
//      printf("FTDONEFONT%d\n", donefontix);
//      FT_Done_Face(fontface[donefontix]);
//    }
  Font *thisfont = &(textbox->font); //.face);
  FT_Done_Face(thisfont->face); // expect FONT0 to be loaded
  WI (thisfont->next) { // if FONT1, free, next nextfont
    Font *nextfont = thisfont->next;
    FT_Done_Face(nextfont->face);
    IF (thisfont NQ &(textbox->font))
      { free(thisfont); } // first is static
    thisfont = nextfont;
  } // free fonts and font list memory
  free(textbox->textstring);
  textbox->ftlib = NULL;
}

typedef struct _Stencil {
  Textbox *textbox; // sets font context and base margin, and has an optional background string
  INT wordmarginx, wordmarginy;
  FPT wordscale, wordscalex, wordscaley;

  INT linebottom; // align top of subline
  INT linecentre; // align middle of symbolword
  INT wordwidth, wordheight; // derived from calculation by drawing with wordscale considered

  CST textword; // only set for symbols
  CH colour;
  CH backgroundcolour;
  CH backgroundshape;
  CH backgroundopacity;
  INT fontindexoverride;
  struct _Stencil *next;
} Stencil;

// - smaller X
// _ smaller Y
// = larger  X
// + larger  Y
// ^- smaller XY
// ^= larger  XY
// ^_ more superscript
// ^+ more subscript

typedef struct _Cursor {
  INT offsetx;
  INT line;
} Cursor;

typedef struct _Text {
  CS string;
  CS cursorstart;
  CS cursorend;
} Text;

VDT writetextstring(Text *text, CS textstring) {
  IF (!(text->string)) { MSG("not attached to textbox\n"); RT; }
  IF (!textstring    ) { MSG("cannot write NULL text\n"); RT; }
  sprintf(text->string, "%s", textstring); // overwrite string buffer
  text->cursorstart = text->string;        // return to start
  WI (*(text->cursorstart)) { text->cursorstart INC; } // move to end
  text->cursorend = text->cursorstart;     // cursor is 0 chars wide
}

VDT appendtextstring(Text *text, CS appendstring) {
  // if cursorend != cursorstart, we are overwriting a selection
  IF (!(text->string) || !(text->cursorstart) || !(text->cursorend))
    { MSG("cannot append to NULL text\n"); RT; }
  IF (!appendstring) { MSG("cannot append NULL text\n"); RT; }
  // not currently checking that cursor is within string bounds
  INT stringwidth = strlen(appendstring);
  CH appendchar = NUL;
  IF (stringwidth EQ 1) // only checking for backspace
    { appendchar = appendstring[0]; }
  IF (appendchar EQ '\b') { // backspace selection or cursor
    IF (text->cursorstart NQ text->cursorend) {
      CS delfromhere = text->cursorstart;
      CS deltohere   = text->cursorend;
      IF (text->cursorend LT text->cursorstart) {
        delfromhere = text->cursorend;
        deltohere   = text->cursorstart;
      } // end-to-start and start-to-end backspace the same
      CS overwrite     = delfromhere;
      CS overwritewith = deltohere;
      WI (*overwritewith) {
        *overwrite = *overwritewith;
        INC overwrite;
        INC overwritewith;
      } // overwrite selection with after-selection until \0
      *overwrite = NUL; // re-tail the string
      text->cursorstart = delfromhere;
      text->cursorend   = delfromhere;
    } EF (text->cursorend EQ text->cursorstart AND // cursor
          text->cursorstart GT text->string) {    // 0-width
      CS overwritewith = text->cursorstart;
      INT charremoved = prevutf8(&(text->cursorstart), text->string);
      charremoved; // prevent unused variable warning
      text->cursorend = text->cursorstart;
      CS overwrite    = text->cursorstart;
      WI (*overwritewith) {
        *overwrite = *overwritewith;
        INC overwrite;
        INC overwritewith;
      } // overwrite selection with after-selection until \0
      *overwrite = NUL; // re-tail the string
//      text->cursorstart = delfromhere;
//      text->cursorend   = delfromhere;
      // all characters after this one need to be shifted left
      // if cursorend is less than cursorstart, use cursorend
    } // EL presumably cursor is at start of string
  } EL { // write a char -- or overwrite selection then write
    IF (text->cursorstart NQ text->cursorend) {
      CS delfromhere = text->cursorstart;
      CS deltohere   = text->cursorend;
      IF (text->cursorend LT text->cursorstart) {
        delfromhere = text->cursorend;
        deltohere   = text->cursorstart;
      } // end-to-start and start-to-end backspace the same
      CS overwrite     = delfromhere + stringwidth;
      CS overwritewith = deltohere;
      IF (overwrite GT overwritewith) { // chars forward
        CS overwritewithend = overwritewith;
        WI (*overwritewithend) { INC overwritewithend; }
        CS overwriteend = overwrite + (overwritewithend - overwritewith);
        *overwriteend = NUL;
        WI (overwritewithend GT overwritewith) {
          DEC overwriteend;
          DEC overwritewithend;
          *overwriteend = *overwritewithend;
        }
      } EL { // small write - move chars back
        WI (*overwritewith) {
          *overwrite = *overwritewith;
          INC overwrite;
          INC overwritewith;
        }
        *overwrite = NUL; // re-tail the string: overwriteend
      } // overwrite selection with after-selection until \0
      CS writehere = delfromhere;
      INT writechix = -1;
      WI (INC writechix LT stringwidth) {
        *writehere = appendstring[writechix];
        INC writehere;
      }
      text->cursorstart = writehere;
      text->cursorend   = writehere;
    } EL { // text->cursorstart == text->cursorend
      CS writehere = text->cursorend;
      CS stringend = writehere;
      WI (*stringend) { INC stringend; }
      CS afterwrite = stringend;
      stringend += stringwidth;
      *stringend = NUL;
      WI (DEC afterwrite GQ writehere) {
        DEC stringend;
        *stringend = *afterwrite;
      }
      stringend += stringwidth;
      IF (*stringend) { MSG("STRINGEND NOTNULL\n"); }
      INT writechix = -1;
      WI (INC writechix LT stringwidth) {
        *writehere = appendstring[writechix];
        IF (INC writehere GQ stringend)
          { *writehere = NUL; MSG("UNEXPECTED TAILEXTEND\n"); }
      }
      text->cursorstart = writehere;
      text->cursorend   = writehere;
    }
  }
}

// initial highlight cursor rightwidth is TEN
#define CURSORWIDTH         10
//#define CURSORHEIGHT        12 set 
#define CURSORTOPMARGIN     1
#define CURSORBOTTOMMARGIN  1
#define CURSORMARGIN        (CURSORTOPMARGIN + CURSORBOTTOMMARGIN)

//#define FONTHEIGHTPX   (CURSORHEIGHT - CURSORMARGIN)
//#define FONTWIDTHPX    (FONTHEIGHTPX)

typedef struct _Highlight {
  INT charsbefore, charsafter;
  INT leftwidth, rightwidth, height;
  CS startpoint;
  CS endpoint;
  CS cursorpoint;
  CH colour;
  Translations menu;
} Highlight;

Translations translationmenu(Highlight *highlight) {
  CS highchar = highlight->startpoint;
  CS highstop = highlight->endpoint;
  IF (highstop LT highchar) {
    highchar = highlight->endpoint;
    highstop = highlight->startpoint;
  }
  INT highlightsize = highstop - highchar;
  IF (highlightsize LT 0) { RT NoTranslations; } // not expected
  EF (highlightsize EQ 0) { RT NoTranslations; } // ZhuTou!!
  CS highlightword = (CS)malloc(highlightsize + 1);
  IF (!highlightword) { RT NoTranslations; } // Memory Error
  INT wordix = 0;
  WI (highchar LT highstop) {
    highlightword[wordix] = *highchar;
    INC highchar;
    INC wordix;
  } // expect wordix == highlightsize
  highlightword[wordix] = NUL;
  Translations menu = translatemnemonic(highlightword);
  free(highlightword);
  RT menu;
}

#define TEN 10
const Highlight NoHighlight = { charsbefore: 0, charsafter: 0,
               leftwidth: 0, rightwidth: TEN, height: TEN,
               NULL, NULL, NULL, EMPTYCOLOUR,
               NoTranslations                                     };

VDT alignhighlight(Textbox *textbox, Highlight *highlight) {
  IF (!textbox || !highlight) { MSG("ALIGNHIGHLIGHT NOCONTEXT\n"); RT; }
  CS srctext = highlight->startpoint;
  CS srcstop = highlight->endpoint;
  CS srcnext = highlight->cursorpoint; // consider multiple cursor points
  IF (!srctext || !srcstop || !srcnext) { MSG("ALIGNHIGHLIGHT NOSRC\n"); RT; }
  IF (srcstop LT srctext) {
    srctext = highlight->endpoint;
    srcstop = highlight->startpoint;
  } // right to left selection
  INT leftwidth = 0;
  INT rightwidth = 0;
  Font *firstfont = &(textbox->font);
  FT_Face *firstface = &(firstfont->face);
  FT_Face *thisface = firstface;
  INT numerscale = textbox->fontscalenumer;
  INT denomscale = textbox->fontscaledenom;
  INT packchars  = textbox->packchars;
  // FT_Set_Pixel_Sizes happens in font init -- load one font per font size
  INT xstart = 0; //textbox.textmarginx;
  INT ystart = 0; //textbox.textmarginy;
  INT x = xstart;
  INT y = ystart;
  CS textp = srctext;
  CS textend = srcstop; //textp + strlen(srctext);
  INT aftercursor = 0;
  UIT lastglyphix = 0;
  INT ch = 0;
  INT charspacegap = 1;  // 1px between chars
  INT spacewidth = textbox->font.width >> 1;
  INT linefootergap = 1; // 1px between lines
  INT fontheight = textbox->font.height;
//  INT lineheight = fontheight + 1;
  INT scaledfontheight = fontheight * numerscale / denomscale;
  INT scaledlineheight = scaledfontheight + linefootergap;
  if (textp == srcnext) {
//    MSG("CURSOR AT START\n");
    aftercursor = 1;
  } else if (textp == textend) {
    rightwidth = CURSORWIDTH;
    aftercursor = 1;
  } // no highlight width, expect 0 0 from the loop
  while (ch >= 0) { // loop until done
    CS textpoint = textp;
    ch = nextutf8(&textp, textend);
    if (!aftercursor AND textpoint GT srcnext) {
      MSG("JUMPED OVER CURSOR\n");
      aftercursor = 1;
    } else if (!aftercursor AND textpoint EQ srcnext) {
      // MSG("SAW CURSOR\n");
      aftercursor = 1; // saw cursor
    } // else we are beforecursor
    if (ch < 0) { printf("HLEOF\n"); break; } // don't write EOF
    if (ch == 0) { /* printf("HLNUL\n"); */ break; } // or NUL
    // cannot handle linebreaks in selections yet
    if (ch == '\n') {
      x = xstart;
      y += scaledlineheight + 1;
      scaledlineheight = scaledfontheight + linefootergap;
      // leftwidth and rightwidth cannot be multiline
      printf("HLNEWLINE\n");
      break;
//      continue;
    }
    if (ch == ' ' ) {
      x += spacewidth + 1;
      IF (aftercursor)
        { rightwidth += spacewidth + 1; }
      EL { leftwidth += spacewidth + 1; }
      continue;
    }
//    if (ch == '\n') { x = xstart; y += LINEHEIGHT; continue; }
    UIT glyphix = FT_Get_Char_Index(*thisface, ch);
    IF (glyphix EQ 0) { // not in this font
      Font *nextfont = firstfont;
      WI (glyphix EQ 0 AND nextfont) {
        thisface = &(nextfont->face);
        glyphix = FT_Get_Char_Index(*thisface, ch);
        nextfont = nextfont->next;
      }
      IF (!nextfont) { thisface = firstface; MSG("MISSINGGLYPH ALIGNHIGHLIGHT\n"); }
    }
    error = FT_Load_Glyph(*thisface, glyphix, FT_LOAD_RENDER | FT_LOAD_COLOR);
    IF (error) { printf("LOADGLYPHFAIL\n"); }
    ELSEDO
//      error = FT_Render_Glyph(glyphix, FT_RENDER_MODE_NORMAL);
// - only need FT_LOAD_COLOR for color emoji sets
// - FT_LOAD_RENDER means no FT_Render_Glyph seemingly
//     (do emojis need re-render?)

      FT_Bitmap *srcbmp = &(*thisface)->glyph->bitmap;
      int srcheight  = srcbmp->rows;
      int thischarhoribearingy = (*thisface)->glyph->metrics.horiBearingY >> 6;
      int thischarvertadvance  = (*thisface)->glyph->metrics.vertAdvance  >> 6;
      int thischarascender     = (*thisface)->ascender  >> 6;
      int thischardescender    = (*thisface)->descender >> 6;
      int scaledheight = srcheight * numerscale / denomscale;
      int scaledhoribearingy = thischarhoribearingy * numerscale / denomscale;
      int scaledvertadvance  = thischarvertadvance * numerscale / denomscale;
      int scaledascender     = thischarascender  * numerscale / denomscale;
      int scaleddescender    = thischardescender * numerscale / denomscale;
      int ydiff = (0 - scaledhoribearingy); // - sdy);
      int gravityy = /*y +*/ ydiff + scaledascender - scaleddescender;
      int gravityfix = scaledvertadvance + scaledheight;
      int glyphheight = gravityy + gravityfix; // ) * sbitmap->pitch;

//    int thischarheight = (*thisface)->glyph->metrics.height >> 6;
//    int thischarwidth  = (*thisface)->glyph->metrics.width  >> 6;
//    int thischarvertbearingy = (*thisface)->glyph->metrics.vertBearingY >> 6;
//    int thischarvertadvance = (*thisface)->glyph->metrics.vertAdvance >> 6;

    int thischarhoribearingx = (*thisface)->glyph->metrics.horiBearingX >> 6;
//    int thischarhoribearingy = (*thisface)->glyph->metrics.horiBearingY >> 6;
    int thischarhoriadvance  = (*thisface)->glyph->metrics.horiAdvance  >> 6;
//    int thischarvertbearingx = (*thisface)->glyph->metrics.vertBearingX >> 6;
//    int thischarvertbearingy = (*thisface)->glyph->metrics.vertBearingY >> 6;
//    int thischarvertadvance  = (*thisface)->glyph->metrics.vertAdvance  >> 6;
//    int thischarascender     = (*thisface)->ascender  >> 6;
//    int thischardescender    = (*thisface)->descender >> 6;
//    int scaledwidth  = srcwidth * numerscale / denomscale;
//    int scaledheight = srcheight * numerscale / denomscale;
    int scaledhoribearingx = thischarhoribearingx * numerscale / denomscale;
//    int scaledhoribearingy = thischarhoribearingy * numerscale / denomscale;
    int scaledhoriadvance  = thischarhoriadvance  * numerscale / denomscale;
//    int scaledvertbearingx = thischarvertbearingx * numerscale / denomscale;
//    int scaledvertbearingy = thischarvertbearingy * numerscale / denomscale;
//    int scaledvertadvance  = thischarvertadvance  * numerscale / denomscale;
//    int scaledascender     = thischarascender     * numerscale / denomscale;
//    int scaleddescender    = thischardescender    * numerscale / denomscale;

//    int glyphheight = scaledascender - scaleddescender;

//    int ydiff = (0 - scaledhoribearingy - sdy);
//    int gravityy = y + ydiff + scaledascender - scaleddescender;
//    int gravityfix = scaledvertadvance + scaledheight;
//    int inversey = (gravityy + gravityfix) * sbitmap->pitch;
//    int targetix = inversey + (gravityx * channels);
//          int sourceix = (dy * srcbmp->pitch) + dx;
//    int sourceix = ((srcheight - dy) * srcbmp->pitch) + dx;

    int thischarbmpwidth  = srcbmp->width;
    int thischarbmpheight = srcbmp->rows;
    int scaledbmpwidth  = thischarbmpwidth  * numerscale / denomscale;
    int scaledbmpheight = thischarbmpheight * numerscale / denomscale;
//    IF (FT_HAS_KERNING(*thisface)) {
//      FT_Vector kernpixels = { x: srcbmp->width + 1, y: srcbmp->rows + 1 }; // bitmap size + 1 pixel by default
//      error = FT_Get_Kerning(*thisface, lastglyphix, glyphix, FT_KERNING_DEFAULT, &kernpixels);
//      if (error) { printf("GETKERNINGERROR\n"); } // continue anyway (hope it didn't update if it failed)
//      MSG("KERNPIXELSX: %ld\n", kernpixels.x);
//      x += kernpixels.x; // TODO: consider partial pixels ...
//      IF (aftercursor)
//        { rightwidth += kernpixels.x; } // + 1 ?
//      EL { leftwidth += kernpixels.x; } // + 1 ?
//      // use y kerning to set maximum line height ? no ... .. irrelevant info unless vertical text
//    } else { // no kerning in this font (use srcbmp width + 1)

    IF (packchars) {  // use width of glyph bitmap
      x += scaledbmpwidth + 1; // one pixel gap !
      IF (aftercursor)
        { rightwidth += scaledbmpwidth + 1; }
      EL { leftwidth += scaledbmpwidth + 1; }
    } EL {            // use font-specified advance
      IF (scaledhoriadvance GT 0) {
        int xoffset = scaledhoriadvance + 1;
        IF (scaledhoribearingx GT 0)
          { xoffset -= scaledhoribearingx; } // or half to centre ?
        x += xoffset;
        IF (aftercursor) { rightwidth += xoffset; }
        EL               { leftwidth  += xoffset; }
      } // EL this is an accent for a previous character
    }
// TODO: fix with scaledlineheight / scaledheight
// not yet needed as no menu highlight
    IF (packchars) { // using bitmap size
      IF (scaledbmpheight GT scaledlineheight)
        { scaledlineheight = scaledbmpheight + 1; }
    } EL {           // using glyph metrics
      IF (glyphheight GT scaledlineheight)
        { scaledlineheight = glyphheight + 1; }
    }
    lastglyphix = glyphix;
    thisface = firstface;
    AFTERDO { } // FT_Done_Glyph only for FT_Get_Glyph? // free srcbmp ?!?!?!
  }
  highlight->leftwidth  = leftwidth;
  highlight->rightwidth = rightwidth;
  highlight->height     = scaledlineheight; // todo: multiline
}

VDT aligncursor(Textbox *textbox, Highlight *highlight, Cursor *cursor) {
  Cursor newcursor = { offsetx: 0, line: 1 };
  CS textstart = textbox->textstring;
  CS textend   = highlight->cursorpoint;
  CS anylines   = textstart;
  CS cursorline = textstart;
  WI (*anylines != '\0') {
    IF (anylines >= highlight->cursorpoint) { BK; }
    IF (*anylines == '\n') {
      INC newcursor.line;
      cursorline = &anylines[1];
    } // align cursor start of line
    INC anylines;
  } // count linebreaks before cursor
  Highlight precursor = NoHighlight;  // initialise blank
  precursor.startpoint  = cursorline; // start of current line
  precursor.endpoint    = textend;    // to current position
  precursor.cursorpoint = textend;    // expect rightwidth 0
  alignhighlight(textbox, &precursor);
  newcursor.offsetx = precursor.leftwidth;
  *cursor = newcursor;
}

grColor getcolour(grBitmap *sbitmap, CH code) {
  Colour c = colourcode(code);
  grColor colour = grFindColor(sbitmap, c.r, c.g, c.b, c.a);
  return colour;
}

typedef struct _Item {
  INT number; // EOF for empty list
  struct _Item *next;
} Item;

Item negadvancechars = { number: EOF, next: NULL };

INT incache(INT number, Item *list) {
  IF (!list) { RT 0; } // no list
  WI (list) {
    IF (number EQ list->number) { RT 1; }
    list = list->next;
  }
  RT 0; // not in list
}

VDT addtocache(INT number, Item *list) {
  IF (!list) { RT; } // no list
  IF (incache(number, list)) { RT; } // don't re-add
  IF (list->number EQ EOF) {
    IF (list->next EQ NULL) {
      IF (number NQ EOF) {
        list->number = number; // set first item
      } EL { MSG("FIRST ITEM CANNOT BE EOF\n"); RT; }
    } EL { MSG("EOF IN ACTIVE LIST\n"); RT; }
  } EL { // one or more items
    Item *newitem = (Item *)malloc(sizeof(Item));
    newitem->number = number;
    newitem->next   = NULL;
    WI (list->next) { list = list->next; }
    list->next = newitem; // add to end of list
  }
}

VDT freecache(Item *list) {
  IF (!list) { RT; } // no list
  Item *firstitem = list;
  WI (list) {
    Item *nextitem = list->next;
    IF (list NQ firstitem) {
      free(list);
    } // don't free first (static) item
    list = nextitem;
  }
}

INT nextcharisaccent(CS here, CS textend) {
  CS *there = &here; // discard offset
  INT nextchar = nextutf8(there, textend);
  RT (incache(nextchar, &negadvancechars)) ? 1 : 0;
}

INT nextcharisasciiwordchar(CS here, CS textend) {
  CS *there = &here; // discard offset
  INT nextchar = nextutf8(there, textend);
  RT ASCIIWORDCHAR(nextchar) ? 1 : 0;
}

INT thischarisaccent(INT thischar) {
  RT (incache(thischar, &negadvancechars)) ? 1 : 0;
}

INT thischarisasciiwordchar(INT thischar) {
  RT ASCIIWORDCHAR(thischar) ? 1 : 0;
}

INT prevcharisasciiwordchar(CS here, CS textstart) {
  CS *there = &here; // discard offset
  INT prevchar = prevutf8(there, textstart);
  RT ASCIIWORDCHAR(prevchar) ? 1 : 0;
}


typedef struct _Context {
  Textbox *textbox;
  Text text;
  Colour colour;
  Cursor cursor;
  Highlight highlight;
} Context;

VDT highlightcursor(Context *context) {
  Highlight highlight = NoHighlight;
  // cursor can only be multiline if it's a highlight (start != end)
  // only using text.cursorend for now until highlight boxes
  highlight.cursorpoint = context->text.cursorend;
  highlight.startpoint = highlight.cursorpoint;
  WI (    highlight.startpoint GT context->text.string
      AND *(highlight.startpoint - 1) NQ '\n') { DEC highlight.startpoint; }
  highlight.endpoint   = highlight.cursorpoint;
  WI (    *highlight.endpoint NQ NUL
      AND *highlight.endpoint         NQ '\n') { INC highlight.endpoint;   }
  alignhighlight(context->textbox, &highlight);
  highlight.startpoint = highlight.cursorpoint;
  highlight.endpoint   = highlight.cursorpoint;
  aligncursor(context->textbox, &highlight, &(context->cursor));
  highlight.menu = NoTranslations; // translationmenu(&highlight);
  // translationmenu can deduce all possible translations involving current cursorpoint
  highlight.colour = DARKCYANCOLOUR; // 0 size except at init
  context->highlight = highlight;
}

VDT selectall(Context *context) {
  CS stringend = context->text.string;
  WI (*stringend) { INC stringend; }
  context->text.cursorstart = context->text.string;
  context->text.cursorend   = stringend;
  highlightcursor(context);
}

VDT cursortostart(Context *context) {
  context->text.cursorstart = context->text.string;
  context->text.cursorend   = context->text.string;
  highlightcursor(context);
}

VDT cursortoend(Context *context) {
  CS stringend = context->text.string;
  WI (*stringend) { INC stringend; }
  context->text.cursorstart = stringend;
  context->text.cursorend   = stringend;
  highlightcursor(context);
}

VDT writetext(Context *context, CS textstring) {
  writetextstring(&(context->text), textstring);
  highlightcursor(context);
}

VDT appendtext(Context *context, CS appendstring) {
  appendtextstring(&(context->text), appendstring);
  highlightcursor(context);
}

VDT appendtextchar(Context *context, CH appendchar) {
  CH appendstring[2] = { appendchar, NUL };
  appendtextstring(&(context->text), appendstring);
  highlightcursor(context);
}

CS getlineend(CS srclines, INT targetline) {
  IF (targetline LT 1) { RT srclines; } // 0 is start
  INT currentline = 1;
  WI (*srclines) {
    IF (*srclines EQ '\n') {
      IF (currentline EQ targetline) { break; }
      EL { INC currentline; }
    }
    INC srclines;
  }
  RT srclines; // at \n or \0 of targetline
}

INT gettextheight(Textbox *textbox, CS cursorend) {
  CS srctext = textbox->textstring;
  Font *firstfont = &(textbox->font);
  FT_Face *firstface = &(firstfont->face);
  FT_Face *thisface = firstface;
  INT numerscale = textbox->fontscalenumer;
  INT denomscale = textbox->fontscaledenom;
  INT packchars  = textbox->packchars;
  INT xstart = 0; // textbox->marginx;
  INT ystart = 0; // textbox->marginy;
  INT x = xstart;
  INT y = ystart;
  CS textp = srctext; // no need for cursorstart
  IF (cursorend EQ textp) { RT y; } // return 0
  CS textend = textp + strlen(textp);
  IF (cursorend) { textend = cursorend; }
  UIT lastglyphix = 0;
  INT ch = 0;
  INT lastch = 0;
  INT spacewidth = textbox->font.width >> 1; // half a char
  INT fontheight = textbox->font.height;
  INT lineheight = fontheight + 1;
  INT scaledspacewidth = spacewidth * numerscale / denomscale;
  INT scaledlineheight = fontheight * numerscale / denomscale + 1;
  INT numlines = 1;
  while (ch >= 0) { // loop until done
    ch = nextutf8(&textp, textend);
    if (ch LT 0) { printf("EOF\n"); break; } // don't write EOF
    if (ch EQ 0) { break; } // or NUL
    if (ch EQ ' ' ) { CT; } // not measuring width
    if (ch EQ '\n') {
      y += scaledlineheight;
      x = xstart;
      INC numlines;
      scaledlineheight = fontheight * numerscale / denomscale + 1;
      CT; // no glyph
    }
    UIT glyphix = FT_Get_Char_Index(*thisface, ch);
    IF (glyphix EQ 0) { // not in this font
      Font *nextfont = firstfont;
      WI (glyphix EQ 0 AND nextfont) {
        thisface = &(nextfont->face);
        glyphix = FT_Get_Char_Index(*thisface, ch);
        nextfont = nextfont->next;
      }
      IF (!nextfont) { thisface = firstface; MSG("MISSINGGLYPH GETTEXTHEIGHT\n"); }
    }
    error = FT_Load_Glyph(*thisface, glyphix, FT_LOAD_RENDER | FT_LOAD_COLOR);
    if (error) { printf("LOADGLYPHFAIL\n"); } // FT_LOAD_DEFAULT
    ELSEDO {
      FT_Bitmap *srcbmp = &(*thisface)->glyph->bitmap;
      int srcheight  = srcbmp->rows;
      int thischarhoribearingy = (*thisface)->glyph->metrics.horiBearingY >> 6;
      int thischarvertadvance  = (*thisface)->glyph->metrics.vertAdvance  >> 6;
      int thischarascender     = (*thisface)->ascender  >> 6;
      int thischardescender    = (*thisface)->descender >> 6;
      int scaledheight = srcheight * numerscale / denomscale;
      int scaledhoribearingy = thischarhoribearingy * numerscale / denomscale;
      int scaledvertadvance  = thischarvertadvance * numerscale / denomscale;
      int scaledascender     = thischarascender  * numerscale / denomscale;
      int scaleddescender    = thischardescender * numerscale / denomscale;
      int ydiff = (0 - scaledhoribearingy); // - sdy);
      int gravityy = /*y +*/ ydiff + scaledascender - scaleddescender;
      int gravityfix = scaledvertadvance + scaledheight;
      int glyphheight = gravityy + gravityfix; // ) * sbitmap->pitch;
      IF (glyphheight GT scaledlineheight)
        { scaledlineheight = glyphheight + 1; }
      lastglyphix = glyphix;
//      lastch = ch;
      thisface = firstface; // prefer first font instead of continuing with current font
    } AFTERDO { } // FT_Done_Glyph only for FT_Get_Glyph? // free srcbmp ?!?!?!
  }
//  if (lastch NQ '\n')
   y += scaledlineheight;
MSG("NUMLINES: %d\n", numlines);
  RT y; // total height until cursorend
}

INT gettextwidth(Textbox *textbox) {
  CS srctext = textbox->textstring;
  Font *firstfont = &(textbox->font);
  FT_Face *firstface = &(firstfont->face);
  FT_Face *thisface = firstface;
  INT numerscale = textbox->fontscalenumer;
  INT denomscale = textbox->fontscaledenom;
  INT packchars  = textbox->packchars;
  INT xstart = 0; // textbox->marginx;
  INT ystart = 0; // textbox->marginy;
  INT x = xstart;
  INT y = ystart;
  CS textp = srctext;
  CS textend = textp + strlen(textp);
  UIT lastglyphix = 0;
  INT ch = 0;
  INT spacewidth = textbox->font.width >> 1; // half a char
  INT lineheight = textbox->font.height;
  INT scaledspacewidth = spacewidth * numerscale / denomscale;
  INT scaledlineheight = lineheight * numerscale / denomscale;
  while (ch >= 0) { // loop until done
    ch = nextutf8(&textp, textend);
    if (ch < 0) { printf("EOF\n"); break; } // don't write EOF
    if (ch == 0) { /*printf("NUL\n");*/ break; } // or NUL
    if (ch == ' ' ) { x += scaledspacewidth + 1; continue; }
    if (ch == '\n') { x = xstart; y += scaledlineheight + 1; continue; }
    // ^ not applying glyphheight but need x to reset
    UIT glyphix = FT_Get_Char_Index(*thisface, ch);
    IF (glyphix EQ 0) { // not in this font
      Font *nextfont = firstfont;
      WI (glyphix EQ 0 AND nextfont) {
        thisface = &(nextfont->face);
        glyphix = FT_Get_Char_Index(*thisface, ch);
        nextfont = nextfont->next;
      }
      IF (!nextfont) { thisface = firstface; MSG("MISSINGGLYPH GETTEXTWIDTH\n"); }
    }
    error = FT_Load_Glyph(*thisface, glyphix, FT_LOAD_RENDER | FT_LOAD_COLOR);
    if (error) { printf("LOADGLYPHFAIL\n"); } // FT_LOAD_DEFAULT
    ELSEDO {

      FT_Bitmap *srcbmp = &(*thisface)->glyph->bitmap;
      int srcwidth  = srcbmp->width;
      int thischarhoribearingx = (*thisface)->glyph->metrics.horiBearingX >> 6;
      int thischarhoriadvance  = (*thisface)->glyph->metrics.horiAdvance  >> 6;
      int scaledwidth  = srcwidth * numerscale / denomscale;
      int scaledhoribearingx = thischarhoribearingx * numerscale / denomscale;
      int scaledhoriadvance  = thischarhoriadvance * numerscale / denomscale;
      IF (scaledhoriadvance GT 0) {
        IF (packchars) {
          x += scaledwidth + 1;
        } EL {
          x += scaledhoriadvance + 1;
          IF (scaledhoribearingx GT 0)
            { x -= scaledhoribearingx; } // or half to centre ?
        }
      } EL { // this is an accent for the previous character
        addtocache(ch, &negadvancechars);
      } // presume here is enough -- rendertext does not addtocache
      lastglyphix = glyphix;
      thisface = firstface;
    } AFTERDO { } // FT_Done_Glyph only for FT_Get_Glyph? // free srcbmp ?!?!?!
  }
  RT x;
}

#define BLEND(magnitude, amplitude) \
  (magnitude * amplitude / 255)
#define ALPHABLEND(fg, bg, alpha) \
  (fg * alpha / 255) + (bg * (255 - alpha) / 255)

VDT rendertext(grBitmap *sbitmap, Textbox *textbox, Colour colour) {
  CS srctext = textbox->textstring;
  Font *firstfont = &(textbox->font);
  FT_Face *firstface = &(firstfont->face);
  FT_Face *thisface = firstface;
  INT numerscale = textbox->fontscalenumer;
  INT denomscale = textbox->fontscaledenom;
  INT packchars  = textbox->packchars;
  INT xstart = textbox->marginx;
  INT ystart = textbox->marginy;
  INT x = xstart;
  INT y = ystart;
  CS textp = srctext;
  CS textend = textp + strlen(textp);
  UIT lastglyphix = 0;
  INT ch = 0;
  INT spacewidth = textbox->font.width >> 1; // half a char
  INT fontheight = textbox->font.height;
  INT lineheight = fontheight + 1;
  INT scaledspacewidth = spacewidth * numerscale / denomscale;
  INT scaledfontheight = fontheight * numerscale / denomscale;
  INT scaledlineheight = scaledfontheight + 1;
  while (ch >= 0) { // loop until done
    ch = nextutf8(&textp, textend);
    if (ch < 0) { printf("EOF\n"); break; } // don't write EOF
    if (ch == 0) { /*printf("NUL\n");*/ break; } // or NUL
    if (ch == ' ' ) { x += scaledspacewidth + 1; continue; }
    if (ch == '\n') {
      x = xstart;
      y += scaledlineheight + 1;
      scaledlineheight = scaledfontheight + 1;
      continue;
    }
    UIT glyphix = FT_Get_Char_Index(*thisface, ch);
    IF (glyphix EQ 0) { // not in this font
      Font *nextfont = firstfont;
      WI (glyphix EQ 0 AND nextfont) {
        thisface = &(nextfont->face);
        glyphix = FT_Get_Char_Index(*thisface, ch);
        nextfont = nextfont->next;
      }
      IF (!nextfont) { thisface = firstface; MSG("MISSINGGLYPH RENDERTEXT\n"); }
    } // if (glyphix == 0) { printf("MISSINGGLYPH RENDERTEXT\n"); } // continue anyway..
    error = FT_Load_Glyph(*thisface, glyphix, FT_LOAD_RENDER | FT_LOAD_COLOR);
    if (error) { printf("LOADGLYPHFAIL\n"); } // FT_LOAD_DEFAULT
    ELSEDO {
//      error = FT_Render_Glyph(glyphix, FT_RENDER_MODE_NORMAL);
      FT_Bitmap *srcbmp = &(*thisface)->glyph->bitmap;
      int srcwidth  = srcbmp->width;
      int srcheight = srcbmp->rows;
//    int thischarheight = (*thisface)->glyph->metrics.height >> 6;
//    int thischarwidth  = (*thisface)->glyph->metrics.width  >> 6;
      int thischarhoribearingx = (*thisface)->glyph->metrics.horiBearingX >> 6;
      int thischarhoribearingy = (*thisface)->glyph->metrics.horiBearingY >> 6;
      int thischarhoriadvance  = (*thisface)->glyph->metrics.horiAdvance  >> 6;
      int thischarvertbearingx = (*thisface)->glyph->metrics.vertBearingX >> 6;
      int thischarvertbearingy = (*thisface)->glyph->metrics.vertBearingY >> 6;
      int thischarvertadvance  = (*thisface)->glyph->metrics.vertAdvance  >> 6;
      int thischarascender     = (*thisface)->ascender  >> 6;
      int thischardescender    = (*thisface)->descender >> 6;
      int scaledwidth  = srcwidth * numerscale / denomscale;
      int scaledheight = srcheight * numerscale / denomscale;
      int scaledhoribearingx = thischarhoribearingx * numerscale / denomscale;
      int scaledhoribearingy = thischarhoribearingy * numerscale / denomscale;
      int scaledhoriadvance  = thischarhoriadvance * numerscale / denomscale;
      int scaledvertbearingx = thischarvertbearingx * numerscale / denomscale;
      int scaledvertbearingy = thischarvertbearingy * numerscale / denomscale;
      int scaledvertadvance  = thischarvertadvance * numerscale / denomscale;
      int scaledascender     = thischarascender  * numerscale / denomscale;
      int scaleddescender    = thischardescender * numerscale / denomscale;
      int ydiff = (0 - scaledhoribearingy); // - sdy);
      int gravityy = /*y +*/ ydiff + scaledascender - scaleddescender;
      int gravityfix = scaledvertadvance + scaledheight;
      int glyphheight = gravityy + gravityfix; // ) * sbitmap->pitch;
      IF (packchars) { // using bitmap size
        IF (scaledheight GT scaledlineheight)
          { scaledlineheight = scaledheight + 1; }
      } EL {           // using glyph metrics
        IF (glyphheight GT scaledlineheight)
          { scaledlineheight = glyphheight + 1; }
      }
      int dy = 0;
      while (dy < srcbmp->rows) {
        int dx = 0;
        while (dx < srcbmp->width) {
          // same pixel overwritten? consider mixing the two values
          int sdx = dx * numerscale / denomscale;
          int sdy = dy * numerscale / denomscale;
          int channels = 3; // rgb24
          int gravityx = sdx + x + scaledhoribearingx;
          int sdydiff = ydiff - sdy;
          int gravityy = y + sdydiff + scaledascender - scaleddescender;
          int gravityfix = scaledvertadvance + scaledheight;
          int inversey = (gravityy + gravityfix) * sbitmap->pitch;

// fixes the I in pighead, but offsets thai script accents forward
//          int inversex = gravityx - scaledhoribearingx;

// scaledhoriadvance

          int inversex = gravityx; // - scaledhoribearingx;
          IF (scaledhoribearingx GT 0)
            { inversex -= scaledhoribearingx; }

          int targetix = inversey + (inversex * channels);
//          int targetix = inversey + (gravityx * channels);
//          int sourceix = (dy * srcbmp->pitch) + dx;
          int sourceix = ((srcheight - 1 - dy) * srcbmp->pitch) + dx;
          int backred   = sbitmap->buffer[targetix + 0];
          int backgreen = sbitmap->buffer[targetix + 1];
          int backblue  = sbitmap->buffer[targetix + 2];
          int forered   = BLEND(colour.r, srcbmp->buffer[sourceix]);
          int foregreen = BLEND(colour.g, srcbmp->buffer[sourceix]);
          int foreblue  = BLEND(colour.b, srcbmp->buffer[sourceix]);
          int forealpha = BLEND(colour.a, srcbmp->buffer[sourceix]);
          int outred   = ALPHABLEND(forered,   backred,   forealpha);
          int outgreen = ALPHABLEND(foregreen, backgreen, forealpha);
          int outblue  = ALPHABLEND(foreblue,  backblue,  forealpha);
          sbitmap->buffer[targetix + 0] = outred;
          sbitmap->buffer[targetix + 1] = outgreen;
          sbitmap->buffer[targetix + 2] = outblue;
          dx++;
        }
        dy++;
      }
//      INT scaledwidth = srcbmp->width * numerscale / denomscale;
      IF (scaledhoriadvance GT 0) {
        IF (packchars) {
          x += scaledwidth + 1;
//        } EL { x += scaledhoriadvance + 1; }
        } EL {
          x += scaledhoriadvance + 1;
          IF (scaledhoribearingx GT 0)
            { x -= scaledhoribearingx; } // or half to centre ?
//          + scaledhoribearingx + 1;
        }
      } EL {
        // this is an accent for the previous character
      }
      lastglyphix = glyphix;
      thisface = firstface;
    } AFTERDO { } // FT_Done_Glyph only for FT_Get_Glyph? // free srcbmp ?!?!?!
  }
}

#define MINIHALF          / 2
#define MINIHALFSIZE      1, 2
#define MINITHIRD         / 3
#define MINITHIRDSIZE     1, 3
#define MINITWOTHIRD      * 2 / 3
#define MINITWOTHIRDSIZE  2, 3

#define MENUTEXTSIZE      MINIHALFSIZE
#define MENUDIVISOR       MINIHALF

INT calculatemenuwidth(Textbox *fontsource, Translation *first) {
  Textbox textbox = minitextbox(fontsource, MENUTEXTSIZE, 0, 0);
  INT maximumwidth = CURSORWIDTH;
  Translation *t = first; // menu.first
  WI (t) {
    IF (!(t->sequence)) { MSG("MISSING SEQUENCE\n"); RT 1; }
    textbox.textstring = (CS)t->sequence;
    textbox.textstringsize = strlen(textbox.textstring);
    INT textwidth = gettextwidth(&textbox);
    IF (textwidth GT maximumwidth)
      { maximumwidth = textwidth; }
    t = t->next;
  }
  freeminitextbox(&textbox);
  RT maximumwidth;
}

INT calculatemenurowheight(Textbox *fontsource, Translation *first) {
  Textbox textbox = minitextbox(fontsource, MENUTEXTSIZE, 0, 0);

  INT lineheight = fontsource->font.height;
  INT menufontheight = lineheight MENUDIVISOR;

  INT maximumheight = menufontheight;
  Translation *t = first; // menu.first;
  WI (t) {
    IF (!(t->sequence)) { MSG("MISSING SEQUENCE\n"); RT 1; }
    textbox.textstring = (CS)t->sequence;
    textbox.textstringsize = strlen(textbox.textstring);
    INT textheight = gettextheight(&textbox, NULL); // scaling applied
    IF (textheight GT maximumheight)
      { maximumheight = textheight; }
    t = t->next;
  }
  freeminitextbox(&textbox);
  RT maximumheight;
}

VDT rendermenutext(grBitmap *sbitmap, Textbox *fontsource, INT left, INT top,
                   INT width, INT rowheight, Translation *first, INT selindex) {
  Colour listcolour   = colourcode(BLACKCOLOUR);
  Colour selectcolour = colourcode(FC3COLOUR);
  Textbox textbox = minitextbox(fontsource, MINIHALFSIZE, left, top);
  INT lineindex = 1;
  Translation *t = first; // menu.first;
  WI (t) {
    IF (!(t->sequence)) { MSG("MISSING SEQUENCE\n"); RT; }
    textbox.textstring = (CS)t->sequence;
    textbox.textstringsize = strlen(textbox.textstring);
    IF (lineindex EQ selindex) {
      rendertext(sbitmap, &textbox, selectcolour);
    } EL { rendertext(sbitmap, &textbox, listcolour); }
    textbox.marginy += rowheight;
    t = t->next;
    INC lineindex;
  }
  freeminitextbox(&textbox);
}

VDT rendermenubox(grBitmap *sbitmap, INT left, INT top, INT width,
                  INT rowheight, INT itemcount, INT selindex       ) {
  INT row = 0;
  WI (INC row LQ itemcount) {
    IF (row EQ selindex) {
      grColor colour = getcolour(sbitmap, YELLOWCOLOUR);
      grFillRect(sbitmap, left, top, width, rowheight, colour);
    } EL {
      grColor colour = getcolour(sbitmap, CYANCOLOUR);
      grFillRect(sbitmap, left, top, width, rowheight, colour);
    }
    top += rowheight;
  }
}

VDT rendertranslationmenu(grBitmap *sbitmap, Context *context) {
  Textbox *srcbox = context->textbox;
  INT topmargin = srcbox->marginy;
  CS srclineend = getlineend(srcbox->textstring, context->cursor.line);
  INT top = topmargin + gettextheight(srcbox, srclineend);
  INT leftmargin = srcbox->marginx;
  INT leftoffset = context->cursor.offsetx;
  INT leftwidth = context->highlight.leftwidth;
  INT rightwidth = context->highlight.rightwidth;
  INT left = leftmargin + leftoffset - leftwidth;
  INT rowleftmargin = 2;
  INT rowrightmargin = 2;
  INT textleft = rowleftmargin + left;
  INT menuwidth  = leftwidth + rightwidth; // underline
  Translations *menu = &(context->highlight.menu);
  Translation *trlist = &(menu->first);
  INT selindex = menu->selectionindex;
  INT itemcount = menu->totalitems;
  IF (!MENUPHASNOTRANSLATIONS(menu)) {
    menuwidth = rowleftmargin + rowrightmargin + calculatemenuwidth(srcbox, trlist);
    INT menurowheight = calculatemenurowheight(srcbox, trlist);
    rendermenubox(sbitmap, left, top, menuwidth, menurowheight, itemcount, selindex);
    rendermenutext(sbitmap, srcbox, textleft, top, menuwidth, menurowheight, trlist, selindex);
  } EL { rendermenubox(sbitmap, left, top, menuwidth, CURSORWIDTH, 1, 1); }
}

VDT renderhighlight(grBitmap *sbitmap, Context *context) {
  Textbox *srcbox = context->textbox;
  INT topmargin = srcbox->marginy;
MSG("ON LINE: %d\n", context->cursor.line);
  CS prevlineend = getlineend(srcbox->textstring, context->cursor.line - 1);
MSG("prevlineend: \"%s\"\n", prevlineend);
  INT lineendlen = prevlineend - srcbox->textstring;
MSG("LINEENDLEN: %d\n", lineendlen);
  INT top = topmargin + gettextheight(srcbox, prevlineend);
MSG("TOP: %d\n", top);
  INT leftmargin = context->textbox->marginx;
  INT leftoffset = context->cursor.offsetx;
  INT leftwidth = context->highlight.leftwidth;
  INT rightwidth = context->highlight.rightwidth;
  INT left = leftmargin + leftoffset - leftwidth;
  INT width  = leftwidth + rightwidth;
  INT height = context->highlight.height;
  INT ctop = top - 1;
  INT cheight = height + 2;
  INT cleft = leftmargin + leftoffset;
  INT cwidth = CURSORWIDTH;
  cleft -= (cwidth / 2); // centred cursor
  grColor backcolour = getcolour(sbitmap, context->highlight.colour);
  grColor cursorcolour = getcolour(sbitmap, FC3COLOUR);
  IF (context->highlight.menu.selectionindex GT 0) {
    grColor swap = backcolour;
    backcolour = cursorcolour;
    cursorcolour = swap;
  }
  grFillRect(sbitmap, left, top, width, height, backcolour);
  grFillRect(sbitmap, cleft, ctop, cwidth, cheight, cursorcolour);
}

VDT rendertextbox(grBitmap *sbitmap, Context *context) {
  rendertext(sbitmap, context->textbox, context->colour);
}

static char hasnocontext(Context *c) {
  return (c->text.string && c->textbox) ? 0 : 1;
} // it'll have context once a string and textbox is attached

// auto-spaces a separate function -- wodezhongwenbushuohenhao


VDT unclipmnemonichighlight(Context *context) {
  Highlight highlight = context->highlight;
  CS text = context->text.string;
  CS here = context->text.cursorend;
  CS washere = here;
  IF (!text OR !here) { MSG("NO STRING ATTACHED\n"); RT; }
  IF (here GT text AND *(here - 1) EQ ' ') {
    INC highlight.charsbefore;
    DEC here;
  } // include space in word select
  WI (1) { // continue until break
    IF (here LT text) { MSG("CURSOR OUTOFLOWBOUND\n"); RT; }
    EF (here EQ text) { /* MSG("STARTOFSTRING\n"); */ BK; }
    EL { // increment left side of highlight
      CH prevch = *(here - 1); // *(here - 1);
      IF (prevch EQ ' ' ) { BK; } // reached a space
      EF (prevch EQ '\n') { BK; } // reached a linebreak
      INT prevchar = prevutf8(&here, text);
      IF ASCIIWORDCHAR(prevchar) {
        INC highlight.charsbefore; // continue
        WI (prevcharisasciiwordchar(here, text)) {
          prevchar = prevutf8(&here, text);
          INC highlight.charsbefore;
        }
        BK; // stop before non-ascii symbol
      } EF (thischarisaccent(prevchar)) {
        INC highlight.charsbefore;
        // don't stop after accent unicode chars
      } EF ASCIIUNICODECHAR(prevchar) {
        INC highlight.charsbefore;
        BK; // stop after each non-accent unicode char
      } EL { // everything else is considered a symbol
        INC highlight.charsbefore;
        BK; // stop after each symbol
      }
    }
  }
  highlight.endpoint    = here;
  highlight.cursorpoint = here;
  alignhighlight(context->textbox, &highlight);
  aligncursor(context->textbox, &highlight, &(context->cursor));
  highlight.menu = translationmenu(&highlight);
  IF (!MENUHASNOTRANSLATIONS(highlight.menu)) {
    highlight.colour = CYANCOLOUR; // YELLOWCOLOUR;
    highlight.menu.selectionindex = 1;
  } EL { // select first row ^
    highlight.colour = CYANCOLOUR;
  }      // no translations ^
  context->text.cursorend = here;
  context->highlight = highlight;
}

VDT extendmnemonichighlight(Context *context) {
  Highlight highlight = context->highlight;
  CS text = context->text.string;
  CS here = context->text.cursorend;
  CS washere = here;
  IF (!text OR !here) { MSG("NO STRING ATTACHED\n"); RT; }
  CS textend = text + strlen(text);
  IF (here LT textend AND *here EQ ' ') {
    INC highlight.charsafter;
    INC here;
  } // include space in word select
  WI (1) { // continue until break
    IF (here GT textend) { MSG("CURSOR OUTOFHIGHBOUND\n"); RT; }
    EF (here EQ textend) { /* MSG("ENDOFSTRING\n"); */ BK; }
    EL { // increment left side of highlight
      CH nextch = *here; // *(here + 1);
      IF (nextch EQ ' ' ) { BK; } // reached a space
      EF (nextch EQ '\n') { BK; } // reached a linebreak
      INT nextchar = nextutf8(&here, textend);
      IF ASCIIWORDCHAR(nextchar) {
        INC highlight.charsafter; // continue
        WI (nextcharisasciiwordchar(here, textend)) {
          nextchar = nextutf8(&here, textend);
          INC highlight.charsafter;
        }
        BK; // stop before non-ascii symbol
      } EF ASCIIUNICODECHAR(nextchar) {
        INC highlight.charsafter;
        WI (nextcharisaccent(here, textend)) {
          nextchar = nextutf8(&here, textend);
          INC highlight.charsafter;
        } // include accents that follow
        BK; // stop after each unicode char
//    this would be used if accents preceded chars:
//      } EF (incache(nextchar, &negadvancechars)) {
//        INC highlight.charsafter;
      } EL { // everything else is considered a symbol
        INC highlight.charsafter;
        BK; // stop after each symbol
      }
    }
  }
  highlight.endpoint    = here;
  highlight.cursorpoint = here;
  alignhighlight(context->textbox, &highlight);
  aligncursor(context->textbox, &highlight, &(context->cursor));
  highlight.menu = translationmenu(&highlight);
  IF (!MENUHASNOTRANSLATIONS(highlight.menu)) {
    highlight.colour = CYANCOLOUR; // YELLOWCOLOUR;
    highlight.menu.selectionindex = 1;
  } EL { highlight.colour = CYANCOLOUR; }
  context->text.cursorend = here;
  context->highlight = highlight;
}

VDT selectcursorleft(Context *context) {
  IF (context->highlight.startpoint LQ context->highlight.endpoint) {
    unclipmnemonichighlight(context);
  } EL { unclipmnemonichighlight(context); } // unextend
}

VDT selectcursorright(Context *context) {
  IF (context->highlight.startpoint GQ context->highlight.endpoint) {
    extendmnemonichighlight(context);
  } EL { extendmnemonichighlight(context); } // clip
}

VDT highlightpreviousmenuitem(Context *context) {
  INT currentindex = context->highlight.menu.selectionindex;
  INT totalitems   = context->highlight.menu.totalitems;
  IF (DEC currentindex LT 0)       // shift off menu
    { currentindex = totalitems; } // loop back to end
  context->highlight.menu.selectionindex = currentindex;
}

VDT highlightnextmenuitem(Context *context) {
  INT currentindex = context->highlight.menu.selectionindex;
  INT totalitems   = context->highlight.menu.totalitems;
  IF (INC currentindex GT totalitems)
    { currentindex = 0; }          // loop off menu
  context->highlight.menu.selectionindex = currentindex;
}

VDT selectcurrentmenuitem(Context *context) {
  Translations *trlist = &(context->highlight.menu);
  INT currentindex = trlist->selectionindex;
  INT totalitems   = trlist->totalitems;
  IF (currentindex LQ 0) { // deselect (emulate self-replace)
    context->text.cursorstart = context->text.cursorend;
//    highlightcursor(context);
  } EL {
    Translation *tr = &(trlist->first);
    CST sequence = tr->sequence;
    INT trindex = 1;
    WI (trindex LT currentindex) {
      tr = tr->next;
      IF (!tr) { MSG("MISSING MENU ITEM\n"); RT; }
      sequence = tr->sequence;
      INC trindex;
    }
MSG("APPENDING SEQUENCE: %s\n", sequence);
    appendtext(context, (CS)sequence);
  }
  context->highlight.menu = NoTranslations;
  highlightcursor(context);
}

#define NEED(var, except) \
  var; IF (!(var)) { MSG(except "\n"); RT; }

VDT movecursorleft(Context *context) {
  Highlight highlight = context->highlight;
  CS text = NEED(context->text.string,    "NO STRING");
  CS here = NEED(context->text.cursorend, "NO CURSOREND");
  CS washere = here;
  CS textstart = text;
//  CS textend = text + strlen(text);
  INT prevchar = prevutf8(&here, textstart);
  IF (prevchar LT 0) { MSG("EOF\n"); RT; }  // not expected
  IF (prevchar EQ 0) { MSG("START OF TEXT\n"); } // expect at start of text
  // TODO have to load the char just to find its hori advance
  // that's quite expensive, so instead we'll keep a cache as they are written
  WI (thischarisaccent(prevchar)) {
    prevchar = prevutf8(&here, textstart);
    IF (prevchar LT 0) { MSG("EOF\n");           RT; } // not expected
    IF (prevchar EQ 0) { MSG("START OF TEXT\n"); BK; } // expect at start of text
  } // allow for multiple accents
  context->text.cursorstart = here;
  context->text.cursorend   = here;
  highlightcursor(context);
}

VDT movecursorright(Context *context) {
  Highlight highlight = context->highlight;
  CS text = NEED(context->text.string,    "NO STRING");
  CS here = NEED(context->text.cursorend, "NO CURSOREND");
  CS washere = here;
  CS textend = text + strlen(text);
  INT nextchar = nextutf8(&here, textend);
  IF (nextchar LT 0) { MSG("EOF\n");     RT; } // not expected
  IF (nextchar EQ 0) { MSG("END OF TEXT\n"); } // expect at end of text
  WI (nextcharisaccent(here, textend)) {
    nextchar = nextutf8(&here, textend);
    IF (nextchar LT 0) { MSG("EOF\n");         RT; } // not expected
    IF (nextchar EQ 0) { MSG("END OF TEXT\n"); BK; } // expect at end of text
  } // allow for multiple accents
  context->text.cursorstart = here;
  context->text.cursorend   = here;
  highlightcursor(context);
}

#define TEXTMODE     0
#define COMMANDMODE  1
INT processmode = TEXTMODE;

SINT processevent(grEvent event, Context *context) {
  INT ret = 1;
  if (event.type == gr_event_resize) {
    printf("RESIZE EVENT\n");
  } else if (event.type & gr_event_mouse == gr_mouse_move) {
    printf("MOUSE IS MOVING\n");
  } else if (event.key == grKeyEsc) {
    if (processmode == TEXTMODE)
      { processmode = COMMANDMODE; }
    else { processmode = TEXTMODE; }
  } else if (processmode == TEXTMODE) {
    if        (grKEY(event.key) >= 'a' && grKEY(event.key) <= 'z') {
      appendtextchar(context, grKEY(event.key) - 'a' + 'a');
    } else if (grKEY(event.key) >= 'A' && grKEY(event.key) <= 'Z') {
      appendtextchar(context, grKEY(event.key) - 'A' + 'A');
    } else if (grKEY(event.key) >= '0' && grKEY(event.key) <= '9') {
      appendtextchar(context, grKEY(event.key) - '0' + '0');
    } else if (grKEY(event.key) == grKeyBackSpace) {
      appendtextchar(context, '\b');
    } else if (grKEY(event.key) == grKeySpace) {
      appendtextchar(context, ' ');
    } else if (grKEY(event.key) == grKeyUp) {
//TODO      highlightpreviousmenuitem();
    } else if (grKEY(event.key) == grKeyDown) {
//TODO      highlightnextmenuitem();
    } else if (grKEY(event.key) == grKeyLeft) {
      if (event.key & grKeyShift) {
        MSG("SHIFT + LEFT\n");
      } else { MSG("LEFT\n"); }
    } else if (grKEY(event.key) == grKeyRight) {
      if (event.key & grKeyShift) {
        MSG("SHIFT + RIGHT\n");
      } else { MSG("RIGHT\n"); }
    } else if (/*event.key == grKeyPlus       ||  */
     //          event.key == grKeyLess       || <-------- this is key DASH
          /*     event.key == grKeyEqual      ||   */
               event.key == grKeyMult       ||
               event.key == grKeyDollar     || /* || event.key == grKeySmaller */ 
               event.key == grKeyGreater    || event.key == grKeyQuestion   ||
               event.key == grKeyComma      || event.key == grKeyDot        ||
               event.key == grKeySemiColon  || event.key == grKeyColon      ||
               event.key == grKeyDiv        || event.key == grKeyExclam     ||
               event.key == grKeyPercent    || event.key == grKeyLeftParen  ||
               event.key == grKeyRightParen || event.key == grKeyAt         ||
               event.key == grKeyBackSlash  || event.key == grKeyCircumflex ||
       /*        event.key == grKeyUnder      ||    */ // underscore is SHIFT dash ?
                                             event.key == grKeyBackTick     ) {
MSG("KEY: %d %c\n", event.key, event.key);
      appendtextchar(context, event.key);
    } else if (event.key == '"') {
      appendtextchar(context, '"');
    } else if (event.key == '\'') {
      appendtextchar(context, '\'');
    } else if (grKEY(event.key) EQ grKeyLeftB) {  // LEFT  [
      movecursorleft(context);
    } else if (grKEY(event.key) EQ grKeyRightB) { // RIGHT ]
      movecursorright(context);
    } else if (grKEY(event.key) EQ '{') { // SHIFT+LEFT    {
      selectcursorleft(context);
    } else if (grKEY(event.key) EQ '}') { // SHIFT+RIGHT   }
      selectcursorright(context);
    } else if (grKEY(event.key) EQ '-') { // SHIFT+RIGHT   }
      highlightpreviousmenuitem(context);
    } else if (grKEY(event.key) EQ '=') { // SHIFT+RIGHT   }
      highlightnextmenuitem(context);
    } else if (grKEY(event.key) == grKeyReturn) {
      IF (context->highlight.menu.selectionindex GT 0) {
        selectcurrentmenuitem(context);
      } EL { appendtextchar(context, '\n'); }
    }
  } else if (processmode == COMMANDMODE) {
    if (event.key == grKEY('q')) {
      ret = 0; // exit event processing loop
    } else if (grKEY(event.key) >= '0' && grKEY(event.key) <= '9') {
      // number commands -- set index of selected feature
    }
  }
  // grKeySpace
  // grKeyBackSpace
  // grKeyTab
  RT ret;
}

static struct setup_ {
  UNT ttiversion;
  INT dispwidth;
  INT dispheight;
  INT dispmode;
//  INT dispres;
  INT dispgreys;
  grSurface *surface;
  grBitmap  *surfacebmp;
} setup = {
  0                  , // ttinterpreterversion
  1600               , // dispwidth
  800                , // dispheight
  gr_pixel_mode_rgb24, // dispdepth
//  72                 , // dispres (ppi? not used?)
  256                , // dispgreys
  NULL                 // surfacebmp
};

int main(int argc, char **argv) {
  FT_Library ftlib;
  if (FT_Init_FreeType(&ftlib) != 0)
    { printf("FREETYPEINITFAIL\n"); return 1; }
  ELSEDO // FreeType is initialised
  FT_Int major, minor, patch;
  FT_Library_Version(ftlib, &major, &minor, &patch);
  FT_Property_Get(ftlib, "truetype", "interpreter-version", &setup.ttiversion);
  printf("FreeType v%d.%d(.%d)", major, minor, patch);
  printf(" TrueType v%d\n", setup.ttiversion); // expect v35, v38 or v40
//  FT_Property_Get(ftlib, "cff",   "hinting-engine", &status.cffhint);
//  FT_Property_Get(ftlib, "type1", "hinting-engine", &status.type1hint);
//  FT_Property_Get(ftlib, "t1cid", "hinting-engine", &status.t1cidhint);
// expect FT_HINTING_FREETYPE or FT_HINTING_ADOBE

  grInitDevices();
 
  grBitmap scrbuf;
  scrbuf.mode  = setup.dispmode;
  scrbuf.width = setup.dispwidth;
  scrbuf.rows  = setup.dispheight;
  scrbuf.grays = setup.dispgreys;
  setup.surface = grNewSurface(0, &scrbuf);
  if (!setup.surface) { printf("NEWSURFACEFAIL\n"); break; }
  setup.surfacebmp = &setup.surface->bitmap;

  printf("scrbuf&: %12lx, surfacebmp: %12lx\n",
           (unsigned long)(void *)&scrbuf,
           (unsigned long)(void *)setup.surfacebmp);
  printf("default GAMMA: %f\n", GAMMA);
    // grSetTargetGamma(bsurface, GAMMA);

  Textbox textbox = newtextbox(&ftlib, DEFAULTTEXTMARGIN,
                           FONT0PATH, DEFAULTFONTWIDTH, DEFAULTFONTHEIGHT);
  textboxaddfont(&textbox, FONT1PATH, DEFAULTFONTWIDTH, DEFAULTFONTHEIGHT);
  textboxaddfont(&textbox, FONT2PATH, EMOJIFONTWIDTH, EMOJIFONTHEIGHT);
  textboxaddfont(&textbox, FONT3PATH, EMOJIFONTWIDTH, EMOJIFONTHEIGHT);
  //textbox.packchars = 1; // use bmp width instead of horiadvance
  // need to implement height / line top

  Context context;
  context.textbox   = &textbox;
  context.text.string      = textbox.textstring;
  context.text.cursorstart = textbox.textstring;
  context.text.cursorend   = textbox.textstring;
  context.cursor = (Cursor){ offsetx: 0, line: 1 };
  context.colour = colourcode('W');
  context.highlight = NoHighlight;

//  grSetTitle(setup.surface, ZHU TOU); no UTF8 in title text?
  grSetTitle(setup.surface, "ZHUTOU");
  status.fails = 0;
  writetext(&context, "Welcome to " ZHU TOU "!! !!");
  selectall(&context); // green selection

  grEvent grevent;
  //grListenSurface(display->surface, gr_event_key, &dummyevent);
  do {
    if (!status.update) { continue; } // <--- undelayed infinite loop
    grColor surfacezerocolour = grFindColor(setup.surfacebmp, 127, 127, 127, 255);
    grColor surfacebgcolour = grFindColor(setup.surfacebmp, 0, 0, 255, 127);
    if (processmode == COMMANDMODE) {
      surfacebgcolour = grFindColor(setup.surfacebmp, 0xFF, 0xCC, 0x33, 127);
    }
    grFillRect(setup.surfacebmp, 0, 0, setup.dispwidth, setup.dispheight, surfacezerocolour); //display->warn_color);
    grFillRect(setup.surfacebmp, 0, 0, scrbuf.width, scrbuf.rows, surfacebgcolour);

    renderhighlight(setup.surfacebmp, &context);
    rendertextbox(setup.surfacebmp, &context);
    rendertranslationmenu(setup.surfacebmp, &context);

    grRefreshSurface(setup.surface);
    grListenSurface(setup.surface, 0, &grevent);
    if (grevent.type == gr_event_resize) {
      printf("THIS IS A RESIZE\n");
      // resize event or any other should trigger redraw by loop iteration
      // better to process resize event here ...
    } else { MSG("grevent.type = %d\n", grevent.type); }
  } while (processevent(grevent, &context));
  if (status.fails > 0) {
    printf("status.fails: %d\n", status.fails);
  }
  freetextbox(&textbox); // free fonts and textstring
  freecache(&negadvancechars); // free accent memory

  AFTERDO { FT_Done_FreeType(ftlib); }
  ftlib = NULL;
  exit(0);
  return 0;
}
#endif
