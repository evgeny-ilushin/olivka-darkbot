bool isRuAlpha(char c) { return c >= 'À' && c <= 'ÿ'; }
bool isRuAlphaNum(char c) { return (c >= 'À' && c <= 'ÿ') || (c >= '0' && c <= '9'); }
bool isRuAlphaCaps(char c) { return c >= 'À' && c <= 'ß'; }
bool isRuAlphaSub(char c) { return c >= 'à' && c <= 'ÿ'; }
bool isNum(char c) { return c >= '0' && c <= '9'; }
