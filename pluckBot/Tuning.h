// calibrated tuning data
// all notes that are sharp or flat are referred to as their sharp for simplicity
#define F2 884
#define Fs2 860 //need to calibrate
#define G2 841
#define Gs2 814
#define A2 777
#define As2 757
#define B2 707
#define C3 669
#define Cs3 619
#define D3 583
#define Ds3 532
#define E3 477
#define F3 388
#define Fs3 304
#define G3 216
#define Gs3 158
#define A3 85
#define As3 6

int midiNoteToTargetVal(int midiNote){
  switch(midiNote){
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