

void FillLEDsFromPaletteColors() {
}


void buildPalette() {
  // Only first two entries matter — FastLED interpolates the rest.
  outputPalette = CHSVPalette16(
    colourA,
    colourB
    );
}