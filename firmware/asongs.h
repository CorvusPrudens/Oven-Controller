const byte blip[] PROGMEM = {
  1,
  7,
  2,
  12, 2,
  16, 2
};

const byte forward[] PROGMEM = {
  0,
  7,
  4,
  12, 1,
  14, 1,
  16, 1,
  24, 1
};

const byte back[] PROGMEM = {
  0,
  7,
  4,
  24, 1,
  16, 1,
  16, 1,
  12, 1
};

const byte mario[] PROGMEM = {
  0,
  2,
  105,
  16, 2,
  16, 2,
  255, 2,
  16, 2,
  255, 2,
  12, 2,
  16, 2,
  255, 2,
  19, 2,
  255, 6,
  7, 2,
  255, 6,
  12, 4,
  255, 2,
  7, 4,
  255, 2,
  4, 4,
  255, 2,
  9, 2,
  255, 2,
  11, 4,
  10, 2,
  9, 4,
  7, 3,
  16, 3,
  19, 2,
  21, 4,
  17, 2,
  19, 2,
  255, 2,
  16, 4,
  12, 2,
  14, 2,
  11, 4,
  255, 2,
  12, 4,
  255, 2,
  7, 4,
  255, 2,
  4, 4,
  255, 2,
  9, 2,
  255, 2,
  11, 4,
  10, 2,
  9, 4,
  7, 3,
  16, 3,
  19, 2,
  21, 4,
  17, 2,
  19, 2,
  255, 2,
  16, 4,
  12, 2,
  14, 2,
  11, 4,
  255, 2,
  255, 4,
  19, 2,
  18, 2,
  17, 2,
  15, 4,
  16, 2,
  255, 2,
  8, 2,
  9, 2,
  12, 2,
  255, 2,
  9, 2,
  12, 2,
  14, 2,
  255, 4,
  19, 2,
  18, 2,
  17, 2,
  15, 4,
  16, 2,
  255, 2,
  24, 2,
  255, 2,
  24, 2,
  24, 2,
  255, 6,
  255, 4,
  19, 2,
  18, 2,
  17, 2,
  15, 4,
  16, 2,
  255, 2,
  8, 2,
  9, 2,
  12, 2,
  255, 2,
  9, 2,
  12, 2,
  14, 2,
  255, 4,
  15, 4,
  255, 2,
  14, 4,
  255, 2,
  12, 4,
  255, 12
};

const byte zelda[] PROGMEM = {
  0,
  1,
  120,
  21, 2,
  14, 2,
  10, 2,
  7, 2,
  19, 2,
  14, 2,
  10, 2,
  7, 2,
  18, 2,
  14, 2,
  10, 2,
  7, 2,
  19, 2,
  14, 2,
  10, 2,
  7, 2,
  19, 2,
  12, 2,
  9, 2,
  5, 2,
  17, 2,
  12, 2,
  9, 2,
  5, 2,
  16, 2,
  12, 2,
  9, 2,
  5, 2,
  17, 2,
  12, 2,
  9, 2,
  5, 2,
  17, 2,
  10, 2,
  7, 2,
  4, 2,
  16, 2,
  10, 2,
  7, 2,
  4, 2,
  15, 2,
  10, 2,
  7, 2,
  4, 2,
  16, 2,
  10, 2,
  7, 2,
  4, 2,
  16, 2,
  9, 2,
  5, 2,
  2, 2,
  14, 2,
  9, 2,
  5, 2,
  2, 2,
  13, 2,
  9, 2,
  5, 2,
  2, 2,
  14, 2,
  9, 2,
  5, 2,
  2, 2,
  21, 2,
  14, 2,
  10, 2,
  7, 2,
  19, 2,
  14, 2,
  10, 2,
  7, 2,
  18, 2,
  14, 2,
  10, 2,
  7, 2,
  19, 2,
  14, 2,
  10, 2,
  7, 2,
  22, 2,
  15, 2,
  12, 2,
  9, 2,
  21, 2,
  15, 2,
  12, 2,
  9, 2,
  20, 2,
  15, 2,
  12, 2,
  9, 2,
  21, 2,
  15, 2,
  12, 2,
  9, 2,
  24, 2,
  14, 2,
  10, 2,
  7, 2,
  22, 2,
  14, 2,
  10, 2,
  7, 2,
  21, 2,
  14, 2,
  10, 2,
  7, 2,
  22, 2,
  14, 2,
  10, 2,
  7, 2,
  21, 2,
  10, 2,
  7, 2,
  4, 2,
  19, 2,
  10, 2,
  7, 2,
  4, 2,
};

const byte ff7myst[] PROGMEM = {
  0,
  1,
  9,
  24, 4,
  19, 4,
  23, 8,
  255, 16,
  22, 4,
  17, 4,
  21, 4,
  19, 8,
  255, 16
};

const byte ff7vict[] PROGMEM = {
  1,
  2,
  10,
  12, 2,
  12, 2,
  12, 2,
  12, 6,
  8, 6,
  10, 6,
  12, 4,
  10, 2,
  12, 12,
  255, 6
};

const byte mariovict[] PROGMEM = {
  1, // MODE
  3, // Tempo index
  28, // number of notes
  7, 2,
  12, 2,
  16, 2,
  19, 2,
  24, 2,
  28, 2,
  31, 6,
  28, 6,
  8, 2,
  12, 2,
  15, 2,
  20, 2,
  24, 2,
  27, 2,
  32, 6,
  27, 6,
  10, 2,
  14, 2,
  17, 2,
  22, 2,
  26, 2,
  29, 2,
  34, 6,
  34, 2,
  34, 2,
  34, 2,
  36, 14,
  255, 10
};
