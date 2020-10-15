// calibrated tuning data
// all notes that are sharp or flat are referred to as their sharp for simplicity
#define F2 884
#define Fs2 860 //need to calibrate
#define G2 841
#define Gs2 814
#define A2 458
#define As2 457
#define B2 446
#define C3 439
#define Cs3 422
#define D3 379
#define Ds3 360
#define E3 346
#define F3 320
#define Fs3 292
#define G3 244
#define Gs3 225
#define A3 186
#define As3 147
#define B3 105
#define C4 62

int midiNoteToTargetVal(int midiNote){
  switch(midiNote){
    case 0:
      return 883;
    case 1:
      return 0;
  	case 41:
      return F2;
    case 42:
      return Fs2;
    case 43:
      return G2;
    case 44: 
      return Gs2;
    case 45:
      return A2;
    case 46:
      return As2;
    case 47:
      return B2;
    case 48:
      return C3;
	case 49:
      return Cs3;
    case 50:
      return D3;
    case 51:
      return Ds3;
    case 52:
      return E3;
    case 53:
      return F3;
    case 54:
      return Fs3;
    case 55:
      return G3;
    case 56: 
      return Gs3;
    case 57:
      return A3;
    case 58:
      return As3;
    default:
      return 0;
  }
}
