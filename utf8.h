#define BYTES  unsigned char *

#define ASCIIWORDCHAR(byte)          \
  (   ('A' RANGE(byte) 'Z')          \
   OR ('a' RANGE(byte) 'z')          \
   OR ('0' RANGE(byte) '9')          \
   OR (byte EQ '-') OR (byte EQ '_') )
#define ASCIIUNICODECHAR(byte) \
  (byte GT 0x7F)

INT utf8ucs4char(BYTES b) {
  IF (!(b[0] & 0x80))          // 0xxxxxxx 1 byte
    { RT (INT)b[0]; }
  EF ((b[0] & 0xE0) EQ 0xC0)   // 110xxxxx 2 byte
    { RT (INT)(((b[0] & 0x1F) << 6) | (b[1] & 0x3F)); }
  EF ((b[0] & 0xF0) EQ 0xE0)   // 1110xxxx 3 byte
    { RT (INT)(((b[0] & 0x0F) << 12) | ((b[1] & 0x3F) << 6) | (b[2] & 0x3F)); }
  EF ((b[0] & 0xFF) LQ 0xF4)   // 1111n 4 byte planes 1-16
    { RT (INT)(((b[0] & 0x07) << 18) | ((b[1] & 0x3F) << 12)
             | ((b[2] & 0x3F) << 6)  |  (b[3] & 0x3F)       ); }
  EL { MSG("UTF8UCS4 FAIL\n"); RT '?'; }
}

INT eatutf8(CS *stringp, CS moveto) {
  INT codepoint = utf8ucs4char((BYTES)*stringp);
  *stringp = moveto; // moves regardless (else infinite loop)
  RT codepoint;      // returns '?' if not UCS2 (1-3 byte)
}

#define OR2(variable, option1, option2) \
  (variable EQ option1 OR variable EQ option2)
#define RANGE(variable) \
  LQ variable AND variable LQ
#define LINEBREAKORTAB(byte) \
  (byte EQ 0x09 OR byte EQ 0x0A)
// carriage return excluded // OR byte EQ 0x0D)
#define UTF8ASCIITEXT(byte) \
  (LINEBREAKORTAB(byte[0]) OR (0x20 RANGE(byte[0]) 0x7E))
#define UTF8ASCII(byte)    \
  (0x01 RANGE(byte[0]) 0x7F)
#define UTF8TWOBYTE(byte)         \
  (    (0xC2 RANGE(byte[0]) 0xDF) \
   AND (0x80 RANGE(byte[1]) 0xBF) )
#define UTF8THREEBYTENOOVERLONG(byte) \
  (    (byte[0] EQ 0xE0)              \
   AND (0xA0 RANGE(byte[1]) 0xBF)     \
   AND (0x80 RANGE(byte[2]) 0xBF)     )
#define UTF8THREEBYTESTRAIGHT(byte)                            \
  (   ((0xE1 RANGE(byte[0]) 0xEC) OR OR2(byte[0], 0xEE, 0xEF)) \
   AND (0x80 RANGE(byte[1]) 0xBF)                              \
   AND (0x80 RANGE(byte[2]) 0xBF)                              )
#define UTF8THREEBYTENOSURROGATE(byte) \
  (    (byte[0] EQ 0xED)               \
   AND (0x80 RANGE(byte[1]) 0x9F)      \
   AND (0x80 RANGE(byte[2]) 0xBF)      )
#define UTF8THREEBYTE(byte)         \
  (UTF8THREEBYTENOOVERLONG(byte)    \
   OR UTF8THREEBYTESTRAIGHT(byte)   \
   OR UTF8THREEBYTENOSURROGATE(byte))
#define UTF8FOURBYTEPLANE1TO3(byte) \
  (    (byte[0] EQ 0xF0)            \
   AND (0x90 RANGE(byte[1]) 0xBF)   \
   AND (0x80 RANGE(byte[2]) 0xBF)   \
   AND (0x80 RANGE(byte[3]) 0xBF)   )
#define UTF8FOURBYTEPLANE4TO15(byte) \
  (    (0xF1 RANGE(byte[0]) 0xF3)    \
   AND (0x80 RANGE(byte[1]) 0xBF)    \
   AND (0x80 RANGE(byte[2]) 0xBF)    \
   AND (0x80 RANGE(byte[3]) 0xBF)    )
#define UTF8FOURBYTEPLANE16(byte) \
  (    (byte[0] EQ 0xF4)          \
   AND (0x80 RANGE(byte[1]) 0x8F) \
   AND (0x80 RANGE(byte[2]) 0xBF) \
   AND (0x80 RANGE(byte[3]) 0xBF) )
#define UTF8FOURBYTE(byte)         \
  (UTF8FOURBYTEPLANE1TO3(byte)     \
   OR UTF8FOURBYTEPLANE4TO15(byte) \
   OR UTF8FOURBYTEPLANE16(byte)    )

INT nextutf8(CS *stringp, CS textend) {
  IF (!*stringp OR !textend) { RT 0; }
  CS texthere = *stringp;
  INT maxbytes = 0;
  IF (texthere LT textend    ) { INC maxbytes; }
  IF (texthere LT textend - 1) { INC maxbytes; }
  IF (texthere LT textend - 2) { INC maxbytes; }
  IF (texthere LT textend - 3) { INC maxbytes; }
  IF (maxbytes EQ 0) { RT NUL; } // end of string
  BYTES b = (BYTES)texthere;
  IF (maxbytes GQ 1 AND UTF8ASCIITEXT(b)) { // if UTF8ASCII(b) {
    RT eatutf8(stringp, (texthere + 1));
  } EF (maxbytes GQ 2 AND UTF8TWOBYTE(b)) {
    RT eatutf8(stringp, (texthere + 2));
  } EF (maxbytes GQ 3 AND UTF8THREEBYTE(b)) {
    RT eatutf8(stringp, (texthere + 3));
  } EF (maxbytes GQ 4 AND UTF8FOURBYTE(b)) {
    RT eatutf8(stringp, (texthere + 4)); // will return '?'
  } EL { MSG("NEXTUTF8 FAIL\n"); RT NUL; }
}

INT prevutf8(CS *stringp, CS textstart) {
  IF (!*stringp OR !textstart) { RT 0; }
  CS texthere = *stringp;
  INT maxbytes = 0;
  IF (texthere GT textstart    ) { INC maxbytes; }
  IF (texthere GT textstart + 1) { INC maxbytes; }
  IF (texthere GT textstart + 2) { INC maxbytes; }
  IF (texthere GT textstart + 3) { INC maxbytes; }
  IF (maxbytes EQ 0) { RT NUL; } // start of string
  BYTES b1 = (BYTES)(texthere - 1);
  BYTES b2 = (BYTES)(texthere - 2);
  BYTES b3 = (BYTES)(texthere - 3);
  BYTES b4 = (BYTES)(texthere - 4);
  CS textprev1 = texthere - 1;
  CS textprev2 = texthere - 2;
  CS textprev3 = texthere - 3;
  CS textprev4 = texthere - 4;
  CS nexttextp = textprev1; // move a char prevent looping
  INT prevcodepoint = NUL; // or error char '#'
  IF (maxbytes GQ 1 AND UTF8ASCIITEXT(b1)) { // if UTF8ASCII(b) {
    nexttextp = textprev1;
    prevcodepoint = eatutf8(&textprev1, textprev1); // '?' or '!' on error
  } EF (maxbytes GQ 2 AND UTF8TWOBYTE(b2)) {
    nexttextp = textprev2;
    prevcodepoint = eatutf8(&textprev2, textprev2);
  } EF (maxbytes GQ 3 AND UTF8THREEBYTE(b3)) {
    nexttextp = textprev3;
    prevcodepoint = eatutf8(&textprev3, textprev3);
  } EF (maxbytes GQ 4 AND UTF8FOURBYTE(b4)) {
    nexttextp = textprev4;
    prevcodepoint = eatutf8(&textprev4, textprev4); // '?'
  } EL { MSG("PREVUTF8 FAIL\n"); RT NUL; }
  *stringp = nexttextp;
  RT prevcodepoint;
}
